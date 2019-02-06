main(Y) :-              %polecam uruchomic main(Y).
    hetmany(_,Y),
    board(Y).

board(L) :-
    length(L, N),
    rysuj_plansze(0, N, L).
   

rysuj_plansze(N, N, _) :-
    rysuj_linie(N),
    !.
    
rysuj_plansze(W, N, L) :-
    rysuj_linie(N),
    rysuj_wiersz(W, 0, N, L),
    rysuj_wiersz(W, 0, N, L),
    W2 is W+1,
    rysuj_plansze(W2, N, L).
   
    
rysuj_linie(0) :-
    writeln("+"),
    !.
   
rysuj_linie(K) :-
    write("+-----"),
    K2 is K-1,
    rysuj_linie(K2). 
     
    
rysuj_wiersz(_, N, N, _) :-
    writeln("|"),
    !.

rysuj_wiersz(W, K, N, [H | L]) :-
    write("|"),
    T is N - W - 1,
    (mod(N,2) =:= 0 ->
        (mod(W,2) =:= 0 ->
            (mod(K,2) =:= 0 ->
                (T =\= H ->
                    write("     ") 
                ;   write(" ### ")
                )
            ;   (T =\= H ->
                    write(":::::") 
                ;   write(":###:")
                )
            )
        ;   (mod(K,2) =:= 0 ->
                (T =\= H ->
                    write(":::::") 
                ;   write(":###:")
                )
            ;   (T =\= H ->
                    write("     ") 
                ;   write(" ### ")
                )
            )
        )            
    ;   (mod(W,2) =:= 0 ->
            (mod(K,2) =:= 0 ->
                (T =\= H ->
                    write(":::::") 
                ;   write(":###:")
                )
            ;   (T =\= H ->
                    write("     ") 
                ;   write(" ### ")
                )
            )
        ;   (mod(K,2) =:= 0 ->
                (T =\= H ->
                    write("     ") 
                ;   write(" ### ")
                )
            ;   (T =\= H ->
                    write(":::::") 
                ;   write(":###:")
                )
            )
        )
    ),   
    K2 is K+1,
    rysuj_wiersz(W, K2, N, L). 

    
hetmany(N, P) :-
    between(0, 20, N),
    N2 is N - 1,
    numlist(0, N2, L),
    permutation(L, P),
    dobra(P).
    
dobra(P) :-
    \+ zla(P).

zla(P) :-
    append(_, [Wi | L1], P),
    append(L2, [Wj | _], L1),
    length(L2, K),
    abs(Wi - Wj) =:= K + 1.
