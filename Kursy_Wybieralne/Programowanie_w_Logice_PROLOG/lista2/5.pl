lista(N, L) :-
    numlist(1, N, A),
    odd_elts(L, A),
    permutation(A, B),
    even_elts(L, B).

odd_elts([], []).
odd_elts([X, _|L], [X|R]) :-
    odd_elts(L, R).

even_elts([], []).
even_elts([_, X|L], [X|R]) :-
    even_elts(L, R).

/*
N   N!          inf		       avg
1	1		    27             27
2	2		    41             20.5
3	6		    90             15
4	24		    317            13.2
5	120		    1606           13.3
6   720         10221          14.1
7   5040        76412          15.1
8   40320       651397         16.1
9   362880      6225186        17.1
10  3628800     65880341       18.1
*/

