#include "table.h"

Table GEONS_LEDGER_DB_TABLES[2] = {
    {"logs", "(\
        id INTEGER PRIMARY KEY AUTOINCREMENT,\
        region TEXT,\
        location TEXT,\
        service TEXT,\
        delay INTEGER,\
        status TEXT CHECK (status IN ('success', 'failed')),\
        device TEXT CHECK (LENGTH(device) <= 128),\
        isp TEXT CHECK (LENGTH(isp) <= 64),\
        internet_type TEXT CHECK (internet_type IN ('phone', 'modem')),\
        timestamp INTEGER\
    )"},
    {"services", "(\
        id INTEGER PRIMARY KEY AUTOINCREMENT,\
        service_name TEXT UNIQUE,\
        service_url TEXT,\
        is_local NUMBER\
    )"}
};

Table GEONS_LOCAL_DB_TABLES[1] = {
    {"settings", "(\
        id INTEGER PRIMARY KEY AUTOINCREMENT,\
        setting_key TEXT UNIQUE,\
        setting_value TEXT\
    )"},
};

Service GEONS_DEFAULT_SERVICES[9] = {
    {"Github", "https://github.com/", 0},
    {"Google", "https://www.google.com/", 0},
    {"Gmail", "https://www.google.com/gmail/about/", 0},
    {"Android Developer", "https://developer.android.com/", 0},
    {"Wikipedia", "https://www.wikipedia.org/", 0},
    {"PlayStation", "https://www.playstation.com/en-ie/", 0},
    {"Aparat", "https://www.aparat.com/", 1},
    {"Filimo", "https://www.filimo.com/", 1},
    {"Digikala", "https://www.digikala.com/", 1}
};


uchar is_geons_configured(Database *db) {
    if (db->is_ledger)
        return 1;

    uchar *sql_query = "SELECT * FROM settings WHERE setting_key = 'geons_config_status';";
    sqlite3_stmt *stmt;
    
    int rc = sqlite3_prepare_v2(db->sqlite_db, sql_query, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Failed to prepare statement: %s\n", sqlite3_errmsg(db->sqlite_db));
        sqlite3_finalize(stmt);
        db_disconnect(db);
    }
    
    if ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        const uchar *value = sqlite3_column_text(stmt, 2);
        sqlite3_finalize(stmt);
        return !strncmp(value, "true", strlen("true") + 1);
    }
    
    sqlite3_finalize(stmt);
    return 0;
}


void set_geons_settings_config_status(Database *db, uchar is_configured) {
    if (db->is_ledger)
        return;
        
    uchar *settings_table = "settings";
    uchar sql_query[MAX_SQL_QUERY_SIZE];
    snprintf(sql_query, MAX_SQL_QUERY_SIZE, "INSERT INTO %s (setting_key, setting_value) VALUES ('geons_config_status', '%s');",
    settings_table, is_configured == 1 ? "true" : "false");
    db_exec(db, sql_query);
}


void insert_default_values(Database *db) {
    if (db->is_ledger) {
        uchar size_of_default_services = sizeof(GEONS_DEFAULT_SERVICES) / sizeof(GEONS_DEFAULT_SERVICES[0]);
        uchar sql_query[MAX_SQL_QUERY_SIZE];

        uchar *services_table = "services";

        for (uchar i = 0; i < size_of_default_services; i++) {
            snprintf(sql_query, MAX_SQL_QUERY_SIZE, 
                "INSERT INTO %s (service_name, service_url, is_local) VALUES ('%s', '%s', %d);", services_table, 
                GEONS_DEFAULT_SERVICES[i].name, GEONS_DEFAULT_SERVICES[i].url, GEONS_DEFAULT_SERVICES[i].is_local == 1
            );

            db_exec(db, sql_query);
        }

    }
    
    if (!db->is_ledger)
        set_geons_settings_config_status(db, 1);
}


void create_default_tables(Database *db) {
    uchar size_of_tables;
    if (db->is_ledger)
        size_of_tables = sizeof(GEONS_LEDGER_DB_TABLES) / sizeof(GEONS_LEDGER_DB_TABLES[0]);
    else
        size_of_tables = sizeof(GEONS_LOCAL_DB_TABLES) / sizeof(GEONS_LOCAL_DB_TABLES[0]);

    uchar sql_query[MAX_SQL_QUERY_SIZE];
    

    for (uchar i = 0; i < size_of_tables; i++) {
        snprintf(sql_query, MAX_SQL_QUERY_SIZE, 
            "CREATE TABLE IF NOT EXISTS %s %s;", 
            db->is_ledger ? GEONS_LEDGER_DB_TABLES[i].name : GEONS_LOCAL_DB_TABLES[i].name,
            db->is_ledger ? GEONS_LEDGER_DB_TABLES[i].schema : GEONS_LOCAL_DB_TABLES[i].schema
        );
        db_exec(db, sql_query);
    }

    if (!is_geons_configured(db))
        insert_default_values(db);
}