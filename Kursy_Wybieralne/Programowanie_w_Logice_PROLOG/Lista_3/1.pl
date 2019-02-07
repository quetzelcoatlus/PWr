suma([], 0).
suma([H|T], Wynik) :-
	suma(T, Tmp),
	Wynik is H+Tmp.

srednia(X, Wynik) :-
	suma(X, Suma),
	length(X, L),
	Wynik is Suma / L.
	
wariancja(X, D) :-
	srednia(X, Srednia),
	length(X, Liczba),
	wariancja_tmp(X, Srednia, Liczba, D).
	
wariancja_tmp([], _, _, 0).	
wariancja_tmp([H|T], Srednia, Liczba, D) :-
	wariancja_tmp(T, Srednia, Liczba, Tmp),
	D is ((Srednia - H)^2)/Liczba + Tmp.
	
%nie dzielic za kazdym razem, ale cala sume
	