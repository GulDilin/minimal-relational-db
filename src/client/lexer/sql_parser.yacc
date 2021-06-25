%{
#include <stdio.h>
#include "lex.yy.h"
#include "../../common/net.h"
#include "../../common/net.pb-c.h"

#define CORRECT_SYNTAX_MESSAGE "Syntax Correct\n"
#define YYERROR_VERBOSE 1

int yylex(void);
int yyparse(void);
int yychar;

void yyerror (const char *str) {
	fprintf(stderr, "error: %s\n text: <%s>\nyylineno:%d\ncolnum:%d\n", str, yytext, yyget_lineno(), yyval);
}

int yywrap() {
	return 1;
}


struct ast_node * new_ast_node (
    int node_type,
    char * text,
    struct ast_node * first,
    struct ast_node * next
) {
  struct ast_node * ast_node = malloc (sizeof (struct ast_node));
  ast_node->node_type = node_type;
  ast_node->text = text;
  ast_node->first = first;
  ast_node->next = next;
  return ast_node;
}

struct ast_node * root;

char *keys[MAX_MSG_SIZE];


%}

%start line
%union {
    struct ast_node * node;
    int number;
    char *text;
}

%type <node> line select insert create update table select_stmt update_stmt insert_stmt create_stmt from_stmt into_stmt
%type <node> add_table_stmt table_name items columns values column_type column_name column_def column_defs condition set_value
%type <node> set_values identifier identifiers column_type_val cmd

%%

%token LINE SELECT INSERT UPDATE CREATE DELETE
FROM IDENTIFIER IDENTIFIERS WHERE AND VALUES INTO SET TABLE STATEMENT TABLE_DEF ALL
TEXT_TYPE NUMBER_TYPE COLUMN_NAME SET_VALUE SET_VALUES CONDITION COLUMN_TYPE ITEMS COLUMNS CMD;

line: cmd  { $$ = new_ast_node(CMD, "cmd", $1, NULL); printf(CORRECT_SYNTAX_MESSAGE); root = $$;};
cmd: select {
              $$ = new_ast_node(LINE, "line", $1, NULL);
            };
| update {
           $$ = new_ast_node(LINE, "line", $1, NULL);
           root = $$;
         };
| create {
  $$ = new_ast_node(LINE, "line", $1, NULL);
  root = $$;
};
| insert {
    $$ = new_ast_node(LINE, "line", $1, NULL);
    root = $$;
};

select: select_stmt items from_stmt {
    $$ = new_ast_node(SELECT, "select", $2, NULL);
    $$->next = $3;
}
| select_stmt items from_stmt where condition {
    $$ = new_ast_node(SELECT, "select", $2, NULL);
    $$->first->next = $3;
    $$->first->next->next = $5;
};

insert: insert_stmt into_stmt columns VALUES values {
    $$ = new_ast_node(INSERT, "insert", $2, NULL);
    $$->first->next = $3;
};

create: create_stmt add_table_stmt {
    $$ = new_ast_node(CREATE, "create", $2, NULL);
};

update: update_stmt table_name set_values where condition {
   $$ = new_ast_node(UPDATE, "create", $2, NULL);
   $$->first->next = $3;
};


table:           TABLE  {
    size_t len = strlen(yytext);
    char * str = malloc(len);
    strncpy(str, yytext, len);
    $$ = new_ast_node(STATEMENT, str, NULL, NULL);
};
select_stmt:     SELECT {
    size_t len = strlen(yytext);
    char * str = malloc(len);
    strncpy(str, yytext, len);
    $$ = new_ast_node(STATEMENT, str, NULL, NULL);
};
update_stmt:     UPDATE {
     size_t len = strlen(yytext);
     char * str = malloc(len);
     strncpy(str, yytext, len);
     $$ = new_ast_node(STATEMENT, str, NULL, NULL);
 };
