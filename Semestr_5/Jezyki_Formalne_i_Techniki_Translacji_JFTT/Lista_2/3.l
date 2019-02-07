/*
    uruchamiac: 
    ./3 < test33.c
    ./3 -doc < stdio
*/

%{
#include<stdio.h> 
#include<string.h>
int yylex();
int com = 0;            /* flaga -> 0 = usun rowniez komentarze dokumentacyjne, 1 = zostaw komentarze dokumentacyjne */                                          
%}

%x single_line          
%x multi_line
%x doc
%x string

%option noyywrap

%%
"/*"  if(com) BEGIN(doc); else BEGIN(multi_line);                   /* poczatek wielolinijkowego komentarza - jesli jest flaga doc, to uwzglednia ja*/                                                             
"//"                        BEGIN(single_line);                     /* poczatek jednolinjkowego komentarza */
\"                          ECHO; BEGIN(string);                    /* poczatek stringa */


<string>\"                  ECHO; BEGIN(INITIAL);                   /* koniec stringa = powrot do normalnego trybu (dotychczas wszystko szlo na wyjscie) */
    
<doc>.*\n([[:blank:]]*"*".*\n)+.*"*/"   printf("/*"); ECHO; BEGIN(INITIAL);       /*
                                                                                   * Komentarz dokumentacyjny jest w tej postaci:
                                                                                   * 1. rozpoczecie = /* (i ewentualnie po nim tekst)
                                                                                   * 2. dodatnia liczba linii zaczynajacych sie od gwiazdki (poprzedzonej bialymi znakami)
                                                                                   * 3. linia konczaca w postaci <ewentualny tekst><gwiazdka><slash>
                                                                                   */

    /* <multi_line,doc> to sekcja dla warunku multi_line lub doc - jezeli ta regula powyzej nie zajdzie, to komentarz wielolinijkowy jest zjadany */
<multi_line,doc>[^*]*                                               /* usuwanie znakow do napotkania gwiazdki */
<multi_line,doc>"*"[^*/]*                                           /* usuwanie znakow, jezeli po gwiazdce nie ma '*' lub '/' */
<multi_line,doc>"*"+"/"         BEGIN(INITIAL);                     /* jezeli jest "gwiazdka+slash", to konczymy komentarz */

<single_line>.*\\\n                                                 /* jesli linia konczy sie na '\', czyli "jest lamana", to polykamy ja i operujemy na kolejnej */
<single_line>.*[^\\\n]$     BEGIN(INITIAL);                         /* jesli linia nie konczy sie na '\', to ja polykamy i konczymy komentarz */
%%

int main(int argc, char** argv) { 
    if(argc>1 && !strcmp(argv[1], "-doc")) com=1;                   /* ustawienie flagi */
    return yylex(); 
}


