kobieta(basia).
kobieta(ona).
kobieta(amanda).

mê¿czyzna(tomek).
mê¿czyzna(krzys).
mê¿czyzna(stefan).

matka(basia, tomek).
matka(ona, tomek).

ojciec(stefan, amanda).
ojciec(stefan, krzys).
ojciec(krzys, basia).

rodzic(X,Y) :- matka(X,Y); ojciec(X,Y).

jest_matk¹(X) :- matka(X,_).
jest_ojcem(X) :- ojciec(X,_).
jest_synem(X) :- rodzic(_,X), mê¿czyzna(X).
jest_córk¹(X) :- rodzic(_,X), kobieta(X).
siostra(X,Y) :- rodzic(Z,X), rodzic(Z,Y), kobieta(X), X \= Y.
dziadek(X,Y) :- rodzic(X,Z), rodzic(Z,Y), mê¿czyzna(X).
rodzeñstwo(X,Y) :- rodzic(Z,X), rodzic(Z,Y), X \= Y.