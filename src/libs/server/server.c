#include "server.h"


Node INIT_NODES[] = {
    {"192.168.124.16", NODE_GATEWAY_PORT, DATA_GATEWAY_PORT}
};


uchar init_node_servers(Database *db) {
    if (!db->is_ledger) {
        uchar size_of_init_nodes = sizeof(INIT_NODES) / sizeof(INIT_NODES[0]);
        for (uchar i = 0; i < size_of_init_nodes; i++) {
            Node init_node = INIT_NODES[i];
            if (is_my_ip(init_node.server_addr))
                continue;
            else {
                SocketServer *server = is_geons_host_available(init_node.server_addr);
                if (server != NULL) {
                    uchar buffer[SOCKET_MAX_BUFFER_SIZE];
                    JSON_Value *add_node_request = construct_add_node_request(
                        GEONS_SERVER_ADDR,
                        NODE_GATEWAY_PORT,
                        DATA_GATEWAY_PORT
                    );
                    uchar *request_payload = json_serialize_to_string(add_node_request);

                    send_message(server->fd, request_payload, strlen(request_payload), 0);
                    int message_length = recv_message(server->fd, buffer, sizeof(buffer), 0);
                    buffer[message_length] = '\0';
                    json_free_serialized_string(request_payload);
                    json_value_free(add_node_request);

                    // TODO: Get list of other nodes from server using GET_NODES request
                    // .....

                    kill_socket_server(server);
                }
                else
                    continue;
            }
        }
        return 1;
    }
    return 0;
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
    if (init_node_servers(server->local_db)) {
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
    // disconnecting databases
    db_disconnect(server->ledger_db);
    db_disconnect(server->local_db);

    // killing socket servers
    kill_socket_server(server->node_gateway_server);
    // kill_socket_server(server->data_gateway_server);

    free(server);
    server = NULL;
}