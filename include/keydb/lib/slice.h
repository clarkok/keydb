#ifndef _KEYDB_LIB_SLICE_H_
#define _KEYDB_LIB_SLICE_H_

#include <string>
#include <cstring>
#include <algorithm>

#include "../config.h"

namespace KeyDB {

class Slice
{
  const char *data_;
  Config::size_t length_;
public:
  Slice(const char *data_ = nullptr, Config::size_t length_ = 0)
  : data_(data_), length_(data_ ? (length_ ? length_ : std::strlen(data_)) : 0)
  { }

  Slice(const Slice &) = default;

  Slice & operator =  (const Slice &) = default;

  inline auto
  data() const -> decltype(data_)
  { return data_; }

  inline auto
  length() const -> decltype(length_) 
  { return length_; }

  inline auto
  begin() const -> decltype(data_)
  { return data(); }

  inline auto
  end() const -> decltype(data_)
  { return data() + length(); }

  inline auto
  operator [] (Config::size_t index) const -> decltype(*data_)
  { return data()[index]; }

  inline int
  compare(Slice b) const
  {
    int flag = std::strncmp(data(), b.data(), std::min(length(), b.length()));
    if (flag) return flag;
    return length() - b.length();
  }

  inline 
  operator bool () const
  { return data() && length(); }

  inline std::string
  toString() const
  { return std::string(begin(), end()); }
};

inline bool
operator < (Slice a, Slice b)
{ return a.compare(b) < 0; }

inline bool
operator <= (Slice a, Slice b)
{ return a.compare(b) <= 0; }

inline bool
operator > (Slice a, Slice b)
{ return a.compare(b) > 0; }

inline bool
operator >= (Slice a, Slice b)
{ return a.compare(b) >= 0; }

inline bool
operator == (Slice a, Slice b)
{ return a.compare(b) == 0; }

inline bool
operator != (Slice a, Slice b)
{ return a.compare(b) != 0; }

}

#endif // _KEYDB_LIB_SLICE_H_
