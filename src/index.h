#ifndef _KEYDB_INDEX_H_
#define _KEYDB_INDEX_H_

#include "lib/buffer.h"

namespace KeyDB {

struct Index
{
  Buffer data;

  Index(Buffer data)
  : data(data)
  { }
};

};

#endif // _KEYDB_INDEX_H_
