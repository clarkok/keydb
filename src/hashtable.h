#ifndef _KEYDB_HASHTABLE_H_
#define _KEYDB_HASHTABLE_H_

#include <limits>

#include "config.h"
#include "lib/buffer.h"
#include "utils.h"

namespace KeyDB {

// not 0
class HashTable
{
  struct Entry;
  struct HashTableImpl;

  struct Entry
  {
    Config::size_t key;
    Config::size_t value;
  };

public:
  enum {
    DELETED_VALUE =
      std::numeric_limits<Config::size_t>::max()
  };

  struct Iterator
  {
    Entry *entry;
    Config::size_t key_;
    Config::size_t idx;
    HashTable *owner;

    Config::size_t
    key() const
    { return key_; }

    Config::size_t &
    value()
    { return entry->value; }
  };

private:
  inline Iterator
  iteratorFactory(Entry *entry)
  {
    Iterator ret;
    ret.owner = this;
    ret.entry = entry;
    if (entry) {
      ret.key_ = entry->key;
      ret.idx = entry - reinterpret_cast<Entry*>(data.data());
    }
    else {
      ret.key_ = DELETED_VALUE;
      ret.idx = DELETED_VALUE;
    }
    return ret;
  }

  Buffer data;
  Config::size_t size;
  Config::size_t capacity;

  inline Entry*
  findSetterPosition(
    Config::size_t key,
    Config::size_t idx = DELETED_VALUE)
  {
    if (idx == DELETED_VALUE)
      idx = key & (capacity - 1);
    else
      idx = (idx + 1) & (capacity - 1);
    Config::size_t start = (idx + capacity - 1) & (capacity - 1);

    while (true) {
      auto p = reinterpret_cast<Entry*>(data.data()) + idx;

      if (p->key == key ||
          p->value == DELETED_VALUE ||
          p->value == 0)
        return p;
      if (idx == start)
        return nullptr;

      idx = (idx + 1) & (capacity - 1);
    }
  }

  inline Entry*
  findGetterPosition(
    Config::size_t key,
    Config::size_t idx = DELETED_VALUE)
  {
    if (idx == DELETED_VALUE)
      idx = key & (capacity - 1);
    else
      idx = (idx + 1) & (capacity - 1);
    Config::size_t start = (idx + capacity - 1) & (capacity - 1);

    while (true) {
      auto p = reinterpret_cast<Entry*>(data.data()) + idx;

      if (p->key == key)
        return p;
      if (p->value == 0 || idx == start)
        return nullptr;

      idx = (idx + 1) & (capacity - 1);
    }
  }

public:
  HashTable()
  : data(Config::BLOCK_SIZE),
    size(0),
    capacity(Config::BLOCK_SIZE / sizeof(Entry))
  { std::fill(data.begin(), data.end(), 0); }

  inline Iterator
  getSetterIterator(Config::size_t key)
  { 
    auto p = findSetterPosition(key);
    if (p) {
      p->key = key;
      if (p->value == 0)
        p->value = DELETED_VALUE;
    }
    return iteratorFactory(p);
  }

  inline Iterator
  nextSetterIterator(Iterator i)
  {
    auto p = findSetterPosition(i.key(), i.idx);
    if (p) {
      p->key = i.key();
      if (p->value == 0)
        p->value = DELETED_VALUE;
    }
    return iteratorFactory(p);
  }

  inline Iterator
  getGetterIterator(Config::size_t key)
  { return iteratorFactory(findGetterPosition(key)); }

  inline Iterator
  nextGetterIterator(Iterator i)
  { return iteratorFactory(findGetterPosition(i.key(), i.idx)); }

  inline Config::size_t
  erase(Iterator i)
  {
    if (i.entry) {
      Config::size_t ret = i.entry->value;
      i.entry->value = DELETED_VALUE;
      return ret;
    }
    return DELETED_VALUE;
  }
};

}

#endif // _KEYDB_HASHTABLE_H_
