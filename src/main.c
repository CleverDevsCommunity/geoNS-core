#include <stdio.h>
#include "libs/server/server.h"

int main(int argc, char *argv[]) {
    GeoNSServer *server = create_geons_server(argv[0]);
    sleep(2); //? MemCheck: killing the server after some seconds
    kill_geons_server(server);

    return 0;
}