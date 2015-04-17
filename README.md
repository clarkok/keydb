# KeyDB

A simple key-value db

# Usage

```C++
// sample/sample1.cpp

#include <iostream>
#include <cmath>
#include "../include/keydb/db.h"

int
main()
{
  KeyDB::DB *db = KeyDB::createDiskDB("./sample1.db");

  db->reset(); // db->open();

  db->insert("Clarkok", "Hello world");
  std::cout << db->get("Clarkok") << std::endl;

  db->close();

  return 0;
}
```

# Build

To build the library it self:

```
make lib
```

To build debug tools:

```
make util
```

To build and run tests:
Google test required

```
make test
```

# Current

Adding database level cache
