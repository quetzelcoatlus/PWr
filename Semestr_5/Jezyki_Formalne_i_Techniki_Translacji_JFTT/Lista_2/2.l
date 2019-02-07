/*
    uruchamiac:
    ./2 < test2
*/

%{
#include<stdio.h> 
int yylex();
%}

/* condition = warunek - jezeli ustawiony przez BEGIN(nazwa_warunku), to tylko reguly z <nazwa_warunku> beda wykonywane (tak jest w przypadku %x; jest tez %s, "na' ktorym wykonuja sie tylko jego zasady oraz te bez niczego) */
%x comment 
%option noyywrap
/* noyywrap - opcja po to, zeby nie musiec deklarowac funkcji yywrap */

%%
"<!--"                  BEGIN(comment);         /* zaczecie komentarza */
 
<comment>[^-]*                                  /* usuwanie znakow dopoki nie spotka sie '-' */
<comment>"-"[^-]*                               /* usuwanie wszystkich ciagow, ktorych pierwszy znak to '-', a kolejne to znaki rozne od '-' */
<comment>"--"[^->]*                             /* usuwanie wszystkich ciagow, ktorych poczatek to '--', a kolejne to znaki rozne od '-' i '>' */
<comment>"-""-"+">"     BEGIN(INITIAL);         /* koniec komentarza - powrot do normalnego stanu = wypisywanie wszystkiego na ekran, bo nic nie pasuje */
%%

int main() { 
    return yylex(); 
}

