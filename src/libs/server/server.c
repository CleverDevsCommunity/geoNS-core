#include "server.h"
#include "../geonsp/geonsp.h"


Node INIT_NODES[] = {
    {0, "192.168.124.16", NODE_GATEWAY_PORT, DATA_GATEWAY_PORT, "active"}
};


uchar connect_localdb_node_servers() {
    Database *db = db_open(LOCAL_DB);
    db_connect(db);
    Node *active_nodes[MAX_ACTIVE_NODES];
    char nodes = get_all_active_nodes(db, active_nodes, MAX_ACTIVE_NODES);
    if (nodes != -1) {
        Node source_node = {
            0,
            GEONS_SERVER_ADDR,
            NODE_GATEWAY_PORT,
            DATA_GATEWAY_PORT,
            "active"
        };

        for (uchar i = 0; i < nodes; i++) {
            Node *destination_node = active_nodes[i];
            handle_node_info_exchange(db, &source_node, destination_node, 1);
            free(destination_node);
        }

        db_disconnect(db);
        return 1;
    }
    else {
        db_disconnect(db);
        return 0;
    }
}


uchar connect_init_node_servers() {
    printf("Connecting to init servers...\n"); //! TEMP: should be removed after debugging.
    uchar size_of_init_nodes = sizeof(INIT_NODES) / sizeof(INIT_NODES[0]);
    if (size_of_init_nodes > 0) {
        Database *db = db_open(LOCAL_DB);
        db_connect(db);
        Node source_node = {
            0,
            GEONS_SERVER_ADDR,
            NODE_GATEWAY_PORT,
            DATA_GATEWAY_PORT,
            "active"
        };
        for (uchar i = 0; i < size_of_init_nodes; i++) {
            Node *destination_node = &INIT_NODES[i];
            handle_node_info_exchange(db, &source_node, destination_node, 0);
        }
        db_disconnect(db);
    }
    return connect_localdb_node_servers();
}


GeoNSServer *create_geons_server(const char *exec_path) {
    init_io_system(exec_path);

    GeoNSServer *server = (GeoNSServer *) malloc(sizeof(GeoNSServer));

    server->ledger_db = db_open(LEDGER_DB);
    server->local_db = db_open(LOCAL_DB);

    // creating node socket server
    server->node_gateway_server = open_server_socket(GEONS_SERVER_ADDR, NODE_GATEWAY_PORT);
    handle_server_socket(server->node_gateway_server);

    // creating data socket server
    // server->data_gateway_server = open_server_socket(GEONS_SERVER_ADDR, DATA_GATEWAY_PORT);
    // handle_server_socket(server->data_gateway_server);

    // TODO: setting up client API
    // .....

    // initializing decentralization
    if (connect_init_node_servers()) {
        // connecting databases
        db_connect(server->ledger_db);
        db_connect(server->local_db);

        printf("geoNS-Core is now running.\n");
        return server;
    }
    printf("Failed to run server.\n\t- Reason: Wrong db type.\n");
    kill_geons_server(server);
    return NULL;
}


void kill_geons_server(GeoNSServer *server) {
    if (server != NULL) {
        // disconnecting databases
        db_disconnect(server->ledger_db);
        db_disconnect(server->local_db);

        // killing socket servers
        kill_socket_server(server->node_gateway_server);
        // kill_socket_server(server->data_gateway_server);

        free(server);
        server = NULL;
    }
}