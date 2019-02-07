browse(X) :-
    chodz(X, [], []).
 
chodz(Gdzie, Lewo, Prawo) :-
    writeln(Gdzie),
    write("command: "),
    read(Akcja),
    
    (out(Akcja) ->
        B1 is 1
    ;   B1 is 0
    ),
    
    (in(Akcja) ->
        (Gdzie =.. [_, Gdzie2 | Prawo2] ->
            chodz(Gdzie2, [], Prawo2)
        ;   true
        )
    ;   true
    ),
    

    (next(Akcja) ->
        (Prawo = [Glowa | Ogon] ->
            (
                append([Gdzie], Lewo, Lewo2),
                chodz(Glowa, Lewo2, Ogon),
                B2 is B1+1
            )
        ;   B2 is B1
        )
    ;   B2 is B1
    ),
    
    (prev(Akcja) ->
        (Lewo = [Glowa2 | Ogon2] ->
            (
                append([Gdzie], Prawo, Prawo2),
                chodz(Glowa2, Ogon2, Prawo2),
                B3 is B2+1
            )
        ;    B3 is B2
        )
    ;   B3 is B2
    ),
    
    (B3 > 0 ->
        true
    ;   chodz(Gdzie, Lewo, Prawo)
    ).

in(i).
out(o).
next(n).
prev(p).