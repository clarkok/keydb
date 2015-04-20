#ifndef _KEYDB_DB_IMPL_H_
#define _KEYDB_DB_IMPL_H_

#include <cstdint>
#include <memory>

#include "db.h"
#include "drive/bitmap.h"
#include "drive/drive.h"
#include "drive/define_on_disk.h"
#include "hashtable.h"

namespace KeyDB {

struct Bitmap;
class Drive;

/*!
 * Entry struct
 *
 *  high address
 * +------------+ 24 + key_length + value_length
 * |   value    |   content of the value
 * +------------+ 24 + key_length
 * |  value_len |   length of the value
 * +------------+ 16 + key_length
 * |    key     |   content of the key
 * +------------+ 16
 * | key_length |   length of the key
 * +------------+ 8
 * |  hash_code |   hash code of the key
 * +------------+ 0
 *  low address
 */

class DBImpl : public DB
{
  struct SuperBlockWrapper
  {
    Buffer buf = Buffer(Config::BLOCK_SIZE);
    SuperBlock *data = reinterpret_cast<SuperBlock*>(buf.data());
    DBImpl *owner;

    SuperBlockWrapper(DBImpl *owner)
    : owner(owner)
    { }

    inline void 
    load(Drive *drv)
    {
      buf = drv->readBlock(0);
      data = reinterpret_cast<SuperBlock*>(buf.data());
    }

    inline void 
    flush(Drive *drv)
    {
      setFileSize(owner->bitmap.fileSize());
      drv->writeBlock(0, buf);
    }

    inline void 
    reset()
    {
      std::memset(data->padding, 0, sizeof(data->padding));

      std::memcpy(
        reinterpret_cast<void*>(data->data.magic),
        reinterpret_cast<const void*>(Config::MAGIC),
        sizeof(Config::MAGIC)
      );
    }

    inline const char *
    getMagic() const
    { return data->data.magic; }

#define DEF_GETTER_SETTER(upper_name, lower_name) \
    inline Config::size_t                         \
    get##upper_name() const                       \
    { return data->data.lower_name; }             \
    inline Config::size_t                         \
    set##upper_name(Config::size_t b)             \
    {                                             \
      return data->data.lower_name = b;           \
    }

    DEF_GETTER_SETTER(FileSize, file_size)
    DEF_GETTER_SETTER(BitmapStart, bitmap_start)
    DEF_GETTER_SETTER(BitmapLength, bitmap_length)
    DEF_GETTER_SETTER(IndexStart, index_start)
    DEF_GETTER_SETTER(IndexLength, index_length)
    DEF_GETTER_SETTER(EntryCount, entry_count)

#undef DEF_GETTER_SETTER
  };

  struct BitmapWrapper
  {
    std::unique_ptr<Bitmap> pimpl;
    DBImpl *owner;

    BitmapWrapper(DBImpl *owner)
    : owner(owner)
    { }

    inline void 
    reset()
    {
      pimpl.reset(new Bitmap(Buffer()));
      pimpl->reset();
    }

    inline void
    load(Drive *drv)
    {
      pimpl.reset(
        new Bitmap(drv->readBlocks(
          owner->super_block.getBitmapStart(),
          owner->super_block.getBitmapLength()
        ))
      );
    }

    inline void 
    flush(Drive *drv)
    {
      drv->writeBlocks(
        owner->super_block.getBitmapStart(),
        owner->super_block.getBitmapLength(),
        pimpl->data
      );
    }

    inline Config::size_t
    alloc(Config::size_t size)
    {
      auto ret = pimpl->alloc(size);

      SuperBlockWrapper &sp = owner->super_block;

      if (
        pimpl->data.length() >
        sp.getBitmapLength() * Config::BLOCK_SIZE
        ) {
        Config::size_t new_length = pimpl->data.length() / Config::BLOCK_SIZE;
        pimpl->free(
          sp.getBitmapStart(),
          sp.getBitmapLength()
        );
        sp.setBitmapStart(
          alloc(sp.setBitmapLength(new_length))
        );
      }
      return ret;
    }

    inline void
    free(Config::size_t index, Config::size_t size)
    { pimpl->free(index, size); }

    inline Config::size_t
    fileSize() const
    { return pimpl->fileSize(); }
  };

