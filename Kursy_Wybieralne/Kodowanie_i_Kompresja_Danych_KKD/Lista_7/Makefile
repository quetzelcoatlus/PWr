# Przyklad uruchomienia potokowego:
# ./koder panTadeusz.txt | ./szum 0.000001 | ./dekoder dekod ; ./sprawdz panTadeusz.txt dekod

.PHONY = clean

all: koder szum dekoder sprawdz

koder: koder.c
	gcc -Wall -Wextra -std=c99 -pedantic -o koder koder.c

szum: szum.c
	gcc -Wall -Wextra -std=c99 -pedantic -o szum szum.c 

dekoder: dekoder.c
	gcc -Wall -Wextra -std=c99 -pedantic -o dekoder dekoder.c


sprawdz: sprawdz.c
	gcc -Wall -Wextra -std=c99 -pedantic -o sprawdz sprawdz.c

clean:
	rm -rf koder szum dekoder sprawdz
