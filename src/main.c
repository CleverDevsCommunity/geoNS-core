#include <stdio.h>
#include "libs/db/db.h"

int main(int argc, char *argv[]) {
    printf("geoNS-Core is now running.\n");

    init_io_system(argv[0]);

    sqlite3 *db;
    db_connect(db);
    db_disconnect(db);

    return 0;
}