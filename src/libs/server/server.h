#ifndef GEONS_SERVER_H
#define GEONS_SERVER_H 1

#include "../core/core.h"
#include "../socket/socket.h"
#include "../parson/parson.h"
#include "../db/db.h"
#include "../geonsp/geonsp.h"

typedef struct sGeoNSServer
{
    Database *ledger_db;
    Database *local_db;
    SocketServer *node_gateway_server;
    SocketServer *data_gateway_server;
} GeoNSServer;

typedef struct sNode
{
    uchar *server_addr;
    ushort node_gateway;
    ushort data_gateway;
} Node;


extern Node INIT_NODES[];


uchar init_node_servers(Database *db);
GeoNSServer *create_geons_server(const char *exec_path);
void kill_geons_server(GeoNSServer *server);

#endif // !GEONS_SERVER_H