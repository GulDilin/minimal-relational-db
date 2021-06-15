#ifndef SPO_LAB_1_5_NET_H
#define SPO_LAB_1_5_NET_H

#define COMMAND_CODE_SELECT 0
#define COMMAND_CODE_CREATE 1
#define COMMAND_CODE_DROP 2
#define COMMAND_CODE_DELETE 3
#define COMMAND_CODE_INSERT 4

#include "stdio.h"
#include "stdbool.h"

#ifdef __WIN32__
#include "windows.h"
#include "ws2tcpip.h"
#define SHUT_RDWR SD_BOTH
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

size_t init_connection(int *connect_socket, struct sockaddr_in *server_address, long port, int *reuse, int max_clients_amount);

int socket_open(int *connect_socket);
int socket_connect (const int *client_socket, struct sockaddr_in *server_address);
int socket_check_connect(const int *socket);
void socket_close(const int *socket);
int socket_set_name(const int *connect_socket, int *reuse);
int socket_bind(const int *connect_socket, struct sockaddr_in *server_address);
int socket_listen(const int *connect_socket, int max_clients_amount);

bool validate_ip(char * address);
bool validate_port(int port);

#define PRINTLN printf("\n")

#define ERROR_MESSAGE_CLIENTS_OVERFLOW "Amount of clients is overflowed"
#define ERROR_MESSAGE_SOCKET_ACCEPT "Socket accept() failed"
#define ERROR_MESSAGE_CONNECT_NEW_CLIENT "New client cant be connected"
#define ERROR_MESSAGE_UNDEFINED "Undefined error"
#define ERROR_MESSAGE_SOCKET_OPEN "Open socket failed"
#define ERROR_MESSAGE_SOCKET_CONNECT "Connect socket failed"
#define ERROR_MESSAGE_SOCKET_LISTEN "Listen socket failed"
#define ERROR_MESSAGE_SOCKET_SET_NAME "Set socket name failed"
#define ERROR_MESSAGE_SOCKET_BIND "Bind socket failed"

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

#endif //SPO_LAB_1_5_NET_H
