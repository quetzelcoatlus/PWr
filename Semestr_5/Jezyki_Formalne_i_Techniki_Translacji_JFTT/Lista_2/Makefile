all: 1 2 3 4

1: 1.c
	gcc -o 1 1.c

1.c: 1.l
	flex -o 1.c 1.l

2: 2.c
	gcc -o 2 2.c

2.c: 2.l
	flex -o 2.c 2.l

3: 3.c
	gcc -o 3 3.c

3.c: 3.l
	flex -o 3.c 3.l

4: 4.c
	gcc -o 4 4.c

4.c: 4.l
	flex -o 4.c 4.l

clean:
	rm 1.c 1 2.c 2 3.c 3 4.c 4

