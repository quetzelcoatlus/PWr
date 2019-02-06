#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

void insertionSort(int* array, int n, int* compares, int* moves, bool comp);

int* mergeSort(int* array, int start, int end, int* compares, int* moves, bool comp);
int* merge(int* A, int* B, int lengthA, int lengthB, int* compares, int* moves, bool comp);

void quickSort(int* array, int start, int end, int* compares, int* moves, bool comp);
int singlePivotPartition(int* array, int start, int end, int* compares, int* moves, bool comp);

void swap(int* a, int* b);

/******************************************** INSERTION *************************************************************/

bool insertionSortCompare(int a, int b, bool comp){
	fprintf(stderr, "Insertion sort: comparing %d with %d\n",a,b);
	if(!comp)			//niemalejaco
		return a>b;
	else				//nierosnaco
		return b>a;
}

void insertionSort(int* array, int n, int* compares, int* moves, bool comp){
	int key;		//klucz
	int i;			

	for(int j = 1; j < n; j++){
		fprintf(stderr, "Insertion sort: key (%d) = array[%d] (%d)\n",key, j, array[j]);
		key = array[j]; (*moves)++;		//przypisanie do klucza
		i = j - 1;
		while(i > -1 && ++(*compares) && insertionSortCompare(array[i], key, comp)){		//dopoki i>=0, zwieksz compares, porownaj array[i] z kluczem
			fprintf(stderr, "Insertion sort: array[%d] (%d) = array[%d] (%d)\n", i+1, array[i+1], i, array[i]);
			array[i+1] = array[i]; (*moves)++;	//przypisz array[i] do array[i+1] i zwieksz 'i' o jeden
			i--;
		}
		fprintf(stderr, "Insertion sort: array[%d] (%d) = key (%d)\n",i+1,array[i+1],key);
		array[i+1] = key; (*moves)++;	//wstaw klucz na wlasciwe miejsce
	}
}


/******************************************** MERGE *************************************************************/


bool mergeSortCompare(int a, int b, bool comp){
	fprintf(stderr, "Merge sort: comparing %d with %d\n",a,b);
	if(comp)		//niemalejaco
		return a>b;
	else			//nierosnaco
		return b>a;
}

int* mergeSort(int* array, int start, int end, int* compares, int* moves, bool comp){
	if (start == end){					//jeden element
		int* r = malloc(sizeof(int));	
		r[0] = array[start];			
		return r;						//zwraca tablice z nim samym
	}
	else{
        int middle = start+(end-start)/2;		//polowa z (start,end)
		
        int* A = mergeSort(array, start, middle, compares, moves, comp);		//merge do polowy
        int* B = mergeSort(array, middle+1, end, compares, moves, comp);		//merge od polowy
 
        return merge(A, B, middle - start +1, end - middle, compares, moves, comp);		//zwroc scalona tablice
    }
}

int* merge(int* A, int* B, int lengthA, int lengthB, int* compares, int* moves, bool comp){
	int* result;
	result = malloc(sizeof(int)*(lengthA + lengthB));			//tablica o lacznej dlugosci tablic do scalenia
	
	int a=0, b=0,i=0;
	while(a < lengthA && b < lengthB){		//dopoki w obu tablicach sa elementy
		if(++(*compares) && mergeSortCompare(A[a], B[b], comp)){		//zwieksz compares i porownaj elementy tablic A i B
			fprintf(stderr, "Merge sort: result[%d] = A[%d] (%d)\n", i, a, A[a]);
			result[i++] = A[a++]; (*moves)++;		//do wyniku wloz element tablicy A  i zwieksz 'i' oraz 'a'
		}
		else{
			fprintf(stderr, "Merge sort: result[%d] = B[%d] (%d)\n", i, b, B[b]);
			result[i++] = B[b++]; (*moves)++;		//do wyniku wloz element tablicy A  i zwieksz 'i' oraz 'b'
		}
	}
	while(a < lengthA){		//wloz pozostale elementy z tablicy A do wyniku
		result[i++] = A[a++]; 
	}
	while(b < lengthB){			//wloz pozostale elementy z tablicy B do wyniku
		result[i++] = B[b++]; 
	}
	
	free(A);		//zwalnianie tablic, ktore scalalismy
	free(B);
	
	return result;
}




