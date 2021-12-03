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

int socket_check_connect(const int *socket) {
    printf(MESSAGE_SOCKET_CHECK);
    Common__Response * response;
    uint8_t buf[BUFSIZ];
    unsigned long response_size = receive_header(*socket);
    size_t msg_len = 0;
    msg_len = read(*socket, buf, response_size);
    printf("Recieved response size: %zu\n", msg_len);

    response = common__response__unpack(NULL, msg_len, buf);
    if (response == NULL)
    {
        fprintf(stderr, "error unpacking incoming message\n");
        return 1;
    }
    printf("Status code: %d\n", response->status_code);
    printf("Recieved text: %s\n", response->text);
    if (response->status_code != STATUS_OK) {
        close(*socket);
        printf("Wrong command\n");
        return ERROR_CONNECT;
    }
    common__response__free_unpacked(response, NULL);
    printf(MESSAGE_OK);
    return 0;
}

int send_header(int socket, unsigned long len) {
    char buffer[BUFSIZ];
    memset(buffer, 0, BUFSIZ);
    snprintf(buffer, BUFSIZ, "%lu", len);
    printf("Packed size: %lu\n", len);
    write(socket, buffer, BUFSIZ);
    return 0;
}

int send_request(int socket, Common__Request * request) {
    size_t len_send = common__request__get_packed_size(request);
    void * buf_send = malloc(len_send);
    common__request__pack(request, buf_send);

    send_header(socket, len_send);
    if (write(socket, buf_send, len_send) == -1) {
        fprintf(stderr, ERROR_MESSAGE_SEND);
        return 1;
    };
    print_request_info(request);
    return 0;
}

int send_response(int socket, Common__Response * response) {
    size_t len_send = common__response__get_packed_size(response);
    void * buf_send = malloc (len_send);
    common__response__pack(response, buf_send);
    send_header(socket, len_send);
    if (write(socket, buf_send, len_send) < 0) return 1;
    print_response_info(response);
    return 0;
}

int receive_request(int socket, Common__Request ** request) {
    uint8_t buf[BUFSIZ];
    size_t msg_len = receive_header(socket);
    msg_len = read(socket, buf, msg_len);
    if (msg_len < 1) {
        printf("Cannot read message\n");
        return 1;
    }
    printf("Received %zu bytes\n", msg_len);
    *request = common__request__unpack(NULL, msg_len, buf);
    if (*request == NULL) return 1;
    print_request_info(*request);
    return 0;
}

int receive_response(int socket, Common__Response ** response) {
    uint8_t buf[BUFSIZ];
    size_t msg_len = receive_header(socket);
    if (msg_len < 1) return 1;
    msg_len = read(socket, buf, msg_len);
    *response = common__response__unpack(NULL, msg_len, buf);
    print_response_info(*response);
    return 0;
}



unsigned long receive_header(int socket) {
    unsigned long response_size;
    char header[BUFSIZ];
    read(socket, header, BUFSIZ);
    response_size = strtol(header, NULL, 10);
    printf("Waiting for response size: %lu\n", response_size);
    return response_size;
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

int socket_nonblock(const int connect_socket) {
    int flags = fcntl(connect_socket, F_GETFL);
    fcntl(connect_socket, F_SETFL, flags | O_NONBLOCK);
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

void print_request_info(Common__Request * request) {
    printf("Request info:\n\tCommand code: %d\n\tTable name: %s\n\tN columns: %zu\n\n",
           request->command_code, request->table_name, request->n_columns);
}

void print_response_info(Common__Response * response) {
    printf("Response info:\n\tStatus code: %d\n\tCommand code: %d\n\tAmount columns: %d\n\tN columns: %zu\n\tText: %s\n\n",
          response->status_code ,response->command_code, response->amount_columns, response->n_columns, response->text);
}

bool validate_ip(char * address) {
    struct sockaddr_in sa;
    return inet_pton(AF_INET, address, &(sa.sin_addr)) != 0;
}

bool validate_port(int port) {
    return port < 65535 && port > 1;
}

size_t read_buffer (int socket, void * out, unsigned max_length) {
//
//    size_t cur_len = 0;
//    size_t total_size;
//    size_t size_recv;
//    char chunk[BUFSIZ];
//
//    while(1)
//    {
//        memset(chunk ,0 , BUFSIZ);	//clear the variable
//        if((size_recv =  recv(socket , chunk , BUFSIZ , 0) ) < 0) break;
//        total_size += size_recv;
//        printf("%s" , chunk);
////        memccpy()
//    }
//    return total_size;
    size_t cur_len = 0;
    size_t nread;
    while ((nread = read(socket, out+cur_len, 1)) != 0 && cur_len < max_length) {
        cur_len += nread;
        if (cur_len == max_length) {
            fprintf(stderr, "max message length exceeded\n");
            return -1;
        }
    }
    return cur_len;
}

size_t write_buffer (int socket, void * in, unsigned length) {
    size_t n_write;
    size_t current_len;
    for (int i = 0; i < length; ++i) {
        n_write = write(socket, in, 1);
        current_len += n_write;
        if (n_write == 0) return current_len;
    }
    return current_len;
}
