#ifndef _KEYDB_UTILS_H_
#define _KEYDB_UTILS_H_

#include <cstdint>

#include "lib/slice.h"

namespace KeyDB {

namespace Utils {

struct Hasher
{
  virtual uint64_t hash(Slice s) const = 0;
};

struct FNVHasher : public Hasher
{
  virtual uint64_t hash(Slice s) const;
};

inline Hasher *
getFNVHasher()
{
  static auto *ret = new FNVHasher();
  return ret;
}

inline Hasher *
getDefaultHasher()
{ return getFNVHasher(); }

};

};

#endif // _KEYDB_UTILS_H_
