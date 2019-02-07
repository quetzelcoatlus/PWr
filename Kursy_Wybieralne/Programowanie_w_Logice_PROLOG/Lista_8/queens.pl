size(10).                       %mozna sobie tutaj zmienic rozmiar planszy
%:- use_module(library(pwp)).   %do predykatu pwp_files. Can be autoloaded, ale jak nie, to odkomentowac.

queens(N, P) :-
	numlist(1, N, L),
	permutation(L, P),
	\+ bad(P).

bad(P) :-
	append(_, [I | L1], P),
	append(L2, [J | _], L1),
	length(L2, K),
	abs(I-J) =:= K+1.

