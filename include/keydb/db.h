#ifndef _KEYDB_DB_H_
#define _KEYDB_DB_H_

#include "db_define.h"
#include "exception.h"

namespace KeyDB {

class Operative
{
public:
  /*!
   * Get a value from the Operative
   * throw E_NO_ENTRY when no entry is found
   *
   * @param key the key to get
   * @return the value of the key
   */
  virtual Value get(Key key) = 0;

  /*!
   * Update a value of an entry
   * throw E_NO_ENTRY when no entry is found
   *
   * @param key the key of the entry
   * @param value the new value of the entry
   * @return the original value
   */
  virtual Value update(Key key, Value value) = 0;

  /*!
   * Insert a new entry into the db
   * throw E_ENTRY_EXISTS when an entry of key exists
   *
   * @param key the key of the entry
   * @param value the value of the entry
   * @return the value
   */
  virtual Value insert(Key key, Value value) = 0;

  /*!
   * Erase a entry
   * throw E_NO_ENTRY when no entry is found
   *
   * @param key the key of the entry
   * @return the current value of the entry
   */
  virtual Value erase(Key key) = 0;
};

class DB : public Operative
{
public:
  DB() = default;
  DB(const DB &) = delete;
  DB & operator = (const DB &) = delete;

  virtual Value get(Key key) = 0;
  virtual Value update(Key key, Value value) = 0;
  virtual Value insert(Key key, Value value) = 0;
  virtual Value erase(Key key) = 0;

  virtual void open() = 0;
  virtual void close() = 0;
  virtual void reset() = 0;
};

DB *createDiskDB(const char *path);

}

#endif // _KEYDB_DB_H_
