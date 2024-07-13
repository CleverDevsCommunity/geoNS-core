#include "db.h"

void db_select(sqlite3 *db, uchar *sql_query, uchar **output) {

}


void db_exec(sqlite3 *db, uchar *sql_query) {
    uchar sql_error = sqlite3_exec(db, sql_query, 0, 0, 0);
    if (sql_error != SQLITE_OK) {
        fprintf(stderr, "SQL error: %s\nquery: %s\n", sqlite3_errmsg(db), sql_query);
        exit(sql_error);
    }
}


void db_init(sqlite3 *db) {
    create_default_tables(db);
}


void db_connect(sqlite3 *db) {
    uchar db_path[SYS_MAX_PATH_LENGTH];
    get_cwd_path(db_path, sizeof(db_path));
    strncat(db_path, DATABASE_NAME, sizeof(db_path) - 1);

    int connection_error = sqlite3_open(db_path, &db);
    if (connection_error) {
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        exit(connection_error);
    }

    db_init(db);
}


void db_disconnect(sqlite3 *db) {
    if (db != NULL)
        sqlite3_close(db);
}