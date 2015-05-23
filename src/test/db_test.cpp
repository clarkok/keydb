#include <gtest/gtest.h>

#include "test_utils.h"
#include "drive/drive.h"
#include "db_impl.h"

using namespace KeyDB;

static const char RESETING_DB_PATH[] =
  "test_data/db_test_reseting.db";

static const char OPERATION_DB_PATH[] =
  "test_data/db_test_operation.db";

static const char ERASE_DB_PATH[] =
  "test_data/db_test_erasing.db";

TEST(DBTest, Reseting)
{
  DBImpl *uut = dynamic_cast<DBImpl*>(
    createDiskDB(RESETING_DB_PATH));

  uut->reset();

  auto sb = uut->superBlock();

  ASSERT_EQ(0,
    std::memcmp(Config::MAGIC, sb->data.magic, sizeof(Config::MAGIC)));
  ASSERT_EQ(3, sb->data.file_size);
  ASSERT_EQ(1, sb->data.bitmap_start);
  ASSERT_EQ(1, sb->data.bitmap_length);

  NaiveDisk disk;
  disk.open("test_data/db_test_reseting.db");
  auto sb_buffer = disk.readBlock(0);

  sb = reinterpret_cast<SuperBlock*>(sb_buffer.data());

  ASSERT_EQ(0,
    std::memcmp(Config::MAGIC, sb->data.magic, sizeof(Config::MAGIC)));
  ASSERT_EQ(3, sb->data.file_size);
  ASSERT_EQ(1, sb->data.bitmap_start);
  ASSERT_EQ(1, sb->data.bitmap_length);
}

TEST(DBTest, Openning)
{
  DBImpl *uut = dynamic_cast<DBImpl*>(
    createDiskDB(RESETING_DB_PATH));

  try {
    uut->open();
  }
  catch (const Exception &e) {
    ASSERT_EQ("", e.toString());
  }

  auto sb = uut->superBlock();

  ASSERT_EQ(0,
    std::memcmp(Config::MAGIC, sb->data.magic, sizeof(Config::MAGIC)));
  ASSERT_EQ(3, sb->data.file_size);
  ASSERT_EQ(1, sb->data.bitmap_start);
  ASSERT_EQ(1, sb->data.bitmap_length);
}

TEST(DBTest, Operation)
{
  DBImpl *uut = dynamic_cast<DBImpl*>(
    createDiskDB(OPERATION_DB_PATH));

  char buffer[17];

  try {
    uut->reset();
    uut->insert("Test", "test");
    
    ASSERT_EQ("test", uut->get("Test"));

    for (int i = 0; i < 50; ++i) {
      randomString(buffer, 16);
      uut->insert(buffer, buffer);
      ASSERT_EQ(buffer, uut->get(buffer));
    }

    uut->close();
  } 
  catch (const Exception &e) {
    ASSERT_EQ("", e.toString());
    uut->close();
  }

  EXPECT_THROW({uut->insert("Test", "test1");}, Exception);
}

TEST(DBTest, EmptyInsertion)
{
  DBImpl *uut = dynamic_cast<DBImpl*>(
    createDiskDB(OPERATION_DB_PATH));

  try {
    uut->reset();
    uut->insert("Test", Value());
    auto value = uut->get("Test");
    ASSERT_EQ(0, value.length());
    uut->close();
  }
  catch (const Exception &e) {
    ASSERT_EQ("", e.toString());
    uut->close();
  }
}

TEST(DBTest, EmptyUpdate)
{
  DBImpl *uut = dynamic_cast<DBImpl*>(
    createDiskDB(OPERATION_DB_PATH));

  try {
    uut->open();
    uut->update("Test", Value());
    auto value = uut->get("Test");
    ASSERT_EQ(0, value.length());
    uut->close();
  }
  catch (const Exception &e) {
    ASSERT_EQ("", e.toString());
    ASSERT_EQ(true, false);
    uut->close();
  }
}

TEST(DBTest, Erasing)
{
  DBImpl *uut = dynamic_cast<DBImpl*>(
    createDiskDB(ERASE_DB_PATH));

  try {
    uut->reset();
    uut->insert("Test", "test");

    ASSERT_EQ("test", uut->get("Test"));

    uut->erase("Test");
  }
  catch (const Exception &e) {
    ASSERT_EQ("", e.toString());
    uut->close();
  };

  EXPECT_THROW({ uut->get("Test"); }, Exception);

  uut->close();
}
