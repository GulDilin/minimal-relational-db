%{
#include <stdio.h>
#include "y.tab.h"
#include "../../common/net.pb-c.h"
extern char *yytext;
char *ctext;
extern int yyval;
%}

%%
select					return SELECT;
create					return CREATE;
insert					return INSERT;
update					return UPDATE;
delete					return DELETE;
table					return TABLE;
number					return NUMBER_TYPE;
text					return TEXT_TYPE;
from					return FROM;
where					return WHERE;
and						return AND;
\*						return ALL;
\(				        return *yytext;
\)				        return *yytext;
[,]						return *yytext;
[=]						return *yytext;
[a-zA-Z][a-zA-Z0-9]*	{ printf("text: %s\n", yytext); yylval.text = yytext; ctext = yytext; return IDENTIFIER; }
[ \t\r\n;]+		        /* ignore whitespace */;
%%

void set_input_string(const char* in) {
  yy_scan_string(in);
}

void end_lexical_scan(void) {
  yy_delete_buffer(YY_CURRENT_BUFFER);
}