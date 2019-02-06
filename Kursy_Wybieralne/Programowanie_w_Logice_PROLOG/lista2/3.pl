arc(a,a).
arc(a, b).
arc(b, a).
arc(b, c).
arc(c, d).

szukaj(A, B, V) :-      			%szukanie drogi z A do B, wiedząc, że było było się już wierzchołkach V
	arc(A, X),          			%jest połączenie A z jakimś X
	\+ (member(X, V)), 				%nie byliśmy jeszcze w X
	(                  				
		B = X           			%X to nasz szukany punkt
	; 	szukaj(X, B, [X | V]) 		%lub jest droga z X do B, wiedząc, że byliśmy już w A
	).                 

osiagalny(X,Y) :- X = Y.
osiagalny(X,Y) :- szukaj(X,Y, [X]).