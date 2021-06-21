
#ifndef SPO_LAB_1_5_CLIENT_H
#define SPO_LAB_1_5_CLIENT_H

#include "../common/net.h"
#include "../common/net.pb-c.h"
#include "../cursor/cursor.h"
#include "../common/utils.h"
#include "parser.h"
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdint.h>

int run_client(char * address,  long port);

#define PROMT "\n\nSQL> "

#endif //SPO_LAB_1_5_CLIENT_H
