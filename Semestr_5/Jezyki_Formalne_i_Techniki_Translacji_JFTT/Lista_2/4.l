/*
    uruchamiac:
    ./4 < test4
*/

%{
#include<stdio.h> 

#define STACK_SIZE 50
    
int stack[STACK_SIZE];          /* stos, do ktorego wrzucamy wczytane liczby */
int i=0;                        /* 'wskaznik' na pierwsze wolne miejsce stosu */
%}

%option noyywrap

/* czy mozna wykonac dzialanie? condition typu s - bedzie rowniez obslugiwac reguly INITIAL - wczytywanie kolejnych liczb, enter, blanki, niedozwolony znak */
%s capable   

/* error - przerywa prace programu, wyrzuca wszystko do konca linii i zaczyna nowa historie */
%x error                        

%%
[1-9][0-9]*         stack[i++]=atoi(yytext); if(i>1) BEGIN(capable);                            /* wczytujemy liczbe dodatnia*/
"-"[1-9][0-9]*      stack[i++]=atoi(yytext); if(i>1) BEGIN(capable);                            /* wczytujemy liczbe ujemna  */
"0"                 stack[i++]=0;            if(i>1) BEGIN(capable);                            /* nie zapominamy o zerze :) */

<capable>"+"        stack[i-2]+=stack[i-1];  i--; if(i<2) BEGIN(INITIAL);                       /* dodawanie */
<capable>"-"        stack[i-2]-=stack[i-1];  i--; if(i<2) BEGIN(INITIAL);                       /* odejmowanie */
<capable>"*"        stack[i-2]*=stack[i-1];  i--; if(i<2) BEGIN(INITIAL);                       /* mnozenie */

<capable>"/" {                                                                                  /* dzielenie calkowite */
    if(stack[i-1]){ 
        stack[i-2]/=stack[i-1]; 
        i--; 
        if(i<2) BEGIN(INITIAL); 
    }else{ 
        printf("Blad: dzielenie przez 0\n"); 
        BEGIN(error); 
    } 
}

<capable>"%" {                                                                                  /* modulo */     
    if(stack[i-1]){ 
        stack[i-2]%=stack[i-1]; 
        i--; 
        if(i<2) BEGIN(INITIAL); 
    }else{ 
        printf("Blad: niedozwolona operacja X modulo 0\n"); 
        BEGIN(error); 
    } 
}

<capable>"^" {                                                                                  /* potega */    
    int temp=stack[i-2];
    stack[i-2]=1;
    for(int j=0; j<stack[i-1]; j++)
        stack[i-2]*=temp;
    i--;
    if(i<2) BEGIN(INITIAL);
}


[+\-*/^%]       printf("Blad: za mala liczba argumentow\n"); BEGIN(error);                      /* dostalismy operator w trybie INITIAL = uncapable */

"\n" {                                                              /* enter = koniec linii i wyrazenia */            
    if(i!=1)                                                        /* jezeli wskaznik nie jest na 1 = w stosie jest wiecej niz 1 liczba */
        printf("Blad: za mala liczba operatorow\n");
    else
        printf("= %d\n",stack[0]);                                  /* a jak nie, to wypisujemy wynik */
    i=0;                                                            /* resetujemy stos (tak naprawde tylko 'wskaznik', smieci zostaja w tablicy, ale nas to nie obchodzi xD) */
    BEGIN(INITIAL);                                                 /* i resetujemy stan */
}

[[:blank:]]+                                                        /* wszystkie blanki pomijamy */
    
<error>.*\n     i=0; BEGIN(INITIAL);                                /* jezeli jest error to pomijamy wszystkie znaki do entera (razem z nim), i resetujemy wskaznik i tryb */

.       printf("Blad: zly symbol \"%s\"\n",yytext); BEGIN(error);   /* inne znaki sa niedozwolone, pierwsze napotkanie powoduje wypisanie bledu i wejscie w error */
%%

   
int main() { 
    yylex();
}