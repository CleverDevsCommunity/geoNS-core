#include "socket.h"
#include "../geonsp/geonsp.h"
#include "../logger/logger.h"


uchar is_my_ip(const uchar *ip) {
    uchar *all_interfaces = "0.0.0.0";
    if (!strncmp(ip, all_interfaces, MAX_IPV4_LENGTH))
        return 1;

    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char addr[INET_ADDRSTRLEN];

    getifaddrs(&ifap);
    for (ifa = ifap; ifa != NULL; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            inet_ntop(AF_INET, &sa->sin_addr, addr, sizeof(addr));
            if (strcmp(ip, addr) == 0) {
                freeifaddrs(ifap);
                return 1; // IP address belongs to the system
            }
        }
    }
    freeifaddrs(ifap);
    return 0; // IP address does not belong to the system
}


void add_connection(SocketConnection **head, SocketConnection *connection) {
    connection->next = NULL;
    if (*head == NULL) {
        *head = connection;
    }
    else {
        SocketConnection *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = connection;
    }
}


uchar remove_connection(SocketConnection **head, SocketConnection *connection) {
    if (*head != NULL) {
        SocketConnection *current = *head;
        SocketConnection *previous = NULL;

        while (current != connection) {
            previous = current;
            current = current->next;
            if (current == NULL)
                return 0;
        }

        if (current->connection_status == CONNECTION_ESTABLISHED) {
            kill_socket(current->fd);
        }

        if (current->next != NULL) {
            if (previous == NULL)
                *head = current->next;
            else
                previous->next = current->next;
        
            free(current);
            current = NULL;
        }
        else {
            if (previous == NULL) {
                free(*head);
                *head = NULL;
            }
            else {
                free(current);
                current = NULL;
                previous->next = NULL;
            }
        }
        return 1;
    }
    return 0;
}


void *server_socket_thread(void *arg) {
    SocketServer *server = (SocketServer *) arg;
    thread thread_id;

    while (server->is_alive) {
        SocketConnection *connection = (SocketConnection *) malloc(sizeof(SocketConnection));
        connection->connection_status = NOT_CONNECTED;
        add_connection(&server->connections, connection);

        if ((connection->fd = accept(server->fd, (struct sockaddr *)&server->address, (socklen_t*)&server->addrlen)) < 0) {
            perror("accept failed");
            remove_connection(&server->connections, connection);
            continue;
        }
        
        connection->connection_status = CONNECTION_ESTABLISHED;
        printf("New connection accepted\n");

        ClientData *client_data = (ClientData *) malloc(sizeof(ClientData));
        client_data->head = &server->connections;
        client_data->current = connection;
        client_data->callback = &node_server_callback;
        strncpy(client_data->server_addr, server->server_addr, strlen(server->server_addr));
        client_data->server_port = client_data->server_port;

        if (pthread_create(&thread_id, NULL, handle_client, (void *)&client_data) != 0) {
            perror("pthread_create failed");
            remove_connection(&server->connections, connection);
        } else {
            pthread_detach(thread_id);
        }
    }

    return NULL;
}


void handle_server_socket(SocketServer *server) {
    server->is_alive = 1;
    if (pthread_create(&server->thread, NULL, server_socket_thread, (void *)server) != 0) {
        perror("pthread_create failed");
        kill_socket_server(server);
        exit(EXIT_FAILURE);
    } else
        pthread_detach(server->thread);
}


SocketServer *open_server_socket(uchar *server_addr, ushort port) {
    SocketServer *server = (SocketServer *) malloc(sizeof(SocketServer));
    server->addrlen = sizeof(server->address);
    server->server_addr = server_addr;
    server->port = port;
    server->is_alive = 0;
    server->connections = NULL;
    
    if ((server->fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, server->server_addr, &server->address.sin_addr) <= 0) {
        perror("Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Bind the socket to the network address and port
    server->address.sin_family = AF_INET;
    server->address.sin_addr.s_addr = INADDR_ANY;
    server->address.sin_port = htons(server->port);

    if (bind(server->fd, (struct sockaddr *)&server->address, sizeof(server->address)) < 0) {
        perror("bind failed");
        kill_socket(server->fd);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server->fd, 3) < 0) {
        perror("listen failed");
        kill_socket(server->fd);
        exit(EXIT_FAILURE);
    }

    return server;
}


void kill_socket_server(SocketServer *server) {
    if (server != NULL) {
        msglog(DEBUG, "Killing socket server %s:%d", server->server_addr, server->port);
        SocketConnection *current = server->connections;
        while (current != NULL) {
            SocketConnection *previous = current;
            current = current->next;
            if (remove_connection(&server->connections, previous))
                msglog(DEBUG, "%s:%d removed a connection.", server->server_addr, server->port);
        }
        kill_socket(server->fd);
        free(server);
        server = NULL;
    }
}


void kill_socket(int fd) {
    close(fd);
}


void *handle_client(void *arg) {
    ClientData **client_data = (ClientData **) arg;
    SocketConnection **head = (*client_data)->head;
    SocketConnection *connection = (*client_data)->current;
    ServerCallback *callback = (*client_data)->callback;
    uchar *server_addr = (*client_data)->server_addr;
    ushort server_port = (*client_data)->server_port;
    free(*client_data);
    *client_data = NULL;

    int client_socket = connection->fd;
    char *buffer = connection->buffer;
    int message_length;

    while ((message_length = read(client_socket, buffer, SOCKET_MAX_BUFFER_SIZE)) > 0) {
        buffer[message_length] = '\0';
        callback(client_socket, buffer);
    }

    msglog(DEBUG, "Client disconnected from %s:%d", server_addr, server_port);
    printf("Client disconnected\n");
    remove_connection(head, connection);
    return NULL;
}


SocketServer *connect_to_socket_server(uchar *server_addr, ushort port) {
    SocketServer *server = (SocketServer *) malloc(sizeof(SocketServer));
    server->port = port;
    server->server_addr = server_addr;
    server->fd = 0;
    server->connections = NULL;
    struct sockaddr_in serv_addr;

    // Create socket
    if ((server->fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Client: Socket creation error");
        kill_socket_server(server);
        return NULL;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(server->port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, server->server_addr, &serv_addr.sin_addr) <= 0) {
        perror("Client: Invalid address/ Address not supported");
        kill_socket_server(server);
        return NULL;
    }

    // Connect to the server
    if (connect(server->fd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Client: Connection failed");
        kill_socket_server(server);
        return NULL;
    }

    return server;
}

ssize_t send_message(int fd, uchar *message, size_t message_length, int flags) {
    return send(fd, message, message_length, flags);
}


ssize_t recv_message(int fd, void *buffer, size_t buffer_size, int flags) {
    return recv(fd, buffer, buffer_size, flags);
}