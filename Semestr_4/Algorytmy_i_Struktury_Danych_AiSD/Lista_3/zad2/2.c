#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>
#include "pcg_basic.h"
#include <math.h>



int partition(int arr[], int l, int h, int* compares, int* moves);
int RandomizedPartition(int a[], int l, int h, int* compares, int* moves);
int RandomizedSelect(int a[], int p, int q, int i, int* compares, int* moves);
bool greater(int a, int b, int* compares);
int partition5(int arr[], int l, int h, int* compares, int* moves);
int pivot(int a[], int l, int h, int* compares, int* moves);
int preselectedPivotPartition(int a[], int l, int h, int pivotvalue, int* compares, int* moves);
int Select(int a[], int l, int h, int k, int* compares, int* moves);


void generateRandomArray(int array[], int n);
void permute(int array[], int n);
void print(int array[], int n, int num);
void printToStderr(int array[], int p, int q);
void swap(int* a, int* b, int* moves);
bool compare(int a, int b, int* compares);
void copyArray(int src[], int dest[], int n);


/**************************************************** RANDOMIZED SELECT ****************************************************/

int partition(int array[], int l, int h, int* compares, int* moves) {       //zwykly partition, ktory dzieli tablice wzgledem lewego indeksu
    fprintf(stderr, "pivot = %d\n", array[l]);
    int i = l;

    for(int j = l + 1; j <= h; j++) {
        if(compare(array[j], array[l], compares)) {
            i++;
            swap(&array[i], &array[j], moves);
        }
    }
    swap(&array[i], &array[l], moves);
    return i;
}

int RandomizedPartition(int a[], int l, int h, int* compares, int* moves) {         //randomized partition - losuje element, swapuje z lewym i robi partition
    pcg32_random_t rng;
    pcg32_srandom_r(&rng, time(NULL) ^ (intptr_t)&printf, 33u);
    int i = pcg32_random_r(&rng) % (h - l + 1);
    swap(&a[l + i], &a[l], moves);
    return partition(a, l, h, compares, moves);
}

int RandomizedSelect(int a[], int p, int q, int i, int* compares, int* moves) {         //randomized select
    fprintf(stderr, "RandomizedSelect------\n");
    printToStderr(a, p, q);
    fprintf(stderr, "k = %d\n", i);
    
    if(p == q)
        return a[p];
    
    int r = RandomizedPartition(a, p, q, compares, moves);                  //robi randomized Partition
    int k = r - p + 1;
    
    if(i == k)
        return a[r];
    else if(i < k)
        return RandomizedSelect(a, p, r-1, i, compares, moves);             //i idzie rekurencyjnie w zaleznosci od stosunku piwota do szukanej statystyki
    else
        return RandomizedSelect(a, r+1, q, i-k, compares, moves);
}


/**************************************************** SELECT ****************************************************/

bool greater(int a, int b, int* compares) {
    fprintf(stderr, "compared %d and %d\n", a, b);
    //(*compares)++;
    return a > b;
}

int partition5(int array[], int l, int h, int* compares, int* moves) {          //partition5
    if(h == l)
        return l;
    
    bool keySet;
    int key;
    
    for(int i = l+1; i <= h; i++) {                     //insertSort
        keySet = false;
        
        int j = i-1;
        while(j >= l && greater(array[j], key, compares)) {
            if(!keySet){	
                key = array[i]; //(*moves)++;
                keySet = true;
            }
            array[j+1] = array[j]; //(*moves)++;
            j--;
        }
        if(keySet) {
            array[j+1] = key; //(*moves)++;
        }
    }
    return l + (h-l)/2;             //zwraca mediane, czyli srodkowy element
}

int pivot(int a[], int l, int h, int* compares, int* moves) {           //funkcja wyznaczajaca piwota dla funkcji Select
    if(h - l < 5) {
        partition5(a, l, h, compares, moves);
        return a[l - (l-h)/2];
    }

    int n = h-l+1;
    int n1 = (int) (n/5);
    int medianArray[n1];            //tworzy tablice median
    
    for(int i = 0; i < n1; i++) {       //dzieli tablice a na piecioelementowe tablice
        int left = 5*i;
        int subRight = left + 4;
        if(subRight > h)
            subRight = h;
        int median5 = partition5(a, l+left, l+subRight, compares, moves);       //wyznacza mediane kazdej z nich
        medianArray[i] = a[median5];            //i dodaje do tablicy median
    }
   // return Select(medianArray, 0, n1-1, n1/2, compares, moves);
    return pivot(medianArray, 0, n1-1, compares, moves);        //wyznacza mediane z median
}

