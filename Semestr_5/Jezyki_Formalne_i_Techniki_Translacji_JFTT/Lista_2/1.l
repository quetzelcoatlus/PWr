/*
    uruchamiac:
    ./1 < test1
*/

%{
#include<stdio.h> 
int yywrap();                                           /* funkcja wykonujaca sie po zakonczeniu analizy tekstu */
int yylex();                                            /* funkcja dokonujaca analizy tekstu */
int lines=0;                                            /* licznik linii */
int words=0;                                            /* licznik slow */
%}

%%
^[[:blank:]]+                                           /* usuwa biale znaki na poczatku linii */
[[:blank:]]+$                                           /* usuwa biale znaki na koncu linii */
\n+                 printf("\n"); lines+=yyleng;        /* zamienia ciag enterow na jeden (eliminacja pustych linii) i zlicza entery = linie*/
[[:blank:]]+        printf(" ");                        /* zamienia ciag bialych znakow na jedna spacje */
[^ \t\n]+           ECHO; words++;                      /* przepisuje slowa (ciag, co najmniej jednoelementowy, kolejnych znakow roznych od spacji, tabulatora i entera) i zlicza je */
%%

int yywrap() { 
    printf("=================\nlinie = %d\nslowa=%d",lines,words); 
    return 1; 
} 
int main() { 
    return yylex(); 
}