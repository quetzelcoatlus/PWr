/*
    Nazwa: comp_parser.y
    Opis: Parser (bison) 
    Autor: Jan Brzeżański
    Indeks: 236814
*/

%{
#include <stdio.h>
#include <stdlib.h>

#include "comp_lib.h"
    
int yylex();
void yyerror(char*);
extern int line;
extern int pos;
%}

%union{
    int val; 
    struct Command* ptr;
}

%token DECLARE IN END

%token IS
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE ENDDO 
%token FOR FROM TO DOWNTO ENDFOR
%token READ
%token WRITE 

%token ADD SUB MUL DIV MOD       

%token EQ NEQ LT GT LEQ GEQ

%token LBR RBR COL SEM

%token <val> PID
%token <val> NUM

%type <ptr> command
%type <ptr> commands
%type <ptr> identifier
%type <ptr> expression
%type <ptr> condition
%type <ptr> value

%%

program
: DECLARE declarations IN commands END {create_program($4);}
;

declarations
: %empty
| declarations PID SEM                          {set_variable($2);} 
| declarations PID LBR NUM COL NUM RBR SEM      {if(set_array($2, $4, $6) < 0) yyerror("Zly zakres tablicy");} 
;

commands
: %empty                {$$ = create_empty_command(COM_COMMANDS);}
| commands command      {$$ = add_command($1, $2);}
| commands error        {yyerror("Blad skladniowy");}  
;

command
: identifier IS expression SEM                          {$$ = create_parent_command(COM_IS,      2, $1, $3);}
| IF condition THEN commands ELSE commands ENDIF        {$$ = create_parent_command(COM_IFELSE,  3, $2, $4, $6);}   
| IF condition THEN commands ENDIF                      {$$ = create_parent_command(COM_IF,      2, $2, $4);}    
| WHILE condition DO commands ENDWHILE                  {$$ = create_parent_command(COM_WHILE,   2, $2, $4);}
| DO commands WHILE condition ENDDO                     {$$ = create_parent_command(COM_DO,      2, $2, $4);}
| FOR PID FROM value TO value DO commands ENDFOR        {$$ = create_parent_command(COM_FOR,     5, create_value_command(COM_PID, $2), create_value_command(COM_PID, $2+1),$4, $6, $8);}
| FOR PID FROM value DOWNTO value DO commands ENDFOR    {$$ = create_parent_command(COM_FORDOWN, 5, create_value_command(COM_PID, $2), create_value_command(COM_PID, $2+1),$4, $6, $8);}
| READ identifier SEM                                   {$$ = create_parent_command(COM_READ,    1, $2);}    
| WRITE value SEM                                       {$$ = create_parent_command(COM_WRITE,   1, $2);} 
;

expression
: value                 {$$ = $1;}
| value ADD value       {$$ = create_parent_command(COM_ADD, 2, $1, $3);}
| value SUB value       {$$ = create_parent_command(COM_SUB, 2, $1, $3);}
| value MUL value       {$$ = create_parent_command(COM_MUL, 2, $1, $3);}
| value DIV value       {$$ = create_parent_command(COM_DIV, 2, $1, $3);}
| value MOD value       {$$ = create_parent_command(COM_MOD, 2, $1, $3);}
;

condition
: value EQ value        {$$ = create_parent_command(COM_EQ,  2, $1, $3);}
| value NEQ value       {$$ = create_parent_command(COM_NEQ, 2, $1, $3);}
| value LT value        {$$ = create_parent_command(COM_LT,  2, $1, $3);}
| value GT value        {$$ = create_parent_command(COM_GT,  2, $1, $3);}
| value LEQ value       {$$ = create_parent_command(COM_LEQ, 2, $1, $3);}
| value GEQ value       {$$ = create_parent_command(COM_GEQ, 2, $1, $3);}
;

value
: NUM                   {$$ = create_value_command(COM_NUM, $1);}
| identifier            {$$ = $1;}
; 

identifier
: PID                   {if(!is_variable($1)) yyerror("Zle uzycie zmiennej tablicowej"); $$ = create_value_command(COM_PID, $1); }
| PID LBR PID RBR       {if(!is_array($1)) yyerror("Zle uzycie zmiennej"); if(!is_variable($3)) yyerror("Zle uzycie zmiennej tablicowej"); $$ = create_parent_command(COM_ARR, 2, create_value_command(COM_PID, $1), create_value_command(COM_PID, $3));}
| PID LBR NUM RBR       {if(!is_array($1)) yyerror("Zle uzycie zmiennej"); $$ = create_parent_command(COM_ARR, 2, create_value_command(COM_PID, $1), create_value_command(COM_NUM, $3));}
;


%%

void yyerror(char *s){
    fprintf(stderr, "Blad: %s w linii %d, kolumnie %d\n", s, line,pos);
    exit(1);
}
