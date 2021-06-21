%{
#include <stdio.h>
#include "lex.yy.h"
#include "../../common/net.h"
#include "../../common/net.pb-c.h"

#define CORRECT_SYNTAX_MESSAGE "Syntax Correct\n"

int yylex(void);
int yyparse(void);

void yyerror (const char *str) {
	fprintf(stderr, "error: %s\n", str);
}

int yywrap() {
	return 1;
}

Common__Request * request;
char *keys[MAX_MSG_SIZE];

%}

%%

%token SELECT INSERT UPDATE CREATE DELETE
FROM IDENTIFIER WHERE AND VALUES INTO SET TABLE
TEXT NUMBER;

line:       select items from table_name { printf(CORRECT_SYNTAX_MESSAGE); }
            |
            select items from table_name where condition { printf(CORRECT_SYNTAX_MESSAGE); }
            |
            insert into table_name columns VALUES values { printf(CORRECT_SYNTAX_MESSAGE); }
            |
            update table_name set_values condition where condition { printf(CORRECT_SYNTAX_MESSAGE); }
            |
            create table table_name '(' column_defs ')' { printf(CORRECT_SYNTAX_MESSAGE); };

table:      TABLE
select:     SELECT          { request->command_code = COMMAND_CODE_SELECT; };
update:     UPDATE          { request->command_code = COMMAND_CODE_UPDATE; };
insert:     INSERT          { request->command_code = COMMAND_CODE_INSERT; };
create:     CREATE          { request->command_code = COMMAND_CODE_CREATE; };
table_name: IDENTIFIER
    {
        request->table_name = malloc(strlen(yytext));
        strncpy(request->table_name, yytext, strlen(yytext));
    };

into:           INTO;

items:          '*'
                |
                identifiers;

columns:        '(' identifiers ')'

values:         identifiers;

identifiers:    IDENTIFIER
                |
                IDENTIFIER ',' identifiers;

column_type:    NUMBER
                |
                TEXT {
                    printf("column type: %s\n", yytext);
                };

column_name:    IDENTIFIER {
    printf("column name: %s\n", yytext);
    if (request->n_columns == 0) {
        request->columns = malloc(sizeof(Common__ColumnValue)
    }
};

column_def:     column_name column_type;

column_defs:    column_def
                |
                column_def ',' column_defs;

from:       FROM;
where:      WHERE;

condition: IDENTIFIER '=' IDENTIFIER | IDENTIFIER '=' IDENTIFIER AND condition;
set_values: IDENTIFIER '=' IDENTIFIER | IDENTIFIER '=' IDENTIFIER AND set_values;

%%

void set_input_string(const char* in);
void end_lexical_scan(void);

int parse_string(const char* in, Common__Request * p_request) {
  set_input_string(in);
  common__request__init(p_request);
  request = p_request;
  int rv = yyparse();
  end_lexical_scan();
  request = NULL;
  return rv;
}

//int main() {
//    yyparse();
//}
