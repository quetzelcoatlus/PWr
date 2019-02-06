#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
 
struct Node {															//Element listy
    int value;															//wartosc elementu
    struct Node* next;													//wskaznik na nastepny element
};
 
void add_last(struct Node** list, int val) {							//dodanie elementu na koniec listy
    struct Node** tmp = list;
    while (*tmp != NULL){												//przejscie przez liste do ostatniego elementu
        tmp = &((*tmp) -> next);
    }
    struct Node* new = malloc(sizeof(struct Node));						//nowy element z okreslona wartoscia
    new -> value = val;
    new -> next = NULL;
   
    *tmp = new;														   	//podpiecie do ostatniego elementu
}
 
void add_first(struct Node** list, int val) { 							//dodanie element na poczatek listy
    struct Node* new = malloc(sizeof(struct Node));						//inicjalizacja struktury
    new -> value = val;													//przypisanie wartosci
    new -> next = *list;												//przypisanie (obecnie) pierwszego elementu do next
   
    *list = new;  														//przepiecie głowy na nowy element
}
 
int get_first(struct Node** list) {										//zabranie pierwszego elementu
    if (*list == NULL)													//lista pusta -> zwroc INT_MIN
        return INT_MIN;
   
    int temp_value = (*list) -> value;									//tymczasowe przechowanie wartosci pierwszego elementu
    struct Node* temp_pointer = (*list) -> next;						//tymczasowe przechowanie wskaznika pierwszego elementu
	
    free(*list);														//zwolnienie pierwszego elementu
    *list = temp_pointer;												//przepiecie wskaznika listy
    return temp_value;
}
 
int get_last(struct Node** list) {										//wziecie ostatniego elementu
    if (*list == NULL)													// pusta -> INT_MIN
        return INT_MIN;
   
    struct Node** tmp = list;
    while (*tmp != NULL){												//przejscie do konca listy
        tmp = &((*tmp) -> next);
    }
   
    int temp_value = (*tmp) -> value;									//przechowanie wartosci
 
    free(*tmp);															//zwolnienie elementu
   
    *tmp = NULL;														//niby free ustawia wskaznik na NULL, no ale ja go nie pisalem, wiec nie jestem pewien...
	
    return temp_value;
}
 
int get_value_at(struct Node* list, int place){							//zczytanie wartosci na danym miejscu
    struct Node* tmp = list;											//pomocniczy wskaznik
    
	int count = 0;														//licznik miejsca
    while (tmp != NULL && count < place){								//dopoki nie koniec i nie doszlismy do miejsca
        count++;
        tmp = tmp -> next;
    }
   
    if (tmp == NULL)													//jezeli nie ma na liscie, to zwroc INT_MIN
        return INT_MIN;
   
    return tmp -> value;   											    //jak jest, to zwroc wartosc
}
 

void delete_at(struct Node* list, int place){							//usuwanie elementu na danej pozycji
    struct Node* tmp = list;
    int count = 0;
    while (tmp != NULL && count < place-1){								//idziemy do miejsca przed
        count++;
        tmp = tmp -> next;
    }
	
   
    if (tmp == NULL || tmp -> next == NULL)
        return;
   
	struct Node* tmp2 = tmp -> next -> next;							//przepinamy wskazniki
	free(tmp -> next);													//zwalniamy element
	tmp -> next = tmp2;
    return;   
}

int get_size(struct Node* list){										//oblicz rozmiar listy
    struct Node* tmp = list;											//pomocniczy wskaznik
    int count = 0;														
    while (tmp != NULL){												//dopoki nie koniec, licz miejsca
        count++;
        tmp = tmp -> next;
    }
    return count;
}
 
void merge(struct Node** lista1, struct Node* lista2){					//scalanie
    struct Node* temp_lista = NULL;										//kopia listy2
    struct Node* temp_pointer = lista2;									//pomocniczy wskaznik
    int i = 0;
    while (temp_pointer != NULL){
        add_last(&temp_lista,get_value_at(lista2,i));					//kopiujemy listę2 do nowej listy
        temp_pointer = temp_pointer -> next;
        i++;
    }
   
    if (*lista1 == NULL){												//jezeli 1 jest pusta, to doklejenie jest trywialne
        *lista1 = temp_lista;
        return;
    }
   
    struct Node** tmp = lista1;
    while ((*tmp) -> next != NULL){										//idziemy na koniec listy1
        tmp = &((*tmp) -> next);
    }
   
    ((*tmp) -> next) = temp_lista; 										//doklejamy kopie listy2
    return;
}
 
void draw(struct Node* list){											//rysowanko
    struct Node* tmp = list;
    printf("list = -> ");
    while (tmp != NULL){
        printf("(%d, %p) -> ",tmp->value, tmp->next);
        tmp = tmp -> next;
    }
    printf(" NULL\n");
}
 
float measure_time( struct Node* head, int place){						//mierzenie czasu
	clock_t start = clock();											//start
	
	for(int i=0; i<10000; i++){
		get_value_at(head,place);										//zczytanie wartosci
	}
	
	clock_t end = clock();
	return (float)(end - start) / CLOCKS_PER_SEC *100.0;					//wartosc w milisekndach
}
 
int get_place_of_value(struct Node* head, struct Node* tmp, int value){	//pierwsze wystapienie wartosci
	
	static int place;													//zmienna statyczna - zachowuje wartosc przy kolejnych przebiegach funkcji
	
	if (head == tmp)													//jezeli to pierwsze wykonanie, wyzeruj place
		place = 0;
	
	if (tmp == NULL){													
		place = INT_MIN;												//jezeli nie ma calej liscie, zwroc INT_MIN
		return place;
	}
	
	if (tmp -> value == value)											//jezeli jest ta wartosc, to zwroc miejsce
		return place;
	else {																//jezeli nie...
		place++;														//...powieksz place...
		return get_place_of_value(head, tmp -> next, value);			//..i wywoalj rekurencyjnie sama siebie z tmp -> next
	}
}	
 
int main(){
    struct Node* head = NULL;
	srand(time(NULL));   
	
	for (int i=0; i<1000; i++){
		add_last(&head,rand());
	}
	
	for(int i=0; i<1000; i+=100)
		printf("Dostep do elementu %d wynosi srednio: %f mikrosekund\n",i,measure_time(head,i));
	
	clock_t start = clock();
	
	for(int i=0; i<10000; i++){
		get_value_at(head,rand()%1000);
	}
	
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC*100.0;
	printf("Dostep do losowego elementu wynosi srednio: %f mikrosekund\n",seconds);
    return 0;
}