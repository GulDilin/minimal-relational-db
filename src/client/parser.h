
#ifndef SPO_LAB_1_5_PARSER_H
#define SPO_LAB_1_5_PARSER_H
#include "../common/net.pb-c.h"
#include "./lexer/y.tab.h"
#include "./lexer/lex.yy.h"

int parse_request_string(const char *in, Common__Request * request);

#endif //SPO_LAB_1_5_PARSER_H
