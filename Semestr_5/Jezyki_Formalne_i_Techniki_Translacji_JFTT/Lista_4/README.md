# O kompilatorze słów kilka

## Przedmowa
Witaj, drogi czytelniku. Jeśli tu dotarłeś, to albo interesuje Cię temat kompilatora z przedmiotu "Języki Formalne i Techniki Translacji" na Wydziale Podstawowych Problemów Techniki na Politechnice Wrocławskiej, albo przypadkiem dokopałeś się do tego folderu, chodząc po tym repozytorium. Tak czy siak, zapraszam Cię do lektury niniejszego *poradnika*, w którym postaram się opisać proces tworzenia kompilatora poprzez wszystkie fazy, zarówno od strony teoretycznej, jak i w oparciu o kod mojego kompilatora.
Pomysł, żeby stworzyć taki poradnik zrodził się już podczas pisania kompilatora, a stosunkowo dobry wynik w rankingu - drugie miejsce - utwierdził mnie w tym postawnowieniu. 

## Spis treści
1. [Analiza leksykalna](#1-analiza-leksykalna)  
  1.1. [Tablica symboli](#11-tablica-symboli)  
  1.2. [Lekser (Flex)](#12-lekser-flex)
2. [Analiza składniowa](#2-analiza-składniowa)  
  2.1. [Drzewo wyprowadzenia](#21-drzewo-wyprowadzenia)  
  2.2. [Parser (Bison)](#22-parser-bison) 
3. [Kod pośredni](#kod-pośredni)
4. [Asembler](#asembler)
5. [Obsługa błędów](#obsługa-błędów)
6. [Optymalizacje](#optymalizacje)


## 1. Analiza leksykalna
Analiza leksykalna ma dwa główne cele:
1. Rozpoznać słowa kluczowe oraz symbole języka wejściowego i zwrócić odpowiadające im tokeny.
2. Obsłużyć zmienne i stałe, umieszczając je w tablicy symboli.

Zanim rozpatrzymy te dwa punkty, najpierw zdefinujemy czym jest i jaki ma cel tablica symboli.

### 1.1. Tablica symboli
Tablica symboli to struktura (niekoniecznie *tablica*), która przechowuje informacje o symolach. W moim przypadku symbolem jest jedno z poniższych:
* zmienna
* tablica
* iterator (zmienna pętli for)
* stała

Informacje, które może zawierać tablica symboli, to na przykład:
* nazwa (zmienne, tablice, iteratory) - ciąg znaków identyfikujący obiekt 
* adres w pamięci (zmienne, tablice, iteratory) - miejsce, w którym znajduje się obiekt w pamięci maszyny docelowej
* rozmiar (tablice) - liczba komórek pamięci zmiennej tablicowej
* offset (tablice) - dla tablic, które nie zaczynają się od zera, warto wyliczyć przesunięcie względem zera
* wartość (stałe) - wartość liczbowa stałej

Dla tak określonej struktury symbolu, tablica symboli może być *tablicą dynamiczną* (jak w moim przypadku) bądź *wektorem* symboli.  
Dodawanie do tablicy symboli omówimy w następnym podrozdziale, a czytanie dopiero wtedy, gdy będzie nam potrzebne. :)

### 1.2. Lekser (Flex)
Zajmiemy się teraz budowaniem leksera przy użyciu Flex-a. Pozwolę sobie pominąć podstawy używania tego narzędzia (pragnącących je poznać zapraszam do lektury list [drugiej](https://github.com/quetzelcoatlus/PWr/tree/master/Semestr_5/Jezyki_Formalne_i_Techniki_Translacji_JFTT/Lista_2) i [trzeciej](https://github.com/quetzelcoatlus/PWr/tree/master/Semestr_5/Jezyki_Formalne_i_Techniki_Translacji_JFTT/Lista_3), które się na nich skupiają) i zająć się następującymi aspektami:

#### Co musimy wczytać?
Program wejściowy jest dany w następującej postaci:
```scala
program      -> DECLARE declarations IN commands END

declarations -> declarations pidentifier;
              | declarations pidentifier(num:num);
              | 

commands     -> commands command
              | command

command      -> identifier := expression;
              | IF condition THEN commands ELSE commands ENDIF
              | IF condition THEN commands ENDIF
              | WHILE condition DO commands ENDWHILE
              | DO commands WHILE condition ENDDO
              | FOR pidentifier FROM value TO value DO commands ENDFOR
              | FOR pidentifier FROM value DOWNTO value DO commands ENDFOR
              | READ identifier;
              | WRITE value;

expression   -> value
              | value + value
              | value - value
              | value * value
              | value / value
              | value % value

condition    -> value = value
              | value != value
              | value < value
              | value > value
              | value <= value
              | value >= value

value        -> num
              | identifier

identifier   -> pidentifier
              | pidentifier(pidentifier)
              | pidentifier(num)
```
gdzie `pidentifier` wyraża się w postaci wyrażenia regularnego `[_a-z]+`, a `num` to liczba naturalna dająca zapisać się w zmiennej typu `long long` (64 bity).

Zatem do wczytania mamy:
* słowa kluczowe = `DECLARE`, `IN`, `END`, `IF`, `THEN`, `ELSE`, `ENDIF`, `WHILE`, `DO`, `ENDWHILE`, `ENDDO`, `FOR`, `FROM`, `TO`, `DOWNTO`, `ENDFOR`, `READ`, `WRITE`
* ciągi symbolów = `;`, `(`, `)`, `:`, `:=`, `+`, `-`, `*`, `/`, `%`, `=`, `!=`, `<`, `>`, `<=`, `>=`
* nazwy zmiennych = `[_a-z]+` (wyrażenie regularne)
* liczby = `[0-9]+` (wyrażenie regularne)

#### Co musimy zwrócić?
Przekazywanie informacji z leksera do parsera może być zrealizowana na wiele różnych sposobów, ale ja przyjąłem, że wartość zwracana przez lekser jest zawsze typu `int`. W przypadku dwóch pierwszych grup (słowa kluczowe i ciągi symbolów) zwrócona zostanie wartość typu wyliczeniowego `enum`, który zostanie wygenerowany przez Bisona po zadklarowaniu w nim tokenów (więcej o tym w następnym rozdziale). Dla nazw zmiennych i liczb wartością zwracaną jest indeks w tablicy symboli, który im odpowiada (ta konwencja, tj. posługiwanie się indeksem w tablicy symboli, będzie obowiązywać praktycznie przez wszystkie fazy kompilatora).

#### Dodawanie do tablicy symboli
W lekserze są dwa stany:
```
%s declare
%s for_loop
```
Pierwszy z nich obsługuje dodanie do tablicy symboli *nieznanego symbolu* (w tym czasie nie wiemy jeszcze, czy jest to zmienna, czy tablica), a drugi pozwala na dodanie iteratora pętli for. Zwrócony zostaje indeks dodanego symbolu.  
Użycie stałej zwraca jej indeks w tablicy symboli (jeżeli jeszcze nie była użyta, to zostaje dodana).  
Znalezienie identyfikatora w innym stanie oznacza użycie zmiennej, więc zwracany jest jej indeks w tablicy symboli.  

Funkcje dodające są raczej trywialne, więc pozwolę sobie pominąć ich implementację. Jedynym zastanawiającym szczegółem może być dodawanie iteratora, podczas której sztucznie jest dodawana dodatkowa zmienna, która będzie przechowywać liczbę pozostałych iteracji pętli. O powodach, wadach i zaletach tego rozwiązania powiemy sobie później.

#### Obsługa błędów
W tej fazie wykrywane są błędy w stylu: 
* drugie zadeklarowanej zmiennej
* użycie niezadeklarowanej zmiennej

Ale na obsługę błędów został poświęcony jeden osobny rozdział, który zbiera sprawdzanie błędów ze wszystkich faz, więc na razie pominiemy ten aspekt.


## 2. Analiza składniowa
Celem tej fazy jest sprawdzenie, czy ciąg tokenów zwracanych przez lekser da się dopasować do gramatyki (jeżeli nie, to jest błąd składniowy). Skutkiem ubocznym jest otrzymanie **drzewa wyprowadzenia**.

### 2.1. Drzewo wyprowadzenia
Drzewo wyprowadzenia jest pierwszą postacią pośrednią na drodze do wynikowego kodu (proszę się nie bać tego, że *jest ich więcej* - wierzę, że później uda mi się przekonać Cię, że parę postaci wręcz ułatwia translację), jednakże nie jest niczym innym jak równoznacznym zapisaniem kodu programu wejściowego w postaci... drzewa.  

W tym celu deklarujemy strukturę pojedynczej komendy:
```c
enum CommandType{
    COM_PROGRAM,      
    COM_COMMANDS,     
        
    COM_IS,           
    
    COM_NUM,          
    COM_ARR,          
    COM_PID,          
    
    COM_ADD,          
    COM_SUB,          
    COM_MUL,          
    COM_DIV,          
    COM_MOD,          
    
    COM_IF,           
    COM_IFELSE,       
    
    COM_EQ,           
    COM_NEQ,          
    COM_LT,           
    COM_GT,           
    COM_LEQ,          
    COM_GEQ,          
    
    COM_WHILE,        
    COM_DO,           
    
    COM_FOR,          
    COM_FORDOWN,      
    
    COM_READ,         
    COM_WRITE         
    
};

struct Command{
    enum CommandType type;
    int index;
        
    int size;
    int maxSize;
    struct Command** commands;
};
```
Gdzie `type` oznacza typ komendy, `index` to indeks w tablicy symboli (przysługuje tylko identyfikatorom zmiennych i stałym czyli odpowiednio `COM_PID` oraz `COM_NUM` - takie komendy można uznać za *liście* drzewa), a `size`, `maxSize` i `commands` stanowią razem tablicę dynamiczną, w której umieszczane są dzieci (podkomendy) danej komendy (takie konstrukcje będą się przewijały przez różne struktury i prawdopodobnie w każdym miejscu można, jeżeli jest dostępny, użyć *wektora*).  

Funkcje, które będą to drzewo tworzyć, zostaną opisane w następnym podrozdziale.

### 2.2. Parser (Bison)
Zajmiemy się teraz budową parsera (na szczęście zrobi to za nas Bison; my tylko podamy mu reguły, którymi ma się kierować). Jako że użytych jest w nim wiele funkcji budujących drzewo wyprowadzenia, które mogą wymagać wyjaśnienia, przejdziemy się przez cały plik definiujący parser, krok po kroku opisując dziejące się akcje.

#### Definicje
Na początku definiujemy typy, które będą używane w regułach:
```
%union{
    int val; 
    struct Command* ptr;
}
```
`val`, czyli typ `int` odnosi się do tokenów zwracanych przez lekser, które zawierają indeks z tablicy symboli.  
`ptr` posłuży jak typ komend, z których będzie budowane drzewo wyprowadzenia.  

Następnie deklarujemy tokeny, które otrzymamy z leksera:  
Te nieniosące wartości:
```
%token DECLARE IN END

%token IS
%token IF THEN ELSE ENDIF
%token WHILE DO ENDWHILE ENDDO 
%token FOR FROM TO DOWNTO ENDFOR
%token READ
%token WRITE 

%token ADD SUB MUL DIV MOD       
%token EQ NEQ LT GT LEQ GEQ

%token LBR RBR COL SEM
```
Oraz te, które zawierają indeks z tablicy symboli:
```
%token <val> PID
%token <val> NUM
```
Na końcu deklarujemy, które reguły gramatyki niosą ze sobą informację w postaci komendy:
```
%type <ptr> command
%type <ptr> commands
%type <ptr> identifier
%type <ptr> expression
%type <ptr> condition
%type <ptr> value
```
