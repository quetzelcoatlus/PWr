/***************************************************uruchamianie, testowanie***************************************************/
    
scanner(X,Y) :-
    czytaj(X,A),
    close(X),
    poszufladkuj(A, [], Y).

    
wykonaj(A) :-
    open(A, read, B),
    scanner(B, C),
    phrase(program(D), C),
    interpreter(D).
 
    
    
/*************************************************** gramatyka ***************************************************/    

    
    
program([H|L]) --> 
    instrukcja(H),
    [sep(;)],
    !,
    program(L).

 program([]) --> 
    [].      

   
instrukcja(assign(X, Y)) -->
    [id(X)],
    [sep(:=)],
    wyrazenie(Y).    
    
instrukcja(read(X)) --> 
    [key(read)],
    [id(X)].

instrukcja(write(Y)) -->
    [key(write)],
    wyrazenie(Y).
 
instrukcja(if(X, Y)) -->
    [key(if)],
    warunek(X),
    [key(then)],
    program(Y),
    [key(fi)].
    
instrukcja(if(X, Y, Z)) -->
    [key(if)],
    warunek(X),
    [key(then)],
    program(Y),
    [key(else)],
    program(Z),
    [key(fi)].
 
instrukcja(while(X, Y)) -->
    [key(while)],
    warunek(X),
    [key(do)],
    program(Y),
    [key(od)].
 
 
wyrazenie(X + Y) -->
    skladnik(X),
    [sep(+)],
    wyrazenie(Y).
    
wyrazenie(X - Y) -->
    skladnik(X),
    [sep(-)],
    wyrazenie(Y).
    
wyrazenie(X) -->
    skladnik(X).
    
   
skladnik(X * Y) -->
    czynnik(X),
    [sep(*)],   
    skladnik(Y).
    
skladnik(X / Y) -->
    czynnik(X),
    [sep(/)],   
    skladnik(Y). 

skladnik(X mod Y) -->
    czynnik(X),
    [key(mod)],   
    skladnik(Y).  

skladnik(X) -->
    czynnik(X).

  
czynnik(id(X)) -->
    [id(X)].
    
czynnik(int(X)) -->
    [int(X)].
    
czynnik(( X )) -->
    [sep('(')],
    wyrazenie(X),
    [sep(')')].
  
  
warunek((X ; Y)) -->
    koniunkcja(X),
    [key(or)],
    warunek(Y).
    
warunek(X) -->
    koniunkcja(X).
    

koniunkcja((X , Y)) -->
    prosty(X),
    [key(and)],
    koniunkcja(Y).
    
koniunkcja(X) -->
    prosty(X).
    
   
prosty(X =:= Y) -->
    wyrazenie(X),
    [sep(=)],
    wyrazenie(Y).
    
prosty(X =\= Y) -->
    wyrazenie(X),
    [sep(/=)],
    wyrazenie(Y).
    
prosty(X < Y) -->
    wyrazenie(X),
    [sep(<)],
    wyrazenie(Y).
    
prosty(X > Y) -->
    wyrazenie(X),
    [sep(>)],
    wyrazenie(Y).
    
prosty(X >= Y) -->
    wyrazenie(X),
    [sep(>=)],
    wyrazenie(Y).
    
prosty(X =< Y) -->
    wyrazenie(X),
    [sep(=<)],
    wyrazenie(Y).
    
prosty(( X )) -->
    [sep('(')],
    warunek(X),
    [sep(')')].
  

/************************************************** INTERPRETER *******************************************************/

interpreter(INSTRUKCJE) :-
	interpreter(INSTRUKCJE, []).

% interpreter(+INSTRUKCJE, +ASOCJACJE)
% ASOCJACJE = []
% ASOCJACJE = [ASOCJACJA | ASOCJACJE]
% ASOCJACJA = ID = NUM

interpreter([], _).
interpreter([read(ID) | INSTRUKCJE], ASOC) :- !,
	read(N),
	integer(N),
	podstaw(ASOC, ID, N, ASOC1),
	interpreter(INSTRUKCJE, ASOC1).
interpreter([write(W) | INSTRUKCJE], ASOC) :- !,
	wartosc(W, ASOC, WART),
	write(WART), nl,
	interpreter(INSTRUKCJE, ASOC).
interpreter([assign(ID, W) | INSTRUKCJE], ASOC) :- !,
	wartosc(W, ASOC, WAR),
	podstaw(ASOC, ID, WAR, ASOC1),
	interpreter(INSTRUKCJE, ASOC1).
