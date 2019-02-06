le(2,3).
le(3,6).
le(6,7).

le(2,6).
le(2,7).
le(3,7).

le(2,2).
le(3,3).
le(6,6).
le(7,7).

zwr :- \+ ((le(X, _); le(_, X)), \+ le(X, X)).

przech :- \+ ((le(X, _); le(_, X)),(le(Y, _); le(_, Y)),(le(Z, _); le(_, Z)),le(X,Y),le(Y,Z), \+ le(X, Z)).

slab :- \+ ((le(X, _); le(_, X)),(le(Y, _); le(_, Y)),le(X,Y),le(Y,X), X =\= Y).

cp :- zwr, przech, slab.