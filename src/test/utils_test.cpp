#include <gtest/gtest.h>

#include "../utils.h"
#include "test_utils.h"

using namespace KeyDB;
using namespace Utils;

TEST(FNVHasherTest, Hash)
{
  Hasher *uut = getFNVHasher();

  ASSERT_EQ(14695981039346656037ull, uut->hash(""));

  static const int MAX_LENGTH = 1024;
  static const int COUNT = 100;

  char buff1[MAX_LENGTH], buff2[MAX_LENGTH];
  for (int i = 0; i < COUNT; ++i)
    ASSERT_NE(
      uut->hash(randomString(buff1, std::rand() % MAX_LENGTH)),
      uut->hash(randomString(buff2, std::rand() % MAX_LENGTH)));
}
