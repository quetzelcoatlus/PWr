//gcc -Wall -Wextra -std=c99 -pedantic 1.c -o main -lm


#include <stdio.h>
#include <math.h>
#include <string.h>

int main(int argc, char** argv){
    if (argc < 2)
        return 1;
    
    FILE* f = fopen(argv[1], "r");
    
    unsigned long int countAll = 0;
    unsigned int count[256];
    unsigned int count2[256][256];
    for(int i=0; i<256; i++){
        count[i]=0;
        for(int j=0; j<256; j++)
            count2[i][j]=0;
    }
    
    int c;
    int prev = 0;
    
    while ((c = getc(f)) != EOF) {
        countAll++;
        count[c]++;
        count2[prev][c]++;
        prev=c;
    }
    
    fclose(f);
    
    double entropy = 0, entropy2 = 0;
    for (int i=0; i<256; i++){
        if(count[i]>0){
            double probability  =  (double) count[i] / (double) countAll;
            entropy += probability*log2(probability);
            
            double h = 0;
            for(int j=0; j<256; j++){
                if(count2[i][j] > 0){
                    double probability2 =  (double) count2[i][j] / (double) count[i];
                    h += probability2*log2(probability2);
                }
            }
            entropy2 += probability * (-1.0) * h;
        }
    }
    if(entropy != 0)
        entropy *= -1;

    printf("entopia = %f\nentropia warunkowa = %f\n", entropy,entropy2);
    
    if (argc > 2 && !strcmp(argv[2], "-r")){

        f = fopen("results", "w");

        fprintf(f,"Dane dla pliku %s: \n\nLiczba znakow: %lu\n\nWystapienia pojedynczych znakow:\n",argv[1],countAll);
        for (int i=0; i< 256; i++)
            fprintf(f, "%d (%c) = %d razy = %0.5f = %0.3f%%\n", i, i, count[i], (double) count[i] / (double) countAll, 100.0* (double) count[i] / (double) countAll);

        fprintf(f,"\nWystapienia par znakow:\n");
        for(int i=0; i<256;i++)
            for(int j=0; j<256;j++)
                fprintf(f,"%d (%c) przed znakiem %d (%c) = %d razy = %0.5f = %0.3f%%\n",i, i, j, j, count2[i][j], (double) count2[i][j] / (double) countAll, 100.0* (double) count2[i][j] / (double) countAll);

        fclose(f);
    }
    return 0;
}