interpreter([if(C, P) | INSTRUKCJE], ASOC) :- !,
	interpreter([if(C, P, []) | INSTRUKCJE], ASOC).
interpreter([if(C, P1, P2) | INSTRUKCJE], ASOC) :- !,
	(   prawda(C, ASOC)
	->  append(P1, INSTRUKCJE, DALEJ)
	;   append(P2, INSTRUKCJE, DALEJ)),
	interpreter(DALEJ, ASOC).
interpreter([while(C, P) | INSTRUKCJE], ASOC) :- !,
	append(P, [while(C, P)], DALEJ),
	interpreter([if(C, DALEJ) | INSTRUKCJE], ASOC).

podstaw([], ID, N, [ID = N]).
podstaw([ID = _ | ASOC], ID, N, [ID = N | ASOC]) :- !.
podstaw([ID1 = W1 | ASOC1], ID, N, [ID1 = W1 | ASOC2]) :-
	podstaw(ASOC1, ID, N, ASOC2).

pobierz([ID = N | _], ID, N) :- !.
pobierz([_ | ASOC], ID, N) :-
	pobierz(ASOC, ID, N).

wartosc(int(N), _, N).
wartosc(id(ID), ASOC, N) :-
	pobierz(ASOC, ID, N).
wartosc(W1+W2, ASOC, N) :-
	wartosc(W1, ASOC, N1),
	wartosc(W2, ASOC, N2),
	N is N1+N2.
wartosc(W1-W2, ASOC, N) :-
	wartosc(W1, ASOC, N1),
	wartosc(W2, ASOC, N2),
	N is N1-N2.
wartosc(W1*W2, ASOC, N) :-
	wartosc(W1, ASOC, N1),
	wartosc(W2, ASOC, N2),
	N is N1*N2.
wartosc(W1/W2, ASOC, N) :-
	wartosc(W1, ASOC, N1),
	wartosc(W2, ASOC, N2),
	N2 =\= 0,
	N is N1 div N2.
wartosc(W1 mod W2, ASOC, N) :-
	wartosc(W1, ASOC, N1),
	wartosc(W2, ASOC, N2),
	N2 =\= 0,
	N is N1 mod N2.

prawda(W1 =:= W2, ASOC) :-
	wartosc(W1, ASOC, N1),
	wartosc(W2, ASOC, N2),
	N1 =:= N2.
prawda(W1 =\= W2, ASOC) :-
	wartosc(W1, ASOC, N1),
	wartosc(W2, ASOC, N2),
	N1 =\= N2.
prawda(W1 < W2, ASOC) :-
	wartosc(W1, ASOC, N1),
	wartosc(W2, ASOC, N2),
	N1 < N2.
prawda(W1 > W2, ASOC) :-
	wartosc(W1, ASOC, N1),
	wartosc(W2, ASOC, N2),
	N1 > N2.
prawda(W1 >= W2, ASOC) :-
	wartosc(W1, ASOC, N1),
	wartosc(W2, ASOC, N2),
	N1 >= N2.
prawda(W1 =< W2, ASOC) :-
	wartosc(W1, ASOC, N1),
	wartosc(W2, ASOC, N2),
	N1 =< N2.
prawda((W1, W2), ASSOC) :-
	prawda(W1, ASSOC),
	prawda(W2, ASSOC).
prawda((W1; W2), ASSOC) :-
	(   prawda(W1, ASSOC),
	    !
	;   prawda(W2, ASSOC)).



  
/***************************************************czytanie z pliku***************************************************/
    
    
czytaj(S,X) :-
	get_char(S, C),                                     /*czytaj pierwszy znak ze streamu S */
	czytaj_dalej(S, C, X).                              /*czytaj kolejna znaki*/

czytaj_dalej(_, end_of_file, []) :-                     /*przeczytano znak konca streamu = warunek wyjscia */
	!.
    
czytaj_dalej(S, C1, X) :-
	bialy(C1),                                          /*jezeli to bialy znak*/
	!,
	get_char(S, C2),                                    
	czytaj_dalej(S, C2, X).                             /*to czytamy nastepny i idziemy dalej*/
    
czytaj_dalej(S, C1, [H | T]) :-                         
	czytaj_slowo(S, C1, C2, '', H),                     /*jezeli to nie bialy znak, to poczatek jakiegos slowa, wiec je czytamy*/
	czytaj_dalej(S, C2, T).                             /*czytanie slowa zwrocilo nam znak, ktory juz nie nalezy do tego slowa, wiec czytamy dalej*/


czytaj_slowo(_, end_of_file, end_of_file, N, N) :-     /*koniec strumienia = koniec slowa*/
	!.
    
