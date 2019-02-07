/*
$ cat Makefile
main: main.c
    gcc -Wall -Wextra -std=c99 -pedantic -o main main.c -lm

Uruchamiac np.:
./main example0.tga 0.tga 8

*/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>

#define MIN3(a,b,c) (((a)<(b)) ? (((a)<(c)) ? (a) : (c)) : (((b)<(c)) ? (b) : (c)))
#define MAX3(a,b,c) (((a)>(b)) ? (((a)>(c)) ? (a) : (c)) : (((b)>(c)) ? (b) : (c)))


void get_division(unsigned char* quant, int bits){                  //funkcja wyznaczajaca tablice-funkcje, ktora przypisuje reprezentanta kolorowi 
    int pow = 1 << bits;                                            //liczba liczb na bits bitach
    //printf("%d\n", pow);
    int* divs=malloc(sizeof(int)*(pow+2));                          //n przedzialow wymaga n+2 punktow (przyklad: 0 przedzialow (czyli de facto jeden przedzial [0,255] to 2 punkty). Na stercie, bo zalezy do zmiennej i na 21 bitach dla stosu by sie wysypalo
    divs[0]=0;                                                      //pierwszy punkt to zawsze 0, a ostatni 255
    
    for(int i=0; i<pow; i++){                                       //for pow razy -> jeden przebieg generuje 1 przedzial
        divs[i+1]=(i+1)*256/(pow);                                  //wyznaczenie wartosci na koncu przedzialu
        for(int j=divs[i]; j<divs[i+1]; j++)                        //dla wszystkich j nalezacych do tego przedzialu
            quant[j]=(unsigned char) ((divs[i]+divs[i+1])/2);       //przypisz wynikowej tablicy wartosc reprezentatna (polowa przedzialu)
    }     
    free(divs);
}

