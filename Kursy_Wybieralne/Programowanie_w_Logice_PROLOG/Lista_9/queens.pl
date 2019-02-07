:- module(queens, [queens/2]).

:- use_module(library(clpfd)).

queens(N, P) :-
	length(P, N),
	P ins 1..N,
	ok(P),
	labeling([ffc], P).


ok([]).
ok([Q|Qs]) :-
	ok(Qs, Q, 1),
	ok(Qs).

ok([], _, _).
ok([Q|Qs], Q0, D0) :-
	Q0 #\= Q,
	abs(Q0 - Q) #\= D0,
	D1 #= D0 + 1,
	ok(Qs, Q0, D1).
