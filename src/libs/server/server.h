#ifndef GEONS_SERVER_H
#define GEONS_SERVER_H 1

#include "../core/core.h"
#include "../db/db.h"
#include "../socket/socket.h"
#include "../parson/parson.h"

typedef struct sGeoNSServer
{
    Database *ledger_db;
    Database *local_db;
    Socket *node_gateway_server;
    Socket *data_gateway_server;
} GeoNSServer;

GeoNSServer *create_geons_server(uchar *exec_path);
void kill_geons_server(GeoNSServer *server);

#endif // !GEONS_SERVER_H