#include "server.h"
#include "../geonsp/geonsp.h"
#include "../logger/logger.h"
#include "../config/config.h"


Node INIT_NODES[] = {
    {0, "10.3.0.30", DEFAULT_NODE_GATEWAY_PORT, DEFAULT_DATA_GATEWAY_PORT, "active"}
};


uchar connect_localdb_node_servers() {
    Database *db = db_open(LOCAL_DB);
    db_connect(db);
    Node *active_nodes[MAX_ACTIVE_NODES];
    char nodes = get_all_active_nodes(db, active_nodes, MAX_ACTIVE_NODES);
    msglog(DEBUG, "Connecting to localdb nodes.");
    if (nodes != -1) {
        Node source_node = {
            0,
            "",
            CONFIG->node_gateway_port,
            CONFIG->data_gateway_port,
            "active"
        };
        strncpy(source_node.server_addr, CONFIG->geons_server_addr, sizeof(source_node.server_addr));

        for (uchar i = 0; i < nodes; i++) {
            Node *destination_node = active_nodes[i];
            handle_node_info_exchange(db, &source_node, destination_node, 1);
            free(destination_node);
        }
    
        msglog(DEBUG, "Communication with localdb nodes success.");
        db_disconnect(db);
        return 1;
    }
    else {
        msglog(WARNING, "No localdb node available. Skipping.");
        db_disconnect(db);
        return 0;
    }
}


uchar connect_init_node_servers() {
    uchar size_of_init_nodes = sizeof(INIT_NODES) / sizeof(INIT_NODES[0]);
    if (size_of_init_nodes > 0) {
        msglog(DEBUG, "Connecting to init nodes.");
        Database *db = db_open(LOCAL_DB);
        db_connect(db);
        Node source_node = {
            0,
            "",
            CONFIG->node_gateway_port,
            CONFIG->data_gateway_port,
            "active"
        };
        strncpy(source_node.server_addr, CONFIG->geons_server_addr, sizeof(source_node.server_addr));

        for (uchar i = 0; i < size_of_init_nodes; i++) {
            Node *destination_node = &INIT_NODES[i];
            handle_node_info_exchange(db, &source_node, destination_node, 0);
        }
        db_disconnect(db);
        msglog(DEBUG, "Communication with init nodes success.");
    }
    else
        msglog(WARNING, "No init node available. Skipping to localdb nodes.");
    return connect_localdb_node_servers();
}


GeoNSServer *create_geons_server() {
    msglog(DEBUG, "Creating geoNS server.");
    GeoNSServer *server = (GeoNSServer *) memalloc(sizeof(GeoNSServer));


    msglog(DEBUG, "Starting decentralization communication.");
    // initializing decentralization
    if (connect_init_node_servers()) {
        msglog(DEBUG, "Decentralization communication success.");

        server->ledger_db = db_open(LEDGER_DB);
        server->local_db = db_open(LOCAL_DB);

        // creating node socket server
        server->node_gateway_server = open_server_socket(CONFIG->geons_server_addr, CONFIG->node_gateway_port);
        if (server->node_gateway_server != NULL) {
            handle_server_socket(server->node_gateway_server, &node_server_callback);

            // creating data socket server
            // server->data_gateway_server = open_server_socket(CONFIG->geons_server_addr, CONFIG->data_gateway_port);
            // handle_server_socket(server->data_gateway_server);

            // TODO: setting up client API
            // .....

            // connecting databases
            db_connect(server->ledger_db);
            db_connect(server->local_db);

            printf("geoNS-Core is now running.\n");
            return server;   
        }

        msglog(ERROR, "Creating node server failed on %s:%d", CONFIG->geons_server_addr, CONFIG->node_gateway_port);
        printf("Failed to run server.\n\t- Reason: Wrong db type.\n");
        free(server->local_db);
        free(server->ledger_db);
        kill_geons_server(server);
        return NULL;
    }
    msglog(ERROR, "Decentralization communication failed.");
    printf("Failed to run server.\n\t- Reason: Wrong db type.\n");
    kill_geons_server(server);
    return NULL;
}


void kill_geons_server(GeoNSServer *server) {
    if (server != NULL) {
        msglog(DEBUG, "Killing GeoNSServer.");
        // disconnecting databases
        db_disconnect(server->ledger_db);
        db_disconnect(server->local_db);
        msglog(DEBUG, "Databases disconnected.");

        // killing socket servers
        kill_socket_server(server->node_gateway_server);
        // kill_socket_server(server->data_gateway_server);
        
        free(server);
        server = NULL;
        msglog(DEBUG, "GeoNSServer shut down.");
    }
}