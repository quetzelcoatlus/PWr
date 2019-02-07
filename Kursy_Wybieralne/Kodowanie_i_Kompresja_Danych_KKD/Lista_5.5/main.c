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
#include <time.h>

//makro wartosci bezwzglednej
#define ABS(x) (((x)<0) ? (-(x)) : (x))

//makro odleglosci w metryce taksowkowej
#define DIST(a,b) (ABS((a[0])-(b[0]))+ABS((a[1])-(b[1]))+ABS((a[2])-(b[2])))



/*

Naszym celem jest kwantyzowanie pikseli zamiast poszczegolnych kolorow, dlatego robimy, co nastepuje:
1. Otrzymujac liczbe bitow na kolor (zlozony z trzech skladowych) wiemy, ze bedzie 2^<ta liczba> kolorow w wynikowym obrazie

Metoda latwiejsza:
2. Losujemy wartosci wszystkich kolorow wynikowych
3. Idziemy po obrazie i kazdy kolor 'wrzucamy' do grupy najblizszego mu koloru wynikowego (wzgledem metryki taksowkowej: d(x,y)=|x0-y0|+|x1-y1|+|x2-y2|)
4. Jezeli jakas grupa jest pusta, to jej kolor mozna przelosowac na inny
5. W kazdej grupie wybieramy srodek ciezkosci wszystkich punktow (x_new=suma(x_i)/n ...) i oznaczamy go nowym kolorem koncowym
6. Liczymy blad sredniokwadratowy dla wyniku
7. Powtarzamy 3-6 dopoki blad wzgledny poprzedniego MSE z obecnym nie bedzie mniejszy od epsilonu

Metoda "trudniejsza":
2. Wybieramy pierwszy kolor wynikowy jako srednia z calego zbioru (punkt ciezkosci)
3. Do kazdego koloru wynikowego (na poczatku bedzie tylko jeden, potem beda potegowo liczniec) dodajemy wektor peturbacji epsilon (mala zmiana) \
   i wykonujemy LBG (kroki 3-5 wyzej)
4. Powtarzamy krok 3. do momentu osiagniecia szukanej liczby kolorow wynikowych (jezeli nie jest to potega dwojki, to w ostatnim kroku nie wszystkie podwajamy)


Potrzebne bedzie:
1. Liczenie MSE (poprzednie listy)
2. Liczenie odleglosci w metryce taksowkowej (suma trzech bezwzglednych roznic)
3. Przechowywanie kolorow w grupach

*/


struct Group{
    int represent[3];           //reprezentant danej grupy
    int size;                   //aktualny rozmiar
    int maxSize;                //maksymalny rozmiar
    int** colors;               //kolory z obrazka, ktore naleza do tej grupy
    int** positions;            //pozycje w obrazku kolorow z `colors` (potrzebne tylko po to, zeby na samym koncu naniesc kolory na obrazek)
};
   

unsigned char*** map;           //obrazek
struct Group* quants;           //grupy
int colors;                     //liczba kolorow
int width;                      //szerokosc obrazka
int height;                     //wysokosc obrazka


void add_color(int group, int color[3], int h, int w){                                          //dodanie koloru do danej grupy
    if(quants[group].size == quants[group].maxSize){                                            //poszerzanie grupy
        void* tmp= realloc(quants[group].colors, sizeof(int*)*quants[group].maxSize*2);
        void* tmp2= realloc(quants[group].positions, sizeof(int*)*quants[group].maxSize*2);
        if(!tmp || !tmp2){
            fprintf(stderr, "(!) Reallocation failed (!)\n");
            return;
        }
        //fprintf(stderr, "Reallocation succes :)\n");
        quants[group].maxSize*=2;
        quants[group].colors=tmp;
        quants[group].positions=tmp2;
        
        for(int i=quants[group].size; i<quants[group].maxSize; i++){
            quants[group].colors[i]=malloc(sizeof(int)*3);
            quants[group].positions[i]=malloc(sizeof(int)*2);
        }
    }
    
    for(int i=0; i<3; i++){                                                 //dodanie koloru z obrazka oraz jego pozycji w obrazku                  
        quants[group].colors[quants[group].size][i]=color[i];
    }
    quants[group].positions[quants[group].size][0]=h;
    quants[group].positions[quants[group].size][1]=w;
    
    quants[group].size++;
}

