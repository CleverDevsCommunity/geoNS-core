#ifndef GEONS_PROTOCOL_H
#define GEONS_PROTOCOL_H 1

#include "../socket/socket.h"
#include "../parson/parson.h"
#include "../db/table.h"

#define GEONSP_MSG_GET_VERSION                      "GET_VERSION"
#define GEONSP_MSG_ADD_NODE                         "ADD_NODE"
#define GEONSP_MSG_GET_NODES                        "GET_NODES"
#define GEONSP_MSG_CLIENT_HELLO                     GEONSP_MSG_GET_VERSION


JSON_Value *construct_base_geonsp_message(uchar *geonsp_message);
JSON_Value *construct_add_node_request(uchar *server_addr, ushort node_gateway_port, ushort data_gateway_port);
JSON_Value *construct_get_nodes_request(void);
JSON_Value *construct_client_hello_request(void);
SocketServer *is_geons_host_available(uchar *server_addr);
void send_server_proto_response(int fd, uchar is_success, uchar *message);
void node_server_callback(int fd, uchar *request);

#endif // !GEONS_PROTOCOL_H