#include "../include/keydb/db.h"
#include <iostream>
#include <ctime>

static const int TEST_COUNT = 10000;
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
  std::cout << "Random write: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << std::endl;

  start = std::clock();
  try {
    for (int i = 0; i < TEST_COUNT; ++i) {
      db->get(KeyDB::Slice(RAND_STR + i, TEST_LENGTH));
    }
  }
  catch(const KeyDB::Exception &e) {
    std::cout << e.toString() << std::endl;
  }
  std::cout << "Random read: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << std::endl;

  start = std::clock();
  try {
    for (int i = 0; i < TEST_COUNT; ++i) {
      db->get(KeyDB::Slice(RAND_STR + std::rand() % TEST_COUNT, TEST_LENGTH));
    }
  }
  catch(const KeyDB::Exception &e) {
    std::cout << e.toString() << std::endl;
  }
  std::cout << "Rnadom Random read: " << (std::clock() - start) / (double) CLOCKS_PER_SEC << std::endl;

  return 0;
}
