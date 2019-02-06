#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>

typedef struct LE{
    int v;
    struct LE* next;
} ListElement;

typedef struct List{
    ListElement* head;
    ListElement* tail;
} List;

List* makeList(void){
    List* l = malloc(sizeof(List));
    l->head = l->tail = NULL;
    return l;
}

void insertList(int v, List* l){
    ListElement* tmp = malloc(sizeof(ListElement));
    tmp->v = v;
    tmp->next = NULL;
    if(l->tail){
        l->tail->next = tmp;
        l->tail = tmp;
        return;
    }
    l->head = l->tail = tmp;
}

int popList(List* l){
    if(l->head){
        ListElement* tmp = l->head;
        int v = tmp->v;
        l->head = tmp->next;
        if(!l->head)
            l->tail=NULL;
        free(tmp);
        return v;
    }
    return INT_MIN;
}

typedef struct E{
    int v;
    int p;
} PriorityQueueElement;

typedef struct PQ{
    PriorityQueueElement** e;
    int size;
    int maxSize;
} PriorityQueue;

PriorityQueue* makeQueue(void){
    PriorityQueue* pq = malloc(sizeof(PriorityQueue));
    pq->size=0;
    pq->maxSize=128;
    pq->e = calloc(pq->maxSize, sizeof(PriorityQueueElement*));
    return pq;
}

int expandQueue(PriorityQueue* pq){
    pq->maxSize *= 2;
    void* tmp;
    if((tmp = realloc(pq->e, pq->maxSize * sizeof(PriorityQueueElement*))) == NULL){
        free(pq->e);
        return 0;
    }
    pq->e = tmp;
    return 1;
}

void swapQueue(PriorityQueueElement* a, PriorityQueueElement* b){
    PriorityQueueElement tmp = *a;
    *a = *b;
    *b = tmp;
}

int parent(int i) {
    return (i - 1) / 2;
}

int left(int i) {
    return 2 * i + 1;
}

int right(int i) {
    return 2 * i + 2;
}

void siftUp(int i, PriorityQueue* pq){
    while(i > 0 && pq->e[parent(i)]->p > pq->e[i]->p){
        swapQueue(pq->e[i], pq->e[parent(i)]);
        i = parent(i);
    }
}

int insertQueue(int v, int p, PriorityQueue* pq){
    if(pq->size == pq->maxSize)
        if(!expandQueue(pq))
            return 0;
    pq->e[pq->size] = malloc(sizeof(PriorityQueueElement));
    pq->e[pq->size]->v = v;
    pq->e[pq->size]->p = p;
    int i = pq->size++;
    siftUp(i, pq);
    return 1;
}
    
int emptyQueue(PriorityQueue* pq){
    return pq->size ? 0 : 1;
}

int topQueue(PriorityQueue* pq){
    return pq->size ? pq->e[0]->v : INT_MIN;
}

void heapifyQueue(int i, PriorityQueue* pq){
    int l = left(i);
    int r = right(i);
    int smallest;
    if(l < pq->size && pq->e[l]->p < pq->e[i]->p)
        smallest = l;
    else 
        smallest = i;
    if(r < pq->size && pq->e[r]->p < pq->e[smallest]->p)
        smallest = r;
    if(smallest != i) {
        swapQueue(pq->e[i], pq->e[smallest]);
        heapifyQueue(smallest, pq);
    }
}

int popQueue(PriorityQueue* pq){
    if(pq->size){
        int result = pq->e[0]->v;
        pq->e[0] = pq->e[--pq->size];
        heapifyQueue(0, pq);
        return result;
    }
    return INT_MIN;
}

void priorityQueue(int v, int p, PriorityQueue* pq) {
    List* list= makeList();
    for(int i = 0; i < pq->size; i++) {
        if(pq->e[i]->v == v && pq->e[i]->p > p) {
            pq->e[i]->p = p;
            insertList(i,list);
        }
    }
    int tmp;
    while((tmp = popList(list)) != INT_MIN) {
        siftUp(tmp, pq);
    }
}

void printQueue(PriorityQueue* pq){
    for(int i=0; i < pq->size; i++)
        printf("(%d, %d) ",pq->e[i]->v,pq->e[i]->p);
    printf("\n");
}



int main(void){
    PriorityQueue* pq = makeQueue();
    int n;
    scanf("%d",&n);

    for(int i = 0; i < n; i++){
        char command[20];
        scanf("%s",command);
        printf("|%s|\n",command);
        if(!strcmp(command,"insert")){
            int v, p;
            scanf("%d",&v);
            scanf("%d",&p);
            if(!insertQueue(v, p, pq)){
                fprintf(stderr,"Blad w relokacji pamieci\n");
                return 1;
            }
        } 
        else if(!strcmp(command,"empty")){
            printf("%d\n",emptyQueue(pq));
        } 
        else if(!strcmp(command,"top")){
            int top;
            if((top = topQueue(pq)) != INT_MIN)
                printf("%d\n",top);
            else
                printf("\n");
        } 
        else if(!strcmp(command,"pop")){
            int pop;
            if((pop = popQueue(pq)) != INT_MIN)
                printf("%d\n",pop);
            else
                printf("\n");
        } 
        else if(!strcmp(command,"priority")) {
            int v, p;
            scanf("%d",&v);
            scanf("%d",&p);
            priorityQueue(v, p, pq);
        } 
        else if(!strcmp(command,"print")) {
            printQueue(pq);
        }
    }

    return 0;
}