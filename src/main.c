#include <stdio.h>
#include "libs/server/server.h"
#include "libs/argparse/argparse.h"
#include "libs/logger/logger.h"
#include "libs/config/config.h"


static const char *const usages[] = {
    "geons-core [options] [[--] args]",
    "geons-core [options]",
    NULL,
};

int main(int argc, const char *argv[]) {
    init_io_system(argv[0]);
    init_logger();
    init_config_manager();

    char server = -1;
    char client = -1;
    char debug = -1;
    struct argparse_option options[] = {
        OPT_HELP(),
        OPT_GROUP("Core options"),
        OPT_BOOLEAN('d', "debug", &debug, "enables debugging mode", NULL, 0, 0),
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
    if (debug != -1)
        is_debugging = 1;

    msglog(DEBUG, "geoNS-core started.");

    if (server != -1) {
        //* Running Server
        GeoNSServer *server = create_geons_server();
        sleep(10); //? MemCheck: killing the server after some seconds
        kill_geons_server(server);
    }
    else {
        // TODO:
        //* Running Client
    }
    release_config();

    msglog(DEBUG, "geoNS-core finished.");
    return 0;
}