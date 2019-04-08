/*
    Nazwa: main.h
    Opis: Funkcja main() programu wywołująca kolejne kroki kompilacji
    Autor: Jan Brzeżański
    Indeks: 236814
*/

#include <stdio.h>
#include <string.h>

#include "comp_lib.h"

int yyparse();
extern FILE* yyin;


int main(int argc, char** argv){  
    FILE* out;
    
    if(argc > 2){
        yyin = fopen(argv[1], "r");
        out = fopen(argv[2], "w");
    } else {
        printf("Usage:\n\t%s <in> <out>\n",argv[0]);
        return 1;
    }
    
    int print=0;
    int comments=0;
    int printAssembler=0;
    int printOptimalized=0;
    int printEndMessage=1;
    
    for(int i=3; i<argc; i++){
        if(!strcmp(argv[i],"-p"))
           print=1;
        else if(!strcmp(argv[i],"-c"))
           comments=1;
        else if(!strcmp(argv[i],"-a"))
           printAssembler=1;
        else if(!strcmp(argv[i],"-o"))
           printOptimalized=1;
        else if(!strcmp(argv[i],"-m"))
           printEndMessage=0;
    }
    
    //////////////////////// LEKSER, PARSER, TABLICA SYMBOLI, DRZEWO WYPROWADZENIA /////////////////
    
    init_table(); 
    yyparse();                      //parser (generowanie drzewa wyprowadzenia)
    fclose(yyin);                   
    set_memories();
    if(print) print_symbols();
    if(print) print_tree();
    cut_tree();
    if(printOptimalized) print_tree();
    
    
    //////////////////////// KOD TROJADRESOWY /////////////////
    
    init_code();  
    transform_tree_to_code();               //translacja drzewa w kod "trojadresowy"
    free_tree();
    check_code();
    enumerate_blocks();
    if(print) print_code();
    optimize_code();
    if(printOptimalized) print_symbols();
    if(printOptimalized) print_code();
    
    
    //////////////////////// ASEMBLER /////////////////
    
    init_assembler();
    init_registers();
    
    transform_code_to_asm();                //translacja kodu w asembler
    
    free_code();
    free_table();
    free_registers();
        
    enumerate_assembler();                  //ponumerowanie komend
    if(printAssembler) print_assembler();   
    resolve_jumps();                        //zastapienie skokow do labeli numerami komend
    print_real_assembler(out, comments);    //wypisanie ostatecznego kodu do pliku wyjsciowego. 1 -> z komentarzami, 0 -> bez
    free_assembler();
    fclose(out);
    
    if(printEndMessage) fprintf(stderr,"Kompilacja %s do %s zakonczona sukcesem!\n", argv[1], argv[2]);
    
    return 0;
}
