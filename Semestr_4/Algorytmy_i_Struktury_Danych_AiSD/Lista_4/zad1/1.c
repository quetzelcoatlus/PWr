#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#define MODULO 128
#define N 200

///////////////////////////////////////////////////////// HASH //////////////////////////////////////////////////////////////

uint32_t jenkins_one_at_a_time_hash(char *key, size_t len){
    uint32_t hash, i;
    for(hash = i = 0; i < len; ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}



///////////////////////////////////////////////////////// structs //////////////////////////////////////////////////////////////

enum treeType  { BST, RBT, HMAP}; 
typedef enum Color { BLACK, RED} Color;


//List

typedef struct ListNode {
    char* value;
    struct ListNode* next;
} ListNode;

typedef struct List{
    ListNode* head;
    int size;
} List;


typedef struct bstNode{
    struct bstNode* p;
    struct bstNode* left;
    struct bstNode* right;
    char* value;
} bstNode;

typedef struct bstTree{
    bstNode* root;
    int size;
    //ewentualne wzbogacenia
} bstTree;

typedef struct rbtNode{
    struct rbtNode* p;
    struct rbtNode* left;
    struct rbtNode* right;
    Color color;
    char* value;
} rbtNode;

typedef struct rbtTree{
    rbtNode* root;
    rbtNode* nil;
    int size;
    //ewentualne wzbogacenia
} rbtTree;

union hashData{
    List* list;
    rbtTree* tree;
};

typedef struct hashListElement{
    union hashData structure;
    int size;
} hashListElement;



typedef struct hashMap{
    hashListElement** data;
    int size;
    int modulo;
    int switchSize;
} hashMap;

union treeData{
    bstTree* bst;
    rbtTree* rbt;
    hashMap* hmap;
};

typedef struct Tree{
    union treeData data;
    enum treeType type;
} Tree;




//////////////////////////////////////////////////////////////////


bool compare(char* a, char* b);
void insertFixRBT(rbtTree* t, rbtNode* z);
void deleteFixRBT(rbtTree* t, rbtNode* z);
char* checkAndCopy(char* key);
char* checkAndCopy2(char* key);


////////////////////////////////////////////LIST ////////////////////////////////


void createList(List* list){
    list -> head = NULL;
    list -> size = 0;
}

void addValueList(List* list, char* val) {
    ListNode* tmp = list->head;
    ListNode* prev = NULL;
    while (tmp && compare(val, tmp->value)){
        prev = tmp;
        tmp = tmp-> next;
    }
    ListNode* new = malloc(sizeof(ListNode));
    new -> value = checkAndCopy2(val);
    new -> next = tmp;
    if(prev)
        prev->next = new;
    else
        list->head=new;
    (list->size)++;
}
 
char* popMiddleValueList(List *list){
    int middle = (list->size/2);
    int i = 0;
    
    ListNode* tmp = list->head;
    ListNode* prev = NULL;
    while (tmp && i < middle){
        prev = tmp;
        tmp = tmp-> next;
        i++;
    }
    
    if (!tmp)
        return NULL;
    
    char* value = checkAndCopy2(tmp->value);
    
    if(prev)
        prev->next = tmp->next;
    else
        list->head=tmp->next;
    
    free(tmp); 
    (list->size)--;
    return value;
}

int deleteValueList(List* list, char* val){
    ListNode* tmp = list->head;
    ListNode* prev = NULL;
    while (tmp && compare(tmp->value,val)){
        prev = tmp;
        tmp = tmp-> next;
    }
    
    if (!tmp || strcmp(tmp->value, val)>0)
        return 0;
    
    if(prev)
        prev->next = tmp->next;
    else
        list->head=tmp->next;
    free(tmp); 
    (list->size)--;
    return 1;
}


 
void findValueList(List* list, char* val){
    ListNode* tmp = list->head;
    while (tmp && compare(tmp->value,val)){
        tmp = tmp-> next;
    }
    if (!tmp || strcmp(tmp->value, val)>0)
        printf("0\n");
    else
        printf("1\n");
}	

void destroyList(List* list){
    ListNode* tmp = list->head;
    ListNode* next = NULL;
    while(tmp && tmp->next){
        next = tmp->next;
        free(tmp);
        tmp = next;
    }
    if(tmp)
        free(tmp);
}

void drawList(List* list){
    ListNode* tmp = list->head;
    printf("list = -> ");
    while (tmp){
        printf("(%s, %p) -> ",tmp->value, tmp->next);
        tmp = tmp -> next;
    }
    printf(" NULL\n");
}

///////////////////////////////////////////////////////// BST ////////////////////////////////////////////////////////////// 

bstNode* searchBST(bstTree* t, char* value) {
    bstNode* tmp = t->root;
    while(tmp && strcmp(value,tmp->value)) {
        if(compare(value, tmp->value))
            tmp = tmp -> left;
        else
            tmp = tmp -> right;
    }
    return tmp;
} 
 
void findBST(bstTree* t, char* value){
    if(searchBST(t,value))
        printf("1\n");
    else
        printf("0\n");
}


bstNode* minimumBST(bstNode* n) {
    if(!n)
        return NULL;
    else {
        while(n->left)
            n = n->left;
        return n;
    }
}

void minBST(bstTree* t){
    bstNode* tmp = minimumBST(t->root);
    if(tmp)
        printf("%s\n",tmp->value);
    else
        printf("\n");
}

bstNode* maximumBST(bstNode* n) {
    if(!n)
        return NULL;
    else {
        while(n->right)
            n = n->right;
        return n;
    }
}

void maxBST(bstTree* t){
    bstNode* tmp = maximumBST(t->root);
    if(tmp)
        printf("%s\n",tmp->value);
    else
        printf("\n");
}

bstNode* successorBST(bstNode* x) {
    if(x->right)
        return minimumBST(x->right);
    bstNode* y=x->p;
    while(y && x == y->right) {
        x = y;
        y = y->p;
    }
    return y;
}

void getSuccessorBST(bstTree* t, char* value){
    bstNode* tmp1 = searchBST(t, value);
    if(tmp1){
        bstNode* tmp2 = successorBST(tmp1);
        if(tmp2){
            printf("%s\n",tmp2->value);
            return;
        }
    }
    printf("\n");
}

void transplantBST(bstTree* t, bstNode* a, bstNode* b) {
    if(!(a->p))
        t->root = b;
    else if(a == a->p->left)
        a->p->left = b;
    else a->p->right = b;
    if(b)
        b->p = a->p;
}
 
void insertBST(bstTree* t, char* key){ 
    bstNode* new = malloc(sizeof(bstNode));
    new->value = checkAndCopy2(key);
    new->left = new->right = new->p = NULL;
    
    if(!(t->root)){
        t->root = new;
    } 
    else {
        bstNode* tmp = t->root;
        bstNode* prev;
        while(tmp){
            prev = tmp;
            if(compare(new->value,tmp->value))
                tmp = tmp -> left;
            else
                tmp = tmp -> right;
        }
        new->p = prev;
        if(compare(new->value, prev->value))
            prev -> left = new;
        else 
            prev -> right = new;
    }
    (t->size)++;
}
    
void deleteBST(bstTree* t, char* key){ 
    bstNode* found = searchBST(t, key);
    if(found) {
        if(!(found->left))
            transplantBST(t, found, found->right);
        else if(!(found->right))
            transplantBST(t, found, found->left);
        else {
            bstNode* minRight = minimumBST(found->right);
            if(minRight->p != found) {
                transplantBST(t, minRight, minRight->right);
                minRight->right = found->right;
                minRight->right->p = found;
            }
            transplantBST(t, found, minRight);
            minRight->left = found->left;
            minRight->left->p = minRight;
        }
        free(found);
        (t->size)--;
    }
}    

void loadBST(bstTree* t, char* filename){
    FILE *file;
    if((file = fopen(filename, "r")) == NULL){
        printf("file %s doesn't exist.\n",filename);
    }
    else {
        char word[101];
        while(fscanf(file,"%100s",word) != EOF){
            insertBST(t, word);
        }
    }
}

void inorderByNodeBST(bstNode* n){
    if(!n) 
        return;
    inorderByNodeBST(n->left);
    printf("%s ",n->value);
    inorderByNodeBST(n->right);
}
 
void inorderBST(bstTree* t){
    inorderByNodeBST(t->root);
    printf("\n");
}
 
void createBST(Tree* t){
    t->type = BST;
    t->data.bst = malloc(sizeof(bstTree));
    t->data.bst->root = NULL;
    t->data.bst->size=0;
}

void deleteTreeByNodeBST(bstNode* n){
    if(!n)
        return;
    deleteTreeByNodeBST(n->left);
    deleteTreeByNodeBST(n->right);
    free(n);
}


void deleteTreeBST(Tree* t){
    bstNode* n = t->data.bst->root;
    deleteTreeByNodeBST(n);
}  

void deleteTreeBST2(bstTree* t){
    bstNode* n = t->root;
    deleteTreeByNodeBST(n);
}   


///////////////////////////////////////////////////////// RBT //////////////////////////////////////////////////////////////

void createRBT(Tree* t){
    t->type = RBT;
    t->data.rbt = malloc(sizeof(rbtTree));
    t->data.rbt->size=0;   
    t->data.rbt->nil = malloc(sizeof(rbtNode));
    t->data.rbt->nil->color = BLACK;
    t->data.rbt->nil->p = t->data.rbt->nil->right = t->data.rbt->nil->left =  NULL;
    t->data.rbt->nil->value = NULL;
    t->data.rbt->root = t->data.rbt->nil;
}

void createRBT2(rbtTree* t){
    t->size=0;   
    t->nil = malloc(sizeof(rbtNode));
    t->nil->color = BLACK;
    t->nil->p = t->nil->right = t->nil->left =  NULL;
    t->nil->value = NULL;
    t->root = t->nil;   
}

rbtNode* searchRBT(rbtTree* t, char* value) {
    rbtNode* tmp = t->root;
    while(tmp != t->nil && strcmp(value,tmp->value)) {
        if(compare(value, tmp->value))
            tmp = tmp -> left;
        else
            tmp = tmp -> right;
    }
    return tmp;
} 
 
void findRBT(rbtTree* t, char* value){
    if(searchRBT(t,value) != t->nil)
        printf("1\n");
    else
        printf("0\n");
}


rbtNode* minimumRBT(rbtNode* nil, rbtNode* n) {
    if(n == nil)
        return nil;
    else {
        while(n->left != nil)
            n = n->left;
        return n;
    }
}

void minRBT(rbtTree* t){
    rbtNode* tmp = minimumRBT(t->nil, t->root);
    if(tmp != t-> nil)
        printf("%s\n",tmp->value);
    else
        printf("\n");
}

rbtNode* maximumRBT(rbtNode* nil, rbtNode* n) {
    if(n == nil)
        return nil;
    else {
        while(n->right != nil)
            n = n->right;
        return n;
    }
}

void maxRBT(rbtTree* t){
    rbtNode* tmp = maximumRBT(t->nil, t->root);
    if(tmp != t->nil)
        printf("%s\n",tmp->value);
    else
        printf("\n");
}

rbtNode* successorRBT(rbtNode* nil, rbtNode* x) {
    if(x->right != nil)
        return minimumRBT(nil, x->right);
    rbtNode* y=x->p;
    while(y != nil && x == y->right) {
        x = y;
        y = y->p;
    }
    return y;
}

void getSuccessorRBT(rbtTree* t, char* value){
    rbtNode* tmp1 = searchRBT(t, value);
    if(tmp1 != t-> nil){
        rbtNode* tmp2 = successorRBT(t->nil, tmp1);
        if(tmp2 != t->nil){
            printf("%s\n",tmp2->value);
            return;
        }
    }
    printf("\n");
}



void inorderByNodeRBT(rbtNode* nil, rbtNode* n){
    if(n == nil) return;
    inorderByNodeRBT(nil, n->left);
    printf("%s ",n->value);
    inorderByNodeRBT(nil, n->right);
}
 
void inorderRBT(rbtTree* t){
    inorderByNodeRBT(t->nil, t->root);
    printf("\n");
}

void reverseInorderToArrayByNodeRBT(rbtNode* nil,rbtNode* n, char** array){
    if(n == nil) return;
    reverseInorderToArrayByNodeRBT(nil, n->right,array);
    int i = 0;
    while(array[i++]);
    array[i-1] = checkAndCopy2(n->value);
    reverseInorderToArrayByNodeRBT(nil, n->left,array);
}

void reverseInorderToArrayRBT(rbtTree* t, char** array){
    reverseInorderToArrayByNodeRBT(t->nil,t->root, array);
}

void deleteTreeByNodeRBT(rbtNode* nil, rbtNode* n){
    if(n == nil)
        return;
    deleteTreeByNodeRBT(nil, n->left);
    deleteTreeByNodeRBT(nil, n->right);
    free(n);
}


void deleteTreeRBT(Tree* t){
    rbtNode* n = t->data.rbt->root;
    deleteTreeByNodeRBT(t->data.rbt->nil, n);
    free(t->data.rbt->nil);
}    

void deleteTreeRBT2(rbtTree *t){
    rbtNode* n = t->root;
    deleteTreeByNodeRBT(t->nil, n);
    free(t->nil);
}  

void leftRotateRBT(rbtTree* t, rbtNode* x) {
    rbtNode* y = x->right;
    x->right = y->left;
    if(y->left != t->nil)
        y->left->p = x;
    y->p = x->p;
    if(x->p == t->nil)
        t->root = y;
    else if(x == x->p->left)
        x->p->left = y;
    else 
        x->p->right = y;
    y->left = x;
    x->p = y;
}

void rightRotateRBT(rbtTree* t, rbtNode* x) {
    rbtNode* y = x->left;
    x->left = y->right;
    if(y->right != t->nil)
        y->right->p = x;
    y->p = x->p;
    if(x->p == t->nil)
        t->root = y;
    else if(x == x->p->left)
        x->p->left = y;
    else 
        x->p->right = y;
    y->right = x;
    x->p = y;
}


void insertRBT(rbtTree* t, char* key){ 
    rbtNode* new = malloc(sizeof(rbtNode));    
    new->value = checkAndCopy2(key);    
    new->color = RED;
    new->left = new->right = new->p = t->nil;
    if(t->root == t->nil)
        t->root = new;
    else {
        rbtNode* tmp = t->root;
        rbtNode* prev = t->nil;
        while(tmp != t->nil){
            prev = tmp;
            if(compare(new->value,tmp->value))
                tmp = tmp -> left;
            else
                tmp = tmp -> right;
        }
        new->p = prev;
        if(compare(new->value, prev->value))
            prev -> left = new;
        else 
            prev -> right = new;
    }
    insertFixRBT(t, new);
    (t->size)++;
}

void insertFixRBT(rbtTree* t, rbtNode* z) {
    while(z != t->root && z->p->color == RED) {
        if(z->p == z->p->p->left) {
            rbtNode* y = z->p->p->right;
            if(y->color == RED) {
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
            } else {
                if(z == z->p->right) {
                    z = z->p;
                    leftRotateRBT(t, z);
                }
                z->p->color = BLACK;
                z->p->p->color = RED;
                rightRotateRBT(t, z->p->p);
            }
        } else {
            rbtNode* y = z->p->p->left;
            if(y->color == RED) {
                z->p->color = BLACK;
                y->color = BLACK;
                z->p->p->color = RED;
                z = z->p->p;
            } else {
                if(z == z->p->left) {
                    z = z->p;
                    rightRotateRBT(t, z);
                }
                z->p->color = BLACK;
                z->p->p->color = RED;
                leftRotateRBT(t, z->p->p);
            }
        }
    }
    t->root->color = BLACK;
}

void transplantRBT(rbtTree* t, rbtNode* a, rbtNode* b){
    if(a->p == t->nil)
        t->root = b;
    else if(a == a->p->left)
        a->p->left = b;
    else 
        a->p->right = b;
    b->p = a->p;
}

int deleteRBT(rbtTree* t, char* key){ 
    rbtNode* found = searchRBT(t, key);
    if(found != t->nil) {
        rbtNode* changed;
        Color foundColor = found->color;
        if(found->left == t->nil){
            changed = found->right;
            transplantRBT(t, found, found->right);
        }
        else if(found->right == t->nil){
            changed = found->left;
            transplantRBT(t, found, found->left);
        }
        else {
            rbtNode* minRight = minimumRBT(t->nil, found->right);
            foundColor = minRight->color;
            changed = minRight->right;
            if(minRight->p != found) {
                transplantRBT(t, minRight, minRight->right);
                minRight->right = found->right;
                minRight->right->p = minRight;
            }
            else
                changed->p = minRight;
            transplantRBT(t, found, minRight);
            minRight->left = found->left;
            minRight->left->p = minRight;
            minRight->color = found->color;
        }
        if(foundColor == BLACK)
            deleteFixRBT(t, changed);
        free(found);
        (t->size)--;
        return 1;
    }
    return 0;
}    

void deleteFixRBT(rbtTree* t, rbtNode* x) {
    while(x != t->root && x->color == BLACK) {
        if(x == x->p->left) {
            rbtNode* w = x->p->right;
            if(w->color == RED) {
                w->color = BLACK;
                x->p->color = RED;
                leftRotateRBT(t, x->p);
                w = x->p->right;
            }
            if(w->left->color == BLACK && w->right->color == BLACK) {
                w->color = RED;
                x = x->p;
            } else {
                if(w->right->color == BLACK) {
                    w->left->color = BLACK;
                    w->color = RED;
                    rightRotateRBT(t, w);
                    w = x->p->right;
                }
                w->color = x->p->color;
                x->p->color = BLACK;
                w->right->color = BLACK;
                leftRotateRBT(t, x->p);
                x = t->root;
            }
        } else {
            rbtNode* w = x->p->left;
            if(w->color == RED) {
                w->color = BLACK;
                x->p->color = RED;
                rightRotateRBT(t, x->p);
                w = x->p->left;
            }
            if(w->right->color == BLACK && w->left->color == BLACK) {
                w->color = RED;
                x = x->p;
            } else {
                if(w->left->color == BLACK) {
                    w->right->color = BLACK;
                    w->color = RED;
                    leftRotateRBT(t, w);
                    w = x->p->left;
                }
                w->color = x->p->color;
                x->p->color = BLACK;
                w->left->color = BLACK;
                rightRotateRBT(t, x->p);
                x = t->root;
            }
        }
    }
    x->color = BLACK;
}

void loadRBT(rbtTree* t, char* filename){
    FILE *file;
    if((file = fopen(filename, "r")) == NULL){
        printf("file %s doesn't exist.\n",filename);
    }
    else {
        char word[101];
        while(fscanf(file,"%100s",word) != EOF){
            insertRBT(t, word);
        }
    }
}
/////////////////////////////////////////////////////////HMAP/////////////////////////////////////////////////////
void createHMAP(Tree* t, int modulo, int n){
    t->type = HMAP;
    t->data.hmap = malloc(sizeof(hashMap));
    t->data.hmap->modulo=modulo;   
    t->data.hmap->switchSize = n;
    t->data.hmap->data = malloc(sizeof(hashListElement*)*modulo);
    for(int i =0; i<modulo; i++){
        t->data.hmap->data[i] = malloc(sizeof(hashListElement));
        t->data.hmap->data[i]->size = 0;
        t->data.hmap->data[i]->structure.list = malloc(sizeof(List));
        createList(t->data.hmap->data[i]->structure.list); 
    }
}

int hash(char* key, int modulo){
    return jenkins_one_at_a_time_hash(key, strlen(key))%modulo;
}

void listToRBT(hashListElement* e){
    rbtTree* tree = malloc(sizeof(rbtTree));
    createRBT2(tree);
    char* tmpKey = popMiddleValueList(e->structure.list);
    while(tmpKey){
        insertRBT(tree, tmpKey);        
        free(tmpKey);
        tmpKey = popMiddleValueList(e->structure.list);
    }
    free(e->structure.list);
    e->structure.tree = tree;
}

void rbtToList(hashListElement* e){
    List* list = malloc(sizeof(List));
    createList(list);
    
    char** tmpKeys = malloc(sizeof(char*)*(e->size));
    for(int i=0; i< e->size; i++)
        tmpKeys[i]=NULL;
    
    reverseInorderToArrayRBT(e->structure.tree, tmpKeys);
    for(int i=0; i<e->size; i++)
        addValueList(list, tmpKeys[i]);   
    
    deleteTreeRBT2(e->structure.tree);
    free(e->structure.tree);
    e->structure.list = list;
}

void insertHMAP(hashMap* m, char* key2){
    char* key = checkAndCopy2(key2);
    int hashIndex = hash(key, m->modulo);
    if (m->data[hashIndex]->size < m->switchSize){
        addValueList(m->data[hashIndex]->structure.list, key);
        (m->data[hashIndex]->size)++;
        (m->size)++;
        if(m->data[hashIndex]->size == m->switchSize)
            listToRBT(m->data[hashIndex]);
    }
    else{
        insertRBT(m->data[hashIndex]->structure.tree, key);
        (m->data[hashIndex]->size)++;
        (m->size)++;
    }
    free(key);
}

void deleteHMAP(hashMap* m, char* key2){
    char* key = checkAndCopy2(key2);
    int hashIndex = hash(key, m->modulo);
    if (m->data[hashIndex]->size < m->switchSize){
        if(deleteValueList(m->data[hashIndex]->structure.list, key)){
            (m->data[hashIndex]->size)--;
            (m->size)--;
        }
    }
    else{
        if(deleteRBT(m->data[hashIndex]->structure.tree, key)){
            (m->data[hashIndex]->size)--;
            (m->size)--;
            if(m->data[hashIndex]->size < m->switchSize)
                rbtToList(m->data[hashIndex]);
        }
    }    
    free(key);
}

void findHMAP(hashMap* m, char* key2){
    char* key = checkAndCopy2(key2);
    int hashIndex = hash(key, m->modulo);
    if (m->data[hashIndex]->size < m->switchSize){
        findValueList(m->data[hashIndex]->structure.list, key);
    }
    else{
        findRBT(m->data[hashIndex]->structure.tree, key);
    }
}

void destroyHMAP(hashMap* m){
    for(int i=0; i< m->modulo; i++){
        if (m->data[i]->size < m->switchSize)
            destroyList(m->data[i]->structure.list);
        else
            deleteTreeRBT2(m->data[i]->structure.tree);
        free(m->data[i]);
    }
    free(m->data);    
}

void loadHMAP(hashMap* m, char* filename){
    FILE *file;
    if((file = fopen(filename, "r")) == NULL){
        printf("file %s doesn't exist.\n",filename);
    }
    else {
        char word[101];
        while(fscanf(file,"%100s",word) != EOF){
            insertHMAP(m, word);
        }
    }
}


///////////////////////////////////////////////////////// OTHER //////////////////////////////////////////////////////////////


int length(char* word){
    int i = 0;
    while(word[i++]);
    return i;
}

char* checkAndCopy(char* key){
    char* p=key;
    char* s=key;
    s+=(strlen(key)-1);
    
    while(strlen(p) > 0 && !( (p[0] >= 'A' && p[0] <= 'Z') || (p[0] >= 'a' && p[0] <= 'z'))) p++;
    while( s != p && !( (s[0] >= 'A' && s[0] <= 'Z') || (s[0] >= 'a' && s[0] <= 'z'))) s--;
    
    char* result = malloc(sizeof(char)*(s-p+2));
    strncpy(result,p,(s-p+1));
    result[s-p+2]='\0';
    return result;
}

char* checkAndCopy2(char* key){
    char* tmp = malloc(sizeof(char)*(strlen(key)+1));
    strcpy(tmp,key);
    
    char* p=tmp;
    while(strlen(p) > 0 && !( (p[0] >= 'A' && p[0] <= 'Z') || (p[0] >= 'a' && p[0] <= 'z'))) p++;
    int s = strlen(p)-1;
    while(strlen(p) > 0 && !( (p[s] >= 'A' && p[s] <= 'Z') || (p[s] >= 'a' && p[s] <= 'z'))){
        p[s] = '\0';
        s--;
    }
    
    char* result = malloc(sizeof(char)*(strlen(p)+1));
    strcpy(result,p);
    free(tmp);
    return result;
}
                         
bool compare(char* a, char* b){
    return (strcmp(a,b) <=0);
}
 

void chooseTree(char* argv[], Tree* tree){
    if(!strcmp((argv[1]),"--type")){
        if(!strcmp(argv[2],"bst")){
            createBST(tree);
        }
        else if(!strcmp(argv[2],"rbt")){
            createRBT(tree);
        }
        else if(!strcmp(argv[2],"hmap")){
            createHMAP(tree,MODULO,N);
        }
    }
}

void destroyTree(Tree* tree){
    switch(tree->type){
        case BST:
            deleteTreeBST2(tree->data.bst);
            free(tree->data.bst);
            break;
        case RBT:
            deleteTreeRBT2(tree->data.rbt);
            free(tree->data.rbt);
            break;
       case HMAP:
            destroyHMAP(tree->data.hmap);
            free(tree->data.hmap);
            break;
    }
}

void executeCommands(int n, Tree* tree){
    char command[20];
    char word[101];
    clock_t start, end;
    int inserts, loads, deletes, finds, mins, maxes, successors, inorders, maxsize;
    inserts = loads = deletes = finds = mins = maxes = successors = inorders = maxsize = 0;
    start = clock();
    for(int i=0; i<n; i++){
        scanf("%19s",command);
        if(!strcmp(command,"insert")){
            scanf("%100s",word);
            switch(tree->type){
                case BST:
                    insertBST(tree->data.bst,word);
                    if(tree->data.bst->size > maxsize)
                        maxsize = tree->data.bst->size;
                    break;
                case RBT:
                    insertRBT(tree->data.rbt,word);
                    if(tree->data.rbt->size > maxsize)
                        maxsize = tree->data.rbt->size;
                    break;
               case HMAP:
                    insertHMAP(tree->data.hmap,word);
                    if(tree->data.hmap->size > maxsize)
                        maxsize = tree->data.hmap->size;
                    break;
            }
            inserts++;
        }
        else if(!strcmp(command,"load")){
            scanf("%100s",word);
            switch(tree->type){
                case BST:
                    loadBST(tree->data.bst,word);
                    if(tree->data.bst->size > maxsize)
                        maxsize = tree->data.bst->size;
                    break;
                case RBT:
                    loadRBT(tree->data.rbt,word);
                    if(tree->data.rbt->size > maxsize)
                        maxsize = tree->data.rbt->size;
                    break;
                case HMAP:
                    loadHMAP(tree->data.hmap,word);
                    if(tree->data.hmap->size > maxsize)
                        maxsize = tree->data.hmap->size;
                    break;
            }
            loads++;
        }
        else if(!strcmp(command,"delete")){
            scanf("%100s",word);
            switch(tree->type){
                case BST:
                    deleteBST(tree->data.bst,word);
                    if(tree->data.bst->size > maxsize)
                        maxsize = tree->data.bst->size;
                    break;
                case RBT:
                    deleteRBT(tree->data.rbt,word);
                    if(tree->data.rbt->size > maxsize)
                        maxsize = tree->data.rbt->size;
                    break;
                case HMAP:
                    deleteHMAP(tree->data.hmap,word);
                    if(tree->data.hmap->size > maxsize)
                        maxsize = tree->data.hmap->size;
                    break;
            }
            deletes++;
        }
        else if(!strcmp(command,"find")){
            scanf("%100s",word);
            switch(tree->type){
                case BST:
                    findBST(tree->data.bst,word);
                    break;
                case RBT:
                    findRBT(tree->data.rbt,word);
                    break;
                case HMAP:
                    findHMAP(tree->data.hmap,word);
                    break;
            }
            finds++;
        }
        else if(!strcmp(command,"min")){
            switch(tree->type){
                case BST:
                    minBST(tree->data.bst);
                    break;
                case RBT:
                    minRBT(tree->data.rbt);
                    break;
                case HMAP:
                    printf("\n");
                    break;
            }
            mins++;
        }
        else if(!strcmp(command,"max")){
            switch(tree->type){
                case BST:
                    maxBST(tree->data.bst);
                    break;
                case RBT:
                    maxRBT(tree->data.rbt);
                    break;
                case HMAP:
                    printf("\n");
                    break;
            }
            maxes++;
        }
        else if(!strcmp(command,"successor")){
            scanf("%100s",word);
            switch(tree->type){
                case BST:
                    getSuccessorBST(tree->data.bst,word);
                    break;
                case RBT:
                    getSuccessorRBT(tree->data.rbt,word);
                    break;
                case HMAP:
                    printf("\n");
                    break;
            }
            successors++;
        }
        else if(!strcmp(command,"inorder")){
            switch(tree->type){
                case BST:
                    inorderBST(tree->data.bst);
                    break;
                case RBT:
                    inorderRBT(tree->data.rbt);
                    break;
                case HMAP:
                    printf("\n");
                    break;
            }
            inorders++;
        }
    }
    end = clock();
    double miliseconds = (double) ((end-start) / 1000.0);
    int size=0;
    switch(tree->type){
        case BST:
            size = tree->data.bst->size;
            break;
        case RBT:
            size = tree->data.rbt->size;
            break;
        case HMAP:
            size = tree->data.hmap->size;
            break;
    }
    
    
    fprintf(stderr, "Time: %f ms\n",miliseconds);
    fprintf(stderr, "Insert: %d\n",inserts);
    fprintf(stderr, "Load: %d\n",loads);
    fprintf(stderr, "Delete: %d\n",deletes);
    fprintf(stderr, "Find: %d\n",finds);
    fprintf(stderr, "Min: %d\n",mins);
    fprintf(stderr, "Max: %d\n",maxes);
    fprintf(stderr, "Successor: %d\n",successors);
    fprintf(stderr, "Inorder: %d\n",inorders);
    fprintf(stderr, "Max size: %d\n",maxsize);
    fprintf(stderr, "Size at end: %d\n",size);
}

int main (int argc, char* argv[]){
    if(argc != 3) return 1;    

    Tree* tree = malloc(sizeof(Tree));
    chooseTree(argv, tree);    
    
    int n;
    scanf("%d",&n);
    executeCommands(n, tree); 
    
    destroyTree(tree);
    free(tree);
    
    return 0;
}