czytaj_slowo(_, C, C, N, N) :-                          
	bialy(C),                                           /*bialy znak = koniec slowa, wracamy, zeby rozprawil sie z nim czytaj_dalej*/
	!. 

czytaj_slowo(_,C, C, N, N) :-                           /*cztery kolejne termy odpowiedzialne za to, ze mozna pisac programy bez spacji*/
    char_type(C, punct),                                /* w tym przypadku sprawdzamy, czy obecna litera jest znakiem interpunkcyjnym*/
    atom_length(N,L), 
    L > 0,                                              /*badamy, czy dlugosc dotychczasowego ulepionego slowa jest niezerowa*/
    atom_chars(N, L2),                                /*zamieniamy slowo na tablice charow*/
    \+ same_jakies(L2, punct),                          /*sprawdzamy, czy dotychczasowe slowo sklada sie z samych znakow interpunkcyjnych. Jezeli tak, to tutaj predykat zawiedzie i wywola sie tamten troche nizszy i dolepi ten znak do slowa. Jezeli nie, to konczymy obecne slowo i wracamy do czytaj_dalej*/
    !.

czytaj_slowo(_,C, C, N, N) :-
    char_type(C, upper),                                /*to samo, co wyzej, ale dla wielkich liter*/
    atom_length(N,L),
    L > 0,
    atom_chars(N, L2),
    \+ same_jakies(L2, upper),
    !.

czytaj_slowo(_,C, C, N, N) :-
    char_type(C, lower),                                 /*to samo, co wyzej, ale dla wielkich liter*/
    atom_length(N,L),
    L > 0,
    atom_chars(N, L2),
    \+ same_jakies(L2, lower),
    !.   

czytaj_slowo(_,C, C, N, N) :-
    char_type(C, digit),                                 /*to samo, co wyzej, ale dla cyfr */
    atom_length(N,L),
    L > 0,
    atom_chars(N, L2),
    \+ same_jakies(L2, digit),
    !.    
    
czytaj_slowo(S, C1, C3, N1, N) :-                       /*jezeli jeszcze nie wyszlismy, to: */
	atom_concat(N1, C1, N2),                            /*doklejamy obecny znak do dotychczasowego slowa*/
	get_char(S, C2),                                    /*czytamy kolejny i sprawdzamy dalej*/
	czytaj_slowo(S, C2, C3, N2, N).
    
same_jakies([], _) :-
    !.
    
same_jakies([H|L],Y) :-
    char_type(H,Y),                                     /*sprawdzenie, czy kazdy znak tablicy jest typu Y */
    same_jakies(L,Y).
    
    
/***************************************************poszufladkowanie slow ze wzgledu na ich typ***************************************************/
 
poszufladkuj([], Y, Y) :-       /*warunek wyjsca*/
    !.
    
poszufladkuj([H1 | X], Y1, Y) :-     /*znajdywanie key(H)*/
    key(H1),
    !,
    append(Y1, [key(H1)], Y2),
    poszufladkuj(X, Y2, Y).
    
poszufladkuj([H1 | X], Y1, Y) :-      /*znajdywanie sep(H)*/
    atom_string(H1,H),
    sep(H),
    !,
    append(Y1, [sep(H1)], Y2),
    poszufladkuj(X, Y2, Y).
    
poszufladkuj([H1 | X], Y1, Y) :-          /*znajdywanie int(H)*/
    atom_number(H1, H),
    !,
    append(Y1, [int(H)], Y2), 
    poszufladkuj(X, Y2, Y).
    
poszufladkuj([H1 | X], Y1, Y) :-          
    atom_chars(H1, L),                    /*zamiana atomu na chary*/
    zgodne(L),                              /*sprawdzamy, czy kazdy znak jest wielka litera*/
    !, 
    append(Y1, [id(H1)], Y2),
    poszufladkuj(X, Y2, Y). 
    
zgodne([]) :-
    !.
    
zgodne([H | L]) :-
    char_type(H, upper),            /*kazda litera 'potencjalnej zmiennej' jest wielka */
    zgodne(L).
    
/***************************************************fakty programu***************************************************/

bialy(' ').
bialy('\t').
bialy('\n').
bialy('\r').

key(read).
key(write).
key(if).
key(then).
key(else).
key(fi).
key(while).
key(do).
key(od).
key(and).
key(or).
key(mod).

sep(";").
sep("+"). 
sep("-"). 
sep("*"). 
sep("/"). 
sep("("). 
sep(")"). 
sep("<"). 
sep(">").  
sep("=<"). 
sep(">="). 
sep(":="). 
sep("="). 