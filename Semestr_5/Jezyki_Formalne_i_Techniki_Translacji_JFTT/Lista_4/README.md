# "Czemu to nie działa?!", czyli o moim kompilatorze słów kilka

## Przedmowa
Witaj, drogi czytelniku. Jeśli tu dotarłeś, to albo interesuje Cię temat kompilatora z przedmiotu "Języki Formalne i Techniki Translacji" na Wydziale Podstawowych Problemów Techniki na Politechnice Wrocławskiej, albo przypadkiem dokopałeś się do tego folderu, chodząc po tym repozytorium. Tak czy siak, zapraszam Cię do lektury niniejszego *poradnika*, w którym postaram się opisać proces tworzenia kompilatora poprzez wszystkie fazy, zarówno od strony teoretycznej, jak i w oparciu o kod mojego kompilatora.
Pomysł, żeby stworzyć taki poradnik zrodził się już podczas pisania kompilatora, a stosunkowo dobry wynik w rankingu - drugie miejsce - utwierdził mnie w tym postawnowieniu. 

## Spis treści
1. [Analiza leksykalna](#1-analiza-leksykalna)  
  1.1. [Tablica symboli](#11-tablica-symboli)  
  1.2. [Lekser (Flex)](#12-lekser-flex)
2. [Analiza składniowa](#analiza-skladniowa)
3. [Kod pośredni](#kod-posredni)
4. [Asembler](#asembler)
5. [Obsługa błędów](#obsluga-bledow)
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
* nazwa (zmienne, tablice, iteratory) - ciąg znaków z klasy [_a-z] identyfikujący obiekt 
* adres w pamięci (zmienne, tablice, iteratory) - miejsce, w którym znajduje się obiekt w pamięci maszyny docelowej
* rozmiar (tablice) - liczba komórek pamięci zmiennej tablicowej
* offset (tablice) - dla tablic, które nie zaczynają się od zera, warto wyliczyć przesunięcie względem zera
* wartość (stałe) - wartość liczbowa stałej

Dla tak określonej struktury symbolu, tablica symboli może być *tablicą dynamiczną* (jak w moim przypadku) bądź *wektorem* symboli.  
Dodawanie do tablicy symboli omówimy w następnym podrozdziale, a czytanie dopiero wtedy, gdy będzie nam potrzebne. :)

### 1.2. Lekser (Flex)
Zajmiemy się teraz budowaniem leksera przy użyciu Flex-a. Pozwolę sobie pominąć podstawy używania tego narzędzia (pragnącących je poznać zapraszam do lektury list [drugiej](https://github.com/quetzelcoatlus/PWr/tree/master/Semestr_5/Jezyki_Formalne_i_Techniki_Translacji_JFTT/Lista_2) i [trzeciej](https://github.com/quetzelcoatlus/PWr/tree/master/Semestr_5/Jezyki_Formalne_i_Techniki_Translacji_JFTT/Lista_3), które się na nich skupiają) i zająć się następującymi aspektami:
* Co musimy wczytać?
* Co musimy zwrócić?

#### Co musimy wczytać?
Program wejściowy jest dany w następującej postaci:
```
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
