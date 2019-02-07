%{
#include <math.h>
#include <stdio.h>
#include <string.h>   
    
int yylex();                //funkcja leksera z FLEXA
void yyerror(char*);        //funkcja erroru - trzeba podac
 
#define SIZE 1000    
char buffer[SIZE];          //tu bedziemy wkladac ONP. Ogolnie polecam robic w C++ na stringu zamiast tego xD. Bufor potrzebny, bo czasem blad moze wyjsc w trakcie analizy, wiec nie mozna sobie na biezaco wypisywac
int pos=0;                  //wskaznik na pierwsze wolne miejsce w buforze
   
int myerror=0;                      //flaga erroru
const char myerrors[][25]={        //errory
    "No errors",
    "Dzielenie przez zero.",
    "Modulo przez zero."
};
    
int mymod(int a, int b){                    //modulo, ktore zwraca dodatnia reszte
    if (b==0) {myerror=2; return 0;}
    int ret = a%b;
    return (ret < 0) ? ret+b : ret;
    
}    
    
int mydiv(int a, int b){                    //dzielenie, ktore zaokragla do mniejszej liczby dla ujemnych
    if (b==0) {myerror=1; return 0;}
    int ret = a/b;
    return (ret < 0) ? ret - ((mymod(a,b) == 0) ? 0 : 1) : ret;   //patrzymy, czy wynik dzielenia jest ujemny. Jezeli nie jest, to zwracamy go, a jezeli jest to patrzymy, czy ta liczba dzieli sie przez reszty. Jezeli tak, to zwracamy iloraz, a jezeli z reszta, to musimy odjac 1, zeby zaokraglic do najblizszej liczby w dol.
}
    

int mypow(int base, int exp){               //potegowanie (chyba szybkie)
    int result = 1;
    while (exp){
        if (exp & 1)
           result *= base;
        exp /= 2;
        base *= base;
    }
    return result;
}
    
    
%}

%token NUM                  //liczby
%token ERR                  //blad leksykalny
%token LBR                  //lewy nawias
%token RBR                  //prawy nawias
%token END                  //koniec linii = \n

%left MIN ADD               //tokeny '-' i '+'. left oznacza, ze lacznosc lewostronna (2+2+2 = 2 2+2+, a nie 2 2 2++)
%left MUL DIV MOD           //mnozenie, dzielenie, modulo
%precedence NEG             //minus unarny = "negacja"
%right POW                  //potegowanie - lacznosc prawostronna

%%

input                               //wejscie to jest:
: %empty                            //nic
| input line {pos=0; myerror=0;}    //lub wejscie i linia. Po wczytaniu kazdej linii zerujemy bufor i flage erroru
;

line                                    //linia to jest:
: END                                   //znak nowej linii
| error END {printf("Błąd.\n");}        //lub blad skladniowy ("error" jest wbudowane w bisona)
| exp END { if(!myerror) printf("%s\nWynik: %d\n",buffer,$1); else printf("%s\n",myerrors[myerror]); } //albo poprawne skladniowo wyrazenie wyrazenie - ale mamy jeszcze "swoje errory" czyli np dzielenie przez 0. Jezeli nie ma errorow, to wypisujemy bufor i wynik, a sa, to nazwe bledu
;


exp                                                                                                     //wyrazenie to jest:
: NUM                   { $$ = $1;              pos+=snprintf(buffer+pos,SIZE-pos,"%d ",$1);    }       //liczba: jako "wynik" tego dzialania podstawiamy po prostu liczbe -> $$ = $1 oraz dopisujemy do bufora "%d ", czyli te liczbe i spacje. Po raz kolejny polecam robic w C++ na stringach. xD
| exp ADD exp           { $$ = $1 + $3;         pos+=snprintf(buffer+pos,SIZE-pos,"+ ");        }       //dodawanie: robimy dodawanie argumentu 1 i 3 (2 to jest znak plusa) i wkladamy jako wynik, a do bufora dokladamy tylko "+ ". Nalezy pamietac, ze linijka wyzej generuje wszystkie liczby, wiec nie nalezy ich tutaj wypisywac
| exp MIN exp           { $$ = $1 - $3;         pos+=snprintf(buffer+pos,SIZE-pos,"- ");        }       //odejmowanie
| exp MUL exp           { $$ = $1 * $3;         pos+=snprintf(buffer+pos,SIZE-pos,"* ");        }       //mnozenie
| exp DIV exp           { $$ = mydiv($1,$3);    pos+=snprintf(buffer+pos,SIZE-pos,"/ ");        }       //dzielenie - wywolanie naszej funkcji do dzielenia
| exp MOD exp           { $$ = mymod($1,$3);    pos+=snprintf(buffer+pos,SIZE-pos,"%% ");       }       //modulo - nasza funkcja. Ciekawostka: zeby wypisac procent w printf, musimy dac %%
| MIN exp %prec NEG     { $$ = -$2;             pos+=snprintf(buffer+pos,SIZE-pos,"~ ");        }       //negacja, minus unarny/ %prec - http://dinosaur.compilertools.net/bison/bison_8.html#SEC76
| exp POW exp           { $$ = mypow ($1, $3);  pos+=snprintf(buffer+pos,SIZE-pos,"^ ");        }       //potega - nasza funkcja
| LBR exp RBR           { $$ = $2;                                                              }       //nawiasy - obslugujemy je, ale nie generuja napisu do bufora - oczywiste
;
%%


void yyerror(char *s){
    (void) s;
}

int main(void){
    yyparse();
    return 0;
}