  struct IndexWrapper
  {
    std::unique_ptr<HashTable> pimpl;
    DBImpl *owner;

    IndexWrapper(DBImpl *owner)
    : owner(owner)
    { }

    inline void
    load(Drive *drv)
    {
      pimpl.reset(
        new HashTable(
          drv->readBlocks(
            owner->super_block.getIndexStart(),
            owner->super_block.getIndexLength()
          )
        )
      );
    }

    inline void
    flush(Drive *drv)
    {
      drv->writeBlocks(
        owner->super_block.getIndexStart(),
        owner->super_block.getIndexLength(),
        pimpl->data
      );
    }

    inline void
    flushBlock(Drive *drv, HashTable::Iterator iter)
    {
      auto block_offset = 
        (reinterpret_cast<char*>(iter.entry) - pimpl->data.cbegin()) /
        Config::BLOCK_SIZE;
      drv->writeBlock(
        owner->super_block.getIndexStart() + 
          block_offset,
        Buffer(pimpl->data.cdata() +
          block_offset * Config::BLOCK_SIZE, Config::BLOCK_SIZE)
      );
    }

    inline void
    reset()
    { pimpl.reset(new HashTable()); }

    inline Config::size_t
    getCapacity() const
    { return pimpl->capacity; }

    inline Config::size_t
    getUpperLimit() const
    { return getCapacity() - (getCapacity() >> 2); }

    inline auto
    getterIterator(Key key, Config::size_t hash_code, Buffer &entry)
      -> decltype(pimpl->getGetterIterator(key))
    {
      for (
        auto iter = pimpl->getGetterIterator(hash_code);
        iter.entry; iter = pimpl->nextGetterIterator(iter)
      ) {
        if (iter.value() == pimpl->DELETED_VALUE)
          break;
        entry = owner->readEntry(
          *reinterpret_cast<IndexLength*>(&iter.value())
        );
        if (keyOfEntry(entry) == key)
          return iter;
      }
      THROW_EXCEPTION(
        E_NO_ENTRY,
        "No entry found",
        key
      );
    }

    inline auto
    setterIterator(Key key, Config::size_t hash_code, Buffer &entry) 
      -> decltype(pimpl->getSetterIterator(key))
    {
      for (
        auto iter = pimpl->getSetterIterator(hash_code);
        ; iter = pimpl->nextSetterIterator(iter)
      ) {
        if (iter.value() != pimpl->DELETED_VALUE) {
          entry = owner->readEntry(
            *reinterpret_cast<IndexLength*>(&iter.value())
          );
          if (keyOfEntry(entry) == key)
            return iter;
        }
        else
          return iter;
      }
    }

    inline void
    resize(Config::size_t size)
    { 
      pimpl->resize(size);
      owner->super_block.setIndexLength(
        (size + Config::BLOCK_SIZE - 1) / Config::BLOCK_SIZE);
      owner->super_block.setIndexStart(
        owner->bitmap.alloc(owner->super_block.getIndexLength())
      );
    }
  };

  Drive *drv = nullptr;
  SuperBlockWrapper super_block;
  BitmapWrapper bitmap;
  IndexWrapper index;

  struct IndexLength
  {
    uint32_t index;
    uint32_t length;
  };

  static Buffer makeEntry(Key key, Config::size_t hash_code, Value value);

  static Key keyOfEntry(Buffer entry);
  static Value valueOfEntry(Buffer entry);

  inline Buffer
  readEntry(IndexLength index_length)
  { return drv->readBlocks(index_length.index, index_length.length); }

  inline void
  writeEntry(IndexLength idxlen, Buffer entry)
  { drv->writeBlocks(idxlen.index, idxlen.length, entry); }

public:
  DBImpl(Drive *drv)
  : drv(drv), super_block(this), bitmap(this), index(this)
  { }

  virtual ~DBImpl();

  virtual Value get(Key key);
  virtual Value update(Key key, Value value);
  virtual Value insert(Key key, Value value);
  virtual Value erase(Key key);

  virtual void open();
  virtual void close();
  virtual void reset();

  void flush();

  friend DB *KeyDB::createDiskDB(const char *path);

  // For debug & test
  const SuperBlock *
  superBlock() const
  { return super_block.data; }
};

}

#endif // _KEYDB_DB_IMPL_H_
