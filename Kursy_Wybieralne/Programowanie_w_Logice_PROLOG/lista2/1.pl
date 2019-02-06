usun_ostatni_element(L, L1) :- 
	append(L1, [_], L).
usun_pierwszy_element([_ | L], L).
	
srodkowy([X], X).
srodkowy(L, X) :- 
	usun_ostatni_element(L, L1),
	usun_pierwszy_element(L1, L2),
	srodkowy(L2, X).
