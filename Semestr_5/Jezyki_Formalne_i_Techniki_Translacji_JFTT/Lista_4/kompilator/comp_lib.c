/*
    Nazwa: comp_lib.c
    Opis: Obfita objętościowo biblioteka z implementacjami wszystkich funkcji programu
    Autor: Jan Brzeżański
    Indeks: 236814
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "comp_lib.h"

#include "imath.h"

#define NONE -1

//////////////////////////////////////////////////// ZMIENNE GLOBALNE /////////////////////////////////////////////

//tablica symboli
struct SymbolTable* table;      //tablica
mp_int memory;                  //zmienna globalna, ktora przyporzadkowuje pamiec kolejno deklarowanym zmiennym

//drzewo wyprowadzenia
struct Command* program;        //drzewo
int deep=0;                     //glebokosc obecnej komendy (robi wciecia przy wypisywaniu xD)
int realSize;                   //zapamietane do zwalniania pamieci przed ucieciem drzewa

//kod posredni
struct CodeProgram* codeProgram;    //tablica komend
int label=1;                        //kolejne tymczasowe labele komend kodu. (0 = HALT)

//rejestry
struct Register registers[8];       //tablica rejestrow 

//kod asemblera
struct AssemblerProgram* assembler; //tablica kodu asemblera


//////////////////////////////////////////////////// TABLICA SYMBOLI /////////////////////////////////////////////////


void init_table(){
    table=malloc(sizeof(struct SymbolTable));
    table->size=0;
    table->maxSize=32;
    table->symbols=malloc(sizeof(struct Symbol)*32);
    
    memory=mp_int_alloc();
    mp_int_init_value(memory, 1);
}

void free_table(){
    for(int i=0; i<table->size; i++){
        free(table->symbols[i].name);
        
        if(table->symbols[i].off)  mp_int_free(table->symbols[i].off);
        if(table->symbols[i].mem)  mp_int_free(table->symbols[i].mem);
        if(table->symbols[i].size) mp_int_free(table->symbols[i].size);
        if(table->symbols[i].val)  mp_int_free(table->symbols[i].val);
        if(table->symbols[i].ret)  mp_int_free(table->symbols[i].ret);
    }
    
    free(table->symbols);
    free(table);
    mp_int_free(memory);
}

int check_symbol(char* name){
    for(int i=0; i<table->size; i++)
        if(!strcmp(table->symbols[i].name, name))
            return i;

    return -1;
}

int check_declared(char* name){
    for(int i=0; i<table->size; i++)
        if(!strcmp(table->symbols[i].name, name) && (table->symbols[i].type==VARIABLE || table->symbols[i].type==ARRAY))
            return i;
    
    return -1;
}

int check_variable(char* name){
    for(int i=table->size-1; i>=0; i--)
        if(!strcmp(table->symbols[i].name, name)) 
            return i;
    
    return -1;
}

int set_variable(int pos){
    if (pos>=table->size)
        return -1;
    
    table->symbols[pos].type=VARIABLE;
       
    
    table->symbols[pos].val=mp_int_alloc();
    mp_int_init_value(table->symbols[pos].val, -1);
    
    return 0;
}

int set_array(int pos, int start, int end){
    if (pos>=table->size)
        return -1;
    
    if(mp_int_compare(get_number(start), get_number(end)) > 0)
        return -1;
    
    table->symbols[pos].type=ARRAY;
   
    table->symbols[pos].off=mp_int_alloc();
    mp_int_init_copy(table->symbols[pos].off, get_number(start));
    
    table->symbols[pos].size=mp_int_alloc();
    mp_int_init_copy(table->symbols[pos].size, get_number(end));
    mp_int_sub(table->symbols[pos].size, get_number(start), table->symbols[pos].size);
    mp_int_add_value(table->symbols[pos].size, 1, table->symbols[pos].size);

    
    table->symbols[pos].ret=mp_int_alloc();
    mp_int_init(table->symbols[pos].ret);
    
    return 0;
}

int resize_table(){
    void* tmp= realloc(table->symbols, sizeof(struct Symbol)*table->maxSize*2);
    if(!tmp){
        fprintf(stderr, "(!) Reallocation failed at resize_table() (!)\n");
        return 1;
    }
    table->maxSize*=2;
    table->symbols=tmp;
    
    return 0;
}

int is_constant(int pos){
    if(table->symbols[pos].type==NUMBER)
        return 1;
    
    return 0;
}

int is_variable(int pos){
    if(table->symbols[pos].type==VARIABLE || table->symbols[pos].type==ITERATOR)
        return 1;
    
    return 0;
}

int is_array(int pos){
    if(table->symbols[pos].type==ARRAY)
        return 1;
    
    return 0;
}

mp_int get_memory_of_variable(int pos){
    return table->symbols[pos].mem;   
}

mp_int get_offset_of_array(int pos){
    return table->symbols[pos].off;
}

mp_int get_start_of_array(int pos){
    mp_int_sub(table->symbols[pos].mem, table->symbols[pos].off, table->symbols[pos].ret);
    
    return table->symbols[pos].ret;
}

mp_int get_memory_of_array(int pos, mp_int index){
    mp_int_add(table->symbols[pos].mem, index, table->symbols[pos].ret);
    mp_int_sub(table->symbols[pos].ret, table->symbols[pos].off, table->symbols[pos].ret);
    
    return table->symbols[pos].ret;
}

mp_int get_number(int pos){
    return table->symbols[pos].val;   
}

int add_number(char* number){
    for(int i=0; i<table->size; i++){
        if(table->symbols[i].type==NUMBER){ 
            int len = mp_int_string_len(table->symbols[i].val, 10);
            char* buf = calloc(len, sizeof(*buf));
            
            mp_int_to_string(table->symbols[i].val, 10, buf, len);
            
            if(!strcmp(number, buf)){
                free(buf);
                return i;
            }
            free(buf);
        }
    }
    
    if(table->size == table->maxSize)
        resize_table();
    
    table->symbols[table->size].type=NUMBER;
    
    table->symbols[table->size].name=strdup("\0");
 
    table->symbols[table->size].val=mp_int_alloc();
    mp_int_init(table->symbols[table->size].val);
    mp_int_read_string(table->symbols[table->size].val, 10, number);
    
    table->symbols[table->size].mem=NULL;
    table->symbols[table->size].off=NULL;
    table->symbols[table->size].size=NULL;
    table->symbols[table->size].ret=NULL;
    
    return table->size++;
}

int add_number2(mp_int num){
    int len = mp_int_string_len(num, 10);
    char* val = calloc(len, sizeof(*val));
    mp_int_to_string(num, 10, val, len);
    
    int ret=add_number(val);
    
    free(val);
    
    return ret;
}

int add_symbol(char* name){
    if(table->size == table->maxSize)
        resize_table();

    table->symbols[table->size].type=UNKNOWN;
    
    table->symbols[table->size].name=strdup(name);
    
    table->symbols[table->size].val=NULL;
    table->symbols[table->size].mem=NULL;
    table->symbols[table->size].off=NULL;
    table->symbols[table->size].size=NULL;
    table->symbols[table->size].ret=NULL;
    
    return table->size++;
} 

int add_iterator(char* name){
    if(table->size == table->maxSize)
        resize_table();

    table->symbols[table->size].type=ITERATOR;

    table->symbols[table->size].name=strdup(name);
    
    table->symbols[table->size].val=mp_int_alloc();
    mp_int_init_value(table->symbols[table->size].val, -1);
    
    table->symbols[table->size].mem=NULL;
    table->symbols[table->size].off=NULL;
    table->symbols[table->size].size=NULL;
    table->symbols[table->size].ret=NULL;
       
    int ret = table->size++;
    
    char* range=malloc(sizeof(char)*(strlen(name)+6));
    sprintf(range, "%srange",name);
    
    if(table->size == table->maxSize)
        resize_table();
    
    table->symbols[table->size].type=ITERATOR;

    table->symbols[table->size].name=range;
    
    table->symbols[table->size].val=mp_int_alloc();
    mp_int_init_value(table->symbols[table->size].val, -1);

    table->symbols[table->size].mem=NULL;
    table->symbols[table->size].off=NULL;
    table->symbols[table->size].size=NULL;
    table->symbols[table->size].ret=NULL;
       
    table->size++;
    
    return ret;                    
}

void set_memory_variable(int pos){
    table->symbols[pos].mem=mp_int_alloc();
    mp_int_init_copy(table->symbols[pos].mem, memory);
    mp_int_add_value(memory, 1, memory);   
}

void set_memory_array(int pos){
    table->symbols[pos].mem=mp_int_alloc();
    mp_int_init_copy(table->symbols[pos].mem, memory);
    mp_int_add(memory, table->symbols[pos].size, memory);
}

void set_memories(){
    for(int i=0; i<table->size; i++)
        if(table->symbols[i].type == ITERATOR)
            set_memory_variable(i);
    
    for(int i=0; i<table->size; i++)
        if(table->symbols[i].type == VARIABLE)
            set_memory_variable(i);
    
    for(int i=0; i<table->size; i++)
        if(table->symbols[i].type == ARRAY)
            set_memory_array(i);
}



void print_symbols(){
    printf("\nTABLICA SYMBOLI:\n");
    for(int i=0; i<table->size; i++){     
        char* mem;
        if(table->symbols[i].mem){
            int len = mp_int_string_len(table->symbols[i].mem, 10);
            mem = calloc(len, sizeof(*mem));
            mp_int_to_string(table->symbols[i].mem, 10, mem, len);
        } else
            mem=strdup("?");
        
        char* size;
        if(table->symbols[i].size){
            int len = mp_int_string_len(table->symbols[i].size, 10);
            size = calloc(len, sizeof(*size));
            mp_int_to_string(table->symbols[i].size, 10, size, len);
        } else
            size=strdup("?");
        
        char* val;
        if(table->symbols[i].val){
            int len = mp_int_string_len(table->symbols[i].val, 10);
            val = calloc(len, sizeof(*val));
            mp_int_to_string(table->symbols[i].val, 10, val, len);
        } else
            val=strdup("?");
        
        printf("Symbol index=%d, type=%d, memory=%s, size=%s, name=%s, val=%s\n", i, table->symbols[i].type, mem, size, table->symbols[i].name, val);
        
        free(mem);
        free(size);
        free(val);
    }
}


/////////////////////////////////////////////////////////////// DRZEWO WYPROWADZENIA ///////////////////////////////////////////////////////////////

const char commandNames[][9] = {
    "PROGRAM",        //0
    "COMMANDS",       //1
     
    "IS",             //2  
    
    "NUM",            //3
    "ARR",            //4
    "PID",            //5
    
    "ADD",            //6
    "SUB",            //7
    "MUL",            //8
    "DIV",            //9
    "MOD",            //10
    
    "IF",             //11
    "IFELSE",         //12
    
    "EQ",             //13
    "NEQ",            //14
    "LT",             //15
    "GT",             //16
    "LEQ",            //17
    "GEQ",            //18
    
    "WHILE",          //19
    "DO",             //20
    
    "FOR",            //21
    "FORDOWN",        //22
    
    "READ",           //23
    "WRITE"           //24
    
};


void create_program(struct Command* commands){
    program=malloc(sizeof(struct Command));
    program->type=COM_PROGRAM;
    program->index=-1;
    program->size=1;
    program->maxSize=1;
    program->commands=malloc(sizeof(struct Command*));
    program->commands[0]=commands;
}

struct Command* create_value_command(enum CommandType type, int index){
    struct Command* c=malloc(sizeof(struct Command));
    c->type=type;
    c->index=index;
    c->size=0;
    c->maxSize=0;
    c->commands=NULL;
    
    return c;
}

struct Command* create_parent_command(enum CommandType type, int noumberOfChilds, ...){  
    va_list ap;
    va_start(ap, noumberOfChilds); 
    
    struct Command* c=malloc(sizeof(struct Command));
    c->type=type;
    c->index=-1;
    c->size=noumberOfChilds;
    c->maxSize=noumberOfChilds;
    c->commands=malloc(sizeof(struct Command*) * noumberOfChilds);
 
    for(int i=0; i<noumberOfChilds; i++){
        c->commands[i]=va_arg(ap, struct Command*);
    }
    
    va_end(ap);
    
    return c;
}

struct Command* create_empty_command(enum CommandType type){    
    struct Command* c=malloc(sizeof(struct Command));
    c->type=type;
    c->index=-1;
    c->size=0;
    c->maxSize=8;
    c->commands=malloc(sizeof(struct Command*)*8);

    return c;
}

int resize_command(struct Command* c){
    void* tmp=realloc(c->commands, sizeof(struct Command*)*c->maxSize*2);
    if(!tmp){
        fprintf(stderr, "(!) Reallocation failed at resize_command() (!)\n");
        return 1;
    }
    c->maxSize*=2;
    c->commands=tmp;
    
    return 0;
}

struct Command* add_command(struct Command* parent, struct Command* child){
    if(parent->size == parent->maxSize)
        if (resize_command(parent))
            return parent;
    
    parent->commands[parent->size]=child; 
    parent->size++;
    
    return parent;
}

void free_tree_r(struct Command* c){
    if(!c) return;

    for(int i=0; i<c->size; i++){  
        free_tree_r(c->commands[i]);
    }
    
    free(c->commands);
    free(c);
}


void free_tree(){   
    program->commands[0]->size=realSize;
    free_tree_r(program);
}

int find_write(struct Command* c){
    int ret;
    switch(c->type){
    case COM_WRITE:
        return 1;
        
    case COM_IF:
    case COM_WHILE:
        return find_write(c->commands[1]);
            
    case COM_IFELSE:
        ret=0;
        ret+=find_write(c->commands[1]);
        ret+=find_write(c->commands[2]);
        return ret;
                      
    case COM_DO:
        return find_write(c->commands[0]);
            
    case COM_FOR:
    case COM_FORDOWN:
        return find_write(c->commands[4]);
        
    case COM_COMMANDS:
        ret=0;
        for(int i=0; i<c->size; i++)
            ret+=find_write(c->commands[i]);
        return ret;
            
    default:
        return 0;
    }
}


void cut_tree(){
    int newSize=0;
    realSize=program->commands[0]->size;
    for(int i=0; i<program->commands[0]->size; i++)
        if(find_write(program->commands[0]->commands[i]))
           newSize=i+1;
           
    program->commands[0]->size=newSize;
}

void print_r(struct Command* c){
    if(!c) return;
    
    deep++;
    for(int i=0; i<deep; i++)
        printf("   ");
    
    printf("%s (size=%d, index=%d, ptr=%p)\n", commandNames[c->type], c->size, c->index, (void *)c);

    for(int i=0; i<c->size; i++){  
        print_r(c->commands[i]);
    }
    
    deep--;
}

void print_tree(){
    printf("\nDRZEWO WYPROWADZENIA:\n");
    print_r(program);   
}

/////////////////////////////////////////////////////////////// KOD POSREDNI ///////////////////////////////////////////////////////////////

const char codeCommandNames[][8] = {
    "UNKNOWN",   //0
    "DELETED",   //1
     
    "LABEL",     //2
    
    "READ",      //3
    "WRITE",     //4
    
    "HALF",      //5
    "INC",       //6
    "DEC",       //7
    
    
    "COPY",      //8
    
    
    "ADD",       //9
    "SUB",       //10
    
    "MUL",       //11
    "DIV",       //12
    "MOD",       //13
     
    "JUMP",      //14
    "JNEQ",      //15
    "JEQ",       //16
    "JGEQ",      //17
    "JLEQ",      //18
    "JGT",       //19   
    "JLT",       //20
    "JZERO"      //21
};


void init_code(){
    codeProgram=malloc(sizeof(struct CodeProgram));
    codeProgram->size=0;
    codeProgram->maxSize=128;
    codeProgram->commands=malloc(sizeof(struct CodeCommand)*128);
}

void free_code(){
    free(codeProgram->commands);
    free(codeProgram);
}

int resize_code(){
    void* tmp=realloc(codeProgram->commands, sizeof(struct CodeCommand)*codeProgram->maxSize*2);
    if(!tmp){
        fprintf(stderr, "(!) Reallocation failed at resize_code() (!)\n");
        return 1;
    }
    codeProgram->maxSize*=2;
    codeProgram->commands=tmp;
    
    return 0;
}

void add_code_command(enum CodeType type){
    if(codeProgram->size == codeProgram->maxSize)
        resize_code();
    
    codeProgram->commands[codeProgram->size].type=type;
    codeProgram->commands[codeProgram->size].size=0;
    codeProgram->commands[codeProgram->size].block=-1;
    
    codeProgram->size++;
}

void add_label(int label){
    if(codeProgram->size == codeProgram->maxSize)
        resize_code();
    
    codeProgram->commands[codeProgram->size].type=CODE_LABEL;
    codeProgram->commands[codeProgram->size].size=1;
    codeProgram->commands[codeProgram->size].args[0]=label;
    codeProgram->commands[codeProgram->size].block=-1;
    
    codeProgram->size++;
}

void set_type_of_current_command(enum CodeType type){
    codeProgram->commands[codeProgram->size-1].type=type;
}

void add_arg_to_current_command(int arg){
    codeProgram->commands[codeProgram->size-1].args[codeProgram->commands[codeProgram->size-1].size]=arg;
    codeProgram->commands[codeProgram->size-1].size++;
}

void change_jump_destination_at_current_command(int arg){
    codeProgram->commands[codeProgram->size-1].args[0]=arg;   
}

int bad_inequality(){
    switch(codeProgram->commands[codeProgram->size-1].type){
    case CODE_JLT:
    case CODE_JGT:
        return 1;
    
    default:
        return 0;
    }
}

void switch_condition_at_current_command(){
    int type;
    switch(codeProgram->commands[codeProgram->size-1].type){
    case CODE_JNEQ:    
        type=CODE_JEQ;
        break;
    case CODE_JEQ:    
        type=CODE_JNEQ;
        break;
    case CODE_JGEQ:
        type=CODE_JLT;
        break;
    case CODE_JLEQ:  
        type=CODE_JGT;
        break;
    case CODE_JGT: 
        type=CODE_JLEQ;
        break;
    case CODE_JLT:   
        type=CODE_JGEQ;
        break;
        
    default: 
        break;         
    }
    
    codeProgram->commands[codeProgram->size-1].type=type;
}


void transform_tree_r(struct Command* c){
    if(!c) return;
    
    int labelExit, labelElse, labelStart, labelTmp;
    switch(c->type){
    case COM_IS:
        add_code_command(CODE_COPY);
        transform_tree_r(c->commands[0]);
        transform_tree_r(c->commands[1]);
        break;
    
    case COM_NUM:
        add_arg_to_current_command(c->index);
        add_arg_to_current_command(-1);
        break;
    
    case COM_PID:
        add_arg_to_current_command(c->index);
        add_arg_to_current_command(-1);
        break;
            
    case COM_ARR:
        add_arg_to_current_command(c->commands[0]->index);
        add_arg_to_current_command(c->commands[1]->index);
        break;
    
    case COM_ADD:
        set_type_of_current_command(CODE_ADD);
        transform_tree_r(c->commands[0]);
        transform_tree_r(c->commands[1]);
        break;
            
    case COM_SUB:
        set_type_of_current_command(CODE_SUB);
        transform_tree_r(c->commands[0]);
        transform_tree_r(c->commands[1]);
        break;
     
    case COM_MUL:
        set_type_of_current_command(CODE_MUL);
        transform_tree_r(c->commands[0]);
        transform_tree_r(c->commands[1]);
        break;
     
    case COM_DIV:
        set_type_of_current_command(CODE_DIV);
        transform_tree_r(c->commands[0]);
        transform_tree_r(c->commands[1]);
        break;
             
    case COM_MOD:
        set_type_of_current_command(CODE_MOD);
        transform_tree_r(c->commands[0]);
        transform_tree_r(c->commands[1]);
        break;
     
    case COM_IF:
        labelExit=label++;
        labelTmp=labelExit;
            
        add_code_command(CODE_UNKNOWN);
        add_arg_to_current_command(labelTmp);
        add_arg_to_current_command(-1);  
        transform_tree_r(c->commands[0]);   //condition
            
        if(bad_inequality()){
            switch_condition_at_current_command();
            labelExit=label++;
            add_code_command(CODE_JUMP);
            add_arg_to_current_command(labelExit);
            add_label(labelTmp);
        }
            
        transform_tree_r(c->commands[1]);   //if true
        add_label(labelExit);
        break;
     
    case COM_IFELSE:
        labelElse=label++;
        labelExit=label++;
        labelTmp=labelElse;
            
        add_code_command(CODE_UNKNOWN);
        add_arg_to_current_command(labelTmp);
        add_arg_to_current_command(-1);         
        transform_tree_r(c->commands[0]);   //condition
            
        if(bad_inequality()){
            switch_condition_at_current_command();
            labelElse=label++;
            add_code_command(CODE_JUMP);
            add_arg_to_current_command(labelElse);
            add_label(labelTmp);
        }    
            
        transform_tree_r(c->commands[1]);   //if true
        add_code_command(CODE_JUMP);
        add_arg_to_current_command(labelExit);
            
        add_label(labelElse);
        
        transform_tree_r(c->commands[2]);   //if false
        add_label(labelExit);  
        break;    
       
    case COM_WHILE:
        labelStart=label++;
        labelExit=label++;
        labelTmp=labelExit;
            
        add_label(labelStart);
            
        add_code_command(CODE_UNKNOWN);
        add_arg_to_current_command(labelTmp);
        add_arg_to_current_command(-1);
        transform_tree_r(c->commands[0]);   //condition
           
        if(bad_inequality()){
            switch_condition_at_current_command();
            labelExit=label++;
            add_code_command(CODE_JUMP);
            add_arg_to_current_command(labelExit);
            add_label(labelTmp);
        }    
            
        transform_tree_r(c->commands[1]);   //while true
        add_code_command(CODE_JUMP);
        add_arg_to_current_command(labelStart);
        add_label(labelExit);
        break;
        
    case COM_DO:
        labelStart=label++;
        
        add_label(labelStart);
        
        transform_tree_r(c->commands[0]);   //commands while true
        
        add_code_command(CODE_UNKNOWN);
        add_arg_to_current_command(labelStart);
        add_arg_to_current_command(-1);        
        transform_tree_r(c->commands[1]);   //condition      
            
        switch_condition_at_current_command();  //chcemy jump on true :/
        
        if(bad_inequality()){
            switch_condition_at_current_command();
            labelExit=label++;
            change_jump_destination_at_current_command(labelExit);
            add_code_command(CODE_JUMP);
            add_arg_to_current_command(labelStart);
            add_label(labelExit);
        }             
        break;
        
    case COM_FOR:
        labelStart=label++;
        labelExit=label++;
        add_code_command(CODE_COPY);        //przypisanie wartosci do iteratora
        transform_tree_r(c->commands[0]);   //[FOR] PID [FROM] ...
        transform_tree_r(c->commands[2]);   //[FROM] VALUE [TO] ...
        
        add_code_command(CODE_COPY);        //liczba iteracji = TO+1-FROM  
        transform_tree_r(c->commands[1]);
        transform_tree_r(c->commands[3]);
            
        add_code_command(CODE_INC);        //(+1)   
        transform_tree_r(c->commands[1]);
        
        add_code_command(CODE_SUB);        //(-FROM) 
        transform_tree_r(c->commands[1]);
        transform_tree_r(c->commands[1]);
        transform_tree_r(c->commands[2]);
            
        add_label(labelStart);    
        
        add_code_command(CODE_JZERO);         //iterator wiekszy niz warunek  
        add_arg_to_current_command(labelExit);
        add_arg_to_current_command(-1);       
        transform_tree_r(c->commands[1]);   //gdy range osiagnie zero, wychodzimy
            
        transform_tree_r(c->commands[4]);   //commands 
            
        add_code_command(CODE_INC);
        transform_tree_r(c->commands[0]);   //i jeszcze raz uzyskujemy iterator (do inkrementacji ["i++"])
        
        add_code_command(CODE_DEC);         //zmniejszamy liczbe pozostalych iteracji   
        transform_tree_r(c->commands[1]);
            
        add_code_command(CODE_JUMP);        //powrot na sprawdzenie warunku
        add_arg_to_current_command(labelStart);
            
        add_label(labelExit);
        break;
            
    case COM_FORDOWN:                     
        labelStart=label++;
        labelExit=label++;
        add_code_command(CODE_COPY);        //przypisanie wartosci do iteratora
        transform_tree_r(c->commands[0]);   //[FOR] PID [FROM] ...
        transform_tree_r(c->commands[2]);   //[FROM] VALUE [TO] ...
        
        add_code_command(CODE_COPY);        //liczba iteracji = FROM+1-TO  
        transform_tree_r(c->commands[1]);
        transform_tree_r(c->commands[2]);
            
        add_code_command(CODE_INC);        //(+1)   
        transform_tree_r(c->commands[1]);
        
        add_code_command(CODE_SUB);        //(-TO) 
        transform_tree_r(c->commands[1]);
        transform_tree_r(c->commands[1]);
        transform_tree_r(c->commands[3]);
            
        add_label(labelStart);    
        
        add_code_command(CODE_JZERO);         //iterator wiekszy niz warunek  
        add_arg_to_current_command(labelExit);
        add_arg_to_current_command(-1);       
        transform_tree_r(c->commands[1]);   //gdy range osiagnie zero, wychodzimy
            
        transform_tree_r(c->commands[4]);   //commands 
            
        add_code_command(CODE_DEC);
        transform_tree_r(c->commands[0]);   //i jeszcze raz uzyskujemy iterator (do inkrementacji ["i++"])
        
        add_code_command(CODE_DEC);         //zmniejszamy liczbe pozostalych iteracji   
        transform_tree_r(c->commands[1]);
            
        add_code_command(CODE_JUMP);        //powrot na sprawdzenie warunku
        add_arg_to_current_command(labelStart);
            
        add_label(labelExit);
        break;
            
    case COM_EQ:
    case COM_NEQ:
    case COM_LT:
    case COM_GT:
    case COM_LEQ:
    case COM_GEQ:
        set_type_of_current_command(c->type-COM_EQ+CODE_JNEQ);
        transform_tree_r(c->commands[0]);
        transform_tree_r(c->commands[1]);
        break;
        
    case COM_READ:
        add_code_command(CODE_READ);
        transform_tree_r(c->commands[0]);
        break;
    
    case COM_WRITE:
        add_code_command(CODE_WRITE);
        transform_tree_r(c->commands[0]);
        break;
            
    case COM_PROGRAM:
    case COM_COMMANDS:
        for(int i=0; i<c->size; i++){  
            transform_tree_r(c->commands[i]);
        }
        break; 
    }  
}

void transform_tree_to_code(){   
    transform_tree_r(program);
}

void enumerate_blocks(){
    int block=0;
    for(int i=0; i<codeProgram->size; i++){
        if(codeProgram->commands[i].type == CODE_LABEL || (codeProgram->commands[i].type >= CODE_JUMP && codeProgram->commands[i].type <= CODE_JZERO)){
            block++;
        }
        codeProgram->commands[i].block=block;
    }
}

void optimize_code(){
    for(int i=0; i<codeProgram->size; i++){
        struct CodeCommand* c = &codeProgram->commands[i];
        switch(c->type){      
        case CODE_ADD:  //dodawanie dwoch stalych, mala stala+zmienna (inc)
            if(is_constant(c->args[2]) && is_constant(c->args[4])){
                mp_int tmp=mp_int_alloc();
                mp_int_init(tmp);
                mp_int_add(get_number(c->args[2]), get_number(c->args[4]), tmp);

                for(int i=3; i<6; i++) c->args[i]=-1;              
                c->args[2]=add_number2(tmp);

                mp_int_free(tmp);
                c->type=CODE_COPY;
                c->size=4;
            } else if (is_constant(c->args[4])){
                if(mp_int_compare_value(get_number(c->args[4]), 0) == 0){
                    c->type=CODE_COPY;
                    c->size=4;
                }
            } else if (is_constant(c->args[2])){
                if(mp_int_compare_value(get_number(c->args[2]), 0) == 0){
                    c->type=CODE_COPY;
                    c->args[2]=c->args[4];
                    c->args[3]=c->args[5];
                    c->size=4;
                }
            } else if (is_constant(c->args[4]) && c->args[0] == c->args[2] && c->args[1] == c->args[3]){
                if(mp_int_compare_value(get_number(c->args[4]), 1) == 0){
                    c->type=CODE_INC;
                    c->size=2;
                }
            } else if (is_constant(c->args[2]) && c->args[0] == c->args[4] && c->args[1] == c->args[5]){
                if(mp_int_compare_value(get_number(c->args[2]), 1) == 0){
                    c->type=CODE_INC;
                    c->args[2]=c->args[4];
                    c->args[3]=c->args[5];
                    c->size=2;
                }
            }
            break;
                
        case CODE_SUB:  //to samo, co wyzej, dla odejmowania
            if(is_constant(c->args[2]) && is_constant(c->args[4])){
                mp_int tmp=mp_int_alloc();
                mp_int_init(tmp);
                mp_int_sub(get_number(c->args[2]), get_number(c->args[4]), tmp);
                if(mp_int_compare_zero(tmp) < 0) mp_int_zero(tmp);

                for(int i=3; i<6; i++) c->args[i]=-1;              
                c->args[2]=add_number2(tmp);

                mp_int_free(tmp);
                c->type=CODE_COPY;
                c->size=4;
            } else if (is_constant(c->args[4])){
                if(mp_int_compare_value(get_number(c->args[4]), 0) == 0){
                    c->type=CODE_COPY;
                    c->size=4;
                }
            } else if (is_constant(c->args[2])){
                if(mp_int_compare_value(get_number(c->args[2]), 0) == 0){
                    c->type=CODE_COPY;
                    c->args[2]=add_number("0");
                    c->args[3]=-1;
                    c->size=4;
                }
            } else if (is_constant(c->args[4]) && c->args[0] == c->args[2] && c->args[1] == c->args[3]){
                if(mp_int_compare_value(get_number(c->args[4]), 1) == 0){
                    c->type=CODE_DEC;
                    c->size=2;
                }
            } else if (is_constant(c->args[2]) && c->args[0] == c->args[4] && c->args[1] == c->args[5]){
                if(mp_int_compare_value(get_number(c->args[2]), 1) == 0){
                    c->type=CODE_DEC;
                    c->args[2]=c->args[4];
                    c->args[3]=c->args[5];
                    c->size=2;
                }
            }
            break;
                
        case CODE_MUL:  //mnozenie stalych, stala*zmienna (2^n : n=0..inf)
            if(is_constant(c->args[2]) && is_constant(c->args[4])){
                mp_int tmp=mp_int_alloc();
                mp_int_init(tmp);
                mp_int_mul(get_number(c->args[2]), get_number(c->args[4]), tmp);

                for(int i=3; i<6; i++) c->args[i]=-1;              
                c->args[2]=add_number2(tmp);

                mp_int_free(tmp);
                c->type=CODE_COPY;
                c->size=4;
            } else if (is_constant(c->args[4]) && c->args[0] == c->args[2] && c->args[1] == c->args[3]){
                if(mp_int_compare_value(get_number(c->args[4]), 1) == 0){
                    c->type=CODE_DELETED;
                } else if(mp_int_compare_value(get_number(c->args[4]), 2) == 0){
                    c->type=CODE_ADD;
                    c->args[4]=c->args[2];
                    c->args[5]=c->args[3];
                }
            } else if (is_constant(c->args[2]) && c->args[0] == c->args[4] && c->args[1] == c->args[5]){
                if(mp_int_compare_value(get_number(c->args[2]), 1) == 0){
                    c->type=CODE_DELETED;
                } else if(mp_int_compare_value(get_number(c->args[2]), 2) == 0){
                    c->type=CODE_ADD;
                    c->args[2]=c->args[4];
                    c->args[3]=c->args[5];
                }
            }
            break;
         
        case CODE_DIV:  //dzielenie stalych zmienna/stala (0, 2^n: n=0..inf)
            if(is_constant(c->args[2]) && is_constant(c->args[4])){
                mp_int tmp=mp_int_alloc();
                mp_int_init(tmp);
                mp_int_div(get_number(c->args[2]), get_number(c->args[4]), tmp, NULL);

                for(int i=3; i<6; i++) c->args[i]=-1;              
                c->args[2]=add_number2(tmp);

                mp_int_free(tmp);
                c->type=CODE_COPY;
                c->size=4;
            } else if (is_constant(c->args[4]) && c->args[0] == c->args[2] && c->args[1] == c->args[3]){
                if(mp_int_compare_value(get_number(c->args[4]), 1) == 0){
                    c->type=CODE_DELETED;
                } else if(mp_int_compare_value(get_number(c->args[4]), 2) == 0){
                    c->type=CODE_HALF;
                    c->size=4;
                }
            } else if (is_constant(c->args[4])){
                if(mp_int_compare_value(get_number(c->args[4]), 0) == 0){
                    c->type=CODE_COPY;
                    c->args[2]=add_number("0");
                    c->args[3]=-1;
                    c->size=4;
                } else if(mp_int_compare_value(get_number(c->args[4]), 1) == 0){
                    c->type=CODE_COPY;
                    c->size=4;
                }
            }
            break;
         
        case CODE_MOD:  //modulo stalych, zmienna % stala (dla 0 i 1 wynik to 0, dla 2 to JODD/JUMP x=1 lub x=0)
            if(is_constant(c->args[2]) && is_constant(c->args[4])){
                mp_int tmp=mp_int_alloc();
                mp_int_init(tmp);
                mp_int_div(get_number(c->args[2]), get_number(c->args[4]), NULL, tmp);

                for(int i=3; i<6; i++) c->args[i]=-1;              
                c->args[2]=add_number2(tmp);

                mp_int_free(tmp);
                c->type=CODE_COPY;
                c->size=4;
            } else if (is_constant(c->args[4])){
                if(mp_int_compare_value(get_number(c->args[4]), 0) == 0){
                    c->type=CODE_COPY;
                    c->args[2]=add_number("0");
                    c->args[3]=-1;
                    c->size=4;
                } else if(mp_int_compare_value(get_number(c->args[4]), 1) == 0){
                    c->type=CODE_COPY;
                    c->args[2]=add_number("0");
                    c->args[3]=-1;
                    c->size=4;
                }
            }
            break;
         
        case CODE_JNEQ:
             if(is_constant(c->args[2]) && is_constant(c->args[4])){
                if(mp_int_compare(get_number(c->args[2]),get_number(c->args[4])) == 0){
                    c->type=CODE_DELETED;
                } else {
                    c->type=CODE_JUMP;
                    c->size=2;
                }
             }
            break;
         
        case CODE_JEQ:  //x==0 -> JZERO, JUMP
            if(is_constant(c->args[2]) && is_constant(c->args[4])){
                if(mp_int_compare(get_number(c->args[2]),get_number(c->args[4])) == 0){
                    c->type=CODE_JUMP;
                    c->size=2;
                } else {
                    c->type=CODE_DELETED;
                }
            } else if(is_constant(c->args[2])){
                if(mp_int_compare_zero(get_number(c->args[2])) == 0){
                    c->type=CODE_JZERO;
                    c->args[2]=c->args[4];
                    c->args[3]=c->args[5];
                    c->size=4;
                }
            } else if(is_constant(c->args[4])){
                if(mp_int_compare_zero(get_number(c->args[4])) == 0){
                    c->type=CODE_JZERO;
                    c->size=4;
                }
            }
            break;
         
        case CODE_JGEQ: 
            if(is_constant(c->args[2]) && is_constant(c->args[4])){
                if(mp_int_compare(get_number(c->args[2]),get_number(c->args[4])) >= 0){
                    c->type=CODE_JUMP;
                    c->size=2;
                } else {
                    c->type=CODE_DELETED;
                }
            } else if(is_constant(c->args[4])){
                if(mp_int_compare_zero(get_number(c->args[4])) == 0){
                    c->type=CODE_JUMP;
                    c->size=2;
                }
            }
            break;
         
        case CODE_JLEQ:
            if(is_constant(c->args[2]) && is_constant(c->args[4])){
                if(mp_int_compare(get_number(c->args[2]),get_number(c->args[4])) <= 0){
                    c->type=CODE_JUMP;
                    c->size=2;
                } else {
                    c->type=CODE_DELETED;
                }
            } else if(is_constant(c->args[4])){
                if(mp_int_compare_zero(get_number(c->args[4])) == 0){
                    c->type=CODE_JZERO;
                    c->size=4;
                }
            }
            break;
         
        case CODE_JGT:  // ni mo
            break;
         
        case CODE_JLT: // tyz ni mo
            break;
        
        default:
            break;
        }
    }    
}

int find_error_symbol(int* symbols, int size, int symbol){
    for(int i=0; i<size; i++)
        if(symbols[i] == symbol)
            return 1;
    
    return 0;
}

void add_error_symbol(int* symbols, int* size, int symbol){
    for(int i=0; i<*size; i++)
        if(symbols[i] == symbol)
            return;
    
    symbols[*size]=symbol;
    (*size)++;
}

void check_code(){
    int symbols[table->size];
    int iterator=0;
    
    symbols[iterator++] = -1;
    for(int i=0; i<table->size; i++)
        if(table->symbols[i].type == NUMBER)
            symbols[iterator++]=i;
    
    for(int i=0; i<codeProgram->size; i++){
        struct CodeCommand* c = &codeProgram->commands[i];
        switch(c->type){   
        case CODE_WRITE:
        case CODE_HALF:
        case CODE_INC:
        case CODE_DEC:
            for(int j=0; j<2; j++){
                if(!find_error_symbol(symbols, iterator, c->args[j])){
                    fprintf(stderr, "Blad: Uzycie niezainicjalizowanej zmiennej %s\n",table->symbols[c->args[j]].name);
                    exit(1);
                }
            }
            break;

        case CODE_COPY:
        case CODE_JZERO:
            for(int j=2; j<4; j++){
                if(!find_error_symbol(symbols, iterator, c->args[j])){
                    fprintf(stderr, "Blad: Uzycie niezainicjalizowanej zmiennej %s\n",table->symbols[c->args[j]].name);
                    exit(1);
                }
            }
            break;

        case CODE_ADD:
        case CODE_SUB:
        case CODE_MUL:
        case CODE_DIV:
        case CODE_MOD:   
        case CODE_JNEQ:
        case CODE_JEQ:
        case CODE_JGEQ:
        case CODE_JLEQ:
        case CODE_JGT:  
        case CODE_JLT: 
            for(int j=2; j<6; j++){
                if(!find_error_symbol(symbols, iterator, c->args[j])){
                    fprintf(stderr, "Blad: Uzycie niezainicjalizowanej zmiennej %s\n",table->symbols[c->args[j]].name);
                    exit(1);
                }
            }
            break;

        default: 
            break;
        }
        
        switch(c->type){   
        case CODE_READ:
        case CODE_COPY:
        case CODE_ADD:
        case CODE_SUB:
        case CODE_MUL:
        case CODE_DIV:
        case CODE_MOD:
            add_error_symbol(symbols, &iterator, c->args[0]);
            add_error_symbol(symbols, &iterator, c->args[1]);
            break;

        default: 
            break;
        }     
    } 
}

void print_code(){
    printf("\nKOD:\n");
    for(int i=0; i<codeProgram->size; i++){
        printf("Command type = %d (%s), size = %d, block = %d, args = ", codeProgram->commands[i].type, codeCommandNames[codeProgram->commands[i].type], codeProgram->commands[i].size, codeProgram->commands[i].block );
        for(int j=0; j<codeProgram->commands[i].size; j++)
            printf("%d ", codeProgram->commands[i].args[j]);
        printf("\n");
    }
}

/////////////////////////////////////////////////////////////// REJESTRY ///////////////////////////////////////////////////////////////


void init_registers(){
    for(int i=0; i<8; i++){
        registers[i].free=1;
        registers[i].canBeFreed=-1;
        registers[i].shouldBeSaved=0;
        registers[i].index=-1;
        registers[i].index2=-1;
        registers[i].command.type=CODE_UNKNOWN;
        registers[i].value=mp_int_alloc();
        mp_int_init_value(registers[i].value, -1);
    }
}

void free_registers(){
    for(int i=0; i<8; i++)
        mp_int_free(registers[i].value);
}

void set_register(int reg, mp_int pos){        
    add_asm_comment("# SET REGISTER");
    
    int known=1;
    if(mp_int_compare_value(registers[reg].value, -1) == 0) known=0;
     
    if(known && mp_int_compare(registers[reg].value, pos) == 0){
        return;
    } else if(known && mp_int_compare(registers[reg].value, pos) < 0){
        mp_int tmp=mp_int_alloc();
        mp_int_init(tmp);
        mp_int_sub(pos, registers[reg].value, tmp);
        if(mp_int_compare_value(tmp, 30) < 0){
            add_asm_comment("# SET REGISTER: INCS");
            mp_small tmp2;
            mp_int_to_int(tmp, &tmp2);
            while(tmp2){
                add_asm_command(ASM_INC, reg, NONE, NONE);
                tmp2--;
            }
            mp_int_free(tmp);
            mp_int_copy(pos, registers[reg].value);
            return;
        }
        mp_int_free(tmp);
    } else if(known && mp_int_compare(registers[reg].value, pos) > 0){
        mp_int tmp=mp_int_alloc();
        mp_int_init(tmp);
        mp_int_sub(registers[reg].value, pos, tmp);
        if(mp_int_compare_value(tmp, 30) < 0){
            add_asm_comment("# SET REGISTER: DECS");
            mp_small tmp2;
            mp_int_to_int(tmp, &tmp2);
            while(tmp2){
                add_asm_command(ASM_DEC, reg, NONE, NONE);
                tmp2--;
            }
            mp_int_free(tmp);
            mp_int_copy(pos, registers[reg].value);
            return;
        }
        mp_int_free(tmp);
    }
     
    add_asm_comment("# SET REGISTER: GENERATE");
    generate_constant(reg, pos);
}

void set_a_register(mp_int pos){   
    set_register(REG_A, pos);   
}

void forget_variable(int index, int index2){
    if(index2==-1){
        for(int i=1; i<8; i++){
            if(registers[i].index2==index){
                store_register(i);

                registers[i].free=1;
                registers[i].index=-1;
                registers[i].index2=-1;
                registers[i].command.type=CODE_UNKNOWN;
                mp_int_set_value(registers[i].value, -1);

                registers[i].canBeFreed=-1;
                registers[i].shouldBeSaved=0;
            }
        }
    }
    
    for(int i=1; i<8; i++){
        if(registers[i].index == index && registers[i].index2 == index2){
            registers[i].free=1;
            registers[i].canBeFreed=-1;
            registers[i].shouldBeSaved=0;
            registers[i].index=-1;
            registers[i].index2=-1;
            mp_int_set_value(registers[i].value, -1);
        }
    }
}

void reset_registers(){
    add_asm_comment("# RESETOWANIE REJESTROW");
    for(int i=1; i<8; i++){
        if(registers[i].free != 1 && registers[i].index2 != -1){
            for(int j=1; j<8; j++){
                if(registers[j].index == registers[i].index2){
                    store_register(i);
                }
            }
        }
    }
    
    for(int i=1; i<8; i++){
        if(registers[i].free != 1 && registers[i].index2 == -1)
            store_register(i);
    }
    
    for(int i=1; i<8; i++){
        if(registers[i].free != 1)
            store_register(i);
    }
    
    for(int i=1; i<8; i++){        
        registers[i].free=1;
        registers[i].canBeFreed=-1;
        registers[i].shouldBeSaved=0;
        registers[i].index=-1;
        registers[i].index2=-1;
        registers[i].command.type=CODE_UNKNOWN;
        mp_int_set_value(registers[i].value, -1);
    }
    
    mp_int_set_value(registers[0].value, -1);
}

int give_me_free_register_or_easy_store(){                   
    for(int i=1; i<8; i++){
        if(registers[i].free==1){                  
            registers[i].free=0;
            registers[i].canBeFreed=0;
            registers[i].shouldBeSaved=0;
            registers[i].command.type=CODE_UNKNOWN;
            mp_int_set_value(registers[i].value, -1);

            return i;
        }
    }
  
    for(int i=1; i<8; i++){
        if(registers[i].canBeFreed==1 && registers[i].shouldBeSaved==0 && registers[i].command.type==CODE_UNKNOWN){                  
            registers[i].free=0;
            registers[i].canBeFreed=0;
            mp_int_set_value(registers[i].value, -1);

            return i;
        }
    }
    
    for(int i=1; i<8; i++){                       
        if(registers[i].canBeFreed==1 && registers[i].index2==-1){
            if(store_register(i) == -1)
                continue;
            
            registers[i].free=0;
            registers[i].index=-1;
            registers[i].index2=-1;
            registers[i].command.type=CODE_UNKNOWN;
            
            mp_int_set_value(registers[i].value, -1);
            
            registers[i].canBeFreed=0;
            registers[i].shouldBeSaved=0;            

            return i;
        }   
    }
    
    for(int i=1; i<8; i++){
        if(registers[i].canBeFreed==1 && registers[i].shouldBeSaved==0){                  
            registers[i].free=0;
            registers[i].canBeFreed=0;
            registers[i].command.type=CODE_UNKNOWN;
            mp_int_set_value(registers[i].value, -1);

            return i;
        }
    }
    
    return -1;
}

int give_me_any_register(){                       
    int ret = give_me_free_register_or_easy_store();
    if(ret != -1) return ret;
    
    for(int i=1; i<8; i++){                       
        if(registers[i].canBeFreed==1){
            if(store_register(i) == -1)
                continue;
            
            registers[i].free=0;
            registers[i].index=-1;
            registers[i].index2=-1;
            registers[i].command.type=CODE_UNKNOWN;
            mp_int_set_value(registers[i].value, -1);

            registers[i].canBeFreed=0;
            registers[i].shouldBeSaved=0;
            
            return i;
        }   
    }
    
    fprintf(stderr, "NIE UDALO MI SIE ZWOLNIC ZADNEGO REJESTRU. KONIEC SWIATA.\n"); //to prawda
    return -1;
}


int give_me_easily_filled_register(int index, int index2){                     //daj mi rejestr i wstaw do niego zmienna
    for(int i=1; i<8; i++){
        if(registers[i].index==index && registers[i].index2==index2){    //jezeli ta zmienna juz siedzi w jakims rejestrze, to po prostu go zwracamy
            add_asm_comment("# EASY FILL FROM REG");
            registers[i].canBeFreed=0;
            
            return i;
        }
    }
    
    add_asm_comment("# EASY FILL VAR");
    int reg=give_me_free_register_or_easy_store(); 
    if(reg == -1) return -1;
    
    set_a_register(get_memory_of_variable(index));
    add_asm_command(ASM_LOAD,reg,NONE,NONE);
   
    add_asm_comment("# EASY FILL END");

    registers[reg].index=index;
    registers[reg].index2=index2;

    return reg;
}

    
int give_me_filled_register(int index, int index2){                     //daj mi rejestr i wstaw do niego zmienna
    for(int i=1; i<8; i++){
        if(registers[i].index==index && registers[i].index2==index2){    //jezeli ta zmienna juz siedzi w jakims rejestrze, to po prostu go zwracamy
            add_asm_comment("# FILL FROM REG");
            registers[i].canBeFreed=0;
            
            return i;
        }
    }
    
    int reg;            //jak nie, to pobieramy wolny rejestr i nastepnie:

    if(is_constant(index)){                         //przypadek prosty: musimy wstawic stala, wiec ja generujemy
        add_asm_comment("# FILL CONST");

        reg=give_me_any_register();
        if(reg == -1) return -1;

        set_register(reg, get_number(index));
    } else if(index2 == -1) {                       //sredni: zmienna. Ustawiamy rejestr A na adres zmiennej i ja pobieramy
        add_asm_comment("# FILL VAR");

        reg=give_me_any_register(); 
        if(reg == -1) return -1;

        set_a_register(get_memory_of_variable(index));
        add_asm_command(ASM_LOAD,reg,NONE,NONE);
    } else {
        for(int i=1; i<8; i++){                       
            if(registers[i].index==index && registers[i].canBeFreed==1){
                store_register(i);

                registers[i].free=1;
                registers[i].index=-1; //czy to tu ma byc ???
                registers[i].index2=-1;
                registers[i].command.type=CODE_UNKNOWN;
                mp_int_set_value(registers[i].value, -1);
                registers[i].canBeFreed=-1;
                registers[i].shouldBeSaved=0;
            }   
        }
        if(is_constant(index2)){                 //trudny: tablica o stalym indeksie: Ustawiamy A na dobry adres i ja pobieramy
            add_asm_comment("# FILL ARR CONST");

            reg=give_me_any_register(); 
            if(reg == -1) return -1;

            set_a_register(get_memory_of_array(index,get_number(index2)));
            add_asm_command(ASM_LOAD,reg,NONE,NONE);
        } else {                                         //najtrudniejszy: tablica o zmiennym indeksie
            add_asm_comment("# FILL ARR VAR");

            reg=give_me_filled_register(index2,-1); 
            if (reg == -1) return -1;
                
            if(mp_int_compare_zero(get_start_of_array(index)) >= 0){
                int reg2=give_me_free_register_or_easy_store();
                if(reg2 != -1){
                    set_a_register(get_start_of_array(index));      
                    add_asm_command(ASM_ADD,REG_A,reg,NONE);  
      
                    registers[reg].canBeFreed=1;
                    reg=reg2;
                } else {
                    store_register(reg);
                    registers[reg].free=0;
                    registers[reg].canBeFreed=0;
                    
                    set_a_register(get_start_of_array(index));      //idziemy na poczatek tablicy
                    add_asm_command(ASM_ADD,REG_A,reg,NONE);        //dodajemy indeks do adresu       
                }
                
            } else {
                int reg2=give_me_free_register_or_easy_store();
                if(reg2 != -1){
                    set_a_register(get_memory_of_variable(index));
                    add_asm_command(ASM_ADD,REG_A,reg,NONE);
                    
                    set_register(reg2,get_offset_of_array(index));
                    add_asm_command(ASM_SUB,REG_A,reg2,NONE);     
                    
                    registers[reg].canBeFreed=1;
                    
                    reg=reg2;
                } else {
                    store_register(reg);
                    registers[reg].free=0;
                    registers[reg].canBeFreed=0;

                    set_a_register(get_memory_of_variable(index));
                    add_asm_command(ASM_ADD,REG_A,reg,NONE);

                    set_register(reg,get_offset_of_array(index));
                    add_asm_command(ASM_SUB,REG_A,reg,NONE);        
                }
            }
            
            add_asm_command(ASM_LOAD,reg,NONE,NONE);        //pobieramy wartosc do rejestru wynikowego
            mp_int_set_value(registers[REG_A].value, -1);   //zaznaczamy, ze zgubilismy wartosc A
        }
    }
    add_asm_comment("# FILL END");

    registers[reg].index=index;
    registers[reg].index2=index2;

    return reg;
}

int store_register(int r){
    registers[r].canBeFreed=0;  //od razu ustawiamy go na nie-do-zwolnienia
    
    if(registers[r].shouldBeSaved == 0){
        registers[r].free=1;  
        add_asm_comment("# SHOULD NOT BE SAVED");
        return 0;
    }
    
    if(is_constant(registers[r].index)){
        registers[r].free=1;
        add_asm_comment("# TO BYLA STALA");
        return 0;
    }
        
    if(registers[r].index2 == -1) {  
        char msg[64];
        sprintf(msg, "# ZAPISYWANIE ZMIENNEJ z %d, ind=%d, ind2=%d",r,registers[r].index,registers[r].index2);
        add_asm_comment(msg);
        set_a_register(get_memory_of_variable(registers[r].index));
        add_asm_command(ASM_STORE,r,NONE,NONE);     //zapisujemy wartosc do pamieci
    } else if (is_constant(registers[r].index2)){
        add_asm_comment("# ZAPISYWANIE TABLICY O STALYM INDEKSIE");

        set_a_register(get_memory_of_array(registers[r].index, get_number(registers[r].index2)));
        add_asm_command(ASM_STORE,r,NONE,NONE);     //zapisujemy wartosc do pamieci
    }
    else{
        char msg[64];
        sprintf(msg, "#ZAPISYWANIE TABLICY O ZMIENNYM INDEKSIE ind=%d, ind2=%d",registers[r].index,registers[r].index2);
        add_asm_comment(msg);
        int reg=give_me_easily_filled_register(registers[r].index2, -1);  
        int emergency=0;
        int free;
        int canBeFreed;
        int index;
        int index2;
        if (reg==-1){ // registers[r].canBeFreed=1; return -1;         
            reg = (r % 7) + 1;

            free=registers[reg].free;
            canBeFreed=registers[reg].canBeFreed;
            index=registers[reg].index;
            index2=registers[reg].index2;
            
            add_asm_command(ASM_SUB, REG_A, REG_A, NONE);
            add_asm_command(ASM_STORE, reg, NONE, NONE);
            
            registers[REG_A].value=0;
            
            set_a_register(get_memory_of_variable(registers[r].index2));
            add_asm_command(ASM_LOAD,reg,NONE,NONE);
            
            registers[reg].index=registers[r].index2;
            registers[reg].index2=-1;
            emergency = 1;
        }          
        
        if(mp_int_compare_zero(get_start_of_array(registers[r].index)) >= 0){
            set_a_register(get_start_of_array(registers[r].index));      //idziemy na poczatek tablicy

            add_asm_command(ASM_ADD,REG_A,reg,NONE);        //dodajemy indeks do adres   
            registers[reg].canBeFreed=1;
        }
        else{
            int reg2=give_me_free_register_or_easy_store();

            if(reg2 != -1){
                set_a_register(get_memory_of_variable(registers[r].index));
                add_asm_command(ASM_ADD,REG_A,reg,NONE);
                set_register(reg2,get_offset_of_array(registers[r].index));
                add_asm_command(ASM_SUB,REG_A,reg2,NONE);

                registers[reg].canBeFreed=1;

                registers[reg2].free=1;
                registers[reg2].index=-1;
                registers[reg2].index2=-1;
            } else {
                store_register(reg); 

                set_a_register(get_memory_of_variable(registers[r].index));
                add_asm_command(ASM_ADD,REG_A,reg,NONE);

                set_register(reg,get_offset_of_array(registers[r].index));
             
                add_asm_command(ASM_SUB,REG_A,reg,NONE);
                
                registers[reg].free=1;
                registers[reg].index=-1;
                registers[reg].index2=-1;
            }                  
        }    
        
        mp_int_set_value(registers[REG_A].value, -1); 
        
        add_asm_command(ASM_STORE,r,NONE,NONE);     //zapisujemy wartosc do pamieci
        
        if(emergency){
            add_asm_command(ASM_SUB, REG_A, REG_A, NONE);
            add_asm_command(ASM_LOAD, reg, NONE, NONE);
            
            mp_int_set_value(registers[REG_A].value, 0); 
            
            registers[reg].free=free;
            registers[reg].canBeFreed=canBeFreed;
            registers[reg].index=index;
            registers[reg].index2=index2;
        }
    }

    registers[r].free=1;
    registers[r].shouldBeSaved=0;
    
    add_asm_comment("# KONIEC ZAPISYWANIA");
    
    return 0;
}

/////////////////////////////////////////////////////////////// ASEMBLER ///////////////////////////////////////////////////////////////

const char asmCommandNames[][8] = {
    "GET",      //0
    "PUT",      //1

    "LOAD",     //2
    "STORE",    //3

    "COPY",     //4
    "ADD",      //5
    "SUB",      //6

    "HALF",     //7
    "INC",      //8
    "DEC",      //9

    "JUMP",     //10
    "JZERO",    //11
    "JODD",     //12

    "HALT",     //13
    "LABEL",    //14
    "COMMENT"   //15
};


void init_assembler(){
    assembler=malloc(sizeof(struct AssemblerProgram));
    assembler->size=0;
    assembler->maxSize=128;
    assembler->commands=malloc(sizeof(struct AssemblerCommand)*128);
}

void free_assembler(){
    for(int i=0; i<assembler->size; i++)
        if(assembler->commands[i].comment)
            free(assembler->commands[i].comment);
    
    free(assembler->commands);
    free(assembler);
}

int resize_assembler(){
    void* tmp= realloc(assembler->commands, sizeof(struct AssemblerCommand)*assembler->maxSize*2);
    if(!tmp){
        fprintf(stderr, "(!) Reallocation failed at resize_assembler() (!)\n");
        return 1;
    }
    assembler->maxSize*=2;
    assembler->commands=tmp;
    
    return 0;
}

void add_asm_command(enum AssemblerCommandType type, int reg1, int reg2, int dest){
    if(assembler->size == assembler->maxSize)
        resize_assembler();
    
    assembler->commands[assembler->size].type=type;
    assembler->commands[assembler->size].reg1=reg1;
    assembler->commands[assembler->size].reg2=reg2;
    assembler->commands[assembler->size].jumpDestination=dest;
    assembler->commands[assembler->size].comment=NULL;
    
    assembler->size++;
}

void add_asm_comment(const char* com){
    if(assembler->size == assembler->maxSize)
        resize_assembler();

    assembler->commands[assembler->size].type=ASM_COMMENT;
    assembler->commands[assembler->size].comment=strdup(com);
    
    assembler->size++;
}


void generate_constant(int reg, mp_int c){ //generowanie stalej:
    char* bits = malloc(20+mp_int_count_bits(c));
    
    mp_int cons=mp_int_alloc();
    mp_int_init_copy(cons, c);
    
    int len = mp_int_string_len(cons, 10);
    char* buf = calloc(len, sizeof(*buf));
    mp_int_to_string(cons, 10, buf, len);
    sprintf(bits, "# Generuje stala %s", buf);
    add_asm_comment(bits);
    free(buf);
    
    int i=0;
    add_asm_command(ASM_SUB,reg, reg, NONE);            //zerujemy bufer
    if(mp_int_compare_zero(cons) == 0) {
        free(bits);
        mp_int_free(cons);
        mp_int_copy(c, registers[reg].value);
        add_asm_comment("# Skonczylem generowac zero");
        
        return; 
    }
    
    if(mp_int_compare_value(cons, 24) < 0){
        mp_small tmp;
        mp_int_to_int(cons, &tmp);
        while(tmp){
            add_asm_command(ASM_INC, reg, NONE, NONE);
            tmp--;
        }
        free(bits);
        mp_int_free(cons);
        mp_int_copy(c, registers[reg].value);
        add_asm_comment("# Skonczylem generowac wartosc mniejsza niz 24");
        
        return;
    }
    
    while(mp_int_compare_zero(cons)){                                     //wyznaczamy bity liczby
        mp_small r;
        mp_int_div_value(cons, 2, cons, &r);
        bits[i++]=r;
    }
    add_asm_command(ASM_INC,reg,NONE,NONE); 
    for(i-=2;i>=0;i--){                                 //idziemy po bitach od najwyzszej potegi, mnozymy przez 2 wynik, jezeli bit = 1, to dodajemy 1
        add_asm_command(ASM_ADD,reg, reg, NONE);
        if(bits[i])
            add_asm_command(ASM_INC,reg,NONE,NONE);
    }
    
    free(bits);
    mp_int_free(cons);
    mp_int_copy(c, registers[reg].value);
    
    add_asm_comment("# Skonczylem generowac");
}

void enumerate_assembler(){                                           //numerujemy komendy, label ma numer nastepnej (po nim) komendy
    int c=0;
    for(int i=0; i<assembler->size; i++){
        if(assembler->commands[i].type == ASM_LABEL || assembler->commands[i].type == ASM_COMMENT)
            assembler->commands[i].noumber=c;
        else
            assembler->commands[i].noumber=c++;
    }
}

void resolve_jumps(){ //wcale nie n^2 xd
    for(int i=0; i<assembler->size; i++){
        if(    assembler->commands[i].type == ASM_JUMP
            || assembler->commands[i].type == ASM_JZERO
            || assembler->commands[i].type == ASM_JODD){    
           
            for(int j=0; j<assembler->size; j++){
                if(assembler->commands[j].type == ASM_LABEL && (assembler->commands[i].jumpDestination == assembler->commands[j].jumpDestination)){
                    assembler->commands[i].jumpDestination=assembler->commands[j].noumber;
                    break;
                }                
            }
        }
    }
}

int previousBlock=-2;
int currentBlock=-1;

void transform_code_to_asm(){
    int reg[7], labels[9], labelCounter=-2;
    for(int i=0; i<codeProgram->size; i++){
        currentBlock=codeProgram->commands[i].block;
        
        if(previousBlock != currentBlock){
            previousBlock=currentBlock;
            reset_registers();
        }
        
        switch (codeProgram->commands[i].type){
        case CODE_LABEL:
            add_asm_command(ASM_LABEL,NONE,NONE,codeProgram->commands[i].args[0]);
            break;
                
        case CODE_READ:
            reg[0]=give_me_any_register();
            add_asm_command(ASM_GET,reg[0],NONE,NONE);
            
            forget_variable(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);
                
            registers[reg[0]].index=codeProgram->commands[i].args[0];
            registers[reg[0]].index2=codeProgram->commands[i].args[1];
            registers[reg[0]].canBeFreed=1;
            registers[reg[0]].shouldBeSaved=1;
            break;
                
        case CODE_WRITE:
            reg[0]=give_me_filled_register(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);
            add_asm_command(ASM_PUT,reg[0],NONE,NONE);
                
            registers[reg[0]].canBeFreed=1;
            break;
                
        case CODE_HALF:            
            if(codeProgram->commands[i].args[1] == -1){
                for(int j=1; j<8; j++){
                   if(registers[j].index2 == codeProgram->commands[i].args[0]){
                        store_register(j);
                       
                        registers[j].free=1;
                        registers[j].index=-1;
                        registers[j].index2=-1;
                        mp_int_set_value(registers[j].value, -1);
                        registers[j].canBeFreed=-1;
                        registers[j].shouldBeSaved=0;
                    }
                }    
            }    
                
            reg[0]=give_me_filled_register(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);
                
            add_asm_command(ASM_HALF,reg[0],NONE,NONE);
                
            registers[reg[0]].canBeFreed=1;
            registers[reg[0]].shouldBeSaved=1;
            break;
                
        case CODE_INC:
            add_asm_comment("# INC START");
            
            if(codeProgram->commands[i].args[1] == -1){
                for(int j=1; j<8; j++){
                   if(registers[j].index2 == codeProgram->commands[i].args[0]){
                        store_register(j);

                        registers[j].free=1;
                        registers[j].index=-1;
                        registers[j].index2=-1;
                        mp_int_set_value(registers[j].value, -1);
                        registers[j].canBeFreed=-1;
                        registers[j].shouldBeSaved=0;
                    }
                }    
            }    
                
            reg[0]=give_me_filled_register(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);
                
            add_asm_command(ASM_INC,reg[0],NONE,NONE);
                
            registers[reg[0]].canBeFreed=1;
            registers[reg[0]].shouldBeSaved=1;
                
            add_asm_comment("# INC END");
            break;
                
        case CODE_DEC:
            add_asm_comment("# DEC START");
            
            if(codeProgram->commands[i].args[1] == -1){
                for(int j=1; j<8; j++){
                   if(registers[j].index2 == codeProgram->commands[i].args[0]){
                        store_register(j);

                        registers[j].free=1;
                        registers[j].index=-1;
                        registers[j].index2=-1;
                        mp_int_set_value(registers[j].value, -1);
                        registers[j].canBeFreed=-1;
                        registers[j].shouldBeSaved=0;
                    }
                }    
            }    
            
            reg[0]=give_me_filled_register(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);
                
            add_asm_command(ASM_DEC,reg[0],NONE,NONE);

            registers[reg[0]].canBeFreed=1;
            registers[reg[0]].shouldBeSaved=1;

            add_asm_comment("# DEC END");
            break;
                
        case CODE_COPY:
            add_asm_comment("# KOPIOWANIE");
            reg[0]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
            reg[1]=give_me_any_register();
               
            add_asm_command(ASM_COPY,reg[1],reg[0],NONE);        
            
            //link_to_register(reg[0],codeProgram->commands[i].args[0],codeProgram->commands[i].args[1]);  //xD
                               
            registers[reg[0]].canBeFreed=1;
            
            forget_variable(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);    
                
            registers[reg[1]].index=codeProgram->commands[i].args[0];
            registers[reg[1]].index2=codeProgram->commands[i].args[1];
            registers[reg[1]].canBeFreed=1;
            registers[reg[1]].shouldBeSaved=1;
                
            add_asm_comment("# KONIEC KOPIOWANIA");
            break;
                
        case CODE_ADD:
            add_asm_comment("# DODAWANIE");
            reg[0]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
            reg[1]=give_me_filled_register(codeProgram->commands[i].args[4], codeProgram->commands[i].args[5]);
            reg[2]=give_me_any_register();
            
                   
            add_asm_command(ASM_COPY,reg[2],reg[0],NONE);    
            add_asm_command(ASM_ADD,reg[2],reg[1],NONE);
                

            registers[reg[0]].canBeFreed=1;
                                
            registers[reg[1]].canBeFreed=1;
            
            forget_variable(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);
                
            registers[reg[2]].index=codeProgram->commands[i].args[0];
            registers[reg[2]].index2=codeProgram->commands[i].args[1];
            registers[reg[2]].canBeFreed=1;    
            registers[reg[2]].shouldBeSaved=1;
                
            add_asm_comment("# KONIEC DODAWANIA");
            break;
                
        case CODE_SUB:
            add_asm_comment("# SUB START");
            reg[0]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
            reg[1]=give_me_filled_register(codeProgram->commands[i].args[4], codeProgram->commands[i].args[5]);
            reg[2]=give_me_any_register();
                 
                
            add_asm_command(ASM_COPY,reg[2],reg[0],NONE); 
            add_asm_command(ASM_SUB,reg[2],reg[1],NONE);
                
                
            registers[reg[0]].canBeFreed=1;
                
            registers[reg[1]].canBeFreed=1;
            
            forget_variable(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);    
                
            registers[reg[2]].index=codeProgram->commands[i].args[0];
            registers[reg[2]].index2=codeProgram->commands[i].args[1];
            registers[reg[2]].canBeFreed=1;        
            registers[reg[2]].shouldBeSaved=1;
                
            add_asm_comment("# SUB END");
            break;
                
        case CODE_JUMP:
            add_asm_command(ASM_JUMP,NONE,NONE,codeProgram->commands[i].args[0]);
            break;
                
        case CODE_JZERO:
            add_asm_comment("# JZERO START");
                
            reg[0]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
                
            add_asm_command(ASM_JZERO,reg[0],NONE,codeProgram->commands[i].args[0]);
                
            registers[reg[0]].canBeFreed=1; 
                
            add_asm_comment("# JZERO END");         
            break;
                
        case CODE_JNEQ:
            if(is_constant(codeProgram->commands[i].args[2]) 
                && mp_int_compare_zero(get_number(codeProgram->commands[i].args[2])) == 0){

                reg[0]=give_me_filled_register(codeProgram->commands[i].args[4], codeProgram->commands[i].args[5]);
                labels[0]=labelCounter--; //rob kod

                add_asm_command(ASM_JZERO, reg[0], NONE, labels[0]);
                add_asm_command(ASM_JUMP, NONE, NONE, codeProgram->commands[i].args[0]);

                add_asm_command(ASM_LABEL,NONE,NONE, labels[0]);
                registers[reg[0]].canBeFreed=1;     
            } else if(is_constant(codeProgram->commands[i].args[4])
                && mp_int_compare_zero(get_number(codeProgram->commands[i].args[4])) == 0){

                reg[0]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
                labels[0]=labelCounter--; //rob kod

                add_asm_command(ASM_JZERO, reg[0], NONE, labels[0]);
                add_asm_command(ASM_JUMP, NONE, NONE, codeProgram->commands[i].args[0]);

                add_asm_command(ASM_LABEL,NONE,NONE, labels[0]);
                registers[reg[0]].canBeFreed=1;     
            } else {
                reg[0]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
                reg[1]=give_me_filled_register(codeProgram->commands[i].args[4], codeProgram->commands[i].args[5]);
                reg[2]=give_me_any_register();

                add_asm_command(ASM_COPY,reg[2],reg[0],NONE);

                add_asm_command(ASM_SUB,reg[2],reg[1],NONE);            //A-B
                add_asm_command(ASM_JZERO,reg[2],NONE,labelCounter);    //drugie sprawdzenie
                add_asm_command(ASM_JUMP,NONE,NONE,codeProgram->commands[i].args[0]); //true   

                add_asm_command(ASM_LABEL,NONE,NONE, labelCounter--);
                add_asm_command(ASM_COPY,reg[2],reg[1],NONE);
                add_asm_command(ASM_SUB,reg[2],reg[0],NONE);            //B-A
                add_asm_command(ASM_JZERO,reg[2],NONE,labelCounter);    //false
                add_asm_command(ASM_JUMP,NONE,NONE,codeProgram->commands[i].args[0]);  //true
                add_asm_command(ASM_LABEL,NONE,NONE, labelCounter--);


                registers[reg[0]].canBeFreed=1;     

                registers[reg[1]].canBeFreed=1;

                registers[reg[2]].index=-1;
                registers[reg[2]].index2=-1;
                registers[reg[2]].free=1;
            }
            break;
                
        case CODE_JEQ:
            reg[0]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
            reg[1]=give_me_filled_register(codeProgram->commands[i].args[4], codeProgram->commands[i].args[5]);
            reg[2]=give_me_any_register();
                
            add_asm_command(ASM_COPY,reg[2],reg[0],NONE);
                
            add_asm_command(ASM_SUB,reg[2],reg[1],NONE);        //A-B
            labels[0]=labelCounter--;   //drugie sprawdzenie zera
            add_asm_command(ASM_JZERO,reg[2],NONE,labels[0]);
            labels[1]=labelCounter--;   //false
            add_asm_command(ASM_JUMP,NONE,NONE,labels[1]);   
               
            add_asm_command(ASM_LABEL,NONE,NONE, labels[0]);
            add_asm_command(ASM_COPY,reg[2],reg[1],NONE);
            add_asm_command(ASM_SUB,reg[2],reg[0],NONE);    //B-A
            add_asm_command(ASM_JZERO,reg[2],NONE,codeProgram->commands[i].args[0]); //true
            
            add_asm_command(ASM_LABEL,NONE,NONE, labels[1]); //false
                

            registers[reg[0]].canBeFreed=1;     
            
            registers[reg[1]].canBeFreed=1;
                
            registers[reg[2]].index=-1;
            registers[reg[2]].index2=-1;
            registers[reg[2]].free=1;
            break;
                
        case CODE_JGEQ:
            add_asm_comment("# JGEQ START");
            reg[0]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
            reg[1]=give_me_filled_register(codeProgram->commands[i].args[4], codeProgram->commands[i].args[5]);
            reg[2]=give_me_any_register();
                
            add_asm_command(ASM_COPY,reg[2],reg[1],NONE);
            add_asm_command(ASM_SUB,reg[2],reg[0],NONE);    //B-A
                
            add_asm_command(ASM_JZERO,reg[2],NONE,codeProgram->commands[i].args[0]); //true
            

            registers[reg[0]].canBeFreed=1;     
            
            registers[reg[1]].canBeFreed=1;
                
            registers[reg[2]].index=-1;
            registers[reg[2]].index2=-1;
            registers[reg[2]].free=1;
            add_asm_comment("# JGEQ END");
            break;
                
        case CODE_JLEQ:
            reg[0]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
            reg[1]=give_me_filled_register(codeProgram->commands[i].args[4], codeProgram->commands[i].args[5]);
            reg[2]=give_me_any_register();
                
            add_asm_command(ASM_COPY,reg[2],reg[0],NONE);
            add_asm_command(ASM_SUB,reg[2],reg[1],NONE);
                
            add_asm_command(ASM_JZERO,reg[2],NONE,codeProgram->commands[i].args[0]); //true
            

            registers[reg[0]].canBeFreed=1;     
            
            registers[reg[1]].canBeFreed=1;
                
            registers[reg[2]].index=-1;
            registers[reg[2]].index2=-1;
            registers[reg[2]].free=1;
            break;
                
        case CODE_JGT: //chyba nieuzywane obecnie, ale zostawie na wszelki wypadek :D
            //fprintf(stderr,"Uzylem JGT (??? )\n");
            reg[0]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
            reg[1]=give_me_filled_register(codeProgram->commands[i].args[4], codeProgram->commands[i].args[5]);
            reg[2]=give_me_any_register();
                
            add_asm_command(ASM_COPY,reg[2],reg[0],NONE);
                
            add_asm_command(ASM_SUB,reg[2],reg[1],NONE);        //A-B
            add_asm_command(ASM_JZERO,reg[2],NONE,labelCounter); //false   
               
            add_asm_command(ASM_COPY,reg[2],reg[1],NONE);
            add_asm_command(ASM_SUB,reg[2],reg[0],NONE);        //B-A
            add_asm_command(ASM_JZERO,reg[2],NONE,codeProgram->commands[i].args[0]); //true
            
            add_asm_command(ASM_LABEL,NONE,NONE, labelCounter--); //false
                

            registers[reg[0]].canBeFreed=1;     
            
            registers[reg[1]].canBeFreed=1;
                
            registers[reg[2]].index=-1;
            registers[reg[2]].index2=-1;
            registers[reg[2]].free=1;
            break;
                
        case CODE_JLT: //tez nie powinno byc uzywane
            //fprintf(stderr,"Uzylem JLT (??? )\n");
            reg[0]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
            reg[1]=give_me_filled_register(codeProgram->commands[i].args[4], codeProgram->commands[i].args[5]);
            reg[2]=give_me_any_register();
            add_asm_command(ASM_COPY,reg[2],reg[1],NONE);
                
            add_asm_command(ASM_SUB,reg[2],reg[0],NONE);        //B-A
            add_asm_command(ASM_JZERO,reg[2],NONE,labelCounter); //false    
              
            add_asm_command(ASM_COPY,reg[2],reg[0],NONE);
            add_asm_command(ASM_SUB,reg[2],reg[1],NONE);        //A-B
            add_asm_command(ASM_JZERO,reg[2],NONE,codeProgram->commands[i].args[0]); //true
            
            add_asm_command(ASM_LABEL,NONE,NONE, labelCounter--); //false
                

            registers[reg[0]].canBeFreed=1;     
            
            registers[reg[1]].canBeFreed=1;
                
            registers[reg[2]].index=-1;
            registers[reg[2]].index2=-1;
            registers[reg[2]].free=1;
            break;
                
        case CODE_MUL:
            add_asm_comment("# MNOZENIE");
                
            if(is_constant(codeProgram->commands[i].args[2]) || is_constant(codeProgram->commands[i].args[4])){
                int index;
                int arg1;
                int arg2;
                if(is_constant(codeProgram->commands[i].args[4])){
                    add_asm_comment("# X * const");
                    index=codeProgram->commands[i].args[4];
                    arg1=codeProgram->commands[i].args[2];
                    arg2=codeProgram->commands[i].args[3];
                } else {
                    add_asm_comment("# const * X");
                    index=codeProgram->commands[i].args[2];
                    arg1=codeProgram->commands[i].args[4];
                    arg2=codeProgram->commands[i].args[5];
                }
                
                mp_int cons=mp_int_alloc();
                mp_int_init_copy(cons, get_number(index));

                reg[1]=give_me_filled_register(arg1, arg2);
                reg[2]=give_me_any_register();
                reg[0]=give_me_any_register();
                
                add_asm_command(ASM_COPY, reg[2], reg[1], NONE);
                add_asm_command(ASM_SUB, reg[0], reg[0], NONE);
                
                while(mp_int_compare_zero(cons)){
                    mp_small r;
                    mp_int_div_value(cons, 2, cons, &r);
                    if(r)
                        add_asm_command(ASM_ADD, reg[0], reg[2], NONE);
                    if(mp_int_compare_zero(cons))
                        add_asm_command(ASM_ADD, reg[2], reg[2], NONE);
                }
                mp_int_free(cons);
                
                registers[reg[1]].canBeFreed=1;
                
                registers[reg[2]].index=-1;
                registers[reg[2]].index2=-1;
                registers[reg[2]].free=1;
                
                forget_variable(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);    
                
                registers[reg[0]].index=codeProgram->commands[i].args[0];
                registers[reg[0]].index2=codeProgram->commands[i].args[1];
                registers[reg[0]].canBeFreed=1;
                registers[reg[0]].shouldBeSaved=1;
                
            } else{    
                reg[3]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
                reg[4]=give_me_filled_register(codeProgram->commands[i].args[4], codeProgram->commands[i].args[5]);    
                reg[1]=give_me_any_register();
                reg[2]=give_me_any_register(); 

                if(reg[3] != reg[4]){
                    labels[3]=labelCounter--;   //a <= b
                    labels[4]=labelCounter--;   //obliczenia
                    add_asm_command(ASM_COPY, reg[1],reg[3],NONE);   
                    add_asm_command(ASM_SUB, reg[1], reg[4],NONE);
                    add_asm_command(ASM_JZERO, reg[1], NONE, labels[3]);

                    add_asm_command(ASM_COPY, reg[1],reg[4],NONE); 
                    add_asm_command(ASM_COPY, reg[2],reg[3],NONE);   
                    add_asm_command(ASM_JUMP, NONE, NONE, labels[4]);

                    add_asm_command(ASM_LABEL,NONE,NONE, labels[3]);
                }

                add_asm_command(ASM_COPY, reg[1],reg[3],NONE); 
                add_asm_command(ASM_COPY, reg[2],reg[4],NONE);   

                registers[reg[3]].canBeFreed=1;
                registers[reg[4]].canBeFreed=1;

                reg[0]=give_me_any_register();

                if(reg[3] != reg[4])
                    add_asm_command(ASM_LABEL,NONE,NONE, labels[4]);

                add_asm_command(ASM_SUB,reg[0],reg[0],NONE);
                labels[0]=labelCounter--;       //JZERO
                labels[1]=labelCounter--;       //JODD
                labels[2]=labelCounter--;       //exit

                add_asm_command(ASM_LABEL,NONE,NONE, labels[0]);
                add_asm_command(ASM_JZERO,reg[1],NONE,labels[2]);

                add_asm_command(ASM_JODD,reg[1],NONE,labels[1]); 
                add_asm_command(ASM_ADD,reg[2],reg[2],NONE);
                add_asm_command(ASM_HALF,reg[1],NONE,NONE);
                add_asm_command(ASM_JUMP,NONE,NONE,labels[0]);

                add_asm_command(ASM_LABEL,NONE,NONE, labels[1]);
                add_asm_command(ASM_ADD,reg[0],reg[2],NONE);
                add_asm_command(ASM_ADD,reg[2],reg[2],NONE);
                add_asm_command(ASM_HALF,reg[1],NONE,NONE);
                add_asm_command(ASM_JUMP,NONE,NONE,labels[0]);

                add_asm_command(ASM_LABEL,NONE,NONE, labels[2]);


                forget_variable(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);    

                registers[reg[0]].index=codeProgram->commands[i].args[0];
                registers[reg[0]].index2=codeProgram->commands[i].args[1];
                registers[reg[0]].canBeFreed=1;
                registers[reg[0]].shouldBeSaved=1;

                registers[reg[1]].index=-1;
                registers[reg[1]].index2=-1;
                registers[reg[1]].free=1;

                registers[reg[2]].index=-1;
                registers[reg[2]].index2=-1;
                registers[reg[2]].free=1;
            }

            add_asm_comment("# KONIEC MNOZENIA");
            break;
                
        case CODE_DIV:
            add_asm_comment("# DZIELENIE");
            int copyDiv=-1;
            for(int j=1; j<8; j++){
                if(registers[j].command.type==CODE_DIV){
                    int good=1;
                    
                    for(int k=2; k<6; k++){
                        if(registers[j].command.args[k] != codeProgram->commands[i].args[k])
                            good=0;
                    }
                    
                    if(good){
                        copyDiv=j;
                    }
                }
            }
            if(copyDiv != -1){
                add_asm_comment("# DZIELENIE JUZ OBLICZONE");
                forget_variable(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);    

                registers[copyDiv].index=codeProgram->commands[i].args[0];
                registers[copyDiv].index2=codeProgram->commands[i].args[1];
                registers[copyDiv].canBeFreed=1;
                registers[copyDiv].shouldBeSaved=1;
            } else if(is_constant(codeProgram->commands[i].args[4]) 
               && mp_int_is_pow2(get_number(codeProgram->commands[i].args[4])) >= 0){
                
                add_asm_comment("# X/2^k");
                
                int pow=mp_int_is_pow2(get_number(codeProgram->commands[i].args[4]));
                
                
                reg[1]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
                reg[0]=give_me_any_register();
                
                add_asm_command(ASM_COPY, reg[0], reg[1], NONE);
                
                for(int i=0; i<pow; i++)
                    add_asm_command(ASM_HALF, reg[0], NONE, NONE);
                
                registers[reg[1]].canBeFreed=1;
                
                forget_variable(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);    
                
                registers[reg[0]].index=codeProgram->commands[i].args[0];
                registers[reg[0]].index2=codeProgram->commands[i].args[1];
                registers[reg[0]].canBeFreed=1;
                registers[reg[0]].shouldBeSaved=1;
            } else{
                reg[5]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
  /*C*/         reg[1]=give_me_filled_register(codeProgram->commands[i].args[4], codeProgram->commands[i].args[5]);  
  /*B*/         reg[0]=give_me_any_register(); 
  /*D*/         reg[2]=give_me_any_register();  
  /*E*/         reg[3]=give_me_any_register();
  /*F*/         reg[4]=give_me_any_register();

                
                labels[0]=labelCounter--;   //while1 (5)
                labels[1]=labelCounter--;   //robienie (9)
                labels[2]=labelCounter--;   //wyjscie (12)
                labels[3]=labelCounter--;   //update (21)
                labels[4]=labelCounter--;   //exit (21)
                labels[5]=labelCounter--;   //div 0
                labels[6]=labelCounter--;   //div 1
                labels[7]=labelCounter--;   //div 2

                add_asm_command(ASM_JZERO, reg[1], NONE, labels[5]);
                
                
                add_asm_command(ASM_DEC,reg[1], NONE, NONE);
                add_asm_command(ASM_JZERO,reg[1],NONE,labels[6]);
                
                add_asm_command(ASM_DEC,reg[1], NONE, NONE);
                add_asm_command(ASM_JZERO,reg[1],NONE,labels[7]);
                
                add_asm_command(ASM_INC,reg[1], NONE, NONE);
                add_asm_command(ASM_INC,reg[1], NONE, NONE);
                
                
                add_asm_command(ASM_SUB,reg[3],reg[3],NONE); 

                add_asm_command(ASM_SUB,reg[2],reg[2],NONE);
                add_asm_command(ASM_INC,reg[2],NONE, NONE);    

                add_asm_command(ASM_COPY, reg[0],reg[5],NONE); 

                add_asm_command(ASM_LABEL,NONE,NONE, labels[0]);
                add_asm_command(ASM_COPY,reg[4],reg[1],NONE); 
                add_asm_command(ASM_SUB,reg[4],reg[0],NONE);    
                add_asm_command(ASM_JZERO, reg[4],NONE, labels[1]); 
                add_asm_command(ASM_JUMP,NONE,NONE, labels[2]);
                    add_asm_command(ASM_LABEL,NONE,NONE, labels[1]);   
                    add_asm_command(ASM_ADD,reg[1],reg[1],NONE);    
                    add_asm_command(ASM_ADD,reg[2],reg[2],NONE);  
                    add_asm_command(ASM_JUMP,NONE,NONE, labels[0]);

                add_asm_command(ASM_LABEL,NONE,NONE, labels[2]);  
                add_asm_command(ASM_DEC,reg[2],NONE, NONE);  
                add_asm_command(ASM_JZERO,reg[2],NONE, labels[4]);   
                add_asm_command(ASM_INC,reg[2],NONE, NONE);  

                add_asm_command(ASM_HALF,reg[1],NONE, NONE);     
                add_asm_command(ASM_HALF,reg[2],NONE, NONE);   

                add_asm_command(ASM_COPY,reg[4],reg[1],NONE); 
                add_asm_command(ASM_SUB,reg[4],reg[0],NONE);  
                add_asm_command(ASM_JZERO,reg[4],NONE, labels[3]);       
                add_asm_command(ASM_JUMP,NONE,NONE, labels[2]);    
                    add_asm_command(ASM_LABEL,NONE,NONE, labels[3]);   
                    add_asm_command(ASM_SUB,reg[0],reg[1],NONE);  
                    add_asm_command(ASM_ADD,reg[3],reg[2],NONE);  
                    add_asm_command(ASM_JUMP,NONE,NONE, labels[2]); 


                add_asm_command(ASM_LABEL,NONE,NONE, labels[5]);        //div 0
                add_asm_command(ASM_SUB,reg[3],reg[3],NONE); 
                add_asm_command(ASM_SUB,reg[0],reg[0],NONE);
                add_asm_command(ASM_JUMP,NONE,NONE, labels[4]); 

                
                add_asm_command(ASM_LABEL,NONE,NONE, labels[6]);        //div 1
                add_asm_command(ASM_INC,reg[1], NONE, NONE);
                add_asm_command(ASM_COPY,reg[3],reg[5],NONE); 
                add_asm_command(ASM_SUB,reg[0],reg[0],NONE);
                add_asm_command(ASM_JUMP,NONE,NONE, labels[4]); 
                
                
                add_asm_command(ASM_LABEL,NONE,NONE, labels[7]);        //div 2
                add_asm_command(ASM_INC,reg[1], NONE, NONE);
                add_asm_command(ASM_INC,reg[1], NONE, NONE);
                
                add_asm_command(ASM_COPY,reg[3],reg[5],NONE); 
                add_asm_command(ASM_HALF,reg[3],NONE,NONE);
                
                labels[8]=labelCounter--;   //nieparzysta        
                
                add_asm_command(ASM_SUB, reg[0], reg[0], NONE);
                
                add_asm_command(ASM_JODD, reg[5], NONE, labels[8]);
                add_asm_command(ASM_JUMP, NONE, NONE, labels[4]);

                add_asm_command(ASM_LABEL,NONE, NONE, labels[8]);
                add_asm_command(ASM_INC, reg[0], NONE, NONE);
                                
                
                add_asm_command(ASM_LABEL,NONE,NONE, labels[4]);       


                forget_variable(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);    

                registers[reg[3]].index=codeProgram->commands[i].args[0];
                registers[reg[3]].index2=codeProgram->commands[i].args[1];
                registers[reg[3]].canBeFreed=1;
                registers[reg[3]].shouldBeSaved=1;
  

                registers[reg[0]].index=-1;
                registers[reg[0]].index2=-1;
                if( (   codeProgram->commands[i].args[0] != codeProgram->commands[i].args[2] 
                        || codeProgram->commands[i].args[1] != codeProgram->commands[i].args[3]) 
                    && (codeProgram->commands[i].args[0] != codeProgram->commands[i].args[4]
                        || codeProgram->commands[i].args[1] != codeProgram->commands[i].args[5])){
                    
                    registers[reg[0]].command.type=CODE_MOD;
                    for(int j=0; j<6; j++)
                        registers[reg[0]].command.args[j]=codeProgram->commands[i].args[j];
                    
                    registers[reg[0]].canBeFreed=1;
                } else {
                    registers[reg[0]].free=1;   
                }
                

                registers[reg[1]].canBeFreed=1; 
                //registers[reg[1]].value=<wartosc samej siebie z poczatku> 

                registers[reg[2]].index=-1;
                registers[reg[2]].index2=-1;
                registers[reg[2]].free=1;
                //registers[reg[2]].value=0;   

                registers[reg[4]].index=-1; 
                registers[reg[4]].index2=-1;
                registers[reg[4]].free=1;   

                registers[reg[5]].canBeFreed=1;
            }
            add_asm_comment("# KONIEC DZIELENIA");
                
            break;
                
                
        case CODE_MOD:
            add_asm_comment("# MODULO");
            int copyMod=-1;
            for(int j=1; j<8; j++){
                if(registers[j].command.type==CODE_MOD){
                    int good=1;
                    
                    for(int k=2; k<6; k++){
                        if(registers[j].command.args[k] != codeProgram->commands[i].args[k])
                            good=0;
                    }
                    
                    if(good){
                        copyMod=j;
                    }
                }
            }
            if(copyMod != -1){
                add_asm_comment("# MODULO JUZ OBLICZONE");
                
                forget_variable(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);    

                registers[copyMod].index=codeProgram->commands[i].args[0];
                registers[copyMod].index2=codeProgram->commands[i].args[1];
                registers[copyMod].canBeFreed=1;
                registers[copyMod].shouldBeSaved=1;
            } else if(is_constant(codeProgram->commands[i].args[4]) 
               && mp_int_is_pow2(get_number(codeProgram->commands[i].args[4])) >= 0){
                
                add_asm_comment("# X % 2");
                
                int pow=mp_int_is_pow2(get_number(codeProgram->commands[i].args[4]));
                
                reg[1]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);                
                reg[0]=give_me_any_register();
               
                
                add_asm_command(ASM_SUB, reg[0], reg[0], NONE);
                
                if(pow>0){
                    reg[2]=give_me_any_register();
                    reg[3]=give_me_any_register();
                    add_asm_command(ASM_COPY, reg[2], reg[1], NONE);   
                    add_asm_command(ASM_SUB, reg[3], reg[3], NONE);  
                    add_asm_command(ASM_INC, reg[3], NONE, NONE);   
                }
                
                for(int i=0; i<pow; i++){
                    labels[0]=labelCounter--;   //nieparzysta
                    labels[1]=labelCounter--;   //koniec          
                
                    add_asm_command(ASM_JODD, reg[2], NONE, labels[0]);
                    add_asm_command(ASM_JUMP, NONE, NONE, labels[1]);

                    add_asm_command(ASM_LABEL,NONE, NONE, labels[0]);
                    add_asm_command(ASM_ADD, reg[0], reg[3], NONE);

                    add_asm_command(ASM_LABEL,NONE, NONE, labels[1]);
                    
                    add_asm_command(ASM_HALF, reg[2], NONE, NONE);
                    add_asm_command(ASM_ADD, reg[3], reg[3], NONE);
                }
                
                registers[reg[1]].canBeFreed=1;
                
                if(pow>0){
                    registers[reg[2]].index=-1;
                    registers[reg[2]].index2=-1;
                    registers[reg[2]].free=1;
                    
                    registers[reg[3]].index=-1;
                    registers[reg[3]].index2=-1;
                    registers[reg[3]].free=1;
                }
                
                forget_variable(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);   
                
                registers[reg[0]].index=codeProgram->commands[i].args[0];
                registers[reg[0]].index2=codeProgram->commands[i].args[1];
                registers[reg[0]].canBeFreed=1;
                registers[reg[0]].shouldBeSaved=1;
            } else{
                reg[5]=give_me_filled_register(codeProgram->commands[i].args[2], codeProgram->commands[i].args[3]);
    /*C*/       reg[1]=give_me_filled_register(codeProgram->commands[i].args[4], codeProgram->commands[i].args[5]);  
    /*B*/       reg[0]=give_me_any_register();
    /*D*/       reg[2]=give_me_any_register();  
    /*E*/       reg[3]=give_me_any_register();
    /*F*/       reg[4]=give_me_any_register();


                labels[0]=labelCounter--;   //while1 (5)
                labels[1]=labelCounter--;   //robienie (9)
                labels[2]=labelCounter--;   //wyjscie (12)
                labels[3]=labelCounter--;   //update (21)
                labels[4]=labelCounter--;   //exit (21)
                labels[5]=labelCounter--;   //mod 0
                labels[6]=labelCounter--;   //mod 1
                labels[7]=labelCounter--;   //mod 2
                
                
                add_asm_command(ASM_JZERO, reg[1], NONE, labels[5]);
                        
                
                add_asm_command(ASM_DEC,reg[1], NONE, NONE);
                add_asm_command(ASM_JZERO,reg[1],NONE,labels[6]);
                
                add_asm_command(ASM_DEC,reg[1], NONE, NONE);
                add_asm_command(ASM_JZERO,reg[1],NONE,labels[7]);
                
                add_asm_command(ASM_INC,reg[1], NONE, NONE);
                add_asm_command(ASM_INC,reg[1], NONE, NONE);
                
                                    
                add_asm_command(ASM_SUB,reg[3],reg[3],NONE); 

                add_asm_command(ASM_SUB,reg[2],reg[2],NONE);
                add_asm_command(ASM_INC,reg[2],NONE, NONE);    

                add_asm_command(ASM_COPY, reg[0],reg[5],NONE); 

                add_asm_command(ASM_LABEL,NONE,NONE, labels[0]);
                add_asm_command(ASM_COPY,reg[4],reg[1],NONE); 
                add_asm_command(ASM_SUB,reg[4],reg[0],NONE);    
                add_asm_command(ASM_JZERO, reg[4],NONE, labels[1]); 
                add_asm_command(ASM_JUMP,NONE,NONE, labels[2]);
                    add_asm_command(ASM_LABEL,NONE,NONE, labels[1]);   
                    add_asm_command(ASM_ADD,reg[1],reg[1],NONE);    
                    add_asm_command(ASM_ADD,reg[2],reg[2],NONE);  
                    add_asm_command(ASM_JUMP,NONE,NONE, labels[0]);

                add_asm_command(ASM_LABEL,NONE,NONE, labels[2]);  
                add_asm_command(ASM_DEC,reg[2],NONE, NONE);  
                add_asm_command(ASM_JZERO,reg[2],NONE, labels[4]);   
                add_asm_command(ASM_INC,reg[2],NONE, NONE);  

                add_asm_command(ASM_HALF,reg[1],NONE, NONE);     
                add_asm_command(ASM_HALF,reg[2],NONE, NONE);   

                add_asm_command(ASM_COPY,reg[4],reg[1],NONE); 
                add_asm_command(ASM_SUB,reg[4],reg[0],NONE);  
                add_asm_command(ASM_JZERO,reg[4],NONE, labels[3]);       
                add_asm_command(ASM_JUMP,NONE,NONE, labels[2]);    
                    add_asm_command(ASM_LABEL,NONE,NONE, labels[3]);   
                    add_asm_command(ASM_SUB,reg[0],reg[1],NONE);  
                    add_asm_command(ASM_ADD,reg[3],reg[2],NONE);  
                    add_asm_command(ASM_JUMP,NONE,NONE, labels[2]); 

                
                add_asm_command(ASM_LABEL,NONE,NONE, labels[5]);        //mod 0
                add_asm_command(ASM_SUB,reg[3],reg[3],NONE); 
                add_asm_command(ASM_SUB,reg[0],reg[0],NONE);
                add_asm_command(ASM_JUMP,NONE,NONE, labels[4]); 

                
                add_asm_command(ASM_LABEL,NONE,NONE, labels[6]);        //mod 1
                add_asm_command(ASM_INC,reg[1], NONE, NONE);
                add_asm_command(ASM_SUB,reg[3],reg[3],NONE); 
                add_asm_command(ASM_SUB,reg[0],reg[0],NONE);
                add_asm_command(ASM_JUMP,NONE,NONE, labels[4]); 
                
                
                add_asm_command(ASM_LABEL,NONE,NONE, labels[7]);        //mod 2
                add_asm_command(ASM_INC,reg[1], NONE, NONE);
                add_asm_command(ASM_INC,reg[1], NONE, NONE);
                
                add_asm_command(ASM_COPY,reg[3],reg[5],NONE); 
                add_asm_command(ASM_HALF,reg[3],NONE,NONE);
                
                labels[8]=labelCounter--;   //nieparzysta        
                
                add_asm_command(ASM_SUB, reg[0], reg[0], NONE);
                
                add_asm_command(ASM_JODD, reg[5], NONE, labels[8]);
                add_asm_command(ASM_JUMP, NONE, NONE, labels[4]);

                add_asm_command(ASM_LABEL,NONE, NONE, labels[8]);
                add_asm_command(ASM_INC, reg[0], NONE, NONE);

                
                add_asm_command(ASM_LABEL,NONE,NONE, labels[4]);       

                forget_variable(codeProgram->commands[i].args[0], codeProgram->commands[i].args[1]);    

                registers[reg[0]].index=codeProgram->commands[i].args[0];
                registers[reg[0]].index2=codeProgram->commands[i].args[1];
                registers[reg[0]].canBeFreed=1;
                registers[reg[0]].shouldBeSaved=1;  

                registers[reg[3]].index=-1;
                registers[reg[3]].index2=-1;
                if(     (codeProgram->commands[i].args[0] != codeProgram->commands[i].args[2] 
                        || codeProgram->commands[i].args[1] != codeProgram->commands[i].args[3]) 
                    && (codeProgram->commands[i].args[0] != codeProgram->commands[i].args[4]
                        || codeProgram->commands[i].args[1] != codeProgram->commands[i].args[5])){
                    
                    registers[reg[3]].command.type=CODE_DIV;
                    for(int j=0; j<6; j++)
                        registers[reg[3]].command.args[j]=codeProgram->commands[i].args[j];
                    
                    registers[reg[3]].canBeFreed=1;
                } else {
                    registers[reg[3]].free=1;
                }

                registers[reg[1]].canBeFreed=1; 
                //registers[reg[1]].value=<wartosc samej siebie z poczatku> 

                registers[reg[2]].index=-1;
                registers[reg[2]].index2=-1;
                registers[reg[2]].free=1;
                //registers[reg[2]].value=0;   

                registers[reg[4]].index=-1; 
                registers[reg[4]].index2=-1;
                registers[reg[4]].free=1;   

                registers[reg[5]].canBeFreed=1;
            }
            add_asm_comment("# KONIEC MODULO");
                
            break;
         
        case CODE_DELETED:
            break;
                
        default: 
            fprintf(stderr,"(?!) NIEZNANA KOMENDA %d (?!)\n", codeProgram->commands[i].type);
        }
        
    }
    
    add_asm_command(ASM_HALT,NONE,NONE,NONE);  //Halte die klappe he he
}


