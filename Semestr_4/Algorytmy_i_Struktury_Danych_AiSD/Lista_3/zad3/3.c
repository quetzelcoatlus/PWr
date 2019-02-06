#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int binarySearch(int* a, int v, int start, int end, int* compares) {  
    if (start > end)        //warunek wyjscia z rekurencji
        return 0;
    int mid = (start + end)/ 2;     //szukanie polowy
    (*compares)++;
    if (v == a[mid])            //jezeli to nasza wartosc, to zwroc 1
        return 1;
    (*compares)++;
    return v < a[mid] ? binarySearch(a, v, start, mid-1, compares) : binarySearch(a, v, mid+1, end, compares);      //odpowiednia rekurencja, w zaleznosci czy szukana wartosc jest mniejsza czy wieksza od a[mid]
}


int main(){
    int n, v, compares = 0, b;
    scanf("%d",&n);
    
    int* a = malloc(sizeof(int)*n);
    
    for (int i=0; i<n; i++)
       scanf("%d", &a[i]);
    
    scanf("%d",&v);
    clock_t start, end;
    double time;
    
    start = clock();
    b = binarySearch(a, v, 0, n-1, &compares);
    end = clock();
    
    time = (double) ((end-start) / 1000.0);
    
    fprintf(stderr,"Wynik: %d\nLiczba porownan: %d\nCzas: %f ms\n", b, compares, time);
    return b;
}