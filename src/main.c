#include <stdio.h>
#include "libs/server/server.h"
#include "libs/argparse/argparse.h"


static const char *const usages[] = {
    "geons-core [options] [[--] args]",
    "geons-core [options]",
    NULL,
};

int main(int argc, const char *argv[]) {
    char server = -1;
    char client = -1;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Server options"),
        OPT_BOOLEAN('s', "server", &server, "starts geoNS server", NULL, 0, 0),
        OPT_GROUP("Client options"),
        OPT_BOOLEAN('c', "client", &client, "starts geoNS client", NULL, 0, 0),
        OPT_END(),
    };

    struct argparse argparse;
    argparse_init(&argparse, options, usages, 0);
    argparse_describe(&argparse, "\ngeoNS (Geolocational Net Stat) is a decentralized service that monitors internet quality.", "\nThe core service is responsible for handling decentralized operations, log collection and API provision.");
    argc = argparse_parse(&argparse, argc, argv);
    if (server != -1) {
        //* Running Server
        GeoNSServer *server = create_geons_server(argv[0]);
        sleep(10); //? MemCheck: killing the server after some seconds
        if (server != NULL)
            kill_geons_server(server);
    }
    else {
        // TODO:
        //* Running Client
    }

    return 0;
}