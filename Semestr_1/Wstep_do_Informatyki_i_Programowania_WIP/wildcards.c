#include <stdio.h>
#include <string.h>
#include <stdbool.h>

bool only_stars(char* s){										//funkcja, ktora sprawdza, czy string sklada sie z samych gwiazdek
    int i;

    for(i=0;i<strlen(s);i++)
        if (s[i]!='*')
            return false;

    return true;
}

bool match(char* wzorzec, char* lancuch){						//funkcja porownawcza
    int wzorzec_length=strlen(wzorzec);							
    int lancuch_length=strlen(lancuch);						

    if (wzorzec_length==0 && lancuch_length==0)
        return true;

    if (wzorzec_length==0 && lancuch_length>0)
        return false;											//wyjscia z rekurencji - oczywiste

    if (only_stars(wzorzec))
        return true;

    if (lancuch_length==0)
        return false;

    if (wzorzec[0]!='*'){												//gdy pierwszy znak wzorca to nie gwiazdka
        if (wzorzec[0]==lancuch[0] || wzorzec[0]=='?'){					//gdy pierwsze litery wzorca i lancucha sa zgadne
            char s1[wzorzec_length];
            char s2[lancuch_length];
            int i;

            for (i=1;i<wzorzec_length;i++)								//skraca wzorzec o jeden znak z lewej
                s1[i-1]=wzorzec[i];										//i umieszcza go w s1
            for (i=1;i<lancuch_length;i++)								//skraca lancuch o jeden znak z lewej
                s2[i-1]=lancuch[i];										//i umieszcza go w s2
			
            s1[wzorzec_length-1]='\0';
            s2[lancuch_length-1]='\0';

            return match(s1,s2);										//zwraca porownanie obcietych stringow
        }
        else
            return false;
    }else{																//gdy pierwszy znak to gwiazdka
        bool b1;
        bool b2;
        int i;

        char s1[wzorzec_length];
        for (i=1;i<wzorzec_length;i++)									//skraca wzorzec o jeden znak z lewej 
            s1[i-1]=wzorzec[i];											//i wklada do s1
        s1[wzorzec_length-1]='\0';

        char s2[lancuch_length];
        for (i=1;i<lancuch_length;i++)									//skraca lancuch o jeden znak z lewej
            s2[i-1]=lancuch[i];											//i wklada do s2
        s2[lancuch_length-1]='\0';

        b1=match(s1,lancuch);											//porownanie skroconego wzorca z lancuchem
        b2=match(wzorzec,s2);											//porownanie wzorca z skroconym lancuchem

        if (b1||b2)														//albo ciag znakow nalezacych do gwiazdki sie skonczyl,
            return true;												//albo nie, wiec alternatywa na dwie drogi
        else
            return false;
    }

}
int main(){
    char s1[50];
    char s2[50];

    while (1){
        scanf("%s",s1);
        scanf("%s",s2);
        if (match(s1,s2))
            printf("Tak.\n\n");
        else
            printf("Nie.\n\n");
    }
    return 0;
}
