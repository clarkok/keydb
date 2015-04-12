#include <gtest/gtest.h>

#include "../disk.h"
#include "../exception.h"

using namespace KeyDB;

static char TEST_STRING[] = "Test String";

TEST(NaiveDiskTest, Operation)
{
  NaiveDisk uut;

  system("rm test_data/non_exist.db");
  EXPECT_THROW({ uut.open("test_data/non_exist.db"); }, Exception);

  uut.reset();

  Buffer buf(TEST_STRING);
  ASSERT_EQ(1, uut.writeBlock(0, buf));
  Buffer r = uut.readBlock(0);
  ASSERT_STREQ(TEST_STRING, r.cdata());

  ASSERT_EQ(1, uut.writeBlock(10, buf));
  r = uut.readBlock(10);
  ASSERT_STREQ(TEST_STRING, r.cdata());
}
