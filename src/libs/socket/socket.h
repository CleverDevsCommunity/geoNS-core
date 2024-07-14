#ifndef GEONS_SOCKET_H
#define GEONS_SOCKET_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "../core/core.h"

typedef struct sSocketConnection
{
    uchar buffer[SOCKET_MAX_BUFFER_SIZE];
    int *client;
    struct sSocketConnection *next;
} SocketConnection;


typedef struct sSocket
{
    int fd;
    struct sockaddr_in address;
    struct sSocketConnection *connections;
    int addrlen;
    ushort port;
    uchar *server_addr;
    uchar is_alive;
} Socket;


Socket *start_server_socket(uchar *server, ushort port);
void handle_server_socket(Socket *server);
void kill_socket(int fd);
void kill_server(Socket *server);
void *handle_client(void *arg);
Socket *connect_to_socket_server(uchar *server_addr, ushort port);
ssize_t send_message(int fd, uchar *message, size_t message_length, int flags);
ssize_t recv_message(int fd, void *buffer, size_t buffer_size, int flags);



#endif // !GEONS_SOCKET_H