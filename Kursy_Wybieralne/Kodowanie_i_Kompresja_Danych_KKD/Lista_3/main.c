/*
    gcc -Wall -Wextra -std=c99 -pedantic main.c -o main
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

long int allBytes=0, codedBytes=0;                      //zmienne globalne (specjalnie globalne, czyli takie "brzydkie, bo sa jedynie do wyliczania stopnia kompresji, czyli niejako nie sa elementami programu, tylko dodatkiem)


/*********************************************** TRANSMISJA BITOW *************************************************/


int sendBit(FILE* out, unsigned char bit){              //wysylanie bitu - to samo, co w arytmetycznym
    static unsigned char byte=0;
    static char numberOfBits=0;
    byte = byte << 1 | bit;
    numberOfBits += 1;

    if(numberOfBits == 8){
        fputc(byte,out);
        numberOfBits=0;
        codedBytes++;
        return 0;
    }
    return numberOfBits;
}

int readBit(FILE* in) {                                 //czytanie bitu - to samo, co w arytmetycznym oprocz jednego
    static unsigned char byte=0;
    static char numberOfBits=0;
    int i;
    if(numberOfBits==0){
        if ((i = fgetc(in)) != EOF){
            byte = (unsigned char) i;
            codedBytes++;
        }
        else
            return -1;                                //jedyna roznica - zwracanie -1 dla konca strumienia. W arytmetycznym po prostu sie dopelnialo zerami, bo tam byl EOT, ale tutaj nie ma
        numberOfBits=8;
    }
    
    int bit = byte >> 7;                       
    byte <<= 1;                                             
    numberOfBits -= 1;

    return bit;
}

void flush_out(FILE* out){                              //dopelnianie bajta do 8 w wysylaniu - nasza strategia to dopelnienie zerami - potem zobaczymy, jak to skutkuje w kodowaniu uniwersalnym
    while(sendBit(out,0) > 1);  
}



/*********************************************** KODOWANIE GAMMA *************************************************/


void gamma_c(int c, FILE* out){                         //kodowanie gamma https://pl.wikipedia.org/wiki/Kodowanie_Eliasa#Algorytm_kodowania
    c+=1;                                               //dodajemy do kodowanej liczby 1, bo nie da sie zakodowac 0
    int tab[32];
    int n=0;
    
    while(c>0){
        tab[n++] = c%2;                                 //zamiana liczby na binarny zapis dlugosci n
        c /= 2;
    }
    
    for(int i=0; i<n-1; i++)                            //wypisanie n-1 zer
        sendBit(out,0);

    for(int i=n-1; i>=0; i--)                           //wypisanie liczby binarnie, czyli bufor od tylu    
        sendBit(out, (unsigned char) tab[i]);
}

int gamma_d(FILE* in){                                  //dekodowanko
    int n=1;
    int b;
    while((b=readBit(in)) == 0)                         //dopoki sa zera, to zliczamy je
        n++;
    
    if(b == -1)                                         //po wyjsciu z while albo trafila sie 1, albo -1, wiec obslugujemy koniec strumienia
        return -1;
    
    int pow = 1;
    for(int i=1; i<n; i++)                              //teraz bedziemy czytac liczbe binarna na n+1 bitow, wiec sobie ustawiamy "potege" na odpowiedni wykladnik
        pow*=2;
    
    int ret = pow;
    pow/=2;
    
    for(int i=1; i<n; i++){                             //no i konstruujemy liczbe uwzgledniajac koniec strumienia
        if((b=readBit(in)) == -1)
            return -1;
        else{
            ret+=b*pow;
            pow/=2;
        }
    }

    return ret-1;                                       //zwracamy liczbe o 1 mniejsza niz nam wyszlo, bo w kodowaniu dodalismy 1 (brak 0)
}



/*********************************************** KODOWANIE DELTA *************************************************/


void delta_c(int c, FILE* out){                         //kodowanie delta - to samo co gamma, tylko ze liczbe bitow kodowanej liczby kodujemy gamma https://pl.wikipedia.org/wiki/Kodowanie_Eliasa#Algorytm_kodowania_2
    c+=1;
    int cBin[32];
    int n=0;
    
    while(c>0){
        cBin[n++] = c%2;
        c /= 2;
    }
    
    int nBin[32];
    int nTemp = n;
    int k = 0;
    
    while(nTemp>0){
        nBin[k++] = nTemp%2;
        nTemp /= 2;
    }
    
    for(int i=0; i<k-1; i++)
        sendBit(out,0);

    for(int i=k-1; i>=0; i--)
        sendBit(out, (unsigned char) nBin[i]);

    
    for(int i=n-2; i>=0; i--)
        sendBit(out, (unsigned char) cBin[i]);
}

