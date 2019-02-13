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
3. [Kod pośredni](#3-kod-pośredni)  
  3.1 [Kod trójadresowy](#31-kod-trójadresowy)
4. [Asembler](#4-asembler)
5. [Obsługa błędów](#5-obsługa-błędów)
6. [Optymalizacje](#6-optymalizacje)


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
Zajmiemy się teraz budową parsera (na szczęście zrobi to za nas Bison; my tylko podamy mu reguły, którymi ma się kierować). Jako że użytych jest w nim wiele funkcji budujących drzewo wyprowadzenia, które mogą wymagać wyjaśnienia, przejdziemy się przez cały plik definiujący parser, krok po kroku opisując dziejące się akcje. Nie będę dokładnie opisywał struktury reguł Bisona, zainteresowanych zapraszam do lektury listy [trzeciej](https://github.com/quetzelcoatlus/PWr/tree/master/Semestr_5/Jezyki_Formalne_i_Techniki_Translacji_JFTT/Lista_3).

#### Definicje
Na początku definiujemy typy, które będą używane w regułach:
```c
%union{
    int val; 
    struct Command* ptr;
}
```
`val`, czyli typ `int` odnosi się do tokenów zwracanych przez lekser, które zawierają indeks z tablicy symboli.  
`ptr` posłuży jak typ komend, z których będzie budowane drzewo wyprowadzenia.  

Następnie deklarujemy tokeny, które otrzymamy z leksera:  
Te, które nie niosą żadnej wartości:
```scala
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
```scala
%token <val> PID
%token <val> NUM
```
Na końcu deklarujemy, które reguły gramatyki niosą ze sobą informację w postaci komendy:
```scala
%type <ptr> command
%type <ptr> commands
%type <ptr> identifier
%type <ptr> expression
%type <ptr> condition
%type <ptr> value
```


#### Reguły gramatyki
Reguły zostały praktycznie jeden do jednego przepisane z gramatyki języka wejściowego. Jedynie należało umiejętnie dodać akcje, które się wykonają podczas przetwarzania:

```scala
program
: DECLARE declarations IN commands END { create_program($4); }
;
```

Po przetworzeniu całego programu w czwartym argumencie (`commands`) uzyskamy de facto drzewo wszystkich komend programu. Zatem jedynie, co musimy zrobić, to podpiąć je jako dziecko komendy o typie `COM_PROGRAM` (dla wygody jest to zmienna globalna).

```c
void create_program(struct Command* commands){
    program = malloc(sizeof(struct Command));
    program->type = COM_PROGRAM;
    program->index = NONE;
    program->size = 1;
    program->maxSize = 1;
    program->commands = malloc(sizeof(struct Command*));
    program->commands[0] = commands;
}
```
Zostało użyte makro `NONE` i będzie się ono jeszcze przewijać przez kolejne fazy. Oznacza wartość pustą. Jest to dokładnie:
```c
#define NONE -1
```

---

```scala
declarations
: %empty
| declarations PID SEM                          { set_variable($2); } 
| declarations PID LBR NUM COL NUM RBR SEM      { set_array($2, $4, $6); } 
;
```
W sekcji deklaracji w końcu jesteśmy w stanie stwierdzić, jakiego typu jest deklarowany obiekt (zmienna/tablica). Dlatego też dla zmiennej wywoływana jest funkcja `set_variable`, która przyjmuje jako argument indeks w tablicy symboli odpowiadający zmiennej i ustawia typ tego symbolu na `VARIABLE`. Funkcja `set_array` jest następującej sygnatury:
```c
int set_array(int pos, int start, int end);
```
Ustawia ona typ symbolu na `pos` pozycji w tablicy symboli na `ARRAY`, ustawia wartość pola `offset` na wartość `start` oraz wylicza rozmiar tablicy (`end` - `start` + 1).

---

```scala
commands
: %empty                { $$ = create_empty_command(COM_COMMANDS); }
| commands command      { $$ = add_command($1, $2); }
| commands error        { yyerror("Blad skladniowy"); }  
;
```
`commands` oznacza blok komend i zawiera lewostronną rekursję, która kończy się, gdy skończą się komendy (`%empty`). Zatem w akcji pustej komendy tworzymy pustą komendę:
```c
struct Command* create_empty_command(enum CommandType type){    
    struct Command* c = malloc(sizeof(struct Command));
    c->type = type;
    c->index = NONE;
    c->size = 0;
    c->maxSize = 8;
    c->commands = malloc(sizeof(struct Command*) * 8);

    return c;
}
```
Domyślnie może ona pomieścić ośmioro dzieci, ale przy każdym dodaniu dziecka będzie sprawdzana konieczność poszerzenia. Dodawanie nowoprzetworzonych komend przebiega następująco:
```c
struct Command* add_command(struct Command* parent, struct Command* child){
    if(parent->size == parent->maxSize)
        resize_command(parent)
    
    parent->commands[parent->size] = child; 
    parent->size++;
    
    return parent;
```

Analizę błędów, z wyjaśnionych wcześniej przyczyn, (na razie) pomijamy.

---

```scala
command
: identifier IS expression SEM                          { $$ = create_parent_command(COM_IS,      2, $1, $3); }
| IF condition THEN commands ELSE commands ENDIF        { $$ = create_parent_command(COM_IFELSE,  3, $2, $4, $6); }   
| IF condition THEN commands ENDIF                      { $$ = create_parent_command(COM_IF,      2, $2, $4); }    
| WHILE condition DO commands ENDWHILE                  { $$ = create_parent_command(COM_WHILE,   2, $2, $4); }
| DO commands WHILE condition ENDDO                     { $$ = create_parent_command(COM_DO,      2, $2, $4); }
| FOR PID FROM value TO value DO commands ENDFOR        { $$ = create_parent_command(COM_FOR,     5, create_value_command(COM_PID, $2), create_value_command(COM_PID, $2+1),$4, $6, $8); }
| FOR PID FROM value DOWNTO value DO commands ENDFOR    { $$ = create_parent_command(COM_FORDOWN, 5, create_value_command(COM_PID, $2), create_value_command(COM_PID, $2+1),$4, $6, $8); }
| READ identifier SEM                                   { $$ = create_parent_command(COM_READ,    1, $2); }
| WRITE value SEM                                       { $$ = create_parent_command(COM_WRITE,   1, $2); }
;

expression
: value                 { $$ = $1;}
| value ADD value       { $$ = create_parent_command(COM_ADD, 2, $1, $3); }
| value SUB value       { $$ = create_parent_command(COM_SUB, 2, $1, $3); }
| value MUL value       { $$ = create_parent_command(COM_MUL, 2, $1, $3); }
| value DIV value       { $$ = create_parent_command(COM_DIV, 2, $1, $3); }
| value MOD value       { $$ = create_parent_command(COM_MOD, 2, $1, $3); }
;

condition
: value EQ value        { $$ = create_parent_command(COM_EQ,  2, $1, $3); }
| value NEQ value       { $$ = create_parent_command(COM_NEQ, 2, $1, $3); }
| value LT value        { $$ = create_parent_command(COM_LT,  2, $1, $3); }
| value GT value        { $$ = create_parent_command(COM_GT,  2, $1, $3); }
| value LEQ value       { $$ = create_parent_command(COM_LEQ, 2, $1, $3); }
| value GEQ value       { $$ = create_parent_command(COM_GEQ, 2, $1, $3); }
;
```
W powyższych regułach kierujemy się jedną zasadą: otrzymujemy ileś komend dzieci i tworzymy dla nich rodzica o określonym typie. Każdy wiersz w gramatyce, np. instrukcje warunkowe `IF`, `IF/ELSE` albo pętle `WHILE`, `DO`, `FOR`, będzie odpowiadać komendzie w drzewie wyprowadzenia i jako dzieci posiadać swoje podkomendy. Przykładowo komenda `WHILE` ma dwoje dzieci: `condition` (warunek, który trzyma nas w pętli) oraz `commands` (wszystkie komendy znajdujące się w WHILE-u). Wyjaśnienie nieco skomplikowanych akcji dla FOR-ów znajdzie się trochę niżej. Do tworzenia komendy rodzica służy następująca funkcja:
```c
struct Command* create_parent_command(enum CommandType type, int numberOfChilds, ...){  
    va_list ap;
    va_start(ap, numberOfChilds); 
    
    struct Command* c = malloc(sizeof(struct Command));
    c->type = type;
    c->index = NONE;
    c->size = numberOfChilds;
    c->maxSize = numberOfChilds;
    c->commands = malloc(sizeof(struct Command*) * numberOfChilds);
 
    for(int i = 0; i < noumberOfChilds; i++){
        c->commands[i] = va_arg(ap, struct Command*);
    }
    
    va_end(ap);
    
    return c;
}
```
Funkcja ta przyjmuje nastepujące argumenty: typ komendy rodzica, liczbę jego dzieci oraz kolejno jego dzieci (`...` sprawia, że możemy podać dowolną liczbę argumentów do funkcji). Struktura `ap_list` oraz funkcje `va_start`, `va_arg` i `va_end` z `<stdarg.h>` pozwalają na stosunkowe łatwe iterowanie po argumentach funkcji, więc jedyne co musimy zrobić to stworzyć komendę rodzica i umieścić jego dzieci w jego tablicy potomków.

---

```scala
value
: NUM                   { $$ = create_value_command(COM_NUM, $1); }
| identifier            { $$ = $1; }
; 

identifier
: PID                   { $$ = create_value_command(COM_PID, $1); }
| PID LBR PID RBR       { $$ = create_parent_command(COM_ARR, 2, create_value_command(COM_PID, $1), create_value_command(COM_PID, $3)); }
| PID LBR NUM RBR       { $$ = create_parent_command(COM_ARR, 2, create_value_command(COM_PID, $1), create_value_command(COM_NUM, $3)); }
;
```
W końcu doszliśmy do liści drzewa, czyli komend, które zawierają indeksy z tablicy symboli. Są 4 możliwości:
* stała
* zmienna
* tablica od stałej
* tablica od zmiennej

Dla każdej z nich odpowiednio używamy funkcji do tworzenia komendy *z wartością*:
```c
struct Command* create_value_command(enum CommandType type, int index){
    struct Command* c = malloc(sizeof(struct Command));
    c->type = type;
    c->index = index;
    c->size = 0;
    c->maxSize = 0;
    c->commands = NULL;
    
    return c;
}
```
Jak widać, nie ma ona dzieci - jedynie co przechowuje to informację o typie oraz wartość `index`, czyli indeks w tablicy symboli.

---

Wróćmy zatem jeszcze na moment do pętli FOR:
```scala
| FOR PID FROM value TO value DO commands ENDFOR        { $$ = create_parent_command(COM_FOR,     5, create_value_command(COM_PID, $2), create_value_command(COM_PID, $2+1),$4, $6, $8); }
| FOR PID FROM value DOWNTO value DO commands ENDFOR    { $$ = create_parent_command(COM_FORDOWN, 5, create_value_command(COM_PID, $2), create_value_command(COM_PID, $2+1),$4, $6, $8); }
```
Akcje te tworzą komendę, która ma pięcioro dzieci:
* iterator
* sztucznie dodany licznik pozostałych iteracji (którego indeks w tablicy symboli jest zawsze o jeden większy niż iteratora, stąd `$2+1`)
* wartość początkowa
* wartość końcowa
* komendy

Sztuczne dodanie licznika pozostałych iteracji trochę kompilikuje sprawę, bo wprowadzamy do drzewa element, którego nie było w wejściowej gramatyce, ale wyjaśnienie potrzeby jego użycia przestanie być tajemnicą już następnym rozdziale!

#### Obsługa błędów
Tak samo, jak w poprzedniej fazie, w tej też występuje obsługa błędów, ale też zostaje ona przeniesiona do osobnego rozdziału.

## 3. Kod pośredni
Głównym celem zamiany drzewa wyprowadzenia na kod pośredni jest chęć pozbycia się konstrukcji, które najbardziej odstają od wynikowego kodu asemblera: pętli i (w mniejszym stopniu) instrukcji warunkowych. Taka dwuetapowa translacja (program -> kod pośredni -> asembler) moim zdaniem znacznie ułatwia zadanie i znacznie redukuje nakład pracy potrzebny do przetłumaczenia niektórych konstrukcji. Jako że w programie wejściowym mamy możliwe następujące relacje: `=`, `!=`, `<`, `>`, `<=` i `>=`, a w maszynie rejestrowej mamy dostępne jedynie następujące instrukcje (pełna definicja maszyny rejestrowej będzie w nastepnym rozdziale): `JUMP` (skok bezwarunkowy), `JZERO` (skok dla zerowej wartości) i `JODD` (skok dla nieparzystej wartości), do kodu pośredniego dodamy *kompromis* między nimi, czyli następujące instrukcje: `JEQ` (skok dla równych liczb), `JNEQ` (dla nierównych), `JGEQ` (pierwsza większa bądź równa drugiej), `JLEQ`, `JGT` i `JLT`. Dzięki takiemu zabiegowi będziemy w stanie zamienić pętle na *bardziej podstawowe* instrukcje. Strukturą, którą będzie nasz kod pośredni jest **kod trójadresowy**.

### 3.1. Kod trójadresowy
