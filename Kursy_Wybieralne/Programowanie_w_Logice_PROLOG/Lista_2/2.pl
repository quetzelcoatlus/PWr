jednokrotnie(X, L) :-				%jednokrotnie, jeśli:
	select(X, L, L1),				%można wyciągnąć ten element
	\+ member(X, L1).				%w reszcie nie ma tego elementu

dwukrotnie(X, L) :- 				%element występuje dwukrotnie, jeśli
    append(B1, [X | A1], L), 		%dzielimy na 2 listy
    \+ member(X, B1), 				%w pierwszej nie ma X
	jednokrotnie(X, A1).			%a w drugiej, bez głowy, ktora jest X, jest tylko jeden X