%option noyywrap

%{
#include "cal_y.h"    //to jest plik naglowkowy, ktory wygeneruje nam bison. Sluzy do tego flaga -d podczas uruchamiania go, patrz: Makefile
%}

%%

[ \t]+                              /* zjadamy blanki */
^\#(.*\\\n)*.*\n                    /* komentarz to # na poczatku potem dowolna liczba zlamanych linii, a potem do konca linii */
\\\n                                /* lamane linie zjadamy */
\n              { return END; }     /* enter zwraca token END */

[0-9]+          { yylval = atoi(yytext); return NUM; }  /*liczby zwracaja token NUM, a pod wartosc tokena podstawiamy liczbe */

\-              { return MIN; }  /* tu raczej oczywiste */
\+              { return ADD; }
\*              { return MUL; }
\/              { return DIV; }
\%              { return MOD; }
\^              { return POW; }
\(              { return LBR; }
\)              { return RBR; }

.               { return ERR; }  /* inny znak niz powyzsze generuje blad */
%%