int preselectedPivotPartition(int a[], int l, int h, int pivotvalue, int* compares, int* moves) {       //partition z wybrana wartoscia piwota
    int i;
    for(i = l; i <= h; i++) {           //szukamy indeksu piwota
        if(a[i] == pivotvalue)
            break;
    }
    swap(&a[i], &a[l], moves);      //zamieniamy z lewym
    return partition(a, l, h, compares, moves); //wykonujemy partition
}

int Select(int a[], int l, int h, int k, int* compares, int* moves) {       //SELECT
    fprintf(stderr, "Select------------\n");
    printToStderr(a, l, h);
    fprintf(stderr, "k = %d\n", k);
    
    if(l == h)
        return a[l];
    
    int pivotvalue = pivot(a, l, h, compares, moves);                   //wyznaczamy piwota przy pomocy algorytmu magicznych piatek
    int pivotIndex = preselectedPivotPartition(a, l, h, pivotvalue, compares, moves);       //robimy wedle niego partitiona

    int i = pivotIndex-l+1;
    if(k == i)
        return pivotvalue;
    else if(k < i)
        return Select(a, l, pivotIndex-1, k, compares, moves);          //i wzgledem wyniku wchodzimy (lub nie) odpowiedno do rekurencji
    else
        return Select(a, pivotIndex+1, h, k-i, compares, moves);
}

/***************************************OTHER******************************************/

void generateRandomArray(int array[], int n) {              //losowanie tablicy o dlugosci n
    pcg32_random_t rng;
    pcg32_srandom_r(&rng, time(NULL) ^ (intptr_t)&printf, 69u);

    for(int i = 0; i < n; i++)
        array[i] = pcg32_random_r(&rng) % 100000;
}

void permute(int array[], int n) {              //generowanie losowej permutacji
    for(int i = 0; i < n; i++) {
        array[i] = i + 1;
    }
    pcg32_random_t rng;
    pcg32_srandom_r(&rng, time(NULL) ^ (intptr_t)&printf, 54u);
    for(int i = n - 1; i > 0; i--) {
        int j = pcg32_random_r(&rng) % (i + 1);
        int tmp = array[j];
        array[j] = array[i];
        array[i] = tmp;
    }
}

void printToStderr(int array[], int p, int q) {         //wypisanie na stderr
    for(int i = p; i <= q; i++) {
        fprintf(stderr, "%d ", array[i]);
    }
    fprintf(stderr, "\n");
}

void swap(int* a, int* b, int* moves) {             //swap 2 elementow
    fprintf(stderr, "swapped %d and %d\n", *a, *b);
    (*moves)++;
    int t = *a;
    *a = *b;
    *b = t;
}

bool compare(int a, int b, int* compares) {         //porownanie
    fprintf(stderr, "compared %d and %d\n", a, b);
    (*compares)++;
    return a <= b;
}

void copyArray(int src[], int dest[], int n) {      //przekopiowanie tablicy
    for(int i = 0; i < n; i++) {
        dest[i] = src[i];
    }
}

/************************************************MAIN ******************************************/

int main(int argc, char *argv[]) {
    int type = 2, n, k, scompares = 0, smoves = 0, rscompares = 0, rsmoves = 0;

    if (argc > 1){
        if (!strcmp(argv[1], "-r"))
            type = 1;
        else
            type = 2;
    }
 
    scanf("%d", &n);
    int* array = malloc(sizeof(int)*n);
    scanf("%d", &k);

    if(type == 1)
        generateRandomArray(array, n);
    else
        permute(array, n);

    int* s = malloc(sizeof(int)*n);
    copyArray(array, s, n);
    int svalue = Select(s, 0, n-1, k, &scompares, &smoves);       
    fprintf(stderr, "Select finished---------\nCompares = %d\nmoves = %d\n", scompares, smoves);

    int* rs = malloc(sizeof(int)*n);
    copyArray(array, rs, n);
    int rsvalue = RandomizedSelect(rs, 0, n-1, k, &rscompares, &rsmoves);
    fprintf(stderr, "RandomizedSelect finished---------\nCompares = %d\nmoves = %d\n", rscompares, rsmoves);

    if(svalue != rsvalue) {
        fprintf(stderr, "\nDIFFERENT RESULTS\n");
        exit(-1);
    }
    else
        fprintf(stderr, "\nSAME RESULTS\n");

    fprintf(stderr, "\nSELECT COMPARES: %d MOVES: %d\n", scompares, smoves);
    fprintf(stderr, "RANDOM SELECT COMPARES %d MOVES: %d\n\n", rscompares, rsmoves);

    for(int i = 0; i < n; i++) {
        if(rs[i] == rsvalue)
            printf("[%d] ", rs[i]);
        else
            printf("%d ", rs[i]);
    }
    printf("\n");

    return 0;
}

