#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <stdint.h>

#include "pcg_basic.h"

void insertionSort(int* array, int n, int* compares, int* moves, bool comp);

int* mergeSort(int* array, int start, int end, int* compares, int* moves, bool comp);
int* merge(int* A, int* B, int lengthA, int lengthB, int* compares, int* moves, bool comp);

void quickSort(int* array, int start, int end, int* compares, int* moves, bool comp);
int singlePivotPartition(int* array, int start, int end, int* compares, int* moves, bool comp);

bool dualPivotFirstCompare(int x, int lp, int rp, bool flag, bool comp);
bool dualPivotSecondCompare(int x, int lp, int rp, bool flag, bool comp);
void dualPivotQuickSort(int* array, int start, int end, int* compares, int* moves, bool comp);
int dualPivotPartition(int* array, int start, int end, int* leftPivot, int* compares, int* moves, bool comp);



void swap(int* a, int* b);

/******************************************** INSERTION *************************************************************/

bool insertionSortCompare(int a, int b, bool comp){
	if(!comp)
		return a>b;
	else
		return b>a;
}

void insertionSort(int* array, int n, int* compares, int* moves, bool comp){
	int key;
	int i;

	for(int j = 1; j < n; j++){
		key = array[j]; (*moves)++;
		i = j - 1;
		while(i > -1 && ++(*compares) && insertionSortCompare(array[i], key, comp)){
			array[i+1] = array[i]; (*moves)++;
			i--;
		}
		array[i+1] = key; (*moves)++;	
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
	/*int r = rand()%(end-start+1);
	swap(&array[start],&array[start+r]);*/
	
    int x = array[start];   
    int i = start;
 
    for (int j = start + 1; j <= end; j++){
        if (++(*compares) && quickSortCompare(array[j], x, comp)){   
			swap(&array[++i], &array[j]); (*moves)+=2;
        }
    }
	swap(&array[i], &array[start]); (*moves)+=2;
	
    return i;
}

/******************************************** DUAL PIVOT QUICK *************************************************************/

void dualPivotQuickSort(int* array, int start, int end, int* compares, int* moves, bool comp){
    if (start < end) {
        int leftPivot, rightPivot; 
        rightPivot = dualPivotPartition(array, start, end, &leftPivot, compares, moves, comp);	//losowanie obu piwotów, prawy zwracany, lewy przez wskaznik
		
        dualPivotQuickSort(array, start, leftPivot - 1, compares, moves, comp);	
        dualPivotQuickSort(array, leftPivot + 1, rightPivot - 1, compares, moves, comp);		//3 rekurencje wzgledem 2 piwotow
        dualPivotQuickSort(array, rightPivot + 1, end, compares, moves, comp);
    }
}
 
bool dualPivotFirstCompare(int x, int lp, int rp, bool flag, bool comp){
	if(!flag && !comp)    //lewy piwot, niemalejaco
		return x < lp;
	
	if(flag && !comp)	//prawy piwot, niemalejaco
		return x >= rp;
	
	if(!flag && comp)	//lewy piwot, nierosnaco
		return x >= lp;
	
	if(flag && comp)	//prawy piwot, nierosnaco
		return x < rp;
	
	return false;
}

bool dualPivotSecondCompare(int x, int lp, int rp, bool flag, bool comp){
	if(!flag && !comp) 		//prawy piwot, niemalejaco
		return x >= rp;
	
	if(flag && !comp)		//lewy piwot, niemalejaco
		return x < lp;
	
	if(!flag && comp)		//prawy piwot, nierosnaco
		return x < rp;
	
	if(flag && comp)		//lewy piwot, nierosnaco
		return x >= lp;
	
	return false;
}
	

int dualPivotPartition(int* array, int start, int end, int* leftPivot, int* compares, int* moves, bool comp){
	/*if (end-start+1 > 1){
		int l = rand()%(end-start+1); 
		int r = rand()%(end-start+1);
		if (l==r || array[l] == array[r])			//losowanie piwotow
			r = rand()%(end-start+1);
		swap(&array[start],&array[start+l]);
		swap(&array[end],&array[start+r]);
		(*moves)+=4;
	}*/
	
    if(++(*compares) && array[start] > array[end] && !comp){		//jezeli sortujemy niemalejaco, to  bierzemy lewy piwot mniejszy od prawego
        swap(&array[start], &array[end]); (*moves)+=2;
	}
	else if(++(*compares) && array[start] < array[end] && comp){	//jezeli nierosnaco, to na odwrot
        swap(&array[start], &array[end]); (*moves)+=2;
	}
	
    int i = start + 1;		//iterator po tablicy
    int right = end - 1;	//prawy 'wskaznik'
	int left = start + 1;	//lewy 'wskaznik'
	int lp = array[start];	//lewy piwot
	int rp = array[end];	//prawy piwot
	int count = 0;			//Licznik do Count strategy
	bool countFlag;			//flaga count -> false = porownuj najpierw z lewym, true = porownuj najpierw z prawy,
	bool swapLeft = false;	//flaga lewego swapu -> zapalona =  swapujemy obecny element z tym  pod lewym 'wskaznikiem'
	bool swapRight = false; 	//flaga prawego swapu -> zapalona =  swapujemy obecny element z tym  pod prawym 'wskaznikiem'
	
    while (i <= right) {		
 		if (count > 0)				//jezeli liczba elementow za prawym piwotem jest wieksza od liczby elementow przed lewym piwotem
			countFlag = true;
		else
			countFlag = false;
		
        if (++(*compares) && dualPivotFirstCompare(array[i], lp, rp, countFlag, comp)) {  // zwiekszamy compares, porownujemy array[i] z leym lub prawym piwotem w zaleznosci od countFlag 
            if(!countFlag)		//jezeli porownanie bylo prawdziwe, to w zaleznosci od tego, z czym porownywalismy (lewy czy prawy piwot) ustalamy flage swapu
				swapLeft=true;
			else 
				swapRight=true;
        }
        else if (++(*compares) && dualPivotSecondCompare(array[i], lp, rp, countFlag, comp)) {		//to samo, co wyzej, dla drugiego piwota
			if (!countFlag)
				swapRight=true;
			else
				swapLeft=true;
        }
		
		if(swapLeft){		//lewy swap
			swap(&array[i], &array[left++]); (*moves)+=2;		//swapujemy array[i] z array[left] i zwiekszamy left o 1
			count--;											//element poszedl za lewy piwot, wiec count = count-1
			swapLeft=false;										//gasimy flage swapu
		}
		else if(swapRight){		//prawy swap
			while (!countFlag && ++(*compares) && dualPivotSecondCompare(array[right],lp,rp, countFlag, comp) && i < right) 	//dopoki elementy pod naszym prawym wskaznikiem sa w dobrej relacji z prawym piwotem, to mozemy zmniejszac prawy wskaznik bez koniecznosci swapowania
            	right--;
			while (countFlag && ++(*compares) && dualPivotFirstCompare(array[right],lp,rp, countFlag, comp) && i < right)  //to samo tutaj, ale dla drugiej relacji
            	right--;
			
            swap(&array[i], &array[right--]); (*moves)+=2;  //swapujemy array[i] z array[right] i zmnieszamy right o 1
			count++;			//element wszedl, za prawy piwot, count = count+1
            
			if (!countFlag && dualPivotFirstCompare(array[i], lp, rp, countFlag, comp)) {		//sprawdzamy, czy element, ktory przenieslismy na i-ta pozycje jest w dobrej relacji (bo potem i zostanie zwiekszone i moglby zostac pominiety)
				swap(&array[i], &array[left++]); (*moves)+=2;	//zwykly swap left
				count--;
			}
			else if (countFlag && dualPivotSecondCompare(array[i], lp, rp, countFlag, comp)) { //to samo, ale dla drugiej relacji
				swap(&array[i], &array[left++]); (*moves)+=2;	//swap left
				count--;
			}
			swapRight=false;  //gasimy flage swapRight
		}
		
        i++;		//zwiekszamy i o 1
    }
    left--;				//ustawiamy piwoty na wlasciwe miejsce, bo po wyjciu z petli sie minely
    right++;

	
    swap(&array[start], &array[left]);		//wstawiamy piwoty na dobre miejsca
    swap(&array[end], &array[right]);
 	(*moves)+=4;

    *leftPivot = left; 			//lewy piwot przekazujemy przez wskaznik
    return right;				//prawy zwracamy
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
	int type=3, k = 1;
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
				if(argc > 0)
					k = atoi(argv[0]);
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
	//srand(time(NULL));
	
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
			}
			end = clock();
			
			miliseconds = (double) ((end-start) / 1000.0);
			
			avgcompares += (double) (1.0*compares/k);
			avgmoves += (double) (1.0*moves/k);
			avgmiliseconds += (double) miliseconds/k;
			
			if (!isSorted(array,n,comp))
				fprintf(stderr,"NIE JEST DOBRZE");
		
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