#include "client.h"

int server_socket;
int client_status = ACTIVE;

void show_response_info(Common__Response *response) {
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
    printf("%s:\t%s\n", column->title, column->value);
}

void parse_response(Common__Response *response) {
    if (response->status_code != NORMAL_END) return;
    int amount_columns = response->amount_columns;
    if(response->n_columns < 1) return;
    for (int i = 0; i < response->n_columns; ++i) {
        if (amount_columns > 0 && i % amount_columns == 0) printf("------------------");
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

    return return_code;
}

int run_client(char *address, long port) {
    signal(SIGINT, exit);
    int amount = 0;
    bool work = true;

    int return_code = connect_server(address, port, &server_socket);
    if (return_code > 0) return return_code;

    if (server_socket < 0) {
        printf("Wrong server socket\n");
        return 1;
    }

    int rc = socket_check_connect(&server_socket);
    if (rc != NORMAL_END) exit(rc);

    uint8_t buf[BUFSIZ];
    size_t msg_len;

    void * buf_send;
    size_t len_send;

    char *input;
    size_t len;

    while (client_status == ACTIVE) {
        printf(PROMT);
        getdelim(&input, &len, ';', stdin);

        Common__Request request = COMMON__REQUEST__INIT;
        int parsed_value = parse_request_string(input, &request);
        if (parsed_value != NORMAL_END) continue;
        int return_send = send_request(server_socket, &request);
        if (return_send != 0) continue;

        printf("Message sent\n");

        Common__Response *response;
        return_send = receive_response(server_socket, &response);
        if (return_send != 0) continue;
        parse_response(response);
        common__response__free_unpacked(response, NULL);
    }
    return NORMAL_END;
}
