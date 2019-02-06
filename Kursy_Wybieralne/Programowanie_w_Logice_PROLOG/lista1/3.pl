left_of(olowek, klepsydra).
left_of(klepsydra, motyl).
left_of(motyl,ryba).

above(rower, olowek).
above(aparat, motyl).

above(chleb, rower).
above(maka, chleb).

above(a,klepsydra).
above(b,a).

right_of(X,Y) :- left_of(Y,X).
below(X,Y) :- above(Y,X).

above2(X,Y) :- above(X,Y).
above2(X,Y) :- (above(Z,Y), above2(X,Z)).

left_of2(X,Y) :- left_of(X,Y).
left_of2(X,Y) :- (left_of(Z,Y), left_of2(X,Z)).

height(X,Y) :- \+ above(X,_), Y is 0.
height(X,Y) :- above(X,Z), height(Z,B), Y is B+1.

higher(X,Y) :- height(X,A), height(Y,B), A > B.
