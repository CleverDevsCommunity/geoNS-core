#include <stdio.h>
#include "libs/db/db.h"

int main(int argc, char *argv[]) {
    printf("geoNS-Core is now running.\n");

    init_io_system(argv[0]);

    Database *ledger_db = db_open(LEDGER_DB);
    Database *local_db = db_open(LOCAL_DB);
    
    db_connect(ledger_db);
    db_connect(local_db);

    db_disconnect(ledger_db);
    db_disconnect(local_db);

    return 0;
}