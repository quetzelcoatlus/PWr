#include <stdio.h>
#include <time.h>
#include <stdlib.h>

_Bool next_bool(double p){     //funkcja zwracajaca prawde z prawdopodobienstwem p ( https://stackoverflow.com/a/3771585 )
    return rand() <  p * ((double)RAND_MAX + 1.0);
}

int main(int argc, char** argv){
    if(argc < 2) return 1;
    srand(time(NULL));  
      
    double p = atof(argv[1]);   //prawdopodobienstwo zamiany bitu
    FILE* in  = argc > 2 ? fopen(argv[2], "r") : stdin;      //tu tez mozna potokami
    FILE* out = argc > 3 ? fopen(argv[3], "w") : stdout;
    
    int c;
    while((c=fgetc(in)) != EOF){    //dopoki sa bajty na wejsciu
        for(int i=0; i<8; i++)      //dla kazdego bitu bajta: 
            if(next_bool(p)) 
                c ^= (1 << i);      //zamieniamy i-ty bit na przeciwny (XOR)
        
        fputc(c, out);   //wypluwamy zmieniony (badz nie) bajt
    }   
    
    if(in != stdin) fclose(in);
    if(out != stdout) fclose(out);
    
    return 0;
}