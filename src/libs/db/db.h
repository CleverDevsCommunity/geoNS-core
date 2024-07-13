#ifndef GEONS_DB_H
#define GEONS_DB_H 1

#include <stdio.h>
#include <sqlite3.h>
#include "table.h"
#include "../core/core.h"
#include "../io/io.h"

void db_select(sqlite3 *db, uchar *sql_query, uchar **output);
void db_exec(sqlite3 *db, uchar *sql_query);
void db_init(sqlite3 *db);
void db_connect(sqlite3 *db);
void db_disconnect(sqlite3 *db);


#endif // !GEONS_DB_H