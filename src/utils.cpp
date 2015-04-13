#include "utils.h"

using namespace KeyDB;
using namespace Utils;

uint64_t
FNVHasher::hash(Slice s) const
{
  static const uint64_t FNV_prime = 1099511628211ull;
  uint64_t ret = 14695981039346656037ull;

  auto limit_64 = 
    s.begin() + (s.length() - sizeof(uint64_t) / sizeof(char));

  auto i = s.begin();

  for (
    ;
    i < limit_64;
    i += sizeof(uint64_t) / sizeof(char)) {
    ret ^= *reinterpret_cast<const uint64_t*>(i);
    ret *= FNV_prime;
  }

  auto d = reinterpret_cast<char*>(&ret);
  for (; i < s.end(); ++i, ++d)
    *d ^= *i;

  return ret;
}
