# KeyDB

A simple key-value db

# Features

* You can store arbitrary byte arrays as keys and values
* Basic operations are `get(key)`, `insert(key, value)`, `update(key, value)`, `erase(key)`

# Limitations

* Only a single process can access a particular database at a time

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
`Google Test` required

```
make test
```

# Performance

Build and run sample2 to test

```
Environment:
  Core i7, 4GB Memory, SSD
  Archlinux inside VirtualBox

Time to complete 10000 ops each:
  Random write: 0.700915s (aka. 142671op/s)
  Random read: 0.081345s (aka. 1.22933e+06op/s)
  Random Random read: 0.147683s (aka. 677126op/s)
```

# Current

Adding database level cache
