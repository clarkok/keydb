#ifndef _KEYDB_HASHTABLE_H_
#define _KEYDB_HASHTABLE_H_

#include <limits>
#include <cstdlib>

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

    Iterator
    next() const
    {
      for (Entry *i = entry; 
        i != reinterpret_cast<Entry*>(owner->data.end()); ++i) {
        if (i->value != 0 && i->value != DELETED_VALUE)
          return owner->iteratorFactory(i);
      }
      return owner->iteratorFactory(nullptr);
    }
  };

  Buffer data;
  Config::size_t capacity;

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
    capacity(Config::BLOCK_SIZE / sizeof(Entry))
  { std::memset(data.begin(), 0, data.length()); }

  HashTable(Buffer data)
  : data(data),
    capacity(data.length() / sizeof(Entry))
  { }

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

  inline Config::size_t
  resize(Config::size_t size)
  {
    Buffer new_data(size);
    std::memset(new_data.begin(), 0, new_data.length());

    std::swap(data, new_data);

    auto limit = reinterpret_cast<const Entry*>(new_data.cend());

    capacity = data.length() / sizeof(Entry);

    for (auto i = reinterpret_cast<const Entry*>(new_data.cbegin());
        i != limit; ++i) {
      if (i->value == DELETED_VALUE || i->value == 0)
        continue;
      auto iter = getSetterIterator(i->key);
      while (iter.value() != DELETED_VALUE)
        iter = nextSetterIterator(iter);
      iter.value() = i->value;
    }

    return data.length();
  }

  inline Iterator
  begin()
  {
    for (Entry *iter = reinterpret_cast<Entry*>(data.begin());
    iter != reinterpret_cast<Entry*>(data.end()); ++iter)
      if (iter->value != 0 && iter->value != DELETED_VALUE)
        return iteratorFactory(iter);
    return iteratorFactory(nullptr);
  }

  inline Iterator
  end()
  { return iteratorFactory(nullptr); }
};

}

#endif // _KEYDB_HASHTABLE_H_
