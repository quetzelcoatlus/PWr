%ma(kto, co).
%daje(kiedy, kto, co, komu).

ma(janek, chleb).
ma(stefan, miecz).
ma(krzysiek, telefon).

daje(n(c0), janek, chleb, stefan).
daje(n(n(c0)), stefan, miecz, janek).
daje(n(n(n(c0))), stefan, chleb, janek).

ma(c0, K, C) :- ma(K, C).    		%ktoś coś posiada w chwili zerowej, jeżeli to jest fakt

ma(X, K, C) :- 			  			%ktoś coś posiada w danej chwili, jeżeli:
	daje(X, _, C, K),			  	%w dokładnie tej chwili coś dostał
	\+ daje(X, K, C, _).		 	%i tego nie oddał nikomu
	
ma(n(X), K, C) :-			  		%ktoś coś posiada w danej chwili (następnej względem jakiejś), jeżeli:
	ma(X, K, C),					%posiadał w poprzedniej chwili
	\+ daje(n(X), K, C, _).			%i nikomu w tej chwili nie oddał  