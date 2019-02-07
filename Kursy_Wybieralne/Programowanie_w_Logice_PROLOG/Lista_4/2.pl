lewo(Lewy,Prawy,[Lewy,Prawy|_]).
lewo(Lewy,Prawy,[_|R]) :-
	lewo(Lewy,Prawy,R).
	
obok(X, Y, Domy) :-
	lewo(X, Y, Domy).

obok(X, Y, Domy) :-
	lewo(Y, X, Domy).

% numer, kolor, kraj, zwierze, picie, palenie
rybki(Kto) :-
	Domy = [[1,_,_,_,_,_],[2,_,_,_,_,_],[3,_,_,_,_,_],[4,_,_,_,_,_],[5,_,_,_,_,_]],
	member([1,_,norweg,_,_,_],Domy),
	member([_,czerwony,anglik,_,_,_],Domy),
	lewo([_,zielony,_,_,_,_],[_,bialy,_,_,_,_],Domy),
	member([_,_,dunczyk,_,herbata,_],Domy),
	obok([_,_,_,_,_,light],[_,_,_,koty,_,_],Domy),
	member([_,zolty,_,_,_,cygaro],Domy),
	member([_,_,niemiec,_,_,fajka],Domy),
	member([3,_,_,_,mleko,_],Domy),
	obok([_,_,_,_,_,light],[_,_,_,_,woda,_],Domy),
	member([_,_,_,ptaki,_,bezfiltra],Domy),
	member([_,_,szwed,psy,_,_],Domy),
	obok([_,_,norweg,_,_,_],[_,niebieski,_,_,_,_],Domy),
	obok([_,_,_,konie,_,_],[_,zolty,_,_,_,_],Domy),
	member([_,_,_,_,piwo,mentol], Domy),
	member([_,zielony,_,_,kawa,_],Domy),
	member([_,_,Kto,rybki,_,_],Domy).