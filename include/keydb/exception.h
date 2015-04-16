#ifndef _KEYDB_EXCEPTION_H_
#define _KEYDB_EXCEPTION_H_

#include <string>
#include <iostream>
#include <cstdio>
#include <cstdarg>

#include "lib/buffer.h"

namespace KeyDB {

enum ExceptionType
{
  E_OK = 0,

  E_IO_ERROR,
  E_FORMAT_ERROR,
  E_NO_ENTRY,
  E_ENTRY_EXISTS,

  E_UNKNOWN = -1
};

struct Exception
{
  static const int BUFFER_LENGTH = 1024;

  ExceptionType type;
  Buffer msg1;
  Buffer msg2;

  Exception (
    ExceptionType type = E_OK,
    Buffer msg1 = Buffer(),
    Buffer msg2 = Buffer())
  : type(type), msg1(msg1), msg2(msg2)
  { }

  ~Exception() = default;

  inline std::string
  toString() const
  {
    if (type)
      return msg1.toString() + "(" + std::to_string(type) + ")" +
        (msg2.length() ? ": " + msg2.toString() : "");
    else
      return "";
  };

  inline void
  vaprintf(const char *format, ...)
  {
    char buffer[BUFFER_LENGTH];
    va_list args;
    va_start(args, format);
    std::vsnprintf(buffer, BUFFER_LENGTH, format, args);
    va_end(args);
    msg2.append(Slice(buffer));
  }
};

inline std::ostream &
operator << (std::ostream &os, const Exception &e)
{ return os << e.toString(); }

}

#define THROW_E_1(code) \
  (::KeyDB::Exception(code))

#define THROW_E_2(code, msg1) \
  (::KeyDB::Exception(code, msg1))

#define THROW_E_3(code, msg1, msg2) \
  (::KeyDB::Exception(code, msg1, msg2))

#define __THROW_E_GET_MACRO(_1, _2, _3, _name, ...) _name

/*!
 * Construct and throw an exception
 * THROW_EXCEPTION(code[, msg1[, msg2])
 */
#define THROW_EXCEPTION(...) \
  do { \
    ::KeyDB::Exception e = __THROW_E_GET_MACRO(__VA_ARGS__, \
      THROW_E_3, THROW_E_2, THROW_E_1)(__VA_ARGS__); \
    e.vaprintf("\nThrow from: %s:%d\n", __FILE__, __LINE__); \
    throw e; \
  } while (0)

/*!
 * Throw an exception
 * THROW(e)
 */
#define THROW(e) \
  do { \
    e.vaprintf("\nThrow from: %s:%d\n", __FILE__, __LINE__); \
    throw e; \
  } while (0)

#endif // _KEYDB_EXCEPTION_H_
