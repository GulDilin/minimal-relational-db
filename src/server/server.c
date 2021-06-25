#include "server.h"

int server_status = ACTIVE;
int client_act_status = INACTIVE;

void serve_client(int socket);

void define_message_by_return_code(int return_code, Common__Response *response, char *success_message) {
    debugf("Define message by rc: %d\n", return_code);
    char *text;
    switch (return_code) {
        case NORMAL_END:
            text = "Success";
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
    if (text != NULL) response->text = text;
}


void execute_command_create(Common__Request *request, Common__Response *response) {
    response->command_code = request->command_code;
    if (request->n_columns < 1) {
        response->status_code = ERROR_END;
        response->text = ERROR_MESSAGE_NO_COLUMNS;
    }

    MetaColumn *columns = malloc(request->n_columns * SIZE_META_COLUMN);
    memset(columns, 0, request->n_columns * SIZE_META_COLUMN);


    for (int i = 0; i < request->n_columns; i++) {
        Common__ColumnValue *column_value = request->columns[i];
        memset(&columns[i], 0, SIZE_META_COLUMN);
        columns[i].type = column_value->column_type_code;
        columns[i].is_key = column_value->is_key;
        columns[i].is_required = column_value->is_required;
        strncpy(columns[i].name, column_value->title, TEXT_LENGTH_MAX);
    }

    int return_code = create_table(request->table_name, request->n_columns, columns);
    debugf("Table creation finish code: %d\n", return_code);
    response->status_code = return_code;
    define_message_by_return_code(return_code, response, "Success created");
}

void execute_command_select(Common__Request *request, Common__Response *response) {
    response->command_code = request->command_code;
//    bool select_all = false;
//    if (request->n_columns == 0) select_all = true;
    for (int i = 0; i < request->n_columns; i++) {
        Common__ColumnValue *column_value = request->columns[i];
        printf("Request column: %s\n", column_value->title);
    }

    MetaColumn *columns_res;
    MetaRow **rows_res;
    char ***data_res;
    int rows_count;
    int amount_columns;
    find_all_table_rows(
            request->table_name,
            NULL, NULL,
            &columns_res, &rows_res, &data_res,
            &rows_count, &amount_columns
    );
    printf("Find all rows finished\n");
    debugf("Amount rows: %d Cols: %d\n", rows_count, amount_columns);

    if (rows_count == 0) {
        response->status_code = ERROR_END;
        response->text = "Not found";
        return;
    }
    response->n_columns = rows_count * amount_columns;
    response->amount_columns = amount_columns;
    debugf("n_columns: %zu\n", response->n_columns);
    response->columns = malloc(sizeof(Common__ColumnValue) * response->n_columns);

    int col = 0;
    for (int i = 0; i < rows_count; ++i) {
        for (int j = 0; j < amount_columns; ++j) {
            char * str = malloc(TEXT_LENGTH_MAX);
            debugf("Col num: %d Col name: %s Value: %s\n", col, columns_res[j].name, data_res[i][j]);
            strncpy(str, data_res[i][j], TEXT_LENGTH_MAX);
            Common__ColumnValue *column_value = malloc(sizeof(Common__ColumnValue));
            common__column_value__init(column_value);
            column_value->title = columns_res[j].name;
            column_value->value = str;
            (response->columns)[col] = column_value;
            col++;
        }
    }

    response->status_code = NORMAL_END;
    response->text = "SELECT FINISHED";
}


void execute_command(Common__Request *request, Common__Response *response) {
    switch (request->command_code) {
        case COMMAND_CODE_CREATE:
            execute_command_create(request, response);
            break;
        case COMMAND_CODE_SELECT:
            execute_command_select(request, response);
            break;
        default:
            response->status_code = STATUS_ERROR;
            response->text = "No such command";
            break;
    }
}


void serve_client(int socket) {
    printf("Serve thread for socket %d created\n", socket);
    do {
        printf("--------------WAIT NEW REQUEST FROM CLIENT: %d\n", socket);
        Common__Request *request;
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

void test_db() {
    printf("___________START DB TEST__________\n");
    MetaColumn *columns = malloc(2 * SIZE_META_COLUMN);
    memset(columns, 0, 2 * SIZE_META_COLUMN);
    columns[0].type = COLUMN_TYPE_INT;
    strcpy(columns[0].name, "id");
    columns[0].is_required = true;
    columns[0].is_key = true;

    columns[1].type = COLUMN_TYPE_INT;
    strcpy(columns[1].name, "one");
    columns[1].is_required = false;
    columns[1].is_key = false;

    int rc = create_table("TableTest", 2, columns);
    printf("Table created with code: %d\n", rc);

    EntryToInsert *items = malloc(2 * sizeof(EntryToInsert));
    strncpy(items[0].column_name, "id", TEXT_LENGTH_MAX);
    items[0].value_data = "1";

    strncpy(items[1].column_name, "one", TEXT_LENGTH_MAX);
    items[1].value_data = "2";

    printf("\n NEW Table INSERT\n");
    rc = insert_into_table("TableTest", 2, items);
    printf("\nTable INSERT with code: %d\n", rc);

    items[0].value_data = "3";
    items[1].value_data = "4";

    printf("\n NEW Table INSERT\n");
    rc = insert_into_table("TableTest", 2, items);
    printf("\nTable INSERT with code: %d\n\n", rc);

    MetaColumn *columns_res;
    MetaRow **rows_res;
    char ***data_res;
    int rows_count;
    int amount_columns;
    find_all_table_rows(
            "TableTest",
            NULL, NULL,
            &columns_res, &rows_res, &data_res,
            &rows_count, &amount_columns
    );


    for (int i = 0; i < rows_count; ++i) {
        printf("\n\nrow size: %d\n", (*rows_res)[i].row_size);
        for (int j = 0; j < amount_columns; ++j) {
            char str[TEXT_LENGTH_MAX];
            strncpy(str, data_res[i][j], TEXT_LENGTH_MAX);
            printf("COLUMN: %s IS_KEY: %d, VALUE: %s\n",
                   columns_res[j].name, columns_res[j].is_key, str
            );
        }
    }

//    rc = delete_table("TableTest");
    printf("Table DELETE with code: %d\n", rc);
    printf("___________END DB TEST__________\n\n");

}

int run_server(int port) {
    db_init("test.db");
    test_db();
    struct sockaddr_in server_address, client_addr;
    int server_socket;
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
                perror("error when accepting connection\n");
                exit(1);
            }
        }
        printf("-----------NEW CONNECTION ACCEPTED; Client Socket: %d\n", client_sock);
        send_connect(client_sock);
        client_act_status = ACTIVE;
        printf("Connection response sent\n");
        serve_client(client_sock);
    }
    if (client_sock < 0) {
        perror("accept failed");
        exit(1);
    }
    return 0;
}
