#ifndef _KEYDB_DISK_DEFINE_ON_DISK_H_
#define _KEYDB_DISK_DEFINE_ON_DISK_H_

#include <cstdint>

#include "config.h"
#include "lib/buffer.h"

namespace KeyDB {

struct SuperBlockData
{
  char magic[sizeof(Config::MAGIC)];
  Config::size_t file_size;     // file size in block
  Config::size_t bitmap_start;  // bitmap_start index in block
  Config::size_t bitmap_length; // bitmap_length in block
  Config::size_t index_start;   // index_start index in block
  Config::size_t index_length;  // index_length in block
};

// SuperBlock is always a block size
union SuperBlock
{
  SuperBlockData data;
  uint8_t padding[Config::BLOCK_SIZE];
};

}

#endif // _KEYDB_DISK_DEFINE_ON_DISK_H_
