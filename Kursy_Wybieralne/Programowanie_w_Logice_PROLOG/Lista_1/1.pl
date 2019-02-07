kobieta(basia).
kobieta(ona).
kobieta(amanda).

mezczyzna(tomek).
mezczyzna(krzys).
mezczyzna(stefan).

matka(basia, tomek).
matka(ona, tomek).

ojciec(stefan, amanda).
ojciec(stefan, krzys).
ojciec(krzys, basia).

rodzic(X,Y) :- matka(X,Y); ojciec(X,Y).

jest_matka(X) :- matka(X,_).
jest_ojcem(X) :- ojciec(X,_).
jest_synem(X) :- rodzic(_,X), mezczyzna(X).
jest_corka(X) :- rodzic(_,X), kobieta(X).
siostra(X,Y) :- rodzic(Z,X), rodzic(Z,Y), kobieta(X), X \= Y.
dziadek(X,Y) :- rodzic(X,Z), rodzic(Z,Y), mezczyzna(X).
rodzenstwo(X,Y) :- rodzic(Z,X), rodzic(Z,Y), X \= Y.