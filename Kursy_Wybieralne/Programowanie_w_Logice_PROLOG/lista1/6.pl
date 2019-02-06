prime(LO, HI, N) :-
	between(LO, HI, N),
	W is integer(floor(sqrt(N))),
	\+ (
		between(2,W,Y), 
		(N mod Y =:= 0)
	).