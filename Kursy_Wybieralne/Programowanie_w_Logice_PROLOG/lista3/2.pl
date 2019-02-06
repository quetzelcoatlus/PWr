max_sum([H|T], S) :-
	max_sum_tmp([H|T], 0, 0, S).

max_sum_tmp([], _, S, S).
max_sum_tmp([X|L], Old1, Old2, S) :-
	New1 is max(X, Old1+X),
	New2 is max(Old2, New1),
	max_sum_tmp(L, New1, New2, S).