int main(int argc, char** argv){
    if (argc!=4) {printf("Uzywac:\n\t$ ./main <plik ze zdjeciem> <plik wynikowy> <liczba bitow na piksel>"); return 1;}

    FILE* image = fopen(argv[1], "r");
    if(!image) {printf("Czy podales wlasciwy plik wejsciowy? (%s)\n", argv[1]); return 1;}
    
    unsigned char header[18];                       //naglowek ma 18 bajtow, ciekawskich odsylam do https://en.wikipedia.org/wiki/Truevision_TGA#Header
    fread(&header,1,18,image);                       //fread wczytuje 1*18 (rozmiar*liczba) 'surowych' bajtow pod dany adres
    int width = header[12]+256*header[13];          //na tych 2 bajtach znajduje sie szerokosc zdjecia, wiec ja obliczamy
    int height = header[14]+256*header[15];         //to samo z wysokoscia
    
    //unsigned char map[height+1][width+1][3];     
    unsigned char*** map = malloc(sizeof(unsigned char**)*height);    //trzy wymiary: Wysokosc, Szerokosc, Kolor (RGB = 0-2)
    unsigned char*** map2 = malloc(sizeof(unsigned char**)*height); 
    for(int i=0; i<height; i++){                                            
        map[i]=malloc(sizeof(unsigned char*)*width);  
        map2[i]=malloc(sizeof(unsigned char*)*width); 
        for(int j=0; j<width; j++){
            map[i][j]=malloc(sizeof(unsigned char)*3);  
            map2[i][j]=malloc(sizeof(unsigned char)*3);  
        }
    }

    
    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            fread(&map[h][w][0],3,1,image);                         //szybkie wczytanko obrazka do pamieci
        }
    }
    
    unsigned char quants[3][256];                                   //tablica-funkcja, ktora bajtowi (0-255) przyporzadkowuje mu jego reprezentanta wedle podanej liczby bitow na kolor (generowana przez funkcje get_division)
    
    int bestMseBits[3]={-1,-1,-1};                                  //najlepszy podzial dla minimalnego z maksymalnego bledu sredniokwadratowego z kazdego koloru               
    int bestSnrBits[3]={-1,-1,-1};                                  //najlepszy podzial dla maksymalnego z minimalnego stosunku sygnalu do szumu z kazdego koloru
 
    double  bestMseAll,                                             //blad sredniokwadratowy calego obrazu dla min MSE       
            bestSnrAll,                                             //stosunek sygnalu do szumu calego obrazu dla maks SNR
            mseForSnr,                                              //MSE calego obrazu dla maks SNR    
            snrForMse,                                              //SNR calego obrazu dla min MSE
            bestMsePerColor[3]={INFINITY, INFINITY, INFINITY},      //MSE poszczegolnych kolorow dla min MSE
            bestSnrPerColor[3]={0.0, 0.0, 0.0},                     //SNR poszczegolnych kolorow dla maks SNR
            snrOfBestMse[3],                                        //SNR poszczegolnych kolorow dla min MSE
            mseOfBestSnr[3];                                        //MSE poszczegolnych kolorow dla maks SNR

    
    int sum=atoi(argv[3]);

    for(int r=0; r<=sum; r++){
        for(int g=0; g<=sum-r; g++){
            int b=sum-r-g;
            
            get_division(quants[0], r);         //obciecie koloru czerwonego do r bitow
            get_division(quants[1], g);         //obciecie koloru zielonego do g bitow
            get_division(quants[2], b);         //obciecie koloru niebieskiego do b bitow
             
            long int mseAllCount=0, snrAllCount=0, mseCount[3]={0, 0, 0}, snrCount[3]={0, 0, 0};    //liczniki dla MSE i SNR
            
            for(int h=0; h<height; h++){
                for(int w=0; w<width; w++){
                    for(int i=0; i<3; i++){
                        map2[h][w][i]=quants[i][map[h][w][i]];              //przepuszczamy kolor piksla przez kwantyzacje
                        
                        int x=map[h][w][i];
                        int y=map2[h][w][i];
                        
                        mseAllCount+=(x-y)*(x-y);           //suma (x_n - y_n)^2
                        snrAllCount+=x*x;                   //sume (x_n)^2
                        
                        mseCount[i]+=(x-y)*(x-y);           //suma (x_n - y_n)^2 dla danego koloru
                        snrCount[i]+=x*x;                   //sume (x_n)^2 dla danego koloru
                    }           
                }
            }   
            double tempMsePerColor[3], tempSnrPerColor[3];  //obecne MSE i SNR dla poszczegolnych kolorow
            
            
            for(int i=0; i<3; i++){
                tempMsePerColor[i]=(double)mseCount[i]/(double) (height*width);
                tempSnrPerColor[i]=(double)snrCount[i]/(double) mseCount[i];
            }
            
            /*printf("MSE= %ld %ld %ld\n",mseCount[0],mseCount[1],mseCount[2]);
            printf("SNR= %ld %ld %ld\n",snrCount[0],snrCount[1],snrCount[2]);
            printf("Temp MSE= %f %f %f\n",tempMsePerColor[0],tempMsePerColor[1],tempMsePerColor[2]);
            printf("Temp SNR= %f %f %f\n\n",tempSnrPerColor[0],tempSnrPerColor[1],tempSnrPerColor[2]);*/
            
            
            //Jeżeli maksymalny blad sredniokwadratowy z obecnych kolorow jest MNIEJSZY niz maksymalny blad sredniokwadratowy z kolorow w dotychczasowym faworycie
            if (MAX3(tempMsePerColor[0],tempMsePerColor[1],tempMsePerColor[2]) < MAX3(bestMsePerColor[0],bestMsePerColor[1],bestMsePerColor[2])){
                bestMseBits[0]=r; bestMseBits[1]=g; bestMseBits[2]=b;           //podmieniamy rozklad bitow na kolory
                
                for(int i=0; i<3; i++){
                    bestMsePerColor[i]=tempMsePerColor[i];                      //podmieniamy MSE i SNR dla poszczegolnych kolorow
                    snrOfBestMse[i]=tempSnrPerColor[i];
                }
                
                bestMseAll=(double) mseAllCount / (double) (3.0*height*width);  //podmieniamy MSE i SNR dla calego obrazu
                snrForMse=(double) snrAllCount / (double) mseAllCount;
            }
            
            //Jeżeli minimalny stosunek sygnalu do szumu z obecnych kolorow jest WIĘKSZY niz minimalny stosunek sygnalu do szumu z kolorow w dotychczasowym faworycie
            if (MIN3(tempSnrPerColor[0],tempSnrPerColor[1],tempSnrPerColor[2]) > MIN3(bestSnrPerColor[0],bestSnrPerColor[1],bestSnrPerColor[2])){
                bestSnrBits[0]=r; bestSnrBits[1]=g; bestSnrBits[2]=b;               //podmieniamy rozklad bitow na kolory
                
                for(int i=0; i<3; i++){
                    bestSnrPerColor[i]=tempSnrPerColor[i];                          //podmieniamy MSE i SNR dla poszczegolnych kolorow
                    mseOfBestSnr[i]=tempMsePerColor[i];
                }
                
                bestSnrAll=(double) snrAllCount / (double) mseAllCount;             //podmieniamy MSE i SNR dla calego obrazu     
                mseForSnr=(double) mseAllCount / (double) (3.0*height*width);
            }
        }
    }
    
    
    unsigned char footer[26];     
    fread(&footer,1,26,image);
    fclose(image);
    
    //MSE
    for(int i=0; i<3; i++)
        get_division(quants[i],bestMseBits[i]);        //ustalamy podzial zwyciezcy w min MSE


    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            for(int i=0; i<3; i++){
               map2[h][w][i]=quants[i][map[h][w][i]];   //generujemy obrazek zwyciezcy w min MSE
            }           
        }
    }
    
    
    printf("Podzial MSE: %d %d %d\n\n",bestMseBits[0],bestMseBits[1],bestMseBits[2]);           //wypisujemy zapamietane statystyki
    printf("MSE   =%f\nMSE(r)=%f\nMSE(g)=%f\nMSE(b)=%f\n\n",bestMseAll, bestMsePerColor[0],bestMsePerColor[1],bestMsePerColor[2]);
    printf("SNR   =%f (%f dB)\nSNR(r)=%f (%f dB)\nSNR(g)=%f (%f dB)\nSNR(b)=%f (%f dB)\n\n",snrForMse, 10.0*log10(snrForMse), snrOfBestMse[0],10.0*log10(snrOfBestMse[0]),snrOfBestMse[1],10.0*log10(snrOfBestMse[1]),snrOfBestMse[2],10.0*log10(snrOfBestMse[2]));
    
    
    char mse_file[strlen(argv[2])+6];
    sprintf(mse_file, "MSE_%s", argv[2]);       //obrazek wrzucamy do pliku z przedrostkiem MSE_
    
    image = fopen(mse_file, "w");
    if(!image) {printf("Czy podales wlasciwy plik wyjsciowy? (%s)\n", argv[2]); return 1;}
   
    fwrite(&header,1,18,image);     
    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            fwrite(&map2[h][w][0],3,1,image);     
        }
    }
    fwrite(&footer,1,26,image);
    
    fclose(image);
    

    
    printf("------------------------------------\n\n");
    
    //SNR
    for(int i=0; i<3; i++)
        get_division(quants[i],bestSnrBits[i]);         //ustalamy podzial zwyciezcy w maks SNR


    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            for(int i=0; i<3; i++){
               map2[h][w][i]=quants[i][map[h][w][i]];   //generujemy obrazek wg podzialu
            }           
        }
    }
    printf("Podzial SNR: %d %d %d\n\n",bestSnrBits[0],bestSnrBits[1],bestSnrBits[2]);           //wypisujemy zapamietane statystyki
    printf("MSE   =%f\nMSE(r)=%f\nMSE(g)=%f\nMSE(b)=%f\n\n",mseForSnr, mseOfBestSnr[0],mseOfBestSnr[1],mseOfBestSnr[2]);
    printf("SNR   =%f (%f dB)\nSNR(r)=%f (%f dB)\nSNR(g)=%f (%f dB)\nSNR(b)=%f (%f dB)\n",bestSnrAll, 10.0*log10(bestSnrAll), bestSnrPerColor[0],10.0*log10(bestSnrPerColor[0]),bestSnrPerColor[1],10.0*log10(bestSnrPerColor[1]),bestSnrPerColor[2],10.0*log10(bestSnrPerColor[2]));
    
    
    char snr_file[strlen(argv[2])+6];
    sprintf(snr_file, "SNR_%s", argv[2]);           //obrazek wrzucamy do pliku z przedrostkiem SNR_
    
    image = fopen(snr_file, "w");
    if(!image) {printf("Czy podales wlasciwy plik wyjsciowy? (%s)\n", argv[2]); return 1;}
   
    fwrite(&header,1,18,image);     
    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            fwrite(&map2[h][w][0],3,1,image);       
        }
    }
    fwrite(&footer,1,26,image);
    
    fclose(image);
    
    
    
    for(int i=0; i<height; i++){                                            
        for(int j=0; j<width; j++){
            free(map[i][j]);
            free(map2[i][j]);                   //zwalniamy pamiec dla dobrego przykladu
        }
        free(map[i]);
        free(map2[i]);
    }
    free(map);
    free(map2);
    
    return 0;
}