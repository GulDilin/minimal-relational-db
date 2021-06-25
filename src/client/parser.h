
#ifndef SPO_LAB_1_5_PARSER_H
#define SPO_LAB_1_5_PARSER_H
#include "../common/net.pb-c.h"
#include "./lexer/lex.yy.h"
#include "../common/utils.h"
#include "../common/net.h"

int parse_request_string(const char *in, Common__Request * request);


#endif //SPO_LAB_1_5_PARSER_H
