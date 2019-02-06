/* Napisz gramatykę metamorficzne akceptujące słowa z języka a^n b^n, gdzie n ≥ 0. */

ab --> ``.
ab --> `a`, ab, `b`.
/* 
uruchamiac poprzez: 
    phrase(ab, X), format("~s~n",[X]). 
*/

/*Napisz gramatykę metamorficzne akceptujące słowa z języka a^n b^n c^n, gdzie n ≥ 0.*/


ab(0) --> ``.
ab(X) --> `a`, ab(X2), `b`, {X is X2 + 1}.      /*najpierw mamy  wchodzimy rekurencyjnie do ab(X2), a dopiero potem ustalamy X*/

c(0) --> ``.
c(X) --> {X > 0, X2 is X - 1}, `c`, c(X2).        /*c(X) wywoluje sie jako drugie w abc (2 linijki nizej), wiec X mamy juz dane, uzyskane z ab(X). Dlatego mozemy go od razu uzyc i wyznaczyc z niego X2 do rekurencji ogonowej*/

abc --> ab(X), c(X).

/*
uruchamiac poprzez:
    phrase(abc,X), format("~s~n", [X]).
*/

/*Napisz gramatykę metamorficzne akceptujące słowa z języka a^n b^fib(n), gdzie n ≥ 0 a fib(n) jest n-tym wyrazem ciągu Fibonacciego.*/

a(0) --> ``.
a(X) --> a(X2), `a`, {X is X2 + 1}.                 /*to samo, co wyzej - tu wyznaczamy pasujace X*/

fib(0) --> ``.
fib(1) --> `b`.
fib(X) --> {X > 1, X1 is X - 1, X2 is X - 2}, fib(X1), fib(X2).     /*a tu juz go uzywamy bez klopotu*/

afib --> a(X), fib(X).

/*
uruchamiac poprzez:
    phrase(afib,X), format("~s~n", [X]).
*/
 
/* Zdefiniowano następującą gramatykę metamorficzną: */

p([]) --> [].
p([X|Xs]) --> [X], p(Xs).


/*Jak jest zależność między listami L1, L2 i L3, jeśli spełniają one warunek phrase(p(L1), L2, L3)? */
/* 
    L1 to jest lista złożona z początkowych wyrazów L2, a L3 to pozostałe, więc:
    
    append(L1,L3,L2).
    
    albo:
    L1 + L3 = L2 
*/

/*
?- listing(p).
p([], A, A).
p([A|B], [A|C], D) :-
    p(B, C, D).    
*/ 
