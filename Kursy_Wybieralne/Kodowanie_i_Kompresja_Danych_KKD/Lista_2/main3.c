/*
$ cat Makefile
main: main3.c
    gcc -Wall -Wextra -std=c99 -pedantic main3.c -o main -lm
*/


#include <stdio.h>          // I/O
#include <string.h>         // strcmp()
#include <math.h>           // log2()

#define PRECISION 63        //liczba bitow, ktore wczytujemy do znacznika


#define ONE     0x2000000000000000LLU   // MAX*0.25
#define HALF    0x4000000000000000LLU   // MAX*0.5
#define THREE   0x6000000000000000LLU   // MAX*0.75 
#define MAX     0x7FFFFFFFFFFFFFFFLLU   // 2^63 - 1


unsigned long long int  l=0,            //lewy koniec przedzialu
                        r=MAX;          //prawy koniec przedzialu
                        
unsigned long int   countAll = 257,     //licznik wszystkich symboli
                    count[257],         //licznik kazdego symbolu
                    bytesCoded=0;       //licznik zakodowanych bajtow

//unsigned char byte;                     //bajt = 8bitow sluzacy do odbierania lub wysylania 8 bitow kodu

//char numberOfBits=0;                    //licznik wlozonych/wyjetych bitow do powyzszego bajta


int sendBit(FILE* out, unsigned char bit){         //funkcja dla kompresji - wysyla bit na wyjscie (tak naprawde to czeka, az sie uzbiera 8 bitow i dopiero wtedy wysyla)
    static unsigned char byte=0;
    static char numberOfBits=0;
    byte = byte << 1 | bit;
    numberOfBits += 1;

    if(numberOfBits == 8){
        fputc(byte,out);
        bytesCoded+=1;
        numberOfBits=0;
        return 0;
    }
    return 1;
}

void compress(FILE* in, FILE* out){
    int symbol=0;
    unsigned int counter=0;
    while (symbol!=256) {                           //dopoki symbol to nie EOT
        if((symbol = getc(in)) == EOF)
            symbol=256;
        
        int i;
        unsigned long int left_count=0;
        for(i=0; i<symbol; i++)
            left_count+=count[i];
        
        unsigned long long int step = (r - l + 1) / countAll;
        r=l+step*(left_count+count[i])-1;
        l=l+step*left_count;
        
        while(r<HALF || l>=HALF){
            if (r<HALF){                            //skalowanie 1: przedzial [0, 0.5)
                l*=2;
                r=r*2+1;
                sendBit(out,0);
                for(;counter>0;counter--)
                    sendBit(out,1);
            }
            else if (l>=HALF){                      //skalowanie 2: przedzial [0.5, 1)
                l=2*(l-HALF);
                r=2*(r-HALF)+1;
                sendBit(out,1);
                for(;counter>0;counter--)
                    sendBit(out,0);
            }
        }
        while (l>=ONE && r<THREE){                  //skalowanie 3: przedzial [0.25, 0.75) (0.5 jest w tym przedziale)
            l=2*(l-ONE);
            r=2*(r-ONE)+1;
            counter+=1;
        }

        if(symbol < 256){                                       //liczonko
            count[symbol]+=1;
            countAll+=1;
        }


    }
    if(l<ONE){                                                 //konczenie transmiji: sa dwie mozliwosci: 1)przedzial [0, 0,75)
        sendBit(out,0);
        for(unsigned int i=0; i<counter+1;i++)
            sendBit(out,1);
    }
    else {                                                      //2) przedzial [0.25, 1)
        sendBit(out,1);
    }

    while(sendBit(out,0));                                         //dopelniamy bajta zerami i wysylami
        
}

unsigned char readBit(FILE* in) {                           //funkcja wczytujaca bit z wejscia i zwracajaca jego wartosc
    static unsigned char byte=0;
    static char numberOfBits=0;
    int i;
    if(numberOfBits==0){                                    //tu tez mamy bufor w postaci jednago bajta, ktory uzupelniamy, jak nam sie skonczy
        if ((i = getc(in)) != EOF) {
            byte = (unsigned char) i;
            bytesCoded+=1;
        }
        else{
            byte=0;
        }
        numberOfBits=8;
    }
    
    unsigned char bit = byte >> 7;                          //wyciagamy bit z bajtu
    byte <<= 1;                                             //skracamy o bajt o tego bita
    numberOfBits -= 1;

    return bit;
}