int delta_d(FILE* in){
    int k=1;
    int b;
    while((b=readBit(in)) == 0)
        k++;
    
    if(b == -1) 
        return -1;
    
    int pow = 1;
    for(int i=1; i<k; i++)
        pow*=2;
    
    int n = pow;
    pow/=2;
    
    for(int i=1; i<k; i++){
        if((b=readBit(in)) == -1)
            return -1;
        else{
            n+=b*pow;
            pow/=2;
        }
    }
    
    pow = 1;
    for(int i=1; i<n; i++)
        pow*=2;
    
    int ret = pow;
    pow/=2;
    
    for(int i=1; i<n; i++){
        if((b=readBit(in)) == -1)
            return -1;
        else{
            ret+=b*pow;
            pow/=2;
        }
    }

    return ret-1;
}


/*********************************************** KODOWANIE OMEGA *************************************************/


void omega_c(int c, FILE* out){                             //omega - "rekurencyjne" kodowanie liczby bitow poprzedniej liczby  https://pl.wikipedia.org/wiki/Kodowanie_Eliasa#Algorytm_kodowania_3
    c+=2;                                                   //w omedze do liczby dodajemy 2, poniewaz reprezentacja jedynki to '0', co sie kloci z naszym dopelnieniem strumienia do 8. bitow zerami, dlatego kodujemy liczby od 2 wzwyz
    int buffer[256];
    int n=0;
    buffer[n++]=0;
    
    //fprintf(stderr,"%d ", c);
    
    int bin[32];
    int k=0;
    //printf("|%d| = ",c);
    while(c>1){
        //fprintf(stderr,"c=%d ",c);
        int temp=c;
        k=0;
        while(temp>0){
           // fprintf(stderr,"temp=%d ",temp);
            bin[k++] = temp%2;
            temp /= 2;
        }
        //fprintf(stderr,"k=%d ",k);
        for(int i=0; i<k; i++)
            buffer[n++]=bin[i];
        c = k-1;
        //fprintf(stderr,"c=%d ",c);
    }
    for(int i=n-1; i>=0; i--){
        sendBit(out, (unsigned char) buffer[i]);
        //printf("[%d] ",buffer[i]);
    }
    //printf("\n");
}

int omega_d(FILE* in){
    int n=1;
    int b;
    int beginsWithZero=1;
    
    while((b=readBit(in)) == 1){
        //printf("[%d] ",b);
        beginsWithZero=0;
        
        int pow = 1;
        for(int i=1; i<n; i++)
            pow*=2;
        int temp = pow*2;
        
        for(int i=0; i<n; i++){
            if((b=readBit(in)) == -1)
                return -1;
            else{
                temp+=b*pow;
                pow/=2;
            }
        }
        n=temp;
    }
    if (b==-1 || beginsWithZero)
        return -1;
    
    return n-2;                                         //dodalismy 2, to odejmujemy tylez od wyniku 
}


/*********************************************** KODOWANIE FIBONACCIEGO *************************************************/


void fibonacci_c(int c, FILE* out){                                     //kodowanie fibonacciego 
    static int fib[] = { 1, 2, 3, 5, 8, 13,                             //no w sumie to tutaj sobie na sztywno wpisalem kolejne liczby, ale jak ktos chce, to moze sobie liczyc na biezaco    
                         21, 34, 55, 89, 144, 
                         233, 377, 610, 987, 
                         1597, 2584, 4181, 6765, 
                         10946, 17711, 28657, 46368, 
                         75025, 121393, 196418, 
                         317811, 514229, 832040 };
    c+=1;
    int buffer[32];
    int n=0;
    
    int j=0;
    while (fib[j+1] <= c)                                               //znajdujemy najwieksza liczbe fibonacciego mniejsza od naszej liczby
        j++;
    
    buffer[n++]=1;
    c-=fib[j--];
    
    while(c>0){                                                         //idac w dol sprawdzamy, czy od naszej liczby mozna odjac liczbe fibonacciego = 1, a jak nie to 0
        if(fib[j] <= c){
            buffer[n++]=1;
            c-=fib[j];
        } else
            buffer[n++]=0;
        j--;
    }
    
    for(;j>=0;j--)                                                      //dochodzimy do poczatku tablicy fibonacciego dopisujac 0
        buffer[n++]=0;
    
    for(int i=n-1; i>=0; i--)
        sendBit(out, (unsigned char) buffer[i]);

    sendBit(out,1);
}

