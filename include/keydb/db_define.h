#ifndef _KEYDB_DB_DEFINE_H_
#define _KEYDB_DB_DEFINE_H_

#include "lib/buffer.h"

namespace KeyDB {

/*!
 * Key in memory
 */
struct Key
{
  uint64_t hash;
  Buffer content;
};

/*!
 * Value in memory
 */
typedef Buffer Value;

}

#endif // _KEYDB_DB_DEFINE_H_
