#ifndef _KEYDB_DB_DEFINE_H_
#define _KEYDB_DB_DEFINE_H_

#include "lib/buffer.h"

namespace KeyDB {

/*!
 * Key in memory
 */
typedef Slice Key;

/*!
 * Value in memory
 */
typedef Buffer Value;

}

#endif // _KEYDB_DB_DEFINE_H_
