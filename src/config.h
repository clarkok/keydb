#ifndef _KEYDB_CONFIG_H_
#define _KEYDB_CONFIG_H_

#include <cstdint>

namespace KeyDB {

namespace Config {
  typedef std::uint64_t size_t;

  static const size_t BLOCK_SIZE = 512;

  static const char MAGIC[] = {'K', 'E', 'Y', 'D', 'B', '\0', 'C', 'L'};
}

struct Exception;

}

#endif // _KEYDB_CONFIG_H_