int fibonacci_d(FILE* in){                                      //dekodowanie analogicznie
    static int fib[] = { 1, 2, 3, 5, 8, 13, 
                         21, 34, 55, 89, 144, 
                         233, 377, 610, 987, 
                         1597, 2584, 4181, 6765, 
                         10946, 17711, 28657, 46368, 
                         75025, 121393, 196418, 
                         317811, 514229, 832040 };
    
    int b;
    int prev = readBit(in);
    if(prev == -1)
        return -1;
    
    int n=0;
    int ret=fib[n++]*prev;
    
    
    while((b=readBit(in)) != -1){
        if(b==1 && prev==1)
            return ret-1;
        ret+=fib[n++]*b;
        prev = b;
    }
    
    return -1;
}


/*********************************************** BRAK KODOWANIA *************************************************/


void no_code_c(int c, FILE* out){                       //brak kodowania = wypluj liczby
    fprintf(out, "%d ", c);
}

int no_code_d(FILE* in){                               //wczytaj liczby
    int c;
    if(fscanf(in,"%d", &c) == 1)
        return c;
    return -1;
}



/*********************************************** SLOWNIK *************************************************/

#define DIC_MAX_SIZE 1<<20


typedef struct Word_{           //slowo
    int prefix;                 //slowa w slowniku sa w tablicy. Prefix to indeks prefiksu danego slowa
    unsigned char c;            //literka, ktora razem z prefiksem tworzy slowo.
} Word;

typedef struct Dictionary_{     //slownik
    Word** words;               //tablica slow
    int size;                   //aktualny rozmiar
} Dictionary;

void dic_init(Dictionary** d){
    *d = malloc(sizeof(Dictionary));
    (*d)->words = malloc(sizeof(Word*) * DIC_MAX_SIZE);
    (*d)->size=0;
}

void dic_reset(Dictionary* d){
    for(int i=256; i<d->size; i++)
        free(d->words[i]);
    d->size=256;
}

void dic_add(Dictionary* d, int prefix, unsigned char c){
    if(d->size == DIC_MAX_SIZE){
        dic_reset(d);
        return;
    }
    
    Word* word = malloc(sizeof(Word));
    word->prefix=prefix;
    word->c=c;
    //printf(" *adding %d -> %d* ", prefix, c);
    d->words[d->size]=word;
    (d->size)++;
}

int dic_find(Dictionary* d, int prefix, unsigned char c){           //szukanie = znalezienie slowa, ktore ma taki sam prefiks i literke
    for(int i=0; i<d->size; i++)
        if(d->words[i]->prefix == prefix && d->words[i]->c == c)
            return i;
    return -1;
}


unsigned char dic_get_first_char(Dictionary* d, int n){             //otrzymanie pierwszego znaku slowa (potrzebne w dekompresji)
    while(d->words[n]->prefix >= 0)                                 //idziemy wglab slowika cofajac sie po prefiksach
        n=d->words[n]->prefix;
    return d->words[n]->c;
}

void dic_print(Dictionary*d, int n, FILE* out){                     //wypisanie slowa: rekursja glowowa po prefiksach
    if(d->words[n]->prefix >= 0)
        dic_print(d, d->words[n]->prefix, out);
    fputc(d->words[n]->c, out);
    allBytes++;
    //printf("|%d| ", d->words[n]->c);
}

void dic_free(Dictionary** d){
    for(int i=0; i < (*d)->size; i++)
        free((*d)->words[i]);
    free((*d)->words);
    free(*d);
    *d=NULL;
}


/*********************************************** KOMPRESJA *************************************************/


