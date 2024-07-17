#include <stdio.h>
#include "libs/socket/socket.h"

int main(int argc, char *argv[]) {
    printf("geoNS-Core is now running.\n");

    Socket *server = open_server_socket("127.0.0.1", NODE_GATEWAY_PORT);
    printf("Server listening on %s:%d\n", server->server_addr, server->port);
    handle_server_socket(server);
    sleep(10); // Let server run for a bit
    server->is_alive = 0;

    kill_server(server);

    return 0;
}
