le(a,a).
le(b,b).
le(c,c).

le(a,b).
le(a,c).

maksymalny(X) :-
	le(X,X),
	\+ (
		(le(Y,_); le(_,Y)),
		le(X,Y),
		\+ X=Y
	).

minimalny(X) :-
	le(X,X),
	\+ (
		(le(Y,_); le(_,Y)),
		le(Y,X),
		\+ X=Y
	).

najwiekszy(X) :- 
	le(X,X),
	\+ (
		(le(Y,_); le(_,Y)),
		\+le(Y,X)
	).

najmniejszy(X) :- 
	le(X,X),
	\+ (
		(le(Y,_); le(_,Y)),
		\+le(X,Y)
	).
