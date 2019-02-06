/*

Autor: Jan Brzeżański
Indeks: 236814

*/


#include <stdio.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

#include "pcg_basic.h"


int getMax(int array[], int size);
void countSort(int array[], int base, int exp, int size, bool comp, int* moves);
void radixSort(int array[], int size, int* compares, int* moves, bool comp);

void insertionSort(int* array, int n, int* compares, int* moves, bool comp);

int* mergeSort(int* array, int start, int end, int* compares, int* moves, bool comp);
int* merge(int* A, int* B, int lengthA, int lengthB, int* compares, int* moves, bool comp);

void quickSort(int* array, int start, int end, int* compares, int* moves, bool comp);
int singlePivotPartition(int* array, int start, int end, int* compares, int* moves, bool comp);

bool dualPivotFirstCompare(int x, int lp, int rp, int count, bool comp);
bool dualPivotSecondCompare(int x, int lp, int rp, int count, bool comp);
void dualPivotQuickSort(int* array, int start, int end, int* compares, int* moves, bool comp);
int dualPivotPartition(int* array, int start, int end, int* leftPivot, int* compares, int* moves, bool comp);



void swap(int* a, int* b);

/***************************************COUNT + RADIX ******************************************/

int getMax(int array[], int size) {     //maksimum z tablicy
    int max = array[0];

    for(int i = 1; i < size; i++) 
        if(array[i] > max) 
            max = array[i];
    
    return max;
}


void countSort(int array[], int base, int exp, int size, bool comp, int* moves) {
    *moves = 3 * size + base;
    int* count = calloc(base, sizeof(int));
    int* result = malloc(sizeof(int)*size);
    
    for(int i = 0; i < size; i++)
        count[(array[i]/exp)%base]++;           //wrzucanie 'cyfr' do odpowiednich kubelkow
    
    if (!comp)
        for(int i = 1; i < base; i++)
            count[i] += count[i - 1];           //dodawanie do kolejnych pojemnikow wartosci poprzedniego, zeby w efekcie uzyskac indeksy tablicy, w ktore trafia wartosci
    else
        for (int i = base - 2; i >= 0; i--)
            count[i] += count[i + 1];

    for(int i = size - 1; i >= 0; i--)
        result[--count[(array[i]/exp)%base]] = array[i];    //przypisanie wartosci array[i] tak, jak wskazuja wartosci tablicy count
    
    for(int i = 0; i < size; i++)
        array[i] = result[i];                       //przekopiowanie do wejsciowej tablicy
    
    free(count);
    free(result);
}

void radixSort(int array[], int size, int* compares, int* moves, bool comp) {
    *compares = size - 1;
    int max = getMax(array, size);
    //int base = log(max);
    int base = 256;             //ustalenie podstawy
    
    for(unsigned long int exp = 1; max/exp > 0; exp *= base) {          //iteracja po potegach podstawy
        countSort(array, base, exp, size, comp, moves);
    }
}

/******************************************** INSERTION *************************************************************/

bool insertionSortCompare(int a, int b, bool comp){
	if(!comp)
		return a>b;
	else
		return b>a;
}

void insertionSort(int* array, int n, int* compares, int* moves, bool comp){
	int key;		//klucz
	int i;			

	for(int j = 1; j < n; j++){
		key = array[j]; (*moves)++;		//przypisanie do klucza
		i = j - 1;
		while(i > -1 && ++(*compares) && insertionSortCompare(array[i], key, comp)){		//dopoki i>=0, zwieksz compares, porownaj array[i] z kluczem
			array[i+1] = array[i]; (*moves)++;	//przypisz array[i] do array[i+1] i zwieksz 'i' o jeden
			i--;
		}
		array[i+1] = key; (*moves)++;	//wstaw klucz na wlasciwe miejsce
	}
}


/******************************************** MERGE *************************************************************/


bool mergeSortCompare(int a, int b, bool comp){
	if(comp)
		return a>b;
	else
		return b>a;
}

int* mergeSort(int* array, int start, int end, int* compares, int* moves, bool comp){
	if (start == end){
		int* r = malloc(sizeof(int));
		r[0] = array[start];
		return r;
	}
	else{
        int middle = start+(end-start)/2;
		
        int* A = mergeSort(array, start, middle, compares, moves, comp);
        int* B = mergeSort(array, middle+1, end, compares, moves, comp);
 
        return merge(A, B, middle - start +1, end - middle, compares, moves, comp);
    }
}

