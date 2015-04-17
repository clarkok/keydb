#include "db.h"
#include "drive/drive.h"
#include "drive/bitmap.h"
#include "drive/define_on_disk.h"
#include "db_impl.h"
#include "utils.h"

using namespace KeyDB;

DB*
KeyDB::createDiskDB(const char *path)
{
  Drive *drv = new NaiveDisk(path);
  return new DBImpl(drv);
}

inline Buffer
DBImpl::readEntry(IndexLength index_length)
{
  return drv->readBlocks(index_length.index, index_length.length);
}

inline Buffer
DBImpl::makeEntry(Key key, Config::size_t hash_code, Value value)
{
  Buffer ret(
    // sizeof hash_code
    sizeof(hash_code) +
    // sizeof key
    sizeof(Config::size_t) +
    key.length() +
    // sizeof value
    sizeof(Config::size_t) +
    value.length()
  );

  *reinterpret_cast<Config::size_t*>(ret.data()) = hash_code;
  *reinterpret_cast<Config::size_t*>(ret.data() + sizeof(Config::size_t)) =
    key.length();
  std::memcpy(
    reinterpret_cast<void*>(ret.data() + 2 * sizeof(Config::size_t)),
    reinterpret_cast<const void*>(key.data()),
    key.length());
  *reinterpret_cast<Config::size_t*>(
    ret.data() + 2 * sizeof(Config::size_t) + key.length()) = 
    value.length();
  std::memcpy(
    reinterpret_cast<void*>(
      ret.data() + 3 * sizeof(Config::size_t) + key.length()),
    reinterpret_cast<const void*>(value.data()),
    value.length());

  return ret;
}

inline Key
DBImpl::keyOfEntry(Buffer entry)
{
  return Slice(
    entry.cdata() + 2 * sizeof(Config::size_t),
    *reinterpret_cast<const Config::size_t*>(
      entry.cdata() + sizeof(Config::size_t)));
}

inline Value
DBImpl::valueOfEntry(Buffer entry)
{
  auto key_length = keyOfEntry(entry).length();
  return Slice(
    entry.cdata() + 3 * sizeof(Config::size_t) + key_length,
    *reinterpret_cast<const Config::size_t*>(
      entry.cdata() + 2 * sizeof(Config::size_t) + key_length));
}


DBImpl::~DBImpl()
{ }

void
DBImpl::open()
{
  drv->open();
  super_block.load(drv);

  // check if magic code matching
  if (std::memcmp(
    super_block.getMagic(),
    Config::MAGIC,
    sizeof(Config::MAGIC))) {
    THROW_EXCEPTION(
      E_FORMAT_ERROR,
      "Magic code mismatch",
      "Maybe you open a file that is not a db");
  }

  // check if bitmap is valid
  if (!super_block.getBitmapStart()) {
    THROW_EXCEPTION(
      E_FORMAT_ERROR,
      "Invalid bitmap start",
      "Maybe this file is broken");
  }
  if (!super_block.getBitmapLength()) {
    THROW_EXCEPTION(
      E_FORMAT_ERROR,
      "Invalid bitmap length",
      "Maybe this file is broken");
  }

  // load bitmap
  bitmap.load(drv);

  // check if index is valid
  if (!super_block.getIndexStart()) {
    THROW_EXCEPTION(
      E_FORMAT_ERROR,
      "Invalid index start",
      "Mabe this file is broken");
  }
  if (!super_block.getIndexLength()) {
    THROW_EXCEPTION(
      E_FORMAT_ERROR,
      "Invalid index length",
      "Mabe this file is broken");
  }

  // load index
  index.load(drv);
}

void
DBImpl::close()
{ 
  flush();
  drv->close();
}

void 
DBImpl::reset()
{
  drv->reset();

  bitmap.reset();
  index.reset();
  super_block.reset();

  super_block.setBitmapStart(
    bitmap.alloc(
      super_block.setBitmapLength(1)
    )
  );

  super_block.setIndexStart(
    bitmap.alloc(
      super_block.setIndexLength(1)
    )
  );

  super_block.setFileSize(bitmap.fileSize());

  flush();
}

void
DBImpl::flush()
{
  super_block.flush(drv);
  bitmap.flush(drv);
  index.flush(drv);
  drv->flush();
}

Value
DBImpl::get(Key key)
{
  Buffer entry;
  index.getterIterator(key, Utils::getDefaultHasher()->hash(key), entry);
  return valueOfEntry(entry);
}

Value
DBImpl::update(Key key, Value value)
{
  auto ret = erase(key);
  insert(key, value);
  return ret;
}

Value
DBImpl::insert(Key key, Value value)
{
  Buffer entry;
  Config::size_t hash_code = Utils::getDefaultHasher()->hash(key);
  auto iter = index.setterIterator(key, hash_code, entry);
  if (iter.value() != index.pimpl->DELETED_VALUE) {
    Exception e(E_ENTRY_EXISTS, "Key exists");
    e.vaprintf(
      "( %s, %s )",
      key.toString().c_str(),
      valueOfEntry(entry).toString().c_str()
    );
    THROW(e);
  }
  entry = makeEntry(key, hash_code, value);

  IndexLength *idxlen = reinterpret_cast<IndexLength*>(&iter.value());
  idxlen->length = 
    (entry.length() + Config::BLOCK_SIZE - 1) / Config::BLOCK_SIZE;
  idxlen->index = bitmap.alloc(idxlen->length);

  if (super_block.setEntryCount(super_block.getEntryCount() + 1)
      >= index.getUpperLimit()) {
    index.resize(index.getCapacity() * 2 * Config::BLOCK_SIZE);
    index.flush(drv);
  }
  else
    index.flushBlock(drv, iter);

  writeEntry(*idxlen, entry);
  super_block.flush(drv);
  bitmap.flush(drv);

  return value;
}

Value
DBImpl::erase(Key key)
{
  Buffer entry;
  auto iter = index.getterIterator(key, 
    Utils::getDefaultHasher()->hash(key), entry);

  iter.value() = index.pimpl->DELETED_VALUE;
  bitmap.free(
    reinterpret_cast<IndexLength*>(&iter.value())->index,
    reinterpret_cast<IndexLength*>(&iter.value())->length
  );
  super_block.setEntryCount(super_block.getEntryCount() - 1);
  super_block.flush(drv);
  bitmap.flush(drv);
  index.flushBlock(drv, iter);

  return valueOfEntry(entry);
}
