//
// Created by user on 20.06.2021.
//

#ifndef SPO_LAB_1_5_LEX_YY_H
#define SPO_LAB_1_5_LEX_YY_H

#include <stdio.h>
#include <stdlib.h>
#include "../../common/net.pb-c.h"
#include "y.tab.h"

void set_string_to_parse(const char *in);

void finish(char ***p_keys, Common__Request * p_request);

int yylex(void);

int yyparse(void);

int yy_scan_string ( const char *yy_str  );

int parse_string(const char* in, struct ast_node * node);

void print_ast_tree(struct ast_node * curr, int level);

char * yytext;
char *ctext;
int yyval;


struct ast_node {
    int node_type;
    struct ast_node * first;
    struct ast_node * next;
    char * text;
};;

int yyget_lineno  (void);

#endif //SPO_LAB_1_5_LEX_YY_H
