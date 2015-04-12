#include <gtest/gtest.h>

#include "../lib/buffer.h"

using namespace KeyDB;

TEST(BufferTest, Constructor)
{
  static char TEST_STRING[] =
    "Test String";

  Buffer uut;
  ASSERT_EQ(0, uut.length());

  Buffer uut2(10);
  ASSERT_EQ(10, uut2.length());

  Buffer uut3(TEST_STRING, std::strlen(TEST_STRING));
  ASSERT_EQ(std::strlen(TEST_STRING), uut3.length());
  ASSERT_EQ(0, std::strncmp(TEST_STRING, uut3.cdata(), uut3.length()));

  Buffer uut4 = Slice(TEST_STRING);
  ASSERT_EQ(std::strlen(TEST_STRING), uut4.length());
  ASSERT_EQ(0, std::strncmp(TEST_STRING, uut4.cdata(), uut4.length()));

  Buffer uut5 = uut4;
  ASSERT_EQ(std::strlen(TEST_STRING), uut5.length());
  ASSERT_EQ(0, std::strncmp(TEST_STRING, uut5.cdata(), uut4.length()));

  const_cast<char*>(uut5.cdata())[0] = 'R';
  ASSERT_EQ('R', uut4.cdata()[0]);
}

TEST(BufferTest, Operation)
{
  static char TEST_STRING[] = 
    "Test String";

  Buffer uut(TEST_STRING);
  Buffer uut2(uut);

  uut2.data()[0] = 'R';
  ASSERT_EQ('R', uut2.data()[0]);
  ASSERT_EQ('T', uut.data()[0]);
}