void compress(FILE* in, FILE* out, void (*write_out)(int, FILE*)){          //kompresja LZW https://pl.wikipedia.org/wiki/LZW#Algorytm_kompresji_(kodowania) . Przyjmuje jako argument funkcje kodujaca indeksy slownika
    Dictionary* d;
    dic_init(&d);
    for(int i=0; i<256; i++)                                                //zainicjalizowanie slownika 8-bitowymi kodami (char). W c++ powinno sie uzyc mapy, czy czegos, co tam przechowuje Stringi
        dic_add(d, -1, (unsigned char) i);
    
    
    int index;
    int c;
    
    int prefix = fgetc(in);                                                 //w C++ prefiksem bedzie po prostu string. wczytamy do niego pierwszy znak
    //printf("|%d| ",prefix);
    if (prefix == EOF) {
        return;
    }
    
    allBytes++;
    while((c=fgetc(in)) != EOF){                                            //dopoki sa znaki na wejsciu
        allBytes++;
        //printf("|%d| ",c);
        if((index=dic_find(d, prefix, (unsigned char) c)) != -1)            //sprawdzamy, czy slowo w postaci prefiks+znak jest w slowniku (w C++ po prostu map.find(prefiks+c), czy jakos tak)
           prefix=index;                                                    //jezeli jest, to zlepiamy prefiks z literka (w C++ prefiks+=c)
        else{                                                                   
            write_out(prefix, out);                                         //jezeli nie ma, to wypisujemy KOD (pozycje w slowniku) prefiksu. (u mnie mam to od razu, w C++ to pewnie map.get_index czy co tam)
            dic_add(d, prefix, (unsigned char) c);                          //dodajemy do slownika slowo w postaci prefiks+znak      
            prefix = c;                                                     //resetujemy prefiks, czyli ustawiamy go na ostatnia literke    
        }
    }   
    write_out(prefix, out);                                                 //na koncu wypisujemy kod prefiksu
    flush_out(out);                                                         //i ewentualnie dopelniamy zerami do 8 bitow
    
    dic_free(&d);
}


/*********************************************** DEKOMPRESJA *************************************************/


void decompress(FILE* in, FILE* out, int (*read_in)(FILE*)){            //dekompresja https://pl.wikipedia.org/wiki/LZW#Algorytm_dekompresji . Przyjmuje jako argument funkcje dekodujaca indeksy slownika
    Dictionary* d;
    dic_init(&d);
    for(int i=0; i<256; i++)
        dic_add(d, -1, (unsigned char) i);
    
    int prev = read_in(in);
    //printf("(%d) ",prev);
    if(prev < 0)
        return;   
    //fputc(prev, out);
    dic_print(d, prev, out);
    
    int c;
    while((c=read_in(in)) != -1){
        //printf("(%d) ",c);
        if(c < d->size){
           // printf("jest w slowniku ");
            dic_add(d, prev, dic_get_first_char(d, c));                 //algorytm krok w krok z wiki, wiec pomijam komentarze                    
            dic_print(d, c, out);
        }else{
            //printf("nie ma w slowniku ");
            dic_add(d, prev, dic_get_first_char(d, prev));
            dic_print(d, d->size - 1, out);
        }
        prev=c;
    }
    dic_free(&d);   
}


/*********************************************** MAIN *************************************************/


int main(int argc, char** argv){
    void (*universal_c[])(int, FILE*)  = {gamma_c, delta_c, omega_c, fibonacci_c, no_code_c};       //funkcje kodujace 
    int (*universal_d[])(FILE*) = {gamma_d, delta_d, omega_d, fibonacci_d, no_code_d};              //funkcje dekodujace
    
    int mode = 0;
    int f = 2;
    int inOut = 0;                      //domyslne parametry
    FILE* in = stdin;
    FILE* out = stdout;
    
    argc--; argv++;
    while(argc>0){
        if(!strcmp(argv[0],"-c"))
            mode=0;
        else if(!strcmp(argv[0],"-d"))
            mode=1;
        else if(!strcmp(argv[0], "-gamma"))
            f=0;
        else if(!strcmp(argv[0], "-delta"))
            f=1;                                            //obsluga argumentow programu
        else if(!strcmp(argv[0], "-omega"))
            f=2;
        else if(!strcmp(argv[0], "-fibon"))
            f=3;
        else if(!strcmp(argv[0], "-nocode"))
            f=4;
        else if(inOut == 0){
            in = fopen(argv[0], "rb");
            inOut++;
        }
        else if(inOut==1){
            out = fopen(argv[0], "wb");
            inOut++;
        }
        argc--; argv++;
    }
    
    printf("mode = %d, f = %d, inOut = %d\n", mode, f, inOut);
    
    if(!in || !out){
        printf("Something wrong with in/out files.\n");
        return 2;
    }
    
    if(!mode)
        compress(in, out, universal_c[f]);
    else
        decompress(in, out, universal_d[f]);
    
    if(inOut>1)
        fclose(out);
    if(inOut>0)
        fclose(in);
    
    printf("Srednia dlugosc kodu: %f\n", (double)codedBytes*8.0/(double)allBytes);                              //srednia dlugosc kodu: liczba zakodowanych bitow przez liczbe znakow
    printf("Stopien kompresji: %lu:%lu = %f:1 (%f%%)\n",allBytes,codedBytes,(double)allBytes/(double)codedBytes, (double)codedBytes/(double)allBytes*100.0);      //stopien kompresji: waga przed kompresja / waga po kompresji


    
    return 0;
}