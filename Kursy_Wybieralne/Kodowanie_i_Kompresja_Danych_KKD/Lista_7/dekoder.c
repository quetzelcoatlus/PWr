#include <stdio.h>

int pop_count(int x){       //to samo, co w koderze
    int count;
    for (count=0; x; count++)
        x &= x - 1;
    return count % 2;
}

int main(int argc, char** argv){
    FILE* out = argc > 1 ? fopen(argv[1], "w") : stdout;    // (!) zostaly zamienione miejscami deskryptory plikow - najpierw wyjscie, potem wejscie. Jest tak dlatego, zeby ulatwic przeplyw potokowi plik -> koder -> [szum] -> dekoder -> plik. ...
    FILE* in  = argc > 2 ? fopen(argv[2], "r") : stdin;     //... Program przydziela deskryptory wedle pierwszenstwa, wiec tutaj wyjscie musi byc pierwsze, zeby mozna bylo nie podawac wejscia
    
    int parityCheck[3] = {170, 102, 30};                    //macierz parzystosci ( https://en.wikipedia.org/wiki/Hamming_code#[7,4]_Hamming_code_with_an_additional_parity_bit ). Usunelismy ostatni wiersz, poniewaz to sprawdzenie parzystosci, a to robi de facto funkcja pop_count
      
    int c[2], repairs=0, doubleBitErrors=0;                //dwa bajty kodu dadza 1 bajt informacji; zliczamy liczbe napraw i wystapien podwojnych bledow
    while((c[0]=fgetc(in)) != EOF){
        c[1]=fgetc(in);
        
        for(int i=0; i<2; i++){                 //dla kazdego z dwoch bajtow kodu:
            unsigned char a=0;
            for(int j=0; j<3; j++)
                a += (pop_count(c[i] & parityCheck[j]) << (2-j));   //liczymy syndrom z macierzy parzystosci
            
            if(pop_count(c[i])){                    //jezeli nie zgadza sie bit parzystosci:
                if(a){                              //jezeli syndrom jest niezerowy, to wskazuje pozycje w kodzie (1-7) blednego bitu
                    c[i] ^= (1 << (8-a));           //zamieniamy zly bit (8-a to miejsce 1-7 w kodzie)
                } else {                            //jezeli syndrom jest rowny 0, to winny jest bit parzystosci...
                    c[i] ^= 1;                      //... wiec zamieniamy go na przeciwny
                }
                repairs++;                          //zliczamy liczbe napraw
            } else if(a)                            //jezeli zgadza sie bit parzystosci, ale syndrom nie, to znaczy, ze wystapily dwa bledy
                doubleBitErrors++;
        }        
        // kod jest w postaci  aababbbc, gdzie 'a' to bity parzystosci z kodu Hamminga (7,4), 'b' to bity informacji, a 'c' to bit parzystosci dodany w kodzie (8,4)...
        unsigned char a = ((c[0] << 2) & (1 << 7)) + ((c[0] << 3) & 112) + ((c[1] >> 2) & (1 << 3)) + ((c[1] >> 1) & 7); //... dlatego odzyskujemy po 4 bity informacji z kazdej z dwoch liczb, ustawiamy je na dobre miejsca i wkladamy do wyniku. FYI: 112 = 01110000, 7 = 00000111
        
        fputc(a, out);  //wypluwamy bajt informacji powstaly z dwoch bajtow kodu
    }
    
    if(in != stdin) fclose(in);
    if(out != stdout) fclose(out);
    
    printf("Liczba korekcji to %d. Liczba kodow z dwoma bledami to %d\n", repairs, doubleBitErrors);
    
    return 0;
}