/*
    Nazwa: comp_lib.h
    Opis: Plik nagłówkowy biblioteki zawierającej implementacje wszystkich funkcji programu 
    Autor: Jan Brzeżański
    Indeks: 236814
*/

#ifndef COMP_LIB_H
#define COMP_LIB_H

#include "imath.h"


///////////////////////////////////////////// TABLICA SYMBOLI ////////////////////////////////////////

enum SymbolType{
    VARIABLE,
    ARRAY,
    ITERATOR,
    NUMBER,
    UNKNOWN
};

struct Symbol{
    enum SymbolType type;   
    char* name;
    mp_int off;
    mp_int mem;
    mp_int size;
    mp_int val;
    mp_int ret;
};

struct SymbolTable{
    int size;
    int maxSize;
    struct Symbol* symbols;
};

//lib
int resize_table();

int is_constant(int pos);
int is_variable(int pos);
int is_array(int pos);

int add_number2(mp_int num);

mp_int get_number(int pos);
mp_int get_memory_of_variable(int pos);
mp_int get_start_of_array(int pos);
mp_int get_memory_of_array(int pos, mp_int index);

void set_memory_variable(int pos);
void set_memory_array(int pos);


//lexer
int check_symbol(char* name);
int check_declared(char* name);
int check_variable(char* name);

int add_symbol(char* name);
int add_iterator(char* name);
int add_number(char* number);

//parser
int set_variable(int pos);
int set_array(int pos, int start, int end);

//main
void set_memories();

void free_table();
void init_table();

void print_symbols();


///////////////////////////////////////////// DRZEWO WYPROWADZENIA ////////////////////////////////////////

enum CommandType{
    COM_PROGRAM,        //0
    COM_COMMANDS,       //1
        
    COM_IS,             //2
    
    COM_NUM,            //3
    COM_ARR,            //4
    COM_PID,            //5
    
    COM_ADD,            //6
    COM_SUB,            //7
    COM_MUL,            //8
    COM_DIV,            //9
    COM_MOD,            //10
    
    COM_IF,             //11
    COM_IFELSE,         //12
    
    COM_EQ,             //13
    COM_NEQ,            //14
    COM_LT,             //15
    COM_GT,             //16
    COM_LEQ,            //17
    COM_GEQ,            //18
    
    COM_WHILE,          //19
    COM_DO,             //20
    
    COM_FOR,            //21
    COM_FORDOWN,        //22
    
    COM_READ,           //23
    COM_WRITE           //24
    
};

struct Command{
    enum CommandType type;
    int index;
        
    int size;
    int maxSize;
    struct Command** commands;
};


//lib
int find_write(struct Command* c);
void print_r(struct Command* c);
void free_tree_r(struct Command* c);
int resize_command(struct Command* c);

//parser
struct Command* create_value_command(enum CommandType type, int index);
struct Command* create_parent_command(enum CommandType type, int kids, ...);
struct Command* create_empty_command(enum CommandType type);
struct Command* add_command(struct Command* parent, struct Command* kid);
void create_program(struct Command* commands);

//main
void cut_tree();
void free_tree();
void print_tree();


///////////////////////////////////////////// KOD POSREDNI ////////////////////////////////////////

enum CodeType{
    CODE_UNKNOWN,   //0
    CODE_DELETED,   //1
    
    
    CODE_LABEL,     //2
    
    CODE_READ,      //3
    CODE_WRITE,     //4
    
    CODE_HALF,      //5
    CODE_INC,       //6
    CODE_DEC,       //7
      
    
    CODE_COPY,      //8
    
    CODE_ADD,       //9
    CODE_SUB,       //10
    
    CODE_MUL,       //11
    CODE_DIV,       //12
    CODE_MOD,       //13
        
    CODE_JUMP,      //14
    CODE_JNEQ,      //15
    CODE_JEQ,       //16
    CODE_JGEQ,      //17
    CODE_JLEQ,      //18
    CODE_JGT,       //19   
    CODE_JLT,       //20
    CODE_JZERO      //21
};

struct CodeCommand{
    enum CodeType type;
    int size;
    int args[6];
    int block;
};

struct CodeProgram{
    int size;
    int maxSize;
    struct CodeCommand* commands;
};


//lib
int resize_code();
void transform_tree_r(struct Command* c);

void add_code_command(enum CodeType type);
void add_label(int label);

void set_type_of_current_command(enum CodeType type);
void add_arg_to_current_command(int arg);
void change_jump_destination_at_current_command(int arg);
void switch_condition_at_current_command();
int bad_inequality();

//main
void init_code();
void free_code();

void print_code();

void transform_tree_to_code();

void enumerate_blocks();
void optimize_code();

int find_error_symbol(int* symbols, int size, int symbol);
void add_error_symbol(int* symbols, int* size, int symbol);
void check_code();


///////////////////////////////////////////// REJESTR ////////////////////////////////////////

struct Register{
    signed char free;           //czy rejestr jest obecnie uzywany
    signed char canBeFreed;     //czy mozna go zwolnic
    signed char shouldBeSaved;
    
    int index;                      //zwiazek z zmienna
    int index2;                     //el. tablicy
    struct CodeCommand command;     //zwiazek z dzialaniem - div/mod
    
    mp_int value;  
};

//lib
void set_register(int reg, mp_int pos);
void set_a_register(mp_int pos);

int give_me_free_register();
int give_me_free_register_or_easy_store();
int give_me_filled_register(int index, int index2);

int store_register(int reg);

void reset_registers();
void forget_variable(int index, int index2);

//main
void init_registers();
void free_registers();


///////////////////////////////////////////// ASEMBLER ////////////////////////////////////////

enum AssemblerCommandType{
    ASM_GET,        //0
    ASM_PUT,        //1

    ASM_LOAD,       //2
    ASM_STORE,      //3

    ASM_COPY,       //4
    ASM_ADD,        //5
    ASM_SUB,        //6

    ASM_HALF,       //7
    ASM_INC,        //8
    ASM_DEC,        //9

    ASM_JUMP,       //10
    ASM_JZERO,      //11
    ASM_JODD,       //12

    ASM_HALT,       //13
    
    ASM_LABEL,      //14
    ASM_COMMENT     //15
};
 
enum AssemblerRegisterType{ //prawie-najmniej uzywany enum swiata
    REG_A,      //0
    REG_B,      //1
    REG_C,      //2
    REG_D,      //3
    REG_E,      //4
    REG_F,      //5
    REG_G,      //6
    REG_H       //7
};


struct AssemblerCommand{
    enum AssemblerCommandType type;
    int reg1;
    int reg2;
    int jumpDestination;
    int noumber;           //tak, wiem, ze to watpliwa pisownia tego slowa
    char* comment;
};
 
struct AssemblerProgram{
    int size;
    int maxSize;
    struct AssemblerCommand* commands;
};


//lib
int resize_assembler();

void add_asm_command(enum AssemblerCommandType type, int reg1, int reg2, int dest);
void add_asm_comment(const char* com);

void generate_constant(int reg, mp_int cons);


//main
void init_assembler();
void free_assembler();

void transform_code_to_asm();

void enumerate_assembler();
void resolve_jumps();

void print_assembler();
void print_real_assembler(FILE* out, int comments);


#endif