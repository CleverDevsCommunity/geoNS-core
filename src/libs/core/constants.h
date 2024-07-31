#ifndef GEONS_CONSTANTS_H
#define GEONS_CONSTANTS_H 1

// Database constants
#define LEDGER_DB                               "ledger.db"
#define LOCAL_DB                                "local.db"
#define MAX_SQL_QUERY_SIZE                      1024

// System constants
#define SYS_MAX_PATH_LENGTH                     1024

// Socket constants
#define GEONS_SERVER_ADDR                       "192.168.1.164"
#define NODE_GATEWAY_PORT                       9060
#define DATA_GATEWAY_PORT                       6090
#define SOCKET_MAX_BUFFER_SIZE                  4096
#define MAX_IPV4_LENGTH                         15
#define MAX_SOCKET_METHOD_NAME                  64

// Decentralization constants
#define MAX_ACTIVE_NODES                        16

// Logger constants
#define MAX_LOG_LENGTH                          1024
#define MAX_LOG_FILE_NAME_LENGTH                32
#define MAX_LOG_PRE_TEXT_LENGTH                 32
#define MAX_LOG_TIME_LENGTH                     20

#endif // !GEONS_CONSTANTS_H