void group(){                               //wypelnianie grup
    int color[3];
    int min_dist;
    int group;
    for(int i=0; i<height; i++){
        for(int j=0; j<width; j++){             //idziemy po calym obrazku
            for(int k=0; k<3; k++)
                color[k]=map[i][j][k];
            
            min_dist=1000;
            group=-1;
            
            for(int k=0; k<colors; k++){                    //dla kazdej grupy patrzymy, do ktorej mamy najblizej
                int dist=DIST(color, quants[k].represent);
                //printf("distance from {%d, %d, %d} to {%d, %d, %d} is %d\n",color[0],color[1],color[2],quants[k].represent[0],quants[k].represent[1],quants[k].represent[2], dist);
                if(dist < min_dist){
                    min_dist= dist;
                    group=k;
                }
            }
            
            add_color(group, color, i, j);                  //dodajemy kolor do grupy do ktorej mamy najblizej
        }
    }
}

double get_group_mse(int group){                    //obliczenie MSE dla danej grupy
    int mseCount=0;   
    for(int j=0; j<quants[group].size; j++){
            for(int k=0; k<3; k++)
                mseCount+=(quants[group].represent[k]-quants[group].colors[j][k])*(quants[group].represent[k]-quants[group].colors[j][k]);
    }
    
    return (double) mseCount / (double) (3.0*height*width);;
}

double get_mse(){                               //obliczenie MSE dla wszystkich grup
    int mseCount=0;
    for(int i=0; i<colors; i++){
        for(int j=0; j<quants[i].size; j++){
            for(int k=0; k<3; k++)
                mseCount+=(quants[i].represent[k]-quants[i].colors[j][k])*(quants[i].represent[k]-quants[i].colors[j][k]);
        }
        //printf("count =%d, size =%d\n",mseCount, quants[i].size);
    }
    return (double) mseCount / (double) (3.0*height*width);;
}

double get_snr(){                       //obliczenie SNR dla wszystkich grup
    int mseCount=0;
    int snrCount=0;
    for(int i=0; i<colors; i++){
        for(int j=0; j<quants[i].size; j++){
            for(int k=0; k<3; k++){
                mseCount+=(quants[i].represent[k]-quants[i].colors[j][k])*(quants[i].represent[k]-quants[i].colors[j][k]);
                snrCount+=(quants[i].colors[j][k])*(quants[i].colors[j][k]);
            }
        }
        //printf("count =%d, size =%d\n",mseCount, quants[i].size);
    }
    return (double) snrCount / (double) mseCount; 
}

