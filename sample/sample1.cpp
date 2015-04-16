#include <iostream>
#include <cmath>
#include "../include/keydb/db.h"

int
main()
{
  KeyDB::DB *db = KeyDB::createDiskDB("./sample1.db");

  db->reset();

  db->insert("Clarkok", "Hello world");
  std::cout << db->get("Clarkok") << std::endl;

  return 0;
}
