#include "parser.h"

int parse_request_string(const char *in, Common__Request * request) {
    return parse_string(in, request);
};