int* merge(int* A, int* B, int lengthA, int lengthB, int* compares, int* moves, bool comp){
	int* result;
	result = malloc(sizeof(int)*(lengthA + lengthB));
	
	int a=0, b=0,i=0;
	while(a < lengthA && b < lengthB){
		if(++(*compares) && mergeSortCompare(A[a], B[b], comp)){
			result[i++] = A[a++]; (*moves)++;
		}
		else{
			result[i++] = B[b++]; (*moves)++;
		}
	}
	while(a < lengthA){
		result[i++] = A[a++];  (*moves)++;
	}
	while(b < lengthB){
		result[i++] = B[b++];  (*moves)++;
	}
	
	free(A);
	free(B);
	
	return result;
}




/******************************************** QUICK *************************************************************/
bool quickSortCompare(int a, int b, bool comp){
	if(!comp)
		return a<=b;
	else
		return b<=a;
}

void quickSort(int* array, int start, int end, int* compares, int* moves, bool comp){
	 if (start < end){
        int pivot = singlePivotPartition(array, start, end, compares, moves, comp);
 
        quickSort(array, start, pivot - 1, compares, moves, comp);
        quickSort(array, pivot + 1, end, compares, moves, comp);
    }	
}

int singlePivotPartition(int* array, int start, int end, int* compares, int* moves, bool comp){ 
    int i = start;
 
    for (int j = start + 1; j <= end; j++){
        if (++(*compares) && quickSortCompare(array[j], array[start], comp)){   
			swap(&array[++i], &array[j]); (*moves)+=3;
        }
    }
	swap(&array[i], &array[start]); (*moves)+=3;
	
    return i;
}

/******************************************** DUAL PIVOT QUICK *************************************************************/

void dualPivotQuickSort(int* array, int start, int end, int* compares, int* moves, bool comp){
    if (start < end) {
        int leftPivot, rightPivot; 
        rightPivot = dualPivotPartition(array, start, end, &leftPivot, compares, moves, comp);
		
        dualPivotQuickSort(array, start, leftPivot - 1, compares, moves, comp);
        dualPivotQuickSort(array, leftPivot + 1, rightPivot - 1, compares, moves, comp);
        dualPivotQuickSort(array, rightPivot + 1, end, compares, moves, comp);
    }
}
 
bool dualPivotFirstCompare(int x, int lp, int rp, int count, bool comp){
	if(count <= 0 && !comp)   
		return x < lp;
	
	if(count > 0 && !comp)	
		return x >= rp;
	
	if(count <= 0 && comp)	
		return x >= lp;
	
	if(count > 0 && comp)	
		return x < rp;
	
	return false;
}

bool dualPivotSecondCompare(int x, int lp, int rp, int count, bool comp){
	if(count <= 0 && !comp) 		
		return x >= rp;
	
	if(count > 0 && !comp)		
		return x < lp;
	
	if(count <= 0 && comp)		
		return x < rp;
	
	if(count > 0 && comp)		
		return x >= lp;
	
	return false;
}
	

int dualPivotPartition(int* array, int start, int end, int* leftPivot, int* compares, int* moves, bool comp){
    if(++(*compares) && array[start] > array[end] && !comp){
        swap(&array[start], &array[end]); (*moves)+=3;
	}
	else if(++(*compares) && array[start] < array[end] && comp){
        swap(&array[start], &array[end]); (*moves)+=3;
	}
	
    int i = start + 1;
    int right = end - 1;
	int left = start + 1;
	int count = 0;
	bool swapLeft = false;
	bool swapRight = false;
	
    while (i <= right) {

        if (++(*compares) && dualPivotFirstCompare(array[i], array[start], array[end], count, comp)) {  
            if(count <= 0)
				swapLeft=true;
			else 
				swapRight=true;
        }
        else if (++(*compares) && dualPivotSecondCompare(array[i], array[start], array[end], count, comp)) {
			if (count <= 0)
				swapRight=true;
			else
				swapLeft=true;
        }
		
		if(swapLeft){
			swap(&array[i], &array[left++]); (*moves)+=3;
			count--;
			swapLeft=false;
		}
		else if(swapRight){
			while (count <= 0 && ++(*compares) && dualPivotSecondCompare(array[right],array[start],array[end], count, comp) && i < right) {
            	right--;
                count++;
            }
			while (count > 0 && ++(*compares) && dualPivotFirstCompare(array[right],array[start],array[end], count, comp) && i < right) {
            	right--;
                count++;
            }
            swap(&array[i], &array[right--]); (*moves)+=3;
			
			count++;
            if (count <= 0 && dualPivotFirstCompare(array[i], array[start], array[end], count, comp)) {
				swap(&array[i], &array[left++]); (*moves)+=3;
				count--;
			}
			else if (count > 0 && dualPivotSecondCompare(array[i], array[start], array[end], count, comp)) {
				swap(&array[i], &array[left++]); (*moves)+=3;
				count--;
			}
			swapRight=false;
		}
		
        i++;
    }
    left--;
    right++;

	
    swap(&array[start], &array[left]);
    swap(&array[end], &array[right]);
 	(*moves)+=6;

    *leftPivot = left; 
    return right;
}



