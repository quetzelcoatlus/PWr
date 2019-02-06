#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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
    float p;
} Element;

typedef struct PQ{
    Element** e;
    int size;
    int maxSize;
} PriorityQueue;

PriorityQueue* makeQueue(void){
    PriorityQueue* pq = malloc(sizeof(PriorityQueue));
    pq->size=0;
    pq->maxSize=128;
    pq->e = calloc(pq->maxSize, sizeof(Element*));
    return pq;
}

int expandQueue(PriorityQueue* pq){
    pq->maxSize *= 2;
    void* tmp;
    if((tmp = realloc(pq->e, pq->maxSize * sizeof(Element*))) == NULL){
        free(pq->e);
        return 0;
    }
    pq->e = tmp;
    return 1;
}

void swapQueue(Element* a, Element* b){
    Element tmp = *a;
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
    while(i > 0 && pq->e[parent(i)]->p > pq->e[i]->p) {
        swapQueue(pq->e[i], pq->e[parent(i)]);
        i = parent(i);
    }
}

int insertQueue(int v, float p, PriorityQueue* pq){
    if(pq->size == pq->maxSize)
        if(!expandQueue(pq))
            return 0;
    pq->e[pq->size] = malloc(sizeof(Element));
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
        printf("[%d, %f] ",pq->e[i]->v,pq->e[i]->p);
    printf("\n");
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////


typedef struct Adjacents{
    Element** edges;
    int size;
    int maxSize;
} Adjacents;

typedef struct DG{
    int n;
    int m;
    Adjacents** adjacents;
} DirectedGraph;


Adjacents* makeAdjacents(int n){
    Adjacents* a = malloc(sizeof(Adjacents));
    a->size = 0;
    a-> maxSize = n;
    a->edges = malloc(sizeof(Element*) * a->maxSize);
    return a;
}

int expandAdjacents(Adjacents* a){
    a->maxSize *= 2;
    void* tmp;
    if((tmp = realloc(a->edges, a->maxSize * sizeof(Element*))) == NULL){
        free(a->edges);
        return 0;
    }
    a->edges = tmp;
    return 1;
}

int insertAdjacents(int v, float p, Adjacents* a){
    if(a->size == a->maxSize)
        if(!expandAdjacents(a))
            return 0;
    a->edges[a->size] = malloc(sizeof(Element));
    a->edges[a->size]->v = v;
    a->edges[a->size]->p = p;
    a->size++;
    return 1;
}

Element* getElementAt(int i, Adjacents* a){
    if(i<0 || i >= a->size)
        return NULL;
    return a->edges[i];
}


DirectedGraph* makeDirectedGraph(int n, int m){
    DirectedGraph* dg = malloc(sizeof(DirectedGraph));
    dg->n = n;
    dg->m = m;
    dg->adjacents = malloc(sizeof(Adjacents*) * (n+1));
    for(int i=0; i<n+1; i++)
        dg->adjacents[i]=makeAdjacents(dg->n);
    return dg;
}

void addEdges(DirectedGraph* dg){
    for(int i=0; i < (dg->m); i++){
        int u,v;
        float w;
        scanf("%d %d %f",&u, &v, &w);
        insertAdjacents(v, w, dg->adjacents[u]);
    }
    
}

float getWeight(int u, int v, DirectedGraph* dg) {
    for(int i=0; i < dg->adjacents[u]->size; i++){
        Element* tmp = getElementAt(i, dg->adjacents[u]);
        if(tmp->v == v)
            return tmp->p;
    }
    return INT_MIN;
}

void shortestPath(int s, DirectedGraph* dg) {
    int n= dg->n;
    float dist[n+1];
    int prev[n+1];
    for(int i = 1; i <= n; i++) {
        dist[i] = INT_MAX;
        prev[i] = INT_MIN;
    }
    dist[s] = 0;
    PriorityQueue* pq = makeQueue();
    for(int i = 1; i <= n; i++)
        insertQueue(i, dist[i], pq);
    while(!emptyQueue(pq)) {
        int u = popQueue(pq);
        for(int i=0; i< dg->adjacents[u]->size; i++){
            Element* tmp = getElementAt(i, dg->adjacents[u]); 
            int v = tmp->v;
            float w = tmp->p;
            if(dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                prev[v] = u;
                priorityQueue(v, dist[v],pq);
            }
        }
    }

    for(int i = 1; i <= n; i++)
        if(dist[i] == INT_MAX)
            printf("%d INFINITY\n",i);
        else
            printf("%d %f\n",i,dist[i]);
    for(int i = 1; i <= n; i++) {
        int curr = i;
        int p = prev[i];
        fprintf(stderr,"(%d) ",i);
        while(p != INT_MIN) {
            fprintf(stderr,"<==%f== (%d) ",getWeight(p, curr,dg),p);
            curr = p;
            p = prev[p];
        }
        fprintf(stderr,"\n");
    }
}


int main(void){
    clock_t start, end;
    int n, m, s;
    scanf("%d",&n);
    scanf("%d",&m);
    DirectedGraph* dg = makeDirectedGraph(n,m);
    addEdges(dg);
    scanf("%d",&s);
    start = clock();
    shortestPath(s,dg);
    end = clock();
    fprintf(stderr, "%f ms\n",((double) (end - start)) / CLOCKS_PER_SEC * 1000);
    return 0;
}