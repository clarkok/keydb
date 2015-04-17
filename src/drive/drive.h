#ifndef _KEYDB_DRIVE_H_
#define _KEYDB_DRIVE_H_

#include <cstdio>

#include "config.h"
#include "lib/buffer.h"

namespace KeyDB {

class Drive
{
public:
  Drive() = default;
  Drive(const Drive &) = delete;
  Drive & operator = (const Drive &) = delete;

  virtual ~Drive() { }

  /*!
   * Read a series of blocks from disk
   *
   * @param index index of the first block
   * @param count count of the series
   * @return data
   */
  virtual Buffer readBlocks(
    Config::size_t index,
    Config::size_t count) = 0;

  /*!
   * Write a series of blocks to disk
   * If length of data is not enough, 0 will be added
   *
   * @param index index of the first block
   * @param count count of the series
   * @param buf the data
   * @return count of blocks writen
   */
  virtual Config::size_t writeBlocks(
    Config::size_t index,
    Config::size_t count,
    Buffer buf) = 0;

  /*!
   * Read a block from disk
   *
   * @param index index of the block
   * @return data
   */
  virtual Buffer
  readBlock(Config::size_t index)
  { return readBlocks(index, 1); }

  /*!
   * Write a block to disk
   *
   * @param index index of the block
   * @param buf data
   * @return 1 if succeed 0 if failed
   */
  virtual Config::size_t 
  writeBlock(Config::size_t index, Buffer buf)
  { return writeBlocks(index, 1, buf); }

  /*!
   * Create a new file on disk, if the file exists the file will be recreated
   */
  virtual void reset(Slice path = Slice()) = 0;

  /*!
   * Open a file on the disk, throw exception when file is unable to read
   */
  virtual void open(Slice path = Slice()) = 0;

  /*!
   * Flush data to disk
   */
  virtual void flush() = 0;

  /*!
   * Close file if opened, not an error if file has not been opened
   */
  virtual void close() = 0;
};

class NaiveDisk : public Drive
{
  std::FILE *fd = nullptr;
  Slice path;

public:
  NaiveDisk(Slice path = Slice())
  : path(path)
  { }

  virtual ~NaiveDisk();

  virtual Buffer readBlocks(Config::size_t index, Config::size_t count);
  virtual Config::size_t writeBlocks(
    Config::size_t index,
    Config::size_t count,
    Buffer buf);

  virtual void reset(Slice path = Slice());
  virtual void open(Slice path = Slice());
  virtual void flush();
  virtual void close();
};

};

#endif // _KEYDB_DRIVE_H_
