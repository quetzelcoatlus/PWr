wstaw_znaki([X], X).
wstaw_znaki(Lista, X + Y) :-
    append([L|Lewy], [P|Prawy], Lista),
    wstaw_znaki([L|Lewy], X),
    wstaw_znaki([P|Prawy], Y).
    
wstaw_znaki(Lista, X - Y) :-
    append([L|Lewy], [P|Prawy], Lista),
    wstaw_znaki([L|Lewy], X),
    wstaw_znaki([P|Prawy], Y).
    
wstaw_znaki(Lista, X * Y) :-
    append([L|Lewy], [P|Prawy], Lista),
    wstaw_znaki([L|Lewy], X),
    wstaw_znaki([P|Prawy], Y).
    
wstaw_znaki(Lista, X / Y) :-
    append([L|Lewy], [P|Prawy], Lista),
    wstaw_znaki([L|Lewy], X),
    wstaw_znaki([P|Prawy], Y).
    
    
licz(X,W) :-
    number(X),
    W is X.
    
licz(X+Y, W) :-
    licz(X,A),
    licz(Y,B),
    W is A+B.
    
licz(X-Y, W) :-
    licz(X,A),
    licz(Y,B),
    W is A-B.

licz(X*Y, W) :-
    licz(X,A),
    licz(Y,B),
    W is A*B.

licz(X/Y, W) :-
    licz(X,A),
    licz(Y,B),
    B =\= 0,
    W is A/B.
    
    
wyrazenie(Lista, Wartosc, Wyrazenie) :-
    wstaw_znaki(Lista, Wyrazenie),
    licz(Wyrazenie, Wynik),
    Wartosc =:= Wynik.