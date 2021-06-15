#include "server.h"

void *serve_client(void *socket_desc);

void define_message_by_return_code(int return_code, Common__Response * response, char * success_message) {
    char * text;
    switch(return_code) {
        case NORMAL_END:
            if (success_message != NULL) strcpy(response->text, success_message);
            break;
        case ERROR_END:
            text = ERROR_MESSAGE_END;
            break;
        case ERROR_DATABASE_META_ALLOC:
            text = ERROR_MESSAGE_DATABASE_META_ALLOC;
            break;
        case ERROR_DATABASE_FILE_OPEN:
            text = ERROR_MESSAGE_DATABASE_FILE_OPEN;
            break;
        case ERROR_DATABASE_FORMAT:
            text = ERROR_MESSAGE_DATABASE_FORMAT;
            break;
        case ERROR_DATABASE_READ:
            text = ERROR_MESSAGE_DATABASE_READ;
            break;
        case ERROR_DATABASE_TABLE_READ:
            text = ERROR_MESSAGE_DATABASE_TABLE_READ;
            break;
        case ERROR_TABLE_NOT_FOUND:
            text = ERROR_MESSAGE_TABLE_NOT_FOUND;
            break;
        case ERROR_NO_TABLES:
            text = ERROR_MESSAGE_NO_TABLES;
            break;
        case ERROR_TABLE_ALREADY_EXISTS:
            text = ERROR_MESSAGE_TABLE_ALREADY_EXISTS;
            break;
        case ERROR_EMPTY_REQUIRED_PARAMETER:
            text = ERROR_MESSAGE_EMPTY_REQUIRED_PARAMETER;
            break;
        case ERROR_ENTRY_WITH_ID_ALREADY_EXISTS:
            text = ERROR_MESSAGE_ENTRY_WITH_ID_ALREADY_EXISTS;
            break;
        case ERROR_COLUMN_NOT_FOUND:
            text = ERROR_MESSAGE_COLUMN_NOT_FOUND;
            break;
        default:
            text = "";
    }
    if (text != NULL) strcpy(response->text, text);
}


Common__Response execute_command_create (Common__Request * request) {
    Common__Response response = COMMON__RESPONSE__INIT;
    response.command_code = request->command_code;
    if (request->n_columns < 1) {
        response.status_code = ERROR_END;
        response.text = ERROR_MESSAGE_NO_COLUMNS;
    }
    MetaColumn columns[request->n_columns];
    for(int i = 0; i < request->n_columns; i++) {
        Common__ColumnValue * column_value = request->columns[i];
        memset(&columns[i], 0, SIZE_META_COLUMN);
        columns[i].type = column_value->column_type_code;
        columns[i].is_key = column_value->is_key;
        columns[i].is_required = column_value->is_required;
        strcpy_s(columns[i].name, TEXT_LENGTH_MAX, column_value->title);
    }

    int return_code = create_table((char *) table, request->n_columns, columns);
    response.status_code = return_code;
    define_message_by_return_code(return_code, &response, MESSAGE_CREATED_SUCCESS);
    return response;
}

Common__Response execute_command(Common__Request * request) {
    switch (request->command_code) {
        case COMMAND_CODE_CREATE:
            return execute_command_create(request);
    }
}

void *serve_client(void *socket_desc) {
    int sock = *(int *) socket_desc;

    Common__Request *request;
    uint8_t buf[BUFSIZ];
    size_t msg_len;
    while((msg_len = read_buffer (sock, BUFSIZ, buf )) != 0) {
        request = common__request__unpack(NULL, msg_len, buf);
        Common__Response response;
        if (request == NULL) {
            fprintf(stderr, "%s\n", ERROR_MESSAGE_UNPACK);
        }

        response = execute_command(request);
        size_t len_send;
        void *buf_send;
        len_send = common__response__get_packed_size(&response);
        buf_send = malloc (len_send);
        common__response__pack(&response, buf_send);
        if (write(sock, buf_send, len_send) < 0) break;
    }
    close(sock);

    return NORMAL_END;
}

int run_server(int port) {
    db_init("test.db");
    struct sockaddr_in server_address, client_addr;
    int server_socket;
    int reuse = 1;


    size_t return_code = init_connection(&server_socket, &server_address, port, &reuse, MAX_CLIENTS_AMOUNT);
    if (return_code != NORMAL_END) return return_code;

    int opt = 1;

    int c = sizeof(struct sockaddr_in), client_sock;
    while ((client_sock = accept(server_socket, (struct sockaddr *) &client_addr, (socklen_t *) &c))) {
        char client_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, client_ip, INET_ADDRSTRLEN);
        printf("Connection accepted; Client IP: %s\n", client_ip);

        int *new_sock = malloc(1);
        *new_sock = client_sock;

        pthread_t pthread;
        pthread_create(&pthread, NULL, serve_client, (void *) (new_sock));
    }
    if (client_sock < 0) {
        perror("accept failed");
        exit(1);
    }
    return 0;
}
