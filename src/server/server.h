
#ifndef SPO_LAB_1_5_SERVER_H
#define SPO_LAB_1_5_SERVER_H

#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>

#ifdef __WIN32__
#include "windows.h"
#include "ws2tcpip.h"
#else
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif

#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include "../cursor/cursor.h"
#include "../common/net.pb-c.h"
#include "../common/net.h"

#define MAX_MSG_SIZE 1024

#define ERROR_MESSAGE_UNPACK "Unpackaging failed"
#define MAX_CLIENTS_AMOUNT 30
int run_server(int port);

#endif //SPO_LAB_1_5_SERVER_H
