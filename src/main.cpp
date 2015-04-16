#include <iostream>
#include <cstdio>
#include "db.h"
#include "db_impl.h"
#include "utils.h"
#include "drive/define_on_disk.h"

using namespace KeyDB;

int
main(int argc, char **argv)
{
  if (argc < 3) {
    std::cerr << "No database selected" << std::endl;
    return -1;
  }

  try {
    auto db = createDiskDB(argv[2]);
    db->open();

    auto sp = dynamic_cast<DBImpl*>(db)->superBlock();

    switch (argv[1][0]) {
      case 's':
        std::printf(
          "SuperBlock:\n"
          "  file_size: %lu\n"
          "  bitmap_start: %lu\n"
          "  bitmap_length: %lu\n"
          "  index_start: %lu\n"
          "  index_length: %lu\n"
          "  entry_count: %lu\n",
          sp->data.file_size,
          sp->data.bitmap_start,
          sp->data.bitmap_length,
          sp->data.index_start,
          sp->data.index_length,
          sp->data.entry_count
        );
        break;
      case 'g':
        std::printf(
          "Get %s: %s\n",
          argv[3],
          db->get(argv[3]).toString().c_str()
        );
        break;
      case 'h':
        std::printf(
          "Hash %s: %lu\n",
          argv[3],
          Utils::getDefaultHasher()->hash(argv[3])
        );
    };

    db->close();
  }
  catch (const Exception &e) {
    std::cerr << e.toString() << std::endl;
    return -1;
  }

  return 0;
}
