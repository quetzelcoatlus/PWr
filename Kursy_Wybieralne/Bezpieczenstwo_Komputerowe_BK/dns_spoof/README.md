Program służący do DNS spoofing (zatruwanie DNS) w języku C.

Program nasłuchuje sieć, czekając aż ktoś zapyta o daną domenę. Gdy tak się stanie, odpowiednio modyfikuje pakiet, tworząc fałszywą odpowiedź, i wysyła go do nadawcy.

Sposób użycia (skopiowany z pliku main.c):

```
Kompilowac przez:
gcc -Wall -Wextra -pedantic main.c -lpcap -o main

Uruchamiac przez:
sudo ./main <strona> <adres ip, na ktory przekierowujemy>

Przklad uruchomienia:
sudo ./main www.spaceadventure.pl 216.18.168.16

Zabijac przez:
<CTRL+C>
```