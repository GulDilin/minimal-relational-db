%{
#include <stdio.h>
#include "lex.yy.h"
#include "../../common/net.h"
#include "../../common/net.pb-c.h"

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

%token SELECT INSERT UPDATE FROM IDENTIFIER WHERE AND VALUES INTO SET;

line:       select items from table { printf("Syntax Correct\n"); }
            |
            select items from table where condition { printf("Syntax Correct\n"); }
            |
            insert into table columns VALUES values { printf("Syntax Correct\n"); }
            |
            update table set_values condition where condition { printf("Syntax Correct\n"); };

select:     SELECT          { request->command_code = COMMAND_CODE_SELECT; };
update:     UPDATE          { request->command_code = COMMAND_CODE_UPDATE; };
insert:     INSERT          { request->command_code = COMMAND_CODE_INSERT; };
table:      IDENTIFIER
    {
        request->table_name = malloc(strlen(yytext));
        strncpy(request->table_name, yytext, strlen(yytext));
    };

into:       INTO;

items:      '*' |
            identifiers;

columns:    '(' identifiers ')'

values:     identifiers;

identifiers: IDENTIFIER |
            IDENTIFIER ',' identifiers;

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
