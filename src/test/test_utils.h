#ifndef _KEYDB_TEST_TEST_UTILS_H_
#define _KEYDB_TEST_TEST_UTILS_H_

#include <cstdlib>

static char *
randomString(char *s, int length)
{
  static const char array[] =
    "0123456789"
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

  for (int i = 0; i < length; ++i)
    s[i] = array[std::rand() % (sizeof(array) - 1)];

  s[length] = '\0';
  return s;
}

#endif // _KEYDB_TEST_TEST_UTILS_H_
