#ifndef GEONS_SQL_TABLE_H
#define GEONS_SQL_TABLE_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "db.h"
#include "../core/constants.h"
#include "../core/types.h"

typedef struct sTable
{
    uchar *name;
    uchar *schema;
} Table;

typedef struct sService 
{
    uchar *name;
    uchar *url;
    uchar is_local;
} Service;

extern Table GEONS_DB_TABLES[];
extern Service GEONS_DEFAULT_SERVICES[];

uchar is_geons_configured(sqlite3 *db);
void insert_default_values(sqlite3 *db);
void set_geons_settings_config_status(sqlite3 *db, uchar is_configured);
void create_default_tables(sqlite3 *db);

#endif // !GEONS_SQL_TABLE_H