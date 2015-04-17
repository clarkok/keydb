#include <gtest/gtest.h>

#include "drive/drive.h"
#include "exception.h"

using namespace KeyDB;

static char TEST_STRING[] = "Test String";

TEST(NaiveDiskTest, Constructor)
{
  system("touch test_data/naive_disk_open.db");
  NaiveDisk uut("test_data/naive_disk_open.db");
  try {
    uut.open(); 
  }
  catch (const Exception &e) {
    ASSERT_EQ("", e.toString());
  }
}

TEST(NaiveDiskTest, Operation)
{
  NaiveDisk uut;

  system("rm test_data/non_exist.db");
  EXPECT_THROW({ uut.open("test_data/non_exist.db"); }, Exception);

  uut.reset();

  Buffer buf(TEST_STRING);
  ASSERT_EQ(1, uut.writeBlock(0, buf));
  Buffer r = uut.readBlock(0);
  r.reserve(buf.length());
  ASSERT_EQ(buf, r);

  ASSERT_EQ(1, uut.writeBlock(10, buf));
  r = uut.readBlock(10);
  r.reserve(buf.length());
  ASSERT_EQ(buf, r);
}