void print_assembler(){
    printf("\nASM:\n");
    for(int i=0; i<assembler->size; i++){
        printf("(%d) Command type = %d (%s), reg1=%d, reg2=%d, jmp=%d\n", assembler->commands[i].noumber, assembler->commands[i].type, asmCommandNames[assembler->commands[i].type], assembler->commands[i].reg1, assembler->commands[i].reg2, assembler->commands[i].jumpDestination);
    }
}

const char regNames[][2] = {
    "A",
    "B",
    "C",
    "D",
    "E",
    "F",
    "G",
    "H"
};


void print_real_assembler(FILE* out, int comments){
    for(int i=0; i<assembler->size; i++){
        if(assembler->commands[i].type == ASM_COMMENT){
            if(comments)
                fprintf(out, "%s\n", assembler->commands[i].comment);
        } else {
            if(assembler->commands[i].type == ASM_LABEL){
                if(comments)
                    fprintf(out, "# (%d) ",assembler->commands[i].noumber);
                else
                   continue;
            }
            fprintf(out,"%s",asmCommandNames[assembler->commands[i].type]);
            
            if(assembler->commands[i].reg1 >= 0)
                fprintf(out," %s",regNames[assembler->commands[i].reg1]);
            
            if(assembler->commands[i].reg2 >= 0)
                fprintf(out," %s",regNames[assembler->commands[i].reg2]);      
            
            if(assembler->commands[i].jumpDestination != -1)
                fprintf(out," %d",assembler->commands[i].jumpDestination);
            
            fprintf(out,"\n");
        }
    }
}
