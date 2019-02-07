#include <stdio.h>

int main(int argc, char** argv){
    if(argc < 3) return 1;
    
    FILE* in1 = fopen(argv[1], "r");
    FILE* in2 = fopen(argv[2], "r");
      
    int c1, c2, count=0;
    while((c1=fgetc(in1)) != EOF){
        if((c2=fgetc(in2)) == EOF)      //pobieramy 2 bajty
            break;
        
        if((c1 >> 4) != (c2 >> 4)) count++;         //testujemy rownosc czterech pierwszych bitow z kazdego...
        if((c1 & 0xF) != (c2 & 0xF)) count++;       //... oraz czterech ostatnich bitow. Zliczamy liczbe roznych czworek
    }
    
    fclose(in1);
    fclose(in2);
    
    printf("Liczba rozniacych sie blokow 4-bitowych to %d\n",count);
    
    return 0;
}