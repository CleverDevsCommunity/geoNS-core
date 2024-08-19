#ifndef GEONS_HTTP_H
#define GEONS_HTTP_H 1

#include <string.h>
#include "../core/core.h"
#include "../socket/socket.h"

#define HTTP_VERSION "HTTP/1.1"

typedef struct sHTTPHeader
{
    uchar *name;
    uchar *value;
} HTTPHeader;

typedef struct sHTTPRequest
{
    ushort size_of_headers;
    HTTPHeader headers[MAX_HTTP_HEADERS];
    uchar uri[MAX_HTTP_URI_LENGTH];
    uchar method[MAX_HTTP_METHOD_LENGTH];
    uchar version[MAX_HTTP_VERSION_LENGTH];
    char *body;
    int fd;
} HTTPRequest;

typedef struct sHTTPResponse
{
    HTTPHeader headers[MAX_HTTP_HEADERS];
} HTTPResponse;

typedef struct sHTTPServer
{
    uchar *public_dir;
    SocketServer *socket_server;
} HTTPServer;


typedef void (HTTPCallback)(void *args, ...);

extern HTTPServer *HTTP_SERVER;


void send_http_response(HTTPRequest *request, uchar *response, ssize_t size_of_response);
void kill_http_connection(HTTPRequest *request);
HTTPServer *create_http_server(uchar *server_addr, ushort port, uchar *public_dir);
void route(HTTPServer *server, uchar *route, HTTPCallback *callback);
void kill_http_server(HTTPServer *server);
void http_server_callback(int fd, uchar *request, PeerInfo *peer_info);

#endif // !GEONS_HTTP_H