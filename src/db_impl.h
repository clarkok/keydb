#ifndef _KEYDB_DB_IMPL_H_
#define _KEYDB_DB_IMPL_H_

#include "db.h"
#include "drive/define_on_disk.h"

namespace KeyDB {

struct Bitmap;
class Drive;

class DBImpl : public DB
{
  Drive *drv = nullptr;
  SuperBlock super_block;
  Bitmap *bitmap = nullptr;

  void resetSuperBlock();

public:
  DBImpl() = default;
  virtual ~DBImpl();

  virtual Value get(Key key);
  virtual Value update(Key key, Value value);
  virtual Value insert(Key key, Value value);
  virtual Value erase(Key key);

  virtual void open();
  virtual void close();
  virtual void reset();

  friend DB *KeyDB::createDiskDB(const char *path);

  // For debug & test
  const SuperBlock *
  superBlock() const
  { return &super_block; }
};

}

#endif // _KEYDB_DB_IMPL_H_
