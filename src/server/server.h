
#ifndef SPO_LAB_1_5_SERVER_H
#define SPO_LAB_1_5_SERVER_H

#include <unistd.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include "../cursor/cursor.h"
#include "../common/net.pb-c.h"
#include "../common/net.h"

#define MAX_MSG_SIZE 1024

#define ERROR_MESSAGE_UNPACK "Unpackaging failed"
#define MAX_CLIENTS_AMOUNT 30
int run_server(int port);

#endif //SPO_LAB_1_5_SERVER_H
