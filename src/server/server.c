#include "server.h"

int server_status = ACTIVE;
int client_act_status = INACTIVE;

void serve_client(int socket);

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


void execute_command_create (Common__Request * request, Common__Response * response) {
    response->command_code = request->command_code;
    if (request->n_columns < 1) {
        response->status_code = ERROR_END;
        response->text = ERROR_MESSAGE_NO_COLUMNS;
    }
    MetaColumn columns[request->n_columns];
    for(int i = 0; i < request->n_columns; i++) {
        Common__ColumnValue * column_value = request->columns[i];
        memset(&columns[i], 0, SIZE_META_COLUMN);
        columns[i].type = column_value->column_type_code;
        columns[i].is_key = column_value->is_key;
        columns[i].is_required = column_value->is_required;
        strncpy(columns[i].name, column_value->title, TEXT_LENGTH_MAX );
    }

    int return_code = create_table(request->table_name, request->n_columns, columns);
    response->status_code = return_code;
    define_message_by_return_code(return_code, response, MESSAGE_CREATED_SUCCESS);
}


void execute_command(Common__Request * request, Common__Response * response) {
    switch (request->command_code) {
        case COMMAND_CODE_CREATE:
            execute_command_create(request, response);
            break;
        default:
            response->status_code=STATUS_ERROR;
            response->text = "No such command";
            break;
    }
}


void serve_client(int socket) {
    printf("Serve thread for socket %d created\n", socket);
    do {
        printf("--------------WAIT NEW REQUEST FROM CLIENT: %d\n", socket);
        Common__Request * request;
        receive_request(socket, &request);

        if (request == NULL) {
            fprintf(stderr, "%s\n", ERROR_MESSAGE_UNPACK);
            client_act_status = INACTIVE;
            printf("deactivate client socket: %d\n", socket);
            close(socket);
            return;
        }

        Common__Response response = COMMON__RESPONSE__INIT;
        execute_command(request, &response);
        int response_stat = send_response(socket, &response);
        if (response_stat == 1) break;
    } while (client_act_status == ACTIVE);
    close(socket);
}

void send_connect(int socket) {
    printf("Prepare check connection packet\n");
    Common__Response response = COMMON__RESPONSE__INIT;
    response.command_code = COMMAND_CODE_CONNECT;
    response.status_code = STATUS_OK;
    response.n_columns = 0;
    response.text = "Connected";
    void *buf;
    unsigned long len;
    len = common__response__get_packed_size(&response);
    printf("Packet packed size: %lu\n", len);
    buf = malloc(len);
    common__response__pack(&response, buf);
    printf("COMMAND CODE: %d\n", response.command_code);

    send_header(socket, len);
    write(socket, buf, len);
    printf("Sent check connection packet\n");
    free(buf);
}

int run_server(int port) {
    db_init("test.db");
    struct sockaddr_in server_address, client_addr;
    int server_socket;
//    socket_nonblock(server_socket);
    int reuse = 1;


    size_t return_code = init_connection(&server_socket, &server_address, port, &reuse, MAX_CLIENTS_AMOUNT);
    if (return_code != NORMAL_END) return return_code;

    int client_sock;
    while (server_status == ACTIVE) {
        client_sock = accept(server_socket, NULL, NULL);
        if (client_sock == -1) {
            if (errno == FNONBLOCK) {
                printf("No pending connections; sleeping for one second.\n");
                sleep(1);
            } else {
                perror("error when accepting connection");
                exit(1);
            }
        }
        printf("-----------NEW CONNECTION ACCEPTED; Client Socket: %d\n", client_sock);
        send_connect(client_sock);
        client_act_status = ACTIVE;
        printf("Connection response sent\n");
        serve_client(client_sock);
//        pthread_t pthread;
//        pthread_create(&pthread, NULL, serve_client, new_sock);
    }
    if (client_sock < 0) {
        perror("accept failed");
        exit(1);
    }
    return 0;
}
