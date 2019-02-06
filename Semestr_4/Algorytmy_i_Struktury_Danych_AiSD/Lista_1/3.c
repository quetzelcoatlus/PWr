#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
 
struct Node {																		//Elementy listy
    int value;
    struct Node* next;
    struct Node* prev;
};
 
void add_last(struct Node** list, int val) {										//dodanie na koniec
    struct Node* new = malloc(sizeof(struct Node));
    new -> value = val;
	new -> next = *list == NULL ? new : *list;										//jezeli lista jest pusta, to wskazuj na sama siebie, jezeli nie, to next na pierwszy element, a prev na poprzedni
	new -> prev = *list == NULL ? new : (*list) -> prev;
   
	if (*list == NULL){																//jezeli lista jest pusta, to skieruj ja na new
	   *list = new;
	}	
	else {																			//jezeli nie, to
		(*list) -> prev -> next = new;												//podepnij poprzedni i nastepny do new
		(*list) -> prev = new;
	}
}
 
void add_first(struct Node** list, int val) { 										//dodanie elementu na poczatek
    struct Node* new = malloc(sizeof(struct Node));							
    new -> value = val;
	new -> next = *list == NULL ? new : *list;										//inicjalizacja: jezeli lista jest pusta, to wskazuje na samego siebie
	new -> prev = *list == NULL ? new : (*list) -> prev;
   
	if (*list != NULL){
	    (*list) -> prev -> next = new;												//to samo, co wczesniej, tylko
		(*list) -> prev = new;
	}
	*list = new;																	//zawsze zmien head na nowy element
}
 
int get_first(struct Node** list) {													//pobranie pierwszego elementu
    if (*list == NULL)
        return INT_MIN;
   
    int temp_value = (*list) -> value;												//tymczasowa wartosc - do zwrocenia
    struct Node* temp_pointer = *list == (*list) -> next ? NULL : (*list) -> next;	//jezeli jest jeden element, to NULL, jak nie, to nastepny
	(*list) -> next -> prev = (*list) -> prev;
	(*list) -> prev -> next = (*list) -> next;										//przepiecie elementow poprzedniego do nastepnego
    free(*list);
    *list = temp_pointer;
    return temp_value;
}
 
int get_last(struct Node** list) {													//zwroc ostatni
    if (*list == NULL)
        return INT_MIN;
   
    int temp_value = (*list) -> prev -> value;										//tymczasowa wartosc
	struct Node* temp_pointer = (*list) -> prev;									//tymczasowy wskaznik
	
	(*list) -> prev -> prev -> next = *list;
	(*list) -> prev = (*list) -> prev -> prev;
	
	if(temp_pointer == *list)														//jezeli jest tylko 1 element, to lista na NULL
		*list = NULL;
    free(temp_pointer);																

    return temp_value;
}
 

int get_size(struct Node* list){													//oblicz rozmiar
    struct Node* tmp = list;
    int count = 0;
    while (tmp != NULL && (tmp != list || count == 0)){								//jezeli nie jest pusta i dopoki nie jest koniec
        count++;																	//dodawaj 1 do count
        tmp = tmp -> next;
    }
    return count;
}

int get_place_of_value(struct Node* list, int value){								//miejsce pierwszego wystapienia wartosci
    struct Node* tmp = list;
    int count = 0;
    while (tmp != NULL && (tmp != list || count == 0) && tmp -> value != value){	//jezeli nie jest pelna, dopoki nie jest koniec listy i nie osiagnelismy wartosci szukanej
        count++;
        tmp = tmp -> next;
    }
    return count;
}

int get_value_at(struct Node* q, int place, int size){								//zwroc wartosc na danym miejscu
	
	if(place < size/2){																//jezeli miejsce jest do polowy
		struct Node* tmp = q;
		int count = 0;
		while (count < place){														//idz od poczatku do miejsca
			count++;
			tmp = tmp -> next;
		}
	   
		return tmp -> value;   
	}
	else {																			//jezeli jest od polowy
		struct Node* tmp = q;
		int count = size;
		while(count > place){														//schodz z gory do danego miejsca
			count--;
			tmp = tmp -> prev;
		}	
		return tmp -> value;
	}
}

void delete_at(struct Node** list, int place){										//usuwanie wartosci na danym miejscu
    struct Node* tmp = *list;
    int count = 0;
    while (tmp != NULL && (tmp != *list || count==0) && count < place-1){				//pusta, koniec lub doszlismy
        count++;
        tmp = tmp -> next;
    }
	
   
    if (tmp == NULL || tmp -> next == NULL)											//pusta = wyjdz
        return;
   
	struct Node* tmp2 = tmp -> next -> next;
	tmp -> next -> next -> prev = tmp;											 	//przepinanie wskaznikow poprzedniego i nastepnego
	
	if (tmp -> next == *list)
		*list = NULL;
	
	free(tmp -> next);
	tmp -> next = tmp2;
    return;   
}

void merge(struct Node** lista1, struct Node* lista2){								//scalanie list
    struct Node* temp_lista = NULL;													//lista, do ktorej skopiujemy liste2
    struct Node* tmp_pointer = lista2;												//tymczasowy wskaznik
    int i = 0;
    while (tmp_pointer != lista2 || i == 0){										//dopoki nie jest koniec listy2
        add_last(&temp_lista,get_value_at(lista2,i,get_size(lista2)));				//kopiuj kolejne elementy
        tmp_pointer = tmp_pointer -> next;
        i++;
    }
   
    if (*lista1 == NULL){
        *lista1 = temp_lista;
        return;
    }
   
	struct Node* tmp_koniec1 = (*lista1) -> prev;									//tymczasowy wskaznik pomocniczy na koniec pierwszej listy
   																					//przepinanie wskaznikow:
	(*lista1) -> prev -> next = temp_lista;											//koniec pierwszej listy na poczatek drugiej
	temp_lista -> prev -> next = *lista1;											//koniec drugiej listy na poczatek pierwszej
	
	(*lista1) -> prev = temp_lista -> prev;											//poczatek pierwszej listy na koniec drugiej
	temp_lista -> prev = tmp_koniec1;												//poczatek drugiej listy na koniec pierwszej
	
	return;
}
 

void draw(struct Node* q){															//rysowanko
    struct Node* tmp = q;
    printf("q = -> ");
	int i=0;
    while (tmp != NULL && (tmp != q || i == 0)){
        printf("(%d, %p) -> ",tmp->value, tmp->next);
        tmp = tmp -> next;
		i++;
    }
    printf(" NULL\n");
}
 
 float measure(struct Node* head, int place,int size){								//mierzenie czasu dostępu
	clock_t start = clock();
	
	for(int i=0; i<10000; i++){
		get_value_at(head,place,size);
	}
	
	clock_t end = clock();
	return (float)(end - start) / CLOCKS_PER_SEC * 100.0;
}
 
int main(){
    struct Node* head = NULL;
	
	srand(time(NULL));   
	
	for (int i=0; i<1000; i++){
		add_last(&head,rand());
	}
	//printf("%d",get_place_of_value(head,444));
	for(int i=0; i<1000; i+=100){
		printf("Dostep do elementu %d wynosi srednio: %f mikrosekund\n",i,measure(head,i,1000));
	}
	int rnd=rand();
	printf("Dostep do elementu %d wynosi srednio: %f mikrosekund\n",rnd,measure(head,rnd,1000));
	
	clock_t start = clock();
	
	
	
	for(int i=0; i<10000; i++){
		get_value_at(head,rand()%1000,1000);
	}
	
	clock_t end = clock();
	float seconds = (float)(end - start) / CLOCKS_PER_SEC*100.0;
	printf("Dostep do losowego elementu wynosi srednio: %f mikrosekund\n",seconds);
	
    return 0;
}