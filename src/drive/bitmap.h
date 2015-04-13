#ifndef _KEYDB_DRIVE_BITMAP_H_
#define _KEYDB_DRIVE_BITMAP_H_

#include <set>

#include "config.h"
#include "lib/buffer.h"

namespace KeyDB {

// NOTE: Bitmap blocks on the disk store all gap between existing entries
struct Bitmap
{
  struct BitmapPair
  {
    Config::size_t index;
    Config::size_t length;
  };

  struct BitmapImpl
  {
    Config::size_t count;
    // file size in block
    Config::size_t file_size;
    BitmapPair data[0];
  };

  Buffer data;

  Bitmap(Buffer buf);
  
  void reset();
  Config::size_t alloc(Config::size_t size);
  void free(Config::size_t index, Config::size_t size);
  Config::size_t fileSize() const;
};

}

#endif // _KEYDB_DRIVE_BITMAP_H_
