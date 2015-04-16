#include "../include/keydb/db.h"
#include <iostream>

static const int TEST_COUNT = 10000;

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
  char buffer[17];

  KeyDB::DB *db = KeyDB::createDiskDB("sample2.db");
  db->reset();

  try{
    for (int i = 0; i < TEST_COUNT; ++i) {
      rand_str(buffer, 16);
      db->insert(buffer, buffer);
    }
  }
  catch (const KeyDB::Exception &e) {
    std::cerr << e.toString() << std::endl;
    db->close();
  }

  return 0;
}
