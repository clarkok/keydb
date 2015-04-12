#ifndef _KEYDB_LIB_BUFFER_H_
#define _KEYDB_LIB_BUFFER_H_

#include <memory>
#include <cstdlib>
#include <string>
#include <iostream>

#include "../config.h"
#include "slice.h"

namespace KeyDB {

class Buffer
{
#define allocBufferImpl(size) \
  (reinterpret_cast<BufferImpl*>(std::malloc(size)))

  struct BufferImpl
  {
    Config::size_t length;
    char content[0];
  };

  typedef std::shared_ptr<BufferImpl> T_ptr;

  static BufferImpl *
  bufferImplFactory(Config::size_t length)
  {
    BufferImpl *ret = allocBufferImpl(length + sizeof(Config::size_t));
    ret->length = length;
    return ret;
  }

  T_ptr
  sharedPtrFactory(BufferImpl *p)
  {
    return T_ptr(p, std::free);
  }

  T_ptr pimpl;
  char *content_;
  Config::size_t length_;

public:
  Buffer()
  : pimpl(nullptr), content_(nullptr), length_(0)
  { }

  Buffer(Config::size_t length)
  : pimpl(sharedPtrFactory(bufferImplFactory(length))),
  content_(pimpl->content), length_(length)
  { }

  Buffer(const char *data, Config::size_t length = 0)
  : pimpl(sharedPtrFactory(
      bufferImplFactory(length ? length : length = std::strlen(data)))),
  content_(pimpl->content), length_(length)
  { std::memcpy(pimpl->content, data, length); }

  Buffer(Slice s)
  : Buffer(s.data(), s.length())
  { }

  Buffer(const Buffer &) = default;
  Buffer & operator = (const Buffer &) = default;

  inline
  operator Slice () const
  { return Slice(cdata(), length()); }

  inline void
  clone(Config::size_t length = 0)
  {
    if (pimpl && pimpl.unique() && (length == 0))
      return;
    else {
      Config::size_t copy_length;
      if (length == 0)
        copy_length = length = this->length();
      else
        copy_length = std::min(length, this->length());

      BufferImpl *p = bufferImplFactory(length);
      if (pimpl)
        std::memcpy(p, pimpl.get(), copy_length + sizeof(Config::size_t));
      p->length = length;
      pimpl.reset(p);

      if (pimpl) {
        content_ = pimpl->content;
        length_ = pimpl->length;
      }
      else {
        content_ = nullptr;
        length_ = 0;
      }
    }
  }

  inline void
  reserve(Config::size_t length)
  { clone(length); }

  inline auto
  length() const -> decltype(pimpl->length)
  { 
    return length_;
  }

  inline const char *
  cdata() const
  {
    return content_;
  }

  inline char *
  data()
  { 
    clone();
    return const_cast<char*>(cdata()); 
  }

  inline char
  at(Config::size_t index) const
  { return cdata()[index]; }

  inline char &
  at(Config::size_t index)
  { clone(); return data()[index]; }

  inline char
  operator [] (Config::size_t index) const
  { return at(index); }

  inline char &
  operator [] (Config::size_t index)
  { return at(index); }

  inline char *
  begin()
  { clone(); return data(); }

  inline char *
  end()
  { clone(); return data() + length(); }

  inline const char *
  cbegin() const
  { return cdata(); }

  inline const char *
  cend() const
  { return cdata() + length(); }

  inline std::string
  toString() const
  { return std::string (cbegin(), cend()); }

  inline Buffer &
  append(const Slice &s)
  {
    auto original_length = length();

    reserve(original_length + s.length());
    std::memcpy(data() + original_length, s.data(), s.length());

    return *this;
  }
};

inline std::ostream &
operator << (std::ostream &os, const Buffer &buf)
{ return os.write(buf.cdata(), buf.length()); }

};

#endif // _KEYDB_LIB_BUFFER_H_
