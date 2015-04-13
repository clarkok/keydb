#include <gtest/gtest.h>

#include "hashtable.h"

using namespace KeyDB;

static const Config::size_t TEST_NUMBER = 10010;
static const Config::size_t TEST_NUMBER_OVER_BORDER = Config::BLOCK_SIZE - 1;

TEST(HashTableTest, Basic)
{
  HashTable uut;

  auto i = uut.getSetterIterator(TEST_NUMBER);
  i.value() = TEST_NUMBER;

  ASSERT_EQ(TEST_NUMBER, uut.getGetterIterator(TEST_NUMBER).value());
}

TEST(HashTableTest, Conflict)
{
  HashTable uut;

  auto i = uut.getSetterIterator(TEST_NUMBER);
  i.value() = TEST_NUMBER;
  ASSERT_EQ(TEST_NUMBER, uut.getSetterIterator(TEST_NUMBER).value());

  i = uut.getSetterIterator(TEST_NUMBER + Config::BLOCK_SIZE);
  i.value() = TEST_NUMBER + Config::BLOCK_SIZE;
  ASSERT_EQ(TEST_NUMBER, uut.getSetterIterator(TEST_NUMBER).value());
  ASSERT_EQ(TEST_NUMBER + Config::BLOCK_SIZE,
    uut.getSetterIterator(TEST_NUMBER + Config::BLOCK_SIZE).value());
}

TEST(HashTableTest, ConflictOverBorder)
{
  HashTable uut;
  auto i = uut.getSetterIterator(TEST_NUMBER_OVER_BORDER);
  i.value() = TEST_NUMBER_OVER_BORDER;
  ASSERT_EQ(TEST_NUMBER_OVER_BORDER,
    uut.getGetterIterator(TEST_NUMBER_OVER_BORDER).value());

  i = uut.getSetterIterator(TEST_NUMBER_OVER_BORDER + Config::BLOCK_SIZE);
  i.value() = TEST_NUMBER_OVER_BORDER + Config::BLOCK_SIZE;
  ASSERT_EQ(TEST_NUMBER_OVER_BORDER,
    uut.getGetterIterator(TEST_NUMBER_OVER_BORDER).value());
  ASSERT_EQ(TEST_NUMBER_OVER_BORDER + Config::BLOCK_SIZE,
    uut.getGetterIterator(
      TEST_NUMBER_OVER_BORDER + Config::BLOCK_SIZE).value());
}

TEST(HashTableTest, MultiKey)
{
  HashTable uut;
  static const int COUNT = 31;

  for (int i = 1; i < COUNT; ++i) {
    auto iter = uut.getSetterIterator(TEST_NUMBER);
    while (iter.value() != uut.DELETED_VALUE)
      iter = uut.nextSetterIterator(iter);
    iter.value() = i;
  }

  auto iter = uut.getGetterIterator(TEST_NUMBER);
  for (int i = 1; i < COUNT; ++i) {
    ASSERT_EQ(i, iter.value());
    iter = uut.nextGetterIterator(iter);
  }
}

TEST(HashTableTest, Erase)
{
  HashTable uut;

  for (int j = 0; j < 50; ++j) {
    auto i = uut.getSetterIterator(1);
    i.value() = 1;
    ASSERT_EQ(1, uut.getGetterIterator(1).value());

    uut.erase(i);
    ASSERT_EQ(uut.DELETED_VALUE, uut.getGetterIterator(1).value());
  }
}

TEST(HashTableTest, EraseOverConflict)
{
  HashTable uut;

  auto i = uut.getSetterIterator(TEST_NUMBER);
  i.value() = TEST_NUMBER;
  i = uut.getSetterIterator(TEST_NUMBER + Config::BLOCK_SIZE);
  i.value() = TEST_NUMBER + Config::BLOCK_SIZE;
  i = uut.getSetterIterator(TEST_NUMBER + 2 * Config::BLOCK_SIZE);
  i.value() = TEST_NUMBER + 2 * Config::BLOCK_SIZE;

  i = uut.getGetterIterator(TEST_NUMBER + Config::BLOCK_SIZE);
  ASSERT_EQ(TEST_NUMBER + Config::BLOCK_SIZE, i.value());
  uut.erase(i);

  ASSERT_EQ(TEST_NUMBER + 2 * Config::BLOCK_SIZE,
    uut.getGetterIterator(TEST_NUMBER + 2 * Config::BLOCK_SIZE).value());
}
