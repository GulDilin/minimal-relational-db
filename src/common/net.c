#include "net.h"

int socket_open(int *connect_socket) {
    printf(MESSAGE_SOCKET_OPEN);
    *connect_socket = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (connect_socket < 0) {
        printf(ERROR_MESSAGE_SOCKET_OPEN);
        perror(ERROR_MESSAGE_SOCKET_OPEN);
        PRINTLN;
        return ERROR_SOCKET_OPEN;
    }
    printf(MESSAGE_OK);
    PRINTLN;
    return 0;
}

int socket_connect (const int *client_socket, struct sockaddr_in *server_address) {
    printf(MESSAGE_SOCKET_CONNECT);
    if (connect(*client_socket, (struct sockaddr *) server_address, sizeof(struct sockaddr_in)) == -1) {
        printf(ERROR_MESSAGE_SOCKET_CONNECT);
        perror(ERROR_MESSAGE_SOCKET_CONNECT);
        PRINTLN;
        return ERROR_CONNECT;
    }
    printf(MESSAGE_OK);
    PRINTLN;
    return 0;
}


int socket_set_name(const int *connect_socket, int *reuse) {
    printf(MESSAGE_SOCKET_SET_NAME);
    if (setsockopt(*connect_socket, SOL_SOCKET, SO_REUSEADDR, (const char *) reuse, sizeof(int)) == -1) {
        printf(ERROR_MESSAGE_SOCKET_SET_NAME);
        perror(ERROR_MESSAGE_SOCKET_SET_NAME);
        PRINTLN;
        return ERROR_SOCKET_SET_NAME;
    }
    printf(MESSAGE_OK);
    PRINTLN;
    return 0;
}

int socket_bind(const int *connect_socket, struct sockaddr_in *server_address) {
    printf(MESSAGE_SOCKET_BIND);
    if (bind(*connect_socket, (struct sockaddr *) server_address, sizeof(struct sockaddr_in)) == -1) {
        printf(ERROR_MESSAGE_SOCKET_BIND);
        perror(ERROR_MESSAGE_SOCKET_BIND);
        PRINTLN;
        return ERROR_SOCKET_BIND;
    }
    printf(MESSAGE_OK);
    PRINTLN;
    return 0;
}

int socket_listen(const int *connect_socket, int max_clients_amount) {
    printf(MESSAGE_SOCKET_LISTEN);
    if (listen(*connect_socket, max_clients_amount) == -1) {
        printf(ERROR_MESSAGE_SOCKET_LISTEN);
        perror(ERROR_MESSAGE_SOCKET_LISTEN);
        PRINTLN;
        return ERROR_SOCKET_LISTEN;
    }
    printf(MESSAGE_OK);
    PRINTLN;
    return 0;
}

size_t init_connection(int *connect_socket, struct sockaddr_in *server_address, long port, int *reuse, int max_clients_amount) {
    int return_code = 0;

    return_code = socket_open(connect_socket);
    if (return_code != 0) return return_code;

    server_address->sin_family = AF_INET;
    server_address->sin_port = htons(port);
    server_address->sin_addr.s_addr = htonl(INADDR_ANY);

    return_code = socket_set_name(connect_socket, reuse);
    if (return_code != 0) return return_code;
    return_code = socket_bind(connect_socket, server_address);
    if (return_code != 0) return return_code;
    return_code = socket_listen(connect_socket, max_clients_amount);
    if (return_code != 0) return return_code;
    printf(MESSAGE_INIT_FINISH);
    PRINTLN;

    return 0;
}

bool validate_ip(char * address) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, address, &(sa.sin_addr)) != 0;
}

bool validate_port(int port) {
    return port < 65535 && port > 1;
}

static size_t read_buffer (int socket, unsigned max_length, uint8_t *out) {
    size_t cur_len = 0;
    size_t nread;
    while ((nread = read(socket, out+cur_len, 1)) != 0)
    {
        cur_len += nread;
        if (cur_len == max_length)
        {
            fprintf(stderr, "max message length exceeded\n");
            exit(1);
        }
    }
    return cur_len;
}
