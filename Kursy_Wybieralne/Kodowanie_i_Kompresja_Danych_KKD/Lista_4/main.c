/*
$ cat Makefile
main: main.c
	gcc -Wall -Wextra -std=c99 -pedantic -o main main.c -lm

Uruchamiac np.:
./main example0.tga

*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

//"nazwy" metod predykcji
const char predictions[9][11]= {
    "Brak      ",
    "W         ",
    "N         ",
    "NW        ",
    "W+N-NW    ",
    "W+(N-NW)/2",
    "N+(W-NW)/2",
    "(W+N)/2   ",
    "LOCO-I    "};


const char colors[3][6]={
    "RED  ",
    "GREEN",
    "BLUE "};


//liczenie entropii jak na liscie pierwszej
double entropy(int* count, int countAll){
    double entropy = 0;
    
    for (int i=0; i<256; i++){
        if(count[i]>0){
            double probability  =  (double) count[i] / (double) countAll;
            entropy += probability*log2(probability);                                                           
        }
    }
    
    return (entropy == 0.0) ? entropy : (-1.0)*entropy;
}


//zliczanie wszystkich bajtow obrazka po zastosowaniu danej metody predykcji i puszczenie do funkcji liczacej entropie
double entropy_prediction(unsigned char**** img, int height, int width, int prediction){
    int countAll=0;
    int count[256]={0};
    
    for(int h=1; h<=height; h++){
        for(int w=1; w<=width; w++){
            for(int i=0; i<3; i++){
                count[img[h][w][prediction][i]]++;
                countAll++;
            }
        }
    }
    
    return entropy(count, countAll);
}


//zliczanie tylko bajtow danego koloru i puszczenie do funkcji liczacej entropie
double entropy_prediction_color(unsigned char**** img, int height, int width, int prediction, int color){
    int countAll=0;
    int count[256]={0};
    
    for(int h=1; h<=height; h++){
        for(int w=1; w<=width; w++){
            count[img[h][w][prediction][color]]++;
            countAll++;
        }
    }
    
    return entropy(count, countAll);
}


int main(int argc, char** argv){
    if (argc!=2) {printf("Uzywac:\n\t$ ./main <plik ze zdjeciem>"); return 1;}

    FILE* image = fopen(argv[1], "r");
    if(!image) {printf("Czy podales wlasciwy plik? (%s)\n", argv[1]); return 1;}
    
    unsigned char header[18];                       //naglowek ma 18 bajtow, ciekawskich odsylam do https://en.wikipedia.org/wiki/Truevision_TGA#Header
    fread(header,1,18,image);                       //fread wczytuje 1*18 (rozmiar*liczba) 'surowych' bajtow pod dany adres
    int width = header[12]+256*header[13];          //na tych 2 bajtach znajduje sie szerokosc zdjecia, wiec ja obliczamy
    int height = header[14]+256*header[15];         //to samo z wysokoscia
    
    //unsigned char map[height+1][width+1][9][3];     
    unsigned char**** map = malloc(sizeof(unsigned char***)*(height+1));    //cztery wymiary: Wysokosc, Szerokosc, Metoda predykcji (0-8), Kolor (RGB = 0-2)
    for(int i=0; i<height+1; i++){                                          //Wysokosc i Szerokosc sa zwiekszone o 1, bo kolumne i wiersz 0 uznajemy za "tlo",    
        map[i]=malloc(sizeof(unsigned char**)*(width+1));                   //ktore jest w kolorze czarnym. Zdjecie jest w tablicy od (1,1) do (height,width)
        for(int j=0; j<width+1; j++){
            map[i][j]=malloc(sizeof(unsigned char*)*9);
            for(int k=0; k<9; k++){
                map[i][j][k] = calloc(3, sizeof(unsigned char));             //calloc, czyli malloc z zerowaniem pamieci - przydatnie
            }
        }
    }
             
    
    for(int h=1; h<=height; h++){
        for(int w=1; w<=width; w++){
            fread(&map[h][w][0][0],3,1,image);      //Brak predykcji = X, wczytujemy piksel z pliku

            for(int i=0; i<3; i++){
                int X = map[h][w][0][i];          //Tutaj sobie do intow wkladamy kolory oryginalnego obrazka: 
                int W = map[h][w-1][0][i];        //obecny, zachodni, polnocny i polnocno-zachodni
                int N = map[h-1][w][0][i];        //nizej robimy operacje na intach, zeby nas nie obchodzili over/underflowy, a na koniec rzutujemy na char
                int NW= map[h-1][w-1][0][i];
                
                
                map[h][w][1][i]=(unsigned char) (X-W);        //tutaj 7 metod predykcji https://en.wikipedia.org/wiki/Lossless_JPEG#Lossless_mode_of_operation
                map[h][w][2][i]=(unsigned char) (X-N);        //kodujemy roznice miedzy oryginalnym kolorem a tym, co obliczylismy
                map[h][w][3][i]=(unsigned char) (X-NW);
                map[h][w][4][i]=(unsigned char) (X-(W+N-NW));       
                map[h][w][5][i]=(unsigned char) (X-(W+(N-NW)/2));                                                                 
                map[h][w][6][i]=(unsigned char) (X-(N+(W-NW)/2));       
                map[h][w][7][i]=(unsigned char) (X-(W+N)/2);                                                                                     
                
                
                map[h][w][8][i]=(unsigned char) (X - ((NW >= MAX(W,N)) ? MIN(W,N) : ((NW <= MIN(W,N)) ? MAX(W,N) : N+W-NW)));  //oraz LOCO-I:
                
                /* LOCO-I: https://en.wikipedia.org/wiki/Lossless_JPEG#Decorrelation/prediction
                 
                       { min(W,N) if NW >= max(W,N)
                  X' = { max(W,N) if NW <= min(W,N)
                       { N+W-NW   otherwise
                 */            
            }           
        }
    }
    
    /*unsigned char footer[26];      //w plikach pana sa jeszcze 26-bajtowe footery: https://en.wikipedia.org/wiki/Truevision_TGA#File_footer_(optional)
    fread(&footer,1,26,image);*/     //ale w tym programie nie musimy wyrzucac zdjecia, wiec nie musimy ich nawet wczytywac
    fclose(image);
    
        
    double minEntropy=9.0;          //liczenie minimalnej entropii obrazka wsrod wszystkich metod
    int min=9; 
    for(int i=0; i<9; i++){
        double entropy=entropy_prediction(map,height,width,i);
        if (entropy < minEntropy){
            minEntropy=entropy;
            min=i;
        }
        printf("Metoda predykcji = %s -> Entropia = %f\n", predictions[i], entropy);
    }
    
    
    printf("\n\n");
    double minColorEntropy[3]={9.0, 9.0, 9.0};      //liczenie minimalnej entropii koloru w calym obrazku wsrod wszystkich metod
    int min2[3]={9,9,9};
    for(int i=0; i<9; i++){
        for(int j=0; j<3; j++){
            double entropy=entropy_prediction_color(map,height,width,i,j);
            if (entropy < minColorEntropy[j]){
                minColorEntropy[j]=entropy;
                min2[j]=i;
            }
            printf("Metody predykcji = %s, Kolor = %s -> Entropia = %f\n",predictions[i], colors[j], entropy);
        }
    }
    
    printf("\n\n");
    printf("Optymalna metoda dla calego obrazu to %s -> Entropia = %f\n",predictions[min],minEntropy);
    for(int i=0; i<3; i++)
        printf("Optymalna metoda dla koloru %s  to %s -> Entropia = %f\n",colors[i],predictions[min2[i]],minColorEntropy[i]);
    
    return 0;
}