insert_stmt:     INSERT  {
    size_t len = strlen(yytext);
    char * str = malloc(len);
    strncpy(str, yytext, len);
    $$ = new_ast_node(STATEMENT, str, NULL, NULL);
};
create_stmt:     CREATE {
    size_t len = strlen(yytext);
    char * str = malloc(len);
    strncpy(str, yytext, len);
    $$ = new_ast_node(STATEMENT, str, NULL, NULL);
};
from_stmt:       from table_name {
    $$ = new_ast_node(FROM, "from", $2, NULL);
};
into_stmt:       into table_name {
    $$ = new_ast_node(FROM, "from", $2, NULL);
};
add_table_stmt:  table table_name '(' column_defs ')' {
    $$ = new_ast_node(TABLE, "table", $2, NULL);
    $$->first->first = $4;
};

table_name:      IDENTIFIER {
    size_t len = strlen(yytext);
    char * str = malloc(len);
    strncpy(str, yytext, len);
    $$ = new_ast_node(TABLE, str, NULL, NULL);
};

into:           INTO;

items:          ALL {
    $$ = new_ast_node(ALL, "all", NULL, NULL);
}
| identifiers {
    $$ = new_ast_node(ITEMS, "values", $1, NULL);
};

columns:        '(' identifiers ')' {
    $$ = new_ast_node(COLUMNS, "values", $2, NULL);
};

values:         identifiers {
    $$ = new_ast_node(VALUES, "values", $1, NULL);
};

identifiers:    identifier {
    $$ = $1;//new_ast_node(IDENTIFIERS, "identifiers", $1, NULL);
}
| identifier ',' identifiers {
    $1->next = $3;
};

identifier: IDENTIFIER {
    size_t len = strlen(yytext);
    char * str = malloc(len);
    strncpy(str, yytext, len);
    $$ = new_ast_node(IDENTIFIER, str, NULL, NULL);
}

column_type_val: NUMBER_TYPE {
    size_t len = strlen(yytext);
    char * str = malloc(len);
    strncpy(str, yytext, len);
    $$ = new_ast_node(NUMBER_TYPE, str, NULL, NULL);
}
| TEXT_TYPE {
    size_t len = strlen(yytext);
    char * str = malloc(len);
    strncpy(str, yytext, len);
    $$ = new_ast_node(TEXT_TYPE, str, NULL, NULL);
};

column_type:    column_type_val {
    size_t len = strlen(yytext);
    char * str = malloc(len);
    strncpy(str, yytext, len);
    $$ = $1;//new_ast_node(COLUMN_TYPE, "column_type", $1, NULL);
}

column_name:    identifier {
    size_t len = strlen(yytext);
    char * str = malloc(len);
    strncpy(str, yytext, len);
    $$ = new_ast_node(COLUMN_NAME, "column_name", $1, NULL);
};

column_def:     column_name column_type {
    size_t len = strlen(yytext);
    char * str = malloc(len);
    strncpy(str, yytext, len);
    $$ = new_ast_node(COLUMN_NAME, str, $1, NULL);
    $1->first->next = $2;
};

column_defs:    column_def {
    $$ = $1;
}
| column_def ',' column_defs {
    $1->next = $3;
};

from:       FROM;
where:      WHERE;

condition: identifier '=' identifier {
    $$ = new_ast_node(CONDITION, "=", $1, NULL);
    $$->first->next = $3;
};

set_value: identifier '=' identifier {
    $$ = new_ast_node(SET_VALUE, "=", $1, NULL);
    $$->first->next = $3;
}

set_values: set_value { $$ = new_ast_node(SET_VALUES, NULL, $1, NULL); }
| set_value ','  set_values { $$ = $1; $$->next = $3; }
%%

void set_input_string(const char* in);
void end_lexical_scan(void);

void print_ast_tree(struct ast_node * curr, int level) {
    if (curr == NULL) {
        return;
    }
    for(int i = 0; i < level; i++) printf("  ");
    printf("|_%s %d\n", curr->text, curr->node_type);
    print_ast_tree(curr->first, level + 1);
    print_ast_tree(curr->next, level);
}

int parse_string(const char* in, struct ast_node * node) {
  set_input_string(in);
  int rv = yyparse();
  *node = *root;
  printf("\n");
  end_lexical_scan();
  return rv;
}

int main_1() {
    struct ast_node * node = malloc(sizeof(struct ast_node));
    parse_string("select a from b ;", node);

    print_ast_tree(root, 0);
}
