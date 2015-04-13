#include <gtest/gtest.h>
#include <cmath>

#include "lib/slice.h"
#include "test_utils.h"

using namespace KeyDB;

static const char TEST_STRING[] = "Test String";

TEST(SliceTest, Constructor)
{
  Slice uut;
  ASSERT_EQ(0, uut.length());
  ASSERT_EQ(nullptr, uut.data());

  EXPECT_FALSE(uut);

  uut = Slice(TEST_STRING);
  ASSERT_EQ(std::strlen(TEST_STRING), uut.length());
  ASSERT_STREQ(TEST_STRING, uut.data());

  EXPECT_TRUE(uut);

  Slice uut2(uut);
  ASSERT_EQ(std::strlen(TEST_STRING), uut2.length());
  ASSERT_STREQ(TEST_STRING, uut2.data());

  ([](Slice t) {
    ASSERT_EQ(std::strlen(TEST_STRING), t.length());
    ASSERT_STREQ(TEST_STRING, t.data());
  })(uut);

  ([](Slice t) {
    ASSERT_EQ(std::strlen(TEST_STRING), t.length());
    ASSERT_STREQ(TEST_STRING, t.data());
  })("Test String");
}

TEST(SliceTest, Comparation)
{
  static const int len = 16;
  static const int count = 8192;
  char bufa[len + 1], bufb[len + 1];

  for (int i = 0; i < count; ++i) {
    Slice uuta(randomString(bufa, len));
    Slice uutb(randomString(bufb, len));

    ASSERT_EQ(true, uuta == uuta);
    ASSERT_EQ(std::strcmp(bufa, bufb), uuta.compare(uutb));
  }
}