/******************************************** QUICK *************************************************************/
bool quickSortCompare(int a, int b, bool comp){
	fprintf(stderr, "Quick sort: comparing %d with %d\n",a,b);
	if(!comp)		//niemalejaco
		return a<=b;
	else		//nierosnaco
		return b<=a;
}

void quickSort(int* array, int start, int end, int* compares, int* moves, bool comp){
	 if (start < end){
        int pivot = singlePivotPartition(array, start, end, compares, moves, comp);  //wyznaczenie piwota
 
        quickSort(array, start, pivot - 1, compares, moves, comp);		//rekurencyjnie QS do piwota
        quickSort(array, pivot + 1, end, compares, moves, comp);		// i od piwota
    }	
}

int singlePivotPartition(int* array, int start, int end, int* compares, int* moves, bool comp){
	/*int r = rand()%(end-start+1);
	swap(&array[start],&array[start+r]);*/
	
    int x = array[start];   	//naszym piwotem pierwszy element
    int i = start;
 
    for (int j = start + 1; j <= end; j++){		//for od start+1 do konca tablicy
        if (++(*compares) && quickSortCompare(array[j], x, comp)){   	//zwiekszamy  compares i porownujemy array[j] z piwotem
			fprintf(stderr, "Quick sort: swapping array[%d] (%d) with array[%d] (%d)\n", i+1, array[i+1], j, array[j]);
			swap(&array[++i], &array[j]); (*moves)+=2;		//jezeli powinien byc po lewej stronie piwota, to zamieniamy z elementem array[i+1]
        }
    }
	fprintf(stderr, "Quick sort: swapping array[%d] (%d) with array[%d] (%d)\n", i, array[i], start, array[start]);
	swap(&array[i], &array[start]); (*moves)+=2;		//piwot trafia na wlasciwe miejsce
	
    return i;
}


/******************************************** OTHER *************************************************************/

void swap(int* a, int* b){		//swap
    int temp = *a;
    *a = *b;
    *b = temp;
}

bool compare(int a, int b, bool comp){
	if(!comp)		//niemalejaco
		return a<=b;
	else		//nierosnaco
		return b<=a;
}

bool isSorted(int* a, int n, bool comp){
	for(int i=0; i+1 < n; i++)
		if(!compare(a[i],a[i+1],comp))  //jezeli sasiednie nie sa w dobrej relacji
		   return false;		//to false
	return true;		
}



/******************************************** MAIN *************************************************************/


int main(int argc, char *argv[]){
	int type = 3;
	bool comp = false;			//default = quicksort, niemalejaco, nielosowo(wczytany z klawiatury)
	bool random = false;
	
	argc--;		//zmiejsz liczbe argumentow
	argv++;		//zmien wskazanie na nastepny argument
	while(argc > 0){	//dopoki sa argumenty
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
		else if(!strcmp((argv[0]), "--rand")){
			random = true;
			argc--;
			argv++;
		}
		else {
			argc--;
			argv++;
		}
	}

	int n;
	scanf("%d",&n);
	
	int* array = malloc(sizeof(int)*n);
	if(random){
		srand(time(NULL));
		for(int i=0; i<n; i++){
			array[i] = rand();
		}
	}
	else {
		for(int i=0; i<n; i++){
			scanf("%d",&(array[i]));
		}
	}
	
	int compares=0, moves=0;
	double miliseconds;
	
	clock_t start = clock();  //poczatkowy pomiar czasu
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
	}
	clock_t end = clock();   //koncowy
	
	miliseconds = (double) ((end-start) / 1000.0);
	if(!isSorted(array, n, comp))
		return 1;
	
	fprintf(stderr, "compares = %d | moves = %d | time %f ms\n", compares, moves, miliseconds);
	
	printf("%d\n",n);
	for(int i=0; i<n-1; i++)
		printf("%d ",array[i]);
	printf("%d\n",array[n-1]);
	
	free(array);
	return 0;
}