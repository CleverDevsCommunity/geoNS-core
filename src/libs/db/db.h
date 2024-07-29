#ifndef GEONS_DB_H
#define GEONS_DB_H 1

#include <stdio.h>
#include <sqlite3.h>
#include "../core/core.h"
#include "../io/io.h"

typedef struct sDatabase
{
    sqlite3 *sqlite_db;
    uchar is_ledger;
    uchar *db_name;
} Database;


Database *db_open(uchar *db_name);
void db_fetch(Database *db, uchar *sql_query, uchar **output);
uchar db_exec(Database *db, uchar *sql_query);
void db_init(Database *db);
void db_connect(Database *db);
void db_disconnect(Database *db);


#endif // !GEONS_DB_H