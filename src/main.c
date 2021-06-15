#include <errno.h>
#include <string.h>
#include "./client/client.h"
#include "./server/server.h"

#define CLIENT "--mode=client"
#define SERVER "--mode=server"

#define MODE_CLIENT 0
#define MODE_SERVER 1
#define MODE_UNKNOWN -1

#define MESSAGE_MODE_UNKNOWN "Use command with the first mode required argument '--mode=client' or '--mode=server'. In client mode next args is server address and server port. In server mode next argument is port."
#define ERROR_MESSAGE_MODE_CLIENT_ARGS "In client mode there is two required arguments. Server ip address in format X.X.X.X and server port number"
#define ERROR_MESSAGE_MODE_SERVER_ARGS "In server mode there is one required argument - server port number"
#define ERROR_MESSAGE_PORT "Port is incorrect. It need to be between 1 and 65535"
#define ERROR_MESSAGE_IP "Ip is incorrect. It need to be in format X.X.X.X and be available."

int get_app_mode(int argc, char **argv) {
    if (argc < 2) return MODE_UNKNOWN;
    if (strcmp(argv[1], CLIENT) == 0) return MODE_CLIENT;
    if (strcmp(argv[1], SERVER) == 0) return MODE_SERVER;
    return MODE_UNKNOWN;
}

long get_port(int mode, char ** argv) {
    printf("Get port from command");
    PRINTLN;
    return strtol(mode == MODE_SERVER ? argv[2] : argv[3], NULL, 10);
}

int validate_args_amount(int mode, int argc) {
    printf("Validate args amount");
    PRINTLN;

    if (mode == MODE_CLIENT && argc != 4) {
        fprintf(stderr, ERROR_MESSAGE_MODE_CLIENT_ARGS);
        return EAGAIN;
    }
    if (mode == MODE_SERVER && argc != 3) {
        fprintf(stderr, ERROR_MESSAGE_MODE_SERVER_ARGS);
        return EAGAIN;
    }
    return NORMAL_END;
}

int validate_args(int mode, char ** argv, long port) {
    if (!validate_port(port)) {
        fprintf(stderr, ERROR_MESSAGE_PORT);
        return EAGAIN;
    }
    if (mode == MODE_CLIENT && !validate_ip(argv[2])) {
        fprintf(stderr, ERROR_MESSAGE_IP);
        return EAGAIN;
    }
    return NORMAL_END;
}

int main(int argc, char **argv) {
    int mode = get_app_mode(argc, argv);

    if (mode == MODE_UNKNOWN) {
        fprintf(stderr, MESSAGE_MODE_UNKNOWN);
        return EAGAIN;
    }

    int return_code = validate_args_amount(mode, argc);
    if (return_code != NORMAL_END) return return_code;
    long port = get_port(mode, argv);
    return_code = validate_args(mode, argv, port);
    if (return_code != NORMAL_END) return return_code;


    if (mode == MODE_SERVER) return run_server(port);
    if (mode == MODE_CLIENT) return run_client(argv[2], port);

    return EFAULT;
}