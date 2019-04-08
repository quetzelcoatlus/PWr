/*
    Nazwa: comp_lexer.l
    Opis: Lekser (flex) 
    Autor: Jan Brzeżański
    Indeks: 236814
*/

%option noyywrap
%option nounput
%option noinput

%{
#include <string.h>

#include "comp_y.h"
#include "comp_lib.h"

int line=1;
int pos=1; 
%}

%s declare
%s for_loop

pid [_a-z]+

%%
 
[ \t]+          { pos+=yyleng; }
\n              { line++; pos=1; }
\[[^]]*\]       { pos+=yyleng; }

"DECLARE"       { pos+=yyleng; BEGIN(declare); return DECLARE; }
"IN"            { pos+=yyleng; BEGIN(INITIAL); return IN; }
"END"           { pos+=yyleng; return END; }
"IF"            { pos+=yyleng; return IF; }
"THEN"          { pos+=yyleng; return THEN; }
"ELSE"          { pos+=yyleng; return ELSE; }
"ENDIF"         { pos+=yyleng; return ENDIF; }
"WHILE"         { pos+=yyleng; return WHILE; }
"DO"            { pos+=yyleng; return DO; }
"ENDWHILE"      { pos+=yyleng; return ENDWHILE; }
"ENDDO"         { pos+=yyleng; return ENDDO; }
"FOR"           { pos+=yyleng; BEGIN(for_loop); return FOR; }
"FROM"          { pos+=yyleng; BEGIN(INITIAL); return FROM; }
"TO"            { pos+=yyleng; return TO; }
"DOWNTO"        { pos+=yyleng; return DOWNTO; }
"ENDFOR"        { pos+=yyleng; return ENDFOR; }
"READ"          { pos+=yyleng; return READ; }
"WRITE"         { pos+=yyleng; return WRITE; }

":="            { pos+=yyleng; return IS; }

"+"             { pos+=yyleng; return ADD; }
"-"             { pos+=yyleng; return SUB; }
"*"             { pos+=yyleng; return MUL; }
"/"             { pos+=yyleng; return DIV; }
"%"             { pos+=yyleng; return MOD; }

"="             { pos+=yyleng; return EQ; }
"!="            { pos+=yyleng; return NEQ; }
"<"             { pos+=yyleng; return LT; }
">"             { pos+=yyleng; return GT; }
"<="            { pos+=yyleng; return LEQ; }
">="            { pos+=yyleng; return GEQ; }

"("             { pos+=yyleng; return LBR; }
")"             { pos+=yyleng; return RBR; }
":"             { pos+=yyleng; return COL; }
";"             { pos+=yyleng; return SEM; }

<declare>{pid}  { pos+=yyleng; if(check_symbol(yytext) < 0) { yylval.val=add_symbol(yytext); return PID; } else { fprintf(stderr,"Blad: Druga deklaracja: %s w linii %d, kolumnie %d\n",yytext,line,pos);  exit(1); } }
<for_loop>{pid} { pos+=yyleng; if(check_declared(yytext) < 0) { yylval.val=add_iterator(yytext); return PID; } else { fprintf(stderr,"Blad: Druga deklaracja: %s w linii %d, kolumnie %d\n",yytext,line,pos); exit(1); } }
{pid}           { pos+=yyleng; int res; if( (res=check_variable(yytext)) < 0) { fprintf(stderr,"Blad: Uzycie niezadeklarowanej zmiennej: %s w linii %d, kolumnie %d\n",yytext,line,pos); exit(1); } else { yylval.val=res; return PID; } }

[0-9]+          { pos+=yyleng; yylval.val=add_number(yytext); return NUM; }

.               { fprintf(stderr,"Blad: Niespodziewany symbol: %s w linii %d, kolumnie %d.\n",yytext,line, pos); pos+=yyleng; exit(1);}
%%
