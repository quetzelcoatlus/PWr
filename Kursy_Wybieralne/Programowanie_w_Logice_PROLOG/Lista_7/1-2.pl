mymerge(X, Y, Z) :-                     %merge posortowanych X i Y w Z. Uwaga: jest 'mymerge' zamiast 'merge', bo 'merge' to jakiś deprecated prologowy term i się pluje o niedozwolony override xd
    freeze(X,                           %zamrażamy X i czekamy na jego wartość
    (   X = [HX | TX]                   %jeżeli X ma co najmniej jeden element
    ->  freeze(Y,                       %to czekamy na wartość Y
        (   Y  = [HY | TY]              %jeżeli Y ma co najmniej 1 element
        ->  (  HX =< HY                 %porównujemy głowy X i Y czyli najmniejsze elementy tych list
            ->  Z = [HX | TZ],          %głowa X mniejsza, to wstawiamy ją jako głowę Z
                mymerge(TX, Y, TZ)      %i merge z ogona X, całego Y, oraz ogona Z
            ;   Z = [HY | TZ],          %przeciwny przypadek - to samo
                mymerge(X, TY, TZ)
            )
        ;   Z = X                       %jeżeli X ma jakiś element, a Y nie, to Z to cała lista X
        ))
    ; Z = Y                             %jeżeli X jest pusty, to Z to cała lista Y
    )).
    
split(X, Y, Z) :-                       %split strumienia X w Y oraz Z
    freeze(X,                           %czekamy na wartość X
    (   X = [HX | TX]                   %jeżeli X ma co najmniej 1 element
    ->  Y = [HX | TY],                  %to wstawiamy go jako głowę Y
        split(TX, Z, TY)                %i rekurencja z (tutaj taki fajny triczek) ogonem X (bo wzięliśmy głowę do Y), Z jako drugi argument, bo do niego będziemy wstawiać ewentualne wartości z TX, oraz ogon TY
    ;   Y = [],                         %jeżeli X jest pusty, to zamykamy Y...
        Z = []                          %...i Z
    )).

merge_sort(X, Y) :-                     %merge sort listy X w listę Y
    freeze(X,                           %czekamy na wartość X
    (   X = [_ | TX]                    %jeżeli X ma co najmniej 1 element
    ->  freeze(TX,                      %czekamy na wartość ogona X
        (   TX = [_ | _]                %jeżeli ogon X (czyli TX) jest niepusty
        ->  split(X, X1, X2),           %to całego X splitujemy na dwa strumienie X1, X2
            merge_sort(X1, Y1),         %sortujemy rekurencyjnie X1 w Y1
            merge_sort(X2, Y2),         %sortujemy rekurencyjnie X2 w Y2
            mymerge(Y1, Y2, Y)          %scalamy w wynikowy Y.
        ; Y = X                         %Jeżeli X ma jeden element, to już jest posortowane - wyjście z rekurencji    
        ))
    ;   Y = X                           %Jeżeli X jest puste, to już jest posortowane - wyjście z rekurencji
    )).