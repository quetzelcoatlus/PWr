filozofowie :-
    mutex_create(W1),
    mutex_create(W2),
    mutex_create(W3),                                       %mutexy - takie jak w C -> wątek lockuje dany mutex i tylko on ma dostęp do kodu w środku 'with_mutex' dla danego mutexa
    mutex_create(W4),
    mutex_create(W5),
	thread_create(filozof(0, W1, W2), ID1, []),
	thread_create(filozof(1, W2, W3), ID2, []),
    thread_create(filozof(2, W3, W4), ID3, []),             %tworzenie wątku z podaniem filozofowi jego numeru oraz jego dwóch widelców-mutexów: lewego i prawego
    thread_create(filozof(3, W4, W5), ID4, []),
    thread_create(filozof(4, W5, W1), ID5, []),
	thread_join(ID1,_),
	thread_join(ID2,_),
    thread_join(ID3,_),                                     %czekamy, aż skończą swoje działanie wątki. Uwaga: nigdy nie skończą, ale warto pozwolić im robić
	thread_join(ID4,_),
    thread_join(ID5,_).
    
filozof(C, WL, WR) :- 
    random(R),                                              %tu taki random i sleep, żeby się jakoś zazębiały wątki. Bez tego każdy podniesie prawy widelec i się zakleszczą
    sleep(R),
    format('[~w] mysli~n', [C]),
    format('[~w] chce prawy widelec~n', [C]),               %wypisujemy swoje rzeczy
    with_mutex(WR,                                          %lokujemy mutex prawego widelca -> jednoznaczne z podniesieniem go
    (   
        format('[~w] podniosl prawy widelec~n', [C]),
        format('[~w] chce lewy widelec~n', [C]),
        with_mutex(WL,                                      %podniesienie prawego widelca
        (   
            format('[~w] podniosl lewy widelec~n', [C]),
            format('[~w] je~n', [C]),
            format('[~w] odklada lewy widelec~n', [C])
        )),
        format('[~w] odklada prawy widelec~n', [C])
    )),
    filozof(C, WL, WR).