void recalculate(){                                         //przeliczenie grup w kazdej iteracji
    for(int i=0; i<colors; i++){                            //idziemy po wszystkich grupach
        if(quants[i].size == 0){                            //jezeli dana grupa jest pusta to:
           /* for(int j=0; j<3; j++){
                quants[i].represent[j]=rand()%256;          //podejscie 1) wylosuj jej nowy kolor jako reprezentanta
                printf("Losu losu\n");
            }*/
            int color=0;
            /* int best_size=0;
            for(int j=0; j<colors; j++){
                if(quants[j].size>best_size){
                    //printf("best size=%d\n",best_size);   // podejscie 2) znajdzmy grupe, ktora ma najwiecej czlonkow i podzielmy ja 
                    best_size=quants[j].size;
                    color=j;
                }
            }*/
            
            double highest_mse=0.0;
            for(int j=0; j<colors; j++){
                double mse=get_group_mse(j);                //podejscie 3) (po radzie pana Gębali) zamiast najliczniejszej grupy, wezmy te grupe, ktora ma najwiekszy blad sredniokwadratowy
                if(mse>highest_mse){
                    //printf("best size=%d\n",best_size);
                    highest_mse=mse;
                    color=j;
                }
            }
            
            for(int j=0; j<3; j++){
                //printf("I change at %d %d for %d\n",i,quants[i].represent[j],quants[color].represent[j]+1);
                quants[i].represent[j]=quants[color].represent[j]+1;        //jak juz znalezlismy grupe, ktora dzielimy, to dodajemy do jej reprezentanta po 1 w kazdym wymiarze (wektor peturbacji) i taki kolor przyjmujemy dla naszej pustej grupy
                if(quants[i].represent[j] == 256)                           //jezeli przekroczymy zakres, to zamiast +1 robimy -1
                    quants[i].represent[j]=254;
                //printf("got it for best size = %d and color =%d is {%d, %d, %d}\n",best_size, color, quants[i].represent[0],quants[i].represent[1],quants[i].represent[2]);
            }
        } else {                                                            //jezeli grupa nie jest pusta
            int sum[3]={0,0,0};
            for (int j=0; j<quants[i].size; j++){
                for(int k=0; k<3; k++)
                    sum[k]+=quants[i].colors[j][k];                         //to wyliczamy srodek ciezkosci (srednia po wspolrzednch)
            }
            //printf("Old representant for quants[%d] is {%d, %d, %d}, ",i,quants[i].represent[0],quants[i].represent[1],quants[i].represent[2]);
            for(int j=0; j<3; j++){
                quants[i].represent[j]=sum[j]/quants[i].size;               //i ustawiamy go jako nastepnego reporezentanta
            }
            //printf("new is {%d, %d, %d}\n",quants[i].represent[0],quants[i].represent[1],quants[i].represent[2]);
            quants[i].size=0;
        }          
    }
}

void draw(){                                        //rysowanie, czyli przeniesienie wszystkich kolorw z grup na obrazek
    for(int i=0; i<colors; i++)
        for(int j=0; j<quants[i].size; j++)
            for(int k=0; k<3; k++)
                map[quants[i].positions[j][0]][quants[i].positions[j][1]][k]=quants[i].represent[k];
}

