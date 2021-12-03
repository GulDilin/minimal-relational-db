#ifndef SPO_LAB_1_5_NET_H
#define SPO_LAB_1_5_NET_H

#define COMMAND_CODE_CONNECT 20
#define COMMAND_CODE_SELECT 21
#define COMMAND_CODE_CREATE 22
#define COMMAND_CODE_DROP 23
#define COMMAND_CODE_DELETE 24
#define COMMAND_CODE_INSERT 25
#define COMMAND_CODE_UPDATE 26

#define COLUMN_TYPE_TEXT_N   0
#define COLUMN_TYPE_INT_N    1

#define STATUS_OK 50
#define STATUS_ERROR 51

#define ACTIVE 1
#define INACTIVE 1

#define  _GNU_SOURCE
#include <stdio.h>

#include <stdint.h>
#include "stdbool.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#ifdef __WIN32
//#define NTDDI_VERSION NTDDI_WIN10
//#define WINVER _WIN32_WINNT_WIN10
#include "winsock2.h"
#include "Ws2tcpip.h"
#include "Windows.h"
//#define inet_pton(a, b, c) ({ InetPtonW(a, b, c); })
//#define inet_ntop(a, b, c, d) ({ InetNtopW(a, b, c, d); })
#define SHUT_RDWR SD_BOTH
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif
#include <fcntl.h>
#include "net.pb-c.h"
#include <stdlib.h>
#include "stdio.h"

size_t init_connection(int *connect_socket, struct sockaddr_in *server_address, long port, int *reuse, int max_clients_amount);

int socket_open(int *connect_socket);
int socket_connect (const int *client_socket, struct sockaddr_in *server_address);
void socket_close(const int *socket);
int socket_set_name(const int *connect_socket, int *reuse);
int socket_check_connect(const int *socket);
int socket_bind(const int *connect_socket, struct sockaddr_in *server_address);
int socket_listen(const int *connect_socket, int max_clients_amount);
int socket_nonblock(const int connect_socket);

bool validate_ip(char * address);
bool validate_port(int port);

size_t read_buffer (int socket, void * out, unsigned max_length);
size_t write_buffer (int socket, void * in, unsigned length);

unsigned long receive_header(int socket);
int send_header(int socket, unsigned long len);

int send_request(int socket, Common__Request * request);
int send_response(int socket, Common__Response * request);

int receive_request(int socket, Common__Request ** request);
int receive_response(int socket, Common__Response ** response);

void print_response_info(Common__Response * response);
void print_request_info(Common__Request * request);

#define PRINTLN printf("\n")

#define ERROR_MESSAGE_CLIENTS_OVERFLOW "Amount of clients is overflowed"
#define ERROR_MESSAGE_SOCKET_ACCEPT "Socket accept() failed"
#define ERROR_MESSAGE_CONNECT_NEW_CLIENT "New client cant be connected"
#define ERROR_MESSAGE_SEND "Send error"
#define ERROR_MESSAGE_SOCKET_OPEN "Open socket failed"
#define ERROR_MESSAGE_SOCKET_CONNECT "Connect socket failed"
#define ERROR_MESSAGE_SOCKET_LISTEN "Listen socket failed"
#define ERROR_MESSAGE_SOCKET_SET_NAME "Set socket name failed"
#define ERROR_MESSAGE_SOCKET_BIND "Bind socket failed"

#define MESSAGE_SOCKET_CHECK "Check connection..."
#define MESSAGE_CLIENTS_FULL "Amount of client is full"
#define MESSAGE_SOCKET_OPEN "Open socket..."
#define MESSAGE_SOCKET_CONNECT "Connect socket..."
#define MESSAGE_SOCKET_LISTEN "Listen socket..."
#define MESSAGE_SOCKET_SET_NAME "Set socket name..."
#define MESSAGE_SOCKET_BIND "Bind socket..."
#define MESSAGE_SOCKET_CLOSE "Close socket"
#define MESSAGE_INIT_FINISH "Init finished"
#define MESSAGE_OK "OK"

#define ERROR_SOCKET_ACCEPT 3
#define ERROR_SOCKET_OPEN 4
#define ERROR_SOCKET_SET_NAME 5
#define ERROR_SOCKET_BIND  6
#define ERROR_SOCKET_LISTEN 7
#define ERROR_CONNECT 9

#define MAX_MSG_SIZE 1024

#endif //SPO_LAB_1_5_NET_H
