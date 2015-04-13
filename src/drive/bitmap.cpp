#include "bitmap.h"

using namespace KeyDB;

Bitmap::Bitmap(Buffer buf)
: data(buf)
{ }

Config::size_t
Bitmap::alloc(Config::size_t size)
{
  BitmapImpl *pimpl = reinterpret_cast<BitmapImpl*>(data.data());
  BitmapPair *limit = pimpl->data + pimpl->count;
  auto i = pimpl->data;
  for (; i != limit; ++i)
    if (i->length >= size)
      break;

  Config::size_t ret;

  if (i != limit) {
    ret = i->index;
    if (i->length == size) {
      std::memmove(
        reinterpret_cast<void*>(i),
        reinterpret_cast<void*>(i+1),
        sizeof(BitmapPair) * (limit - i - 1)
      );
      --(pimpl->count);
    }
    else {
      i->index += size;
      i->length -= size;
    }
  }
  else {
    ret = pimpl->file_size;
    pimpl->file_size += size;
  }

  return ret;
}

void
Bitmap::free(Config::size_t index, Config::size_t size)
{
  BitmapImpl *pimpl = reinterpret_cast<BitmapImpl*>(data.data());
  if (data.length() < 
    sizeof(BitmapImpl) + sizeof(BitmapPair) * pimpl->count) {
    data.reserve(data.length() + Config::BLOCK_SIZE);
    pimpl = reinterpret_cast<BitmapImpl*>(data.data());
  }

  pimpl->data[pimpl->count++] = {index, size};
}

void
Bitmap::reset()
{
  data.reserve(Config::BLOCK_SIZE);

  auto pimpl = reinterpret_cast<BitmapImpl*>(data.data());
  pimpl->count = 0;
  pimpl->file_size = 1; // SuperBlock
}

Config::size_t
Bitmap::fileSize() const
{ return reinterpret_cast<const BitmapImpl*>(data.cdata())->file_size; }
