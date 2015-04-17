#include "../include/keydb/db.h"
#include <iostream>
#include <ctime>

static const int TEST_COUNT = 100000;
static const int TEST_LENGTH = 16;

char RAND_STR[TEST_COUNT + TEST_LENGTH + 1];

char *
rand_str(char *buf, int len)
{
  static const char ALPHA[] = 
    "abcdefghijklmnopqrstuvwxyz"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "1234567890";

  for (int i = 0; i < len; ++i)
    buf[i] = ALPHA[std::rand() % (sizeof(ALPHA) - 1)];

  buf[len] = '\0';
  return buf;
}

int
main()
{
  rand_str(RAND_STR, TEST_COUNT + TEST_LENGTH);

  KeyDB::DB *db = KeyDB::createDiskDB("sample2.db");
  db->reset();

  auto start = std::clock();
  try {
    for (int i = 0; i < TEST_COUNT; ++i) {
      db->insert(KeyDB::Slice(RAND_STR + i, TEST_LENGTH), KeyDB::Slice(RAND_STR + i, TEST_LENGTH));
    }
  }
  catch(const KeyDB::Exception &e) {
    std::cout << e.toString() << std::endl;
  }
  auto duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  std::cout << "Random write: " << duration << "s (aka. " << TEST_COUNT / duration << "op/s)" << std::endl;

  start = std::clock();
  try {
    for (int i = 0; i < TEST_COUNT; ++i) {
      db->get(KeyDB::Slice(RAND_STR + i, TEST_LENGTH));
    }
  }
  catch(const KeyDB::Exception &e) {
    std::cout << e.toString() << std::endl;
  }
  duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  std::cout << "Random read: " << duration << "s (aka. " << TEST_COUNT / duration << "op/s)" << std::endl;

  start = std::clock();
  try {
    for (int i = 0; i < TEST_COUNT; ++i) {
      db->get(KeyDB::Slice(RAND_STR + std::rand() % TEST_COUNT, TEST_LENGTH));
    }
  }
  catch(const KeyDB::Exception &e) {
    std::cout << e.toString() << std::endl;
  }
  duration = (std::clock() - start) / (double) CLOCKS_PER_SEC;
  std::cout << "Random Random read: " << duration << "s (aka. " << TEST_COUNT / duration << "op/s)" << std::endl;

  return 0;
}
