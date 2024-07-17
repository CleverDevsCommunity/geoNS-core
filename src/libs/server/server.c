#include "server.h"


GeoNSServer *create_geons_server(uchar *exec_path) {
    init_io_system(exec_path);

    GeoNSServer *server = (GeoNSServer *) malloc(sizeof(GeoNSServer));

    server->ledger_db = db_open(LEDGER_DB);
    server->local_db = db_open(LOCAL_DB);
    server->node_gateway_server = open_server_socket(GEONS_SERVER_ADDR, NODE_GATEWAY_PORT);
    server->data_gateway_server = open_server_socket(GEONS_SERVER_ADDR, DATA_GATEWAY_PORT);

    // connecting databases
    db_connect(server->ledger_db);
    db_connect(server->local_db);

    // creating socket servers
    handle_server_socket(server->node_gateway_server);
    handle_server_socket(server->data_gateway_server);

    // TODO: setting up client API
    // .....

    printf("geoNS-Core is now running.\n");
    return server;
}


void kill_geons_server(GeoNSServer *server) {
    // disconnecting databases
    db_disconnect(server->ledger_db);
    db_disconnect(server->local_db);

    // killing socket servers
    kill_socket_server(server->node_gateway_server);
    kill_socket_server(server->data_gateway_server);

    free(server);
    server = NULL;
    sleep(3);
}