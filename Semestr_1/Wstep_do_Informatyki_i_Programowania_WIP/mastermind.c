#include <stdio.h>
#include <stdbool.h>

#define CODE_LENGTH 4
#define COLORS 6

struct array{          								//tablica na kody
    int tab[CODE_LENGTH];
};

bool maybe[CODE_LENGTH][COLORS+1];       			//tablica mowiaca, czy dany kolor [j](1-6) moze byc na danym miejscu [i](0-4)
int sure[CODE_LENGTH];            					//tablica z 100% pewnymi kolorami na danych miejscach [i](0-4)
int queue[CODE_LENGTH+1];           				//tablica kolejki do sprawdzania koloru - 5 pól, zeby zgarnial 0 z koñca przy usuwaniu elementu
int sum=0;              							//suma bia³ych i czarnych kolkow w danym ruchu


void load(struct array x, int *w, int *b){
	for (int i=0;i<CODE_LENGTH;i++)
		printf("[%d] ",x.tab[i]);
	printf("?\n");
	printf("white: ");
	scanf("%d",w);                                                            //dla danej tablicy x[4] wypisuje ja i pobiera od
	printf("black: ");                                                        //uzytkownika ilosc bialych i czarnych kolkow
	scanf("%d",b);
	printf("\n");
}

void choose(struct array *x,int *place, int putme, int testme){         	//procedura, ktora wytwarza tablice x[4] do wypisania
    int i;
    for(i=0;i<CODE_LENGTH;i++)                                        		//zerowanie tablicy
        (*x).tab[i]=-1;

    for (i=0;i<CODE_LENGTH;i++)                                       		//wkladanie elementow poprawnych (sure[i])
       if (sure[i]!=0){
           (*x).tab[i]=sure[i];
       }

    for (i=0;i<CODE_LENGTH;i++)                                       		//wkladanie elementu obecnie sprawdzanego (putme)
        if(sure[i]==0 && maybe[i][putme]){
            *place=i;
            (*x).tab[i]=putme;
            break;
        }

    for (i=0;i<CODE_LENGTH;i++)                                      	 //wypelnianie elementami testujacymi, probnymi (testme)
        if((*x).tab[i]==-1)
            (*x).tab[i]=testme;
}

void addToQueue(int x){                                 //procedura dodajaca kolor na koniec kolejki sprawdzania
    int i;	
    for(i=0;i<CODE_LENGTH;i++)
        if(queue[i]==0){
            queue[i]=x;
            return;
        }
}

void deleteFromQueue(int x){                            //procedura usuwajaca dany element z kolejki
    int i,j;

    for(i=0;i<CODE_LENGTH;i++)
        if(queue[i]==x){                                //znajduje pierwszy
            for(j=i;j<CODE_LENGTH;j++)
                queue[j]=queue[j+1];                    //przesuwa o jeden w lewo niszczac element, ktory usuwamy
            return;
        }
}

int sures(){                                             //funkcja, ktora zlicza ilosc pewnych (trafionych) kolorow w sure[i]
    int i;
    int x=0;

    for(i=0;i<CODE_LENGTH;i++)
        if(sure[i]!=0)
            x++;
    return x;
}

bool eliminate(struct array x,int w,int b,int place, int* putme, int* testme){      //najwazniejsza funkcja, zmienia tablice maybe[i][j]
    int i,j;                                                                        //analizuje kolki, i robi podwaliny pod choose()

    if((w+b>CODE_LENGTH)||(b<sures())||(w>2))                           //gracz oszukal
        return true;

    if(w==0 && b==CODE_LENGTH)                    //zgadl
        return false;

    if (b==0)                           //jezeli nic trafionego, to falszuje wszystkie maybe[i][j]
        for(i=0;i<CODE_LENGTH;i++)
            maybe[i][x.tab[i]]=false;

    if (w+b>sum)                        //zauwaza, ze pojawil sie nowy element, wiec dodaje kolor do kolejki tyle razy, ile trzeba
        for(i=1;i<=w+b-sum;i++)
            addToQueue(*testme);


    if (w==2){                           //dwa biale kolki
        sure[place]=*testme;             //element testowany,probny wchodzi na miejsce elementu sprawdzanego (tak musi byc)
        deleteFromQueue(*testme);        //jedna pozycja pewna
        maybe[place][*putme]=false;
    }
    else if (w==1){                      //jeden bialy kolek
        maybe[place][*putme]=false;
        if (w+b>sum)
            maybe[place][*testme]=false;
    }
    else if ((w==0) && (b>0) && (*putme!=0)){    //same czarne
        sure[place]=x.tab[place];                //element sprwadzany jest na dobrym miejscu
        deleteFromQueue(*putme);                 //usuwamy go z kolejki
        maybe[place][*testme]=false;

    }

     for(i=0;i<CODE_LENGTH;i++)							 //petla, ktora sprawdza, czy przypadkiem nie jest tak,
	 	if (sure[i]==0){                     	//ze tylko jeden kolor pasuje na dane pole.
       		int k=0;							//nie musi sprawdzac, gdy pole juz jest zajete                            
        	int l=0;
        	for(j=1;j<COLORS+1;j++)
	            if(maybe[i][j]){
	                k++;
	                l=j;
	            }
        	if (k==1){                          //jezeli tak jest, to wklepuje go do sure[i]
	            sure[i]=l;
	            deleteFromQueue(l);             //i usuwa z kolejki
        	}
    	}

    if (w+b>sum||sures()>0){                //gdy doszly jakies nowe elementy //albo w tej rundzie cos wskoczylo na miejsce i jest nowy putme,
        sum=sures()+1;                      //trzeba zmienic sum odpowiednio, zeby potem dobrze wykrywal
    }

    *putme=queue[0];                        //element sprawdzany to pierwszy w kolejce

    if (*testme<COLORS)
        *testme=*testme+1;                  //nastepny element testowany (+1)
    else{                                   //lub
        *testme=queue[1];                   //drugi element z kolejki
    }

    return false;                            //ze dobrze
}

void guess(){                                               //Procedura nadzorujaca przebieg gry.
    struct array x;       									//zerowanie zmiennych:
    for (int i=0;i<CODE_LENGTH;i++)
		x.tab[i]=-1;              							//tablica na kody
    int place=0;                                            //miejsce, w ktorym sprawdzamy element sprawdzany
    int putme=0;                                            //element sprawdzany
    int testme=1;                                           //element testowany
    int w=0;                                                //biale kolki
    int b=0;                                                //czarne kolki

    do{
        choose(&x,&place,putme,testme);                     //wytwarza tablice
       	for (int i=0;i<CODE_LENGTH;i++)
			if (x.tab[i]<1){
            	printf("You lied to me! </3 \n");
            	return;
        	}
        load(x,&w,&b);                                      //pobiera od gracza ilosci kolkow
        if (eliminate(x,w,b,place,&putme,&testme)){         //jezeli nie ma bledu, to sobie liczy pod nastepny krok
            printf("You lied to me! </3 \n");
            return;
        }
    } while (w!=0 || b!=CODE_LENGTH);                                 //zapetla dopoki nie zgadnie

    printf("I win! <3\n");                                    //cieszy sie z wygranej
}

int main(){
	int i,j;
	queue[CODE_LENGTH]=0;
	for(i=0;i<CODE_LENGTH;i++){
        sure[i]=0;
        queue[i]=0;                                         //zerowanie zmiennych globalnych
        maybe[i][0]=false;
        for(j=1;j<COLORS+1;j++){
            maybe[i][j]=true;
        }
	}

    guess();                                                //uruchomienie procedury guess
	return 0;
}