void decompress(FILE* in, FILE* out){
    unsigned long long int z=0;
    for(int i=0; i<PRECISION; i++)                                    //wczytujemy 63 bity do znacznika
        z = (z<<1) + readBit(in);

    //printf("z=%llu\n",z);
    int symbol;

    do{
        unsigned long long int step = (r-l+1)/countAll;                 //liczymy "krok" miedzy dwoma znakami (niekoniecznie roznymi)
        unsigned long long int value = (z-l)/step;                      //liczymy wartosc naszego znacznika w odniesieniu do krokow
        //printf("l=%llu r=%llu countall=%lu step=%llu z=%llu value=%llu\n",l,r,countAll,step,z,value);
        unsigned long int left_count=0;
        for(symbol=0; left_count + count[symbol] <= value; symbol++){   //znajdujemy symbol, ktory odpowiada przedzialowi, w ktorym jest znacznik
            left_count += count[symbol];
        }
        
        //printf("symbol =%d\n",symbol);
        
        if(symbol < 256){  
            fputc(symbol,out);
            //fputc(symbol,stdin);
        }
        
        r=l+step*(left_count+count[symbol])-1;
        l=l+step*left_count;

        while(r<HALF || l>=HALF){                           //analogiczne skalowania jak w kompresji,
            if (r<HALF){
                l*=2;
                r=r*2+1;
                z=z*2+readBit(in);                          //przedzial [0, 0.5) - znacznik*2 i wczytujemy mu bit
            }
            else if (l>=HALF){
                l=2*(l-HALF);
                r=2*(r-HALF)+1;
                z=2*(z-HALF)+readBit(in);                   //przedzial [0.5, 1) - znacznik*2-1 i wczytujemy mu bit
            }
        }
        while (l>=ONE && r<THREE){
            l=2*(l-ONE);
            r=2*(r-ONE)+1;
            z=2*(z-ONE)+readBit(in);                        //przedzial [0.25, 0.75) - znacznik*2-0.5 i wczytujemy mu bit
        }
        
        if(symbol < 256){                                   //jezeli to nie EOT, to zliczamy go i drukujemy na wyjscie
            count[symbol]+=1;
            countAll+=1;
            //fputc(symbol,stdin);
        }
        
    } while(symbol != 256);
}

int main(int argc, char** argv){
    //printf("%llu %llu %llu\n%llu %llu %llu\n", MAX/4, MAX/2, MAX/4*3, ONE, HALF, THREE);
    if (argc < 4){
        printf("Usage: ./main -c (compression) / -d (decompression) <in_file> <out_file>\n");
        return 1;
    }

    FILE* in = fopen(argv[2], "rb");
    FILE* out = fopen(argv[3], "wb");

    if(!in || !out){
        printf("Something wrong with files\n");
        return 2;
    }
    
    for(int i=0; i<257; i++){
        count[i]=1;
    }
    
    if(!strcmp(argv[1], "-c")){
        compress(in, out);
    }else{
        decompress(in, out);
    }

    fclose(in);
    fclose(out);

    countAll-=257;
    double entropy = 0;
    for (int i=0; i<256; i++){
        count[i]-=1;
        if(count[i]>0){
            double probability  =  (double) count[i] / (double) countAll;
            entropy += probability*log2(probability);                                                           //wyliczenie entropii jak na liscie 1
        }
    }
    if(entropy != 0)
        entropy *= -1;

    printf("Entopia = %f\n", entropy);
    printf("Srednia dlugosc kodu: %f\n", (double)bytesCoded*8.0/(double)countAll);                              //srednia dlugosc kodu: liczba zakodowanych bitow przez liczbe znakow
    printf("Stopien kompresji: %lu:%lu = %f:1 (%f%%)\n",countAll,bytesCoded,(double)countAll/(double)bytesCoded, (double)bytesCoded/(double)countAll*100.0);      //stopien kompresji: waga przed kompresja / waga po kompresji

    return 0;
}