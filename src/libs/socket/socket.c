#include "socket.h"


void *server_socket_thread(void *arg) {
    Socket *server = (Socket *) arg;
    thread thread_id;
    while (server->is_alive) {
        SocketConnection *connection = (SocketConnection *) malloc(sizeof(SocketConnection));
        connection->client = malloc(sizeof(int));
        connection->next = NULL;
        if (server->connections == NULL) {
            server->connections = connection;
        }
        else {
            SocketConnection *current = server->connections;
            while (current != NULL) {
                current = current->next;
            }
            current = connection;
        }
        if ((*connection->client = accept(server->fd, (struct sockaddr *)&server->address, (socklen_t*)&server->addrlen)) < 0) {
            perror("accept failed");
            free(connection->client);
            continue;
        }
        
        printf("New connection accepted\n");

        if (pthread_create(&thread_id, NULL, handle_client, (void *)connection) != 0) {
            perror("pthread_create failed");
            kill_socket(*connection->client);
            free(connection->client);
        } else {
            pthread_detach(thread_id);
        }
    }

    server->is_alive = 0;
    kill_server(server);
    return NULL;
}


void handle_server_socket(Socket *server) {
    thread server_thread;
    server->is_alive = 1;
    if (pthread_create(&server_thread, NULL, server_socket_thread, (void *)server) != 0) {
        perror("pthread_create failed");
        kill_server(server);
        exit(EXIT_FAILURE);
    } else
        pthread_detach(server_thread); // Detach the thread so that resources are freed when it terminates
}


Socket *start_server_socket(uchar *server_addr, ushort port) {
    Socket *server = (Socket *) malloc(sizeof(Socket));
    server->addrlen = sizeof(server->address);
    server->server_addr = server_addr;
    server->port = port;
    server->is_alive = 0;
    
    thread thread_id;
    
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


void kill_server(Socket *server) {
    SocketConnection *current = server->connections;
    while (current != NULL) {
        close(*current->client);
        free(current->client);
        server->connections = current->next;
        free(current);
        current = server->connections;
    }
    kill_socket(server->fd);
    free(server);
}


void kill_socket(int fd) {
    close(fd);
}


void *handle_client(void *arg) {
    SocketConnection connection = *(SocketConnection *) arg;
    int client_socket = *connection.client;
    free(arg);

    char *buffer = connection.buffer;
    int valread;

    while ((valread = read(client_socket, buffer, SOCKET_MAX_BUFFER_SIZE)) > 0) {
        buffer[valread] = '\0';
        printf("Received: %s\n", buffer);
        send(client_socket, buffer, valread, 0); // Echo back the received message
    }

    printf("Client disconnected\n");
    close(client_socket);
    return NULL;
}


Socket *connect_to_socket_server(uchar *server_addr, ushort port) {
    Socket *client = (Socket *) malloc(sizeof(Socket));
    client->port = port;
    client->server_addr = server_addr;
    int sock = 0;
    struct sockaddr_in serv_addr;
    // char buffer[BUFFER_SIZE] = {0};
    // char *hello = "Hello from client";

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Client: Socket creation error");
        exit(EXIT_FAILURE);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(client->port);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, client->server_addr, &serv_addr.sin_addr) <= 0) {
        perror("Client: Invalid address/ Address not supported");
        exit(EXIT_FAILURE);
    }

    // Connect to the server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Client: Connection failed");
        exit(EXIT_FAILURE);
    }

    // send(sock, hello, strlen(hello), 0);
    // printf("Hello message sent\n");

    // int valread = read(sock, buffer, BUFFER_SIZE);
    // printf("Received: %s\n", buffer);

    // kill_socket(sock);
    // free(client);
    return client;
}

ssize_t send_message(int fd, uchar *message, size_t message_length, int flags) {
    send(fd, message, message_length, flags);
}


ssize_t recv_message(int fd, void *buffer, size_t buffer_size, int flags) {
    recv(fd, buffer, buffer_size, flags);
}