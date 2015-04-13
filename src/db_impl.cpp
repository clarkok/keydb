#include "db.h"
#include "drive/drive.h"
#include "drive/bitmap.h"
#include "drive/define_on_disk.h"
#include "db_impl.h"

using namespace KeyDB;

DB*
KeyDB::createDiskDB(const char *path)
{
  Drive *drv = new NaiveDisk(path);
  DBImpl *ret = new DBImpl();
  ret->drv = drv;

  return ret;
}

DBImpl::~DBImpl()
{
  delete bitmap;
}

void
DBImpl::open()
{
  drv->open();
  auto super_buff = drv->readBlock(0);
  std::memcpy(
    reinterpret_cast<void*>(&super_block.data),
    reinterpret_cast<const void*>(super_buff.cdata()),
    sizeof(SuperBlockData)
  );

  // check if magic code matching
  if (std::memcmp(
    super_block.data.magic,
    Config::MAGIC,
    sizeof(Config::MAGIC))) {
    THROW_EXCEPTION(
      E_FORMAT_ERROR,
      "Magic code mismatch",
      "Maybe you open a file that is not a db");
  }

  // check if bitmap is valid
  if (!super_block.data.bitmap_start) {
    THROW_EXCEPTION(
      E_FORMAT_ERROR,
      "Invalid bitmap start",
      "Maybe this file is broken");
  }
  if (!super_block.data.bitmap_length) {
    THROW_EXCEPTION(
      E_FORMAT_ERROR,
      "Invalid bitmap length",
      "Maybe this file is broken");
  }

/*
  // check if index is valid
  if (!super_block.data.index_start) {
    THROW_EXCEPTION(
      E_FORMAT_ERROR,
      "Invalid index start",
      "Mabe this file is broken");
  }
  if (!super_block.data.index_length) {
    THROW_EXCEPTION(
      E_FORMAT_ERROR,
      "Invalid index length",
      "Mabe this file is broken");
  }
*/
}

void
DBImpl::resetSuperBlock()
{
  // reset magic
  std::memcpy(
    reinterpret_cast<void*>(super_block.data.magic),
    reinterpret_cast<const void*>(Config::MAGIC),
    sizeof(Config::MAGIC)
  );

  super_block.data.bitmap_start = bitmap->alloc(1);
  super_block.data.bitmap_length = 1;

  super_block.data.file_size = bitmap->fileSize();
}

void
DBImpl::close()
{ drv->close(); }

void 
DBImpl::reset()
{
  drv->reset();
  if (!bitmap)
    bitmap = new Bitmap(Buffer());
  bitmap->reset();

  resetSuperBlock();

  // write SuperBlock
  drv->writeBlock(0, 
    Buffer(reinterpret_cast<char*>(&super_block), sizeof(super_block)));

  // write bitmap
  drv->writeBlocks(
    super_block.data.bitmap_start,
    super_block.data.bitmap_length,
    bitmap->data);

  drv->flush();
}

Value
DBImpl::get(Key key)
{
  return Value();
}

Value
DBImpl::update(Key key, Value value)
{
  return value;
}

Value
DBImpl::insert(Key key, Value value)
{
  return value;
}

Value
DBImpl::erase(Key key)
{
  return Value();
}
