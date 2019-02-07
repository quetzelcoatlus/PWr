on(k1,k2).
on(k2,k3).
on(k3,k4).

above(X,Y) :- on(X,Y).
above(X,Y) :- on(Z,Y), above(X,Z).