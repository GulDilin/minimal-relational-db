%{
#include <stdio.h>
#include "y.tab.h"
#include "../../common/net.pb-c.h"
extern int yylval;
extern char *yytext;
%}

%%
select					return SELECT;
from					return FROM;
where					return WHERE;
and						return AND;
\*						return *yytext;
[,]						return *yytext;
[=]						return *yytext;
[a-zA-Z][a-zA-Z0-9]*	return IDENTIFIER;
[ \t\r\n;]+		        /* ignore whitespace */;
%%

void set_input_string(const char* in) {
  yy_scan_string(in);
}

void end_lexical_scan(void) {
  yy_delete_buffer(YY_CURRENT_BUFFER);
}