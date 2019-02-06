#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
 
struct Node {														//Węzeł - struktura - elementy kolejki
    int value;														//Wartość
    struct Node* next;												//Wskaźnik na następny element
};
 
void add(struct Node** head, struct Node** tail, int val) {			//Dodanie wartości do kolejki
    struct Node* new = malloc(sizeof(struct Node));					//Tworzenie nowego elementu
    new -> value = val;												//O danej wartości
    new -> next = NULL;
    
	if (*head == NULL){												//Jeżeli głowa jest pusta :--)
		*head = new;												//Zarówno head, jak i tail będą wskazywać na nowy element
		*tail = new;
	} 
	else {															//jeśli nie
		(*tail) -> next = new;										//dopinamy za tail
		*tail = new;												//przepinamy tail na nowy element
	}		
    printf("Dodaje %d\n", val);
}
 
int get_first(struct Node** head, struct Node** tail) {				//wyciagamy element z kolejki
    if (*head == NULL)												//jezeli jest pusta
        return INT_MIN;												//to zwracamy INT_MIN
   
    int temp_value = (*head) -> value;								//tymczasowo przechowujemy wartosc
    struct Node* temp_pointer = (*head) -> next;					//i wskaznik na nastepny element

    free(*head);													//zwalniamy pierwszy element

    *head = temp_pointer;											//przepinamy head
	if (*head == NULL)												//i ewentualnie tail
		*tail=NULL;
    return temp_value;												//zwracamy przechowana wartosc
}
 
void draw(struct Node* q){											//rysowanie kolejki
    struct Node* tmp = q;
    printf("q = -> ");
    while (tmp != NULL){
        printf("(%d, %p) -> ",tmp->value, tmp->next);				//graficzne strzaleczki i takie tam
        tmp = tmp -> next;
    }
    printf(" NULL\n");
}
 
int main(){
    struct Node* head = NULL;
	struct Node* tail = NULL;
    draw(head);
    add(&head,&tail, 5);
    draw(head);
    add(&head,&tail,  10);
    draw(head);
    printf("Biore %d\n", get_first(&head,&tail));
    draw(head);
    add(&head,&tail,  11);
    draw(head);
    printf("Biore %d\n", get_first(&head,&tail));
    draw(head);
    printf("Biore %d\n", get_first(&head,&tail));
    draw(head);
    add(&head,&tail,  666);
    draw(head);
    printf("Biore %d\n", get_first(&head,&tail));
    draw(head);
    return 0;
}