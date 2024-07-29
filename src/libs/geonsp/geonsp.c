#include "geonsp.h"


JSON_Value *construct_base_geonsp_message(uchar *geonsp_message) {
    JSON_Value *json_value = json_value_init_object();
    JSON_Object *json_object = json_value_get_object(json_value);
    json_object_set_string(json_object, "method", geonsp_message);
    return json_value;
}

JSON_Value *construct_add_node_request(uchar *server_addr, ushort node_gateway_port, ushort data_gateway_port) {
    JSON_Value *json_value = construct_base_geonsp_message(GEONSP_MSG_ADD_NODE);
    JSON_Object *json_object = json_value_get_object(json_value);
    json_object_dotset_string(json_object, "data.server_addr", server_addr);
    json_object_dotset_number(json_object, "data.node_gateway_port", node_gateway_port);
    json_object_dotset_number(json_object, "data.data_gateway_port", data_gateway_port);
    return json_value;
}


JSON_Value *construct_get_nodes_request(void) {
    return construct_base_geonsp_message(GEONSP_MSG_GET_NODES);
}


JSON_Value *construct_client_hello_request(void) {
    return construct_base_geonsp_message(GEONSP_MSG_CLIENT_HELLO);
}


SocketServer *is_geons_host_available(uchar *server_addr) {
    uchar command[100];
    
    JSON_Value *json_value = construct_client_hello_request();
    uchar *client_hello = json_serialize_to_string(json_value);
    snprintf(command, sizeof(command), "ping -c 1 %s > /dev/null 2>&1", server_addr);
    SocketServer *server = connect_to_socket_server(server_addr, NODE_GATEWAY_PORT);

    ushort is_host_available = !system(command);
    ushort is_geons_port_open = server != NULL;

    if (is_host_available && is_geons_port_open) {
        uchar buffer[SOCKET_MAX_BUFFER_SIZE] = {0};

        send_message(server->fd, client_hello, strlen(client_hello), 0);
        json_free_serialized_string(client_hello);
        json_value_free(json_value);
        
        int message_length = recv_message(server->fd, buffer, SOCKET_MAX_BUFFER_SIZE, 0);
        buffer[message_length] = '\0';
        JSON_Value *response_value = json_parse_string(buffer);
        JSON_Object *response_json = json_value_get_object(response_value);
        
        if (json_object_has_value(response_json, "status") && json_object_has_value(response_json, "message")) {
            uchar *status = (uchar *) json_object_get_string(response_json, "status");
            if (!strncmp(status, "success", strlen("success"))) {
                json_value_free(response_value);
                return server;
            }
            json_value_free(response_value);
            return NULL;
        }

        json_value_free(response_value);
        kill_socket_server(server);
        return NULL;
    }
    
    json_free_serialized_string(client_hello);
    json_value_free(json_value);
    kill_socket_server(server);
    return NULL;
}


void send_server_proto_response(int fd, uchar is_success, uchar *message) {
    JSON_Value *response_root = json_value_init_object();
    JSON_Object *response_json = json_value_get_object(response_root);
    json_object_set_string(response_json, "status", is_success ? "success" : "failed");
    json_object_set_string(response_json, "message", message);
    uchar *response = json_serialize_to_string(response_root);
    send_message(fd, response, strlen(response), 0);
    json_free_serialized_string(response);
    json_value_free(response_root);
}


void node_server_callback(int fd, uchar *request) {
    if (strlen(request) != 0) {
        JSON_Value *request_value = json_parse_string(request);
        JSON_Object *request_json = json_value_get_object(request_value);
        if (json_object_has_value(request_json, "method")) {
            uchar *method = (uchar *) json_object_get_string(request_json, "method");
            if (!strncmp(method, GEONSP_MSG_GET_VERSION, strlen(GEONSP_MSG_GET_VERSION))) {
                uchar version[128];
                snprintf(version, sizeof(version), "GeoNS v%s %s", GEONS_VERSION, COMPILE_TIME);
                send_server_proto_response(fd, 1, version);
            }
            else if (!strncmp(method, GEONSP_MSG_GET_NODES, strlen(GEONSP_MSG_GET_NODES))) {
                Database *local_db = db_open(LOCAL_DB);
                db_connect(local_db);
                db_disconnect(local_db);
            }
            else if (!strncmp(method, GEONSP_MSG_ADD_NODE, strlen(GEONSP_MSG_ADD_NODE))) {
                uchar *server_addr = (uchar *) json_object_dotget_string(request_json, "data.server_addr");
                ushort node_gateway_port = (ushort) json_object_dotget_number(request_json, "data.node_gateway_port");
                ushort data_gateway_port = (ushort) json_object_dotget_number(request_json, "data.data_gateway_port");
                Database *local_db = db_open(LOCAL_DB);
                db_connect(local_db);
                uchar insert_node = insert_new_node(local_db, server_addr, node_gateway_port, data_gateway_port);
                db_disconnect(local_db);

                if (!insert_node)
                    send_server_proto_response(fd, 0, "There was a db error inserting new node.");
                else if (insert_node == 2)
                    send_server_proto_response(fd, 0, "This node has been already added.");
                else
                    send_server_proto_response(fd, 1, "New node added into database.");
            }
            else
                send_server_proto_response(fd, 0, "GEONSP: Invalid protocol method.");
            
            json_value_free(request_value);
            return;
        }
        json_value_free(request_value);
        send_server_proto_response(fd, 0, "GEONSP: Invalid protocol message.");
        return;
    }
    send_server_proto_response(fd, 0, "GEONSP: Empty protocol message.");
}
