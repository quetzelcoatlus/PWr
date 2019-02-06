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

typedef struct Edge{
    int u;
    int v;
    float w;
} Edge;

Edge* makeEdge(int u, int v, float w){
    Edge* e = malloc(sizeof(Edge));
    e->u=u;
    e->v=v;
    e->w=w;
    return e;
}

typedef struct Adjacents{
    Element** edges;
    int size;
    int maxSize;
} Adjacents;


Adjacents* makeAdjacents(int n){
    Adjacents* a = malloc(sizeof(Adjacents));
    a->size =0;
    a -> maxSize = n;
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

typedef struct UG{
    int n;
    int m;
    Adjacents** adjacents;
    Edge** edges;
} UndirectedGraph;

UndirectedGraph* makeUndirectedGraph(int n, int m, int p){
    UndirectedGraph* ug = malloc(sizeof(UndirectedGraph));
    ug->n = n;
    ug->m = m;
    if(p){
        ug->adjacents = malloc(sizeof(Adjacents*) * (n+1));
        for(int i=0; i<n+1; i++)
            ug->adjacents[i]=makeAdjacents(ug->n);
        ug->edges = NULL;
    }
    else {
        ug->edges = malloc(sizeof(Edge*) * ug->m);
        ug->adjacents = NULL;
    }
    return ug;
}

void addEdges(UndirectedGraph* ug, int p){
    for(int i=0; i < (ug->m); i++){
        int u,v;
        float w;
        scanf("%d %d %f",&u, &v, &w);
        if(p){
            insertAdjacents(v, w, ug->adjacents[u]);
            insertAdjacents(u, w, ug->adjacents[v]);
        }
        else {
            ug->edges[i] = makeEdge(u,v,w);
        }
    }
    
}

float getWeight(int u, int v, UndirectedGraph* ug){
    for(int i=0; i<ug->adjacents[u]->size; i++){
        Element* tmp = getElementAt(i, ug->adjacents[u]);
        if(tmp->v == v)
            return tmp->p;
    }
    return INT_MIN;
}


void primMST(int s, UndirectedGraph* ug){
    int n=ug->n;
    float key[n+1];
    int prev[n+1];
    int inMST[n+1];
    for(int i = 1; i <= n; i++) {
        key[i] = INT_MAX;
        prev[i] = INT_MIN;
        inMST[i] = 0;
    }
    key[s] = 0;

    PriorityQueue* pq = makeQueue();
    for(int i = 1; i <= n; i++)
        insertQueue(i, key[i], pq);

    while(!emptyQueue(pq)) {
        int u = popQueue(pq);
        inMST[u] = 1;
        for(int i=0; i< ug->adjacents[u]->size; i++){
            Element* tmp = getElementAt(i, ug->adjacents[u]); 
            int v = tmp->v;
            float w = tmp->p;
            if(inMST[v] == 0 && key[v] > w) {
                key[v] = w;
                priorityQueue(v, key[v],pq);
                prev[v] = u;           
            }
        }
    }

    float acc = 0;
    for(int i = 1; i <= n; ++i) {
        if(i == s)
            continue;
        float w = getWeight(prev[i], i,ug);
        acc += w;
        if(prev[i] < i)
            printf("%d %d %f\n",prev[i],i,w);
        else
             printf("%d %d %f\n",i,prev[i],w);
    }
    printf("%f\n",acc);
}


typedef struct Subset {
    int p, rank;
} Subset;

int find(Subset subs[], int i) {
    if(subs[i].p != i)
        subs[i].p = find(subs, subs[i].p);

    return subs[i].p;
}

void Union(Subset subs[], int x, int y) {
    int xroot = find(subs, x);
    int yroot = find(subs, y);

    if(xroot == yroot) return;

    if (subs[xroot].rank < subs[yroot].rank)
        subs[xroot].p = yroot;
    else if (subs[xroot].rank > subs[yroot].rank)
        subs[yroot].p = xroot;
    else {
        subs[yroot].p = xroot;
        subs[xroot].rank++;
    }
}


void kruskalMST(UndirectedGraph* ug) {
    int n=ug->n;
    Subset subs[n+1];
    int s = 0, tree_size = n - 1;
    Edge result[tree_size];
    for(int i = 1; i <= n; i++) {
        subs[i].p = i;
        subs[i].rank = 0;
    }

    PriorityQueue* pq = makeQueue();
    for(int i = 0; i < ug->m; i++)
        insertQueue(i, ug->edges[i]->w, pq);

    while(s < tree_size) {
        int e =popQueue(pq);

        int x = find(subs, ug->edges[e]->u);
        int y = find(subs, ug->edges[e]->v);

        if(x != y) {
            result[s++] = *(ug->edges[e]);
            Union(subs, x, y);
        }
    }

    float acc = 0;
    for(int i = 0; i < s; i++) {
        acc += result[i].w;
        if(result[i].u < result[i].v)
            printf("%d %d %f\n",result[i].u,result[i].v, result[i].w);
        else 
            printf("%d %d %f\n",result[i].v,result[i].u, result[i].w);
    }
     printf("%f\n",acc);
}


int main(int argc, char *argv[]){
    if (argc < 2)
        return 1;
    srand (time(NULL));
    int n, m,p;
    scanf("%d",&n);
    scanf("%d",&m);
    if(!strcmp(argv[1],"-p"))
        p=1;
    else
        p=0;
    
    UndirectedGraph* ug = makeUndirectedGraph(n,m,p);
    addEdges(ug,p);
    if(p) {
        int random = rand() % n + 1;
        primMST(random,ug);
    }
    else
        kruskalMST(ug);

    return 0;
}