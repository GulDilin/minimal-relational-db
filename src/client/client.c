#include "client.h"
#include <stdint.h>

void print_response_info(Common__Response *response) {
    switch(response->command_code){
        case COMMAND_CODE_CREATE:
            printf("COMMAND CREATE status<%d> : %s", response->status_code, response->text);
            break;
        case COMMAND_CODE_INSERT:
            printf("COMMAND INSERT status<%d> : %s", response->status_code, response->text);
            break;
    }
}

void print_column(Common__ColumnValue * column) {
    if (column->is_start) printf("\n");
    printf("%s:\t%s", column->title, column->value);
}

void parse_response(Common__Response *response) {
    print_response_info(response);
    if (response->status_code != NORMAL_END) return;

    if(response->n_columns < 1) return;
    for (int i = 0; i < response->n_columns; ++i) {
        print_column(response->columns[i]);
    }
}

int connect_server(char *ip, long port, int *socket) {
    int return_code = NORMAL_END;
    return_code = socket_open(socket);
    if (return_code != NORMAL_END) return return_code;

    struct sockaddr_in server_address = {.sin_family = AF_INET, .sin_port = htons(port)};
    struct in_addr serv_address;
    if (inet_pton(AF_INET, ip, &serv_address) != 0) {
        server_address.sin_addr = serv_address;
    } else {
        server_address.sin_addr.s_addr = htonl(INADDR_LOOPBACK);
    }

    return_code = socket_connect(socket, &server_address);
    if (return_code != NORMAL_END) return return_code;

    return_code = socket_check_connect(socket);
    return return_code;
}

int run_client(char *address, long port) {
    signal(SIGINT, exit);
    int amount = 0;
    bool work = true;

    int server_socket;
    int return_code = connect_server(address, port, &server_socket);
    if (return_code > 0) return return_code;

    Common__Response *response;
    uint8_t buf[BUFSIZ];
    size_t msg_len;

    uint8_t * buf_send;
    size_t len_send;

    char *input;
    size_t len;

    while (server_socket > 0) {
        printf(PROMT);
        getline(&input, &len, stdin);

        Common__Request request = parse_request_string(input);
        len_send = common__request__get_packed_size(&request);
        buf_send = malloc (len_send);
        common__request__pack(&request, buf_send);

        if (write(server_socket, buf_send, len_send) < 0) {
            fprintf(stderr, ERROR_MESSAGE_SEND);
            continue;
        };

        msg_len = read_buffer (server_socket, BUFSIZ, buf);
        response = common__response__unpack(NULL, msg_len, buf);

        parse_response(response);
        common__response__free_unpacked(response, NULL);
    }
    return NORMAL_END;
}