/******************************************** OTHER *************************************************************/

void swap(int* a, int* b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

bool compare(int a, int b, bool comp){
	if(!comp)
		return a<=b;
	else
		return b<=a;
}

bool isSorted(int* a, int n, bool comp){
	for(int i=0; i+1 < n; i++)
		if(!compare(a[i],a[i+1],comp))
		   return false;
	return true;		
}



/******************************************** MAIN *************************************************************/


int main(int argc, char *argv[]){
	int type=3, k = 1, mod = 0;
	bool comp = false;
	char* name= NULL;	
    
	argc--;
	argv++;
	while(argc > 0){
		if(!strcmp((argv[0]),"--type")){
			argc--;
			argv++;
			if(argc > 0){
				if(!strcmp(argv[0],"insert"))
					type=1;
				else if(!strcmp(argv[0],"merge"))
					type=2;
				else if(!strcmp(argv[0],"quick"))
					type=3;
				else if(!strcmp(argv[0],"dpqs"))
					type=4;
                else if(!strcmp(argv[0],"radix"))
					type=5;
				argc--;
				argv++;
			}
		}
		else if(!strcmp((argv[0]), "--comp")){
			argc--;
			argv++;
			if(argc > 0){
				if(!strcmp(argv[0],"<="))
					comp = false;
				else if(!strcmp(argv[0],">="))
					comp = true;
				argc--;
				argv++;
			}
		}
		else if(!strcmp((argv[0]), "--stat")){
			argc--;
			argv++;
			if(argc > 0){
				name = argv[0];
				argc--;
				argv++;
				if(argc > 0){
					k = atoi(argv[0]);
                    argc--;
				    argv++;
                }
			}
		}
        else if(!strcmp((argv[0]), "--mod")){
			argc--;
			argv++;
			if(argc > 0){
				mod = atoi(argv[0]);
                argc--;
				argv++;
			}
		}
		else {
			argc--;
			argv++;
		}
	}
	
	if (name == NULL){
		name = malloc(sizeof(char)*10);
		strcpy(name, "default");
	}
	
	int seed = 0;
	pcg32_random_t rng;
	pcg32_srandom_r(&rng, time(NULL) ^ (intptr_t)&printf, (intptr_t)&seed);
	
	int* array;
	int compares,moves;
	double avgcompares = 0, avgmoves=0, avgmiliseconds=0;
	double miliseconds;
	clock_t start, end;
	FILE *file = fopen(name,"w");
	if (file == NULL){
		printf("file open error");
		return 1;
	}
	fprintf(file, "n compares moves ms\n");
	for(int n=100;  n<=10000; n+=100){
		for(int i=0; i<k; i++){	
			array = malloc(sizeof(int)*n);
			for(int j=0; j<n; j++){
                if (mod != 0)
				    array[j] =  pcg32_random_r(&rng) % mod;
                else 
                    array[j] =  pcg32_random_r(&rng);
			}
			compares=0;
			moves=0;
	
			start = clock();
			switch (type) {
				case 1:
					insertionSort(array, n, &compares, &moves, comp);
					break;
				case 2:
					array = mergeSort(array, 0, n-1, &compares, &moves, comp);
					break;
				case 3:
					quickSort(array, 0, n-1, &compares, &moves, comp);
					break;
				case 4:
					dualPivotQuickSort(array, 0, n-1, &compares, &moves, comp);
					break;
                case 5:
					radixSort(array, n, &compares, &moves, comp);
					break;    
			}
			end = clock();
			
			miliseconds = (double) ((end-start) / 1000.0);
			
			avgcompares += (double) (1.0*compares/k);
			avgmoves += (double) (1.0*moves/k);
			avgmiliseconds += (double) miliseconds/k;
			
			if (!isSorted(array,n,comp))
				fprintf(stderr,"NIE DOBRZE");
		
			free(array);
		}
		compares = (int) avgcompares;
		moves = (int) avgmoves;
		fprintf(file, "%d %d %d %f\n",n,compares,moves,avgmiliseconds);
		avgcompares=avgmoves=avgmiliseconds=0;
	}
	fclose (file);
	return 0;
} 
    