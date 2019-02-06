%:- use_module(library(sgml)).                      %biblioteka z load_html/3. Wg dokumentacji "can be autoloaded", co tez sie u mnie dzieje, ale jakby komus sie nie ladowala automatycznie, to odkomentowac.
%:- use_module(library(xpath)).                     %biblioteka z xpath/3. Wg dokumentacji "can be autoloaded", co tez sie u mnie dzieje, ale jakby komus sie nie ladowala automatycznie, to odkomentowac.
%:- use_module(library(url)).                       %biblioteka z parse_url/2, parse_url/3. Wg dokumentacji "can be autoloaded", co tez sie u mnie dzieje, ale jakby komus sie nie ladowala automatycznie, to odkomentowac.
:- use_module(library(http/http_open)).             %biblioteka z http_open.  Nie laduje sie automatycznie, wiec ladujemy recznie. http_open nie jest potrzebne w tym zadaniu, ja sobie zrobilem dla zabawy, wiec mozna to usunac, ale o tym wiecej w predykacie serwery_online

serwery_online(X,Y) :-                              %moj dodaktowy predykat, ktory pozwala zczytac strone z internetu i z niej wydobyc linki. Przyklad uzycia: serwery_online('http://www.pwr.edu.pl',X), print_term(X,[]).
    http_open(X, S, []),                            %otwieramy strumien htmla z jakiejs strony http
    serwery(S, Y),                                  %odpalamy predykat serwery/2
    close(S).                                       %zamykamy strumien
    
serwery(X,Y) :-                                     %tu mozemy sobie pobrac jakas strone na dysk i odpalic, np. serwery('index.html',X),print_term(X,[]).
    load_html(X, List, []),                         %ladujemy plik/strumien html (X) do przyjazno-prologowej postaci (dla ciekawskich mozna zobaczyc: print_term(List, []).
    findall(H,                                      %znajdujemy wszystkie takie 'H', ze:
    (   xpath(List, //(a(@href)), A),               %znajdujemy (niedeterministycznie, dlatego beda nawroty) link (znacznik 'a') i wkladamy go do A
        parse_url(A,'http://localhost/',B) ,        %zamieniamy go na liste tokenow (np. protocol, host, port, path, search)
        member(host(H), B)                          %wyluskujemy z tokenow token host i jego wartosc wkladamy do H
    ), Z),                                          %jezeli wszystko sie powiodlo w przebiegu, to wkladamy H do listy Z
    sort(Z,Y).                                      %sortujemy Z, zeby pozbyc sie powtorzen