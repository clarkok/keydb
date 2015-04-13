#include <algorithm>

#include "drive.h"
#include "config.h"
#include "exception.h"

using namespace KeyDB;

NaiveDisk::~NaiveDisk()
{
  if (fd)
    close();
}

Buffer
NaiveDisk::extendBuffer(Buffer src) const
{
  auto original_length = src.length();
  src.reserve(
    (
      (original_length + Config::BLOCK_SIZE + 1) / Config::BLOCK_SIZE
    ) * Config::BLOCK_SIZE
  );
  std::fill(src.data() + original_length, src.end(), 0);
  return src;
}

Buffer
NaiveDisk::readBlocks(Config::size_t index, Config::size_t count)
{
  if (!fd) {
    THROW_EXCEPTION(
      E_IO_ERROR,
      "File have not been opened",
      path.length() ? path : "");
  }

  Buffer ret(Config::BLOCK_SIZE * count);

  std::fseek(fd, Config::BLOCK_SIZE * index, SEEK_SET);
  ret.reserve(
    Config::BLOCK_SIZE * std::fread(ret.data(), Config::BLOCK_SIZE, count, fd)
  );

  return ret;
}

Config::size_t
NaiveDisk::writeBlocks(
  Config::size_t index,
  Config::size_t count,
  Buffer buf)
{
  buf = extendBuffer(buf);
  if (!fd) {
    THROW_EXCEPTION(
      E_IO_ERROR,
      "File have not been opened",
      path.length() ? path : "");
  }

  std::fseek(fd, Config::BLOCK_SIZE * index, SEEK_SET);
  return std::fwrite(buf.cdata(), Config::BLOCK_SIZE, count, fd);
}

void
NaiveDisk::reset(Slice path)
{
  if (fd)
    close();
  if (path.length())
    this->path = path;
  else
    path = this->path;
  fd = std::fopen(path.data(), "w+");
  if (!fd) {
    THROW_EXCEPTION(
      E_IO_ERROR,
      "File unable to open",
      path);
  }
}

void
NaiveDisk::open(Slice p)
{
  if (fd)
    close();
  if (p.length())
    this->path = p;
  else
    p = this->path;
  fd = std::fopen(p.data(), "r+");
  if (!fd) {
    THROW_EXCEPTION(
      E_IO_ERROR,
      "File unable to open",
      p);
  }
}

void
NaiveDisk::flush()
{
  std::fflush(fd);
}

void
NaiveDisk::close()
{
  std::fclose(fd);
  fd = nullptr;
}