int main(int argc, char** argv){
    srand(time(NULL));
    if (argc!=4) {printf("Uzywac:\n\t$ ./main <plik ze zdjeciem> <plik wynikowy> <liczba bitow na piksel>"); return 1;}

    FILE* image = fopen(argv[1], "r");
    if(!image) {printf("Czy podales wlasciwy plik wejsciowy? (%s)\n", argv[1]); return 1;}
    
    unsigned char header[18];                           //naglowek ma 18 bajtow, ciekawskich odsylam do https://en.wikipedia.org/wiki/Truevision_TGA#Header
    fread(&header,1,18,image);                          //fread wczytuje 1*18 (rozmiar*liczba) 'surowych' bajtow pod dany adres
    width = header[12]+256*header[13];                  //na tych 2 bajtach znajduje sie szerokosc zdjecia, wiec ja obliczamy
    height = header[14]+256*header[15];                 //to samo z wysokoscia
    
    
    //unsigned char map[height+1][width+1][3];     
    map = malloc(sizeof(unsigned char**)*height);    //trzy wymiary: Wysokosc, Szerokosc, Kolor (RGB = 0-2)
    for(int i=0; i<height; i++){                                            
        map[i]=malloc(sizeof(unsigned char*)*width);  
        for(int j=0; j<width; j++){
            map[i][j]=malloc(sizeof(unsigned char)*3);  
        }
    }     
    //unsigned char map[height][width][3];
    
    
    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            fread(&map[h][w][0],1,3,image);                
        }
    }
    
    unsigned char footer[26];     
    fread(&footer,1,26,image);
    fclose(image);
    
    
    colors= 1 << atoi(argv[3]);
    
    //printf("%d ", colors);
    
    //struct Group quants[colors];
    quants=malloc(sizeof(struct Group)*colors);
    
    
    for(int i=0; i<colors; i++){
        for(int j=0; j<3; j++)
            quants[i].represent[j]=rand()%256;
        quants[i].size=0;
        quants[i].maxSize=32;
        quants[i].colors=malloc(sizeof(int*)*32);
        quants[i].positions=malloc(sizeof(int*)*32);
        for(int j=0; j<32; j++){
            quants[i].colors[j]=malloc(sizeof(int)*3);
            quants[i].positions[j]=malloc(sizeof(int)*2);
        }
    }

    double eps=1.0/(1<<25), d_old=0.0, d_new=0.0;       //ustalamy epsilon na 2^(-25). d_old i d_new to znieksztalcenia
    //printf("%f ",eps);
    
    group();        //grupowanie
    d_new=get_mse(); //zsumuj MSE grup do d_new
    do {
        d_old=d_new;
        recalculate(); //jezeli grupa jest pusta, przelosuj (w nowej wersji juz nie). Jak nie, to zamien kolor na srodek ciezkosci  
        group();
        d_new=get_mse(); // zsumuj MSE nowych grup do d_new
        //printf("d_old = %f, d_new = %f, blad = %f, eps =%f\n",d_old,d_new,ABS((d_new-d_old)/d_new),eps);
    } while (ABS((d_new-d_old)/d_new) > eps);       //sprawdzenie, czy blad wzgledny jest mniejszy niz nasz epsilon
    
    double snr=get_snr();
    
    //printf("Wyszlem\n");
    draw();     //przeniesienie kolorow na obrazek
    //wstaw w miejsce kolorow obrazka reprezentanta grupy, do ktorej naleza i wypluj go
    //wypisz MSE i SNR dla wynikowego obrazka
    
    for(int i=0; i<colors; i++)
        printf("Color = {%3d, %3d, %3d} Members = %d\n", quants[i].represent[0], quants[i].represent[1], quants[i].represent[2], quants[i].size);
    
    printf("\nMSE = %f\nSNR = %f\n", d_new,snr);
   
    
    image = fopen(argv[2], "w");
    if(!image) {printf("Czy podales wlasciwy plik wyjsciowy? (%s)\n", argv[2]); return 1;}
   
    fwrite(&header,1,18,image);     
    for(int h=0; h<height; h++){
        for(int w=0; w<width; w++){
            fwrite(&map[h][w][0],1,3,image);     
        }
    }
    fwrite(&footer,1,26,image);
    
    fclose(image);
    
    
    for(int i=0; i<height; i++){                                            
        for(int j=0; j<width; j++){
            free(map[i][j]);
        }
        free(map[i]);
    }
    free(map);
    
    for(int i=0; i<colors; i++){
        for(int j=0; j<quants[i].maxSize; j++){
            free(quants[i].colors[j]);
            free(quants[i].positions[j]);
        }
        free(quants[i].colors);
        free(quants[i].positions);
    }
    free(quants);
    
/* 
Zwalniamy pamiec, zeby potem zobaczy satysfakcjonujace:
$  valgrind --leak-check=full ./main example0.tga 0.tga 5

[...]
==7422== 
==7422== HEAP SUMMARY:
==7422==     in use at exit: 0 bytes in 0 blocks
==7422==   total heap usage: 202,647 allocs, 202,647 frees, 4,768,400 bytes allocated
==7422== 
==7422== All heap blocks were freed -- no leaks are possible
==7422== 
==7422== For counts of detected and suppressed errors, rerun with: -v
==7422== ERROR SUMMARY: 0 errors from 0 contexts (suppressed: 0 from 0)

*/
   
    return 0;
}