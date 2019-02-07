#include <stdio.h>

int pop_count(int x){       //przerobiona funkcja 'int popcount64d(uint64_t x)' z... https://en.wikipedia.org/wiki/Hamming_weight#Efficient_implementation
    int count;              
    for (count=0; x; count++)
        x &= x - 1;
    return count % 2;       //zamiast zwracac sume bitow, zwracamy ja modulo
}

int main(int argc, char** argv){
    FILE* in  = argc > 1 ? fopen(argv[1], "r") : stdin;     //mozna stdin/stdout, jak ktos chce potokami...
    FILE* out = argc > 2 ? fopen(argv[2], "w") : stdout;    //... jak nie, to pliki 'in' i 'out' z argumentow
    
    int generator[8] = {13, 11, 8, 7, 4, 2, 1, 14};         //macierz generujaca kodu Hamminga (8,4) - https://en.wikipedia.org/wiki/Hamming_code#[7,4]_Hamming_code_with_an_additional_parity_bit
      
    int c;
    while((c=fgetc(in)) != EOF){        //dopoki sa symbole do kodowania
        unsigned char a=0, b=0; 
        for(int i=0; i<8; i++){         //dla kazdej 'kolumny' macierzy generujacej
            a += (pop_count((c >> 4)  & generator[i]) << (7-i)); // 1) wyciagamy pierwsze 4 bity z bajta (c >> 4), 2) robimy iloczyn skalarny z kolumna macierzy ((c >> 4)  & generator[i]), 3) sumujemy jedynki modulo 2 (pop_count), 4) wynik ustawiamy na odpowiednie miejsce bitowe ( << (7-i) ), 5) dodajemy go do wynikowego bajta ( a += ... )
            b += (pop_count((c & 0xF) & generator[i]) << (7-i)); // to samo, co wyzej, tylko bierzemy 4 ostatnie bity liczby (c & 0xF)
        }      
        fputc(a,out);   //wypluwamy zakodowane bajty (8 bitow (=2*4) -> 16 bitow (=2*8))
        fputc(b,out);
    }  
    
    if(in != stdin) fclose(in);
    if(out != stdout) fclose(out);
    
    return 0;
}