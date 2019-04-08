#
#   Nazwa: Makefile
#   Opis: Zautomatyzowana kompilacja kompilatora
#   Autor: Jan Brzeżański
#   Indeks: 236814


.PHONY = debug clean cleanAll

OBJECTS = parser.o lexer.o imath.o lib.o main.o
FLAGS = -Wall -Wextra -std=c99 -pedantic
DEFINES = -D_XOPEN_SOURCE=500

kompilator: ${OBJECTS} Makefile
	gcc ${FLAGS} ${DEFINES} -o kompilator ${OBJECTS}

parser.o: comp_parser.y
	bison -Wall -o comp_y.c -d comp_parser.y
	gcc -c ${FLAGS} -o parser.o comp_y.c

lexer.o: comp_lexer.l comp_y.h
	flex -o comp_l.c comp_lexer.l
	gcc -c ${FLAGS} ${DEFINES} -o lexer.o comp_l.c

imath.o: imath.h imath.c
	gcc -c ${FLAGS} -o imath.o imath.c

lib.o: comp_lib.h comp_lib.c
	gcc -c ${FLAGS} ${DEFINES} -o lib.o comp_lib.c

main.o: main.c
	gcc -c ${FLAGS} -o main.o main.c

debug:
	gcc -g ${FLAGS} ${DEFINES} -o kompilator ${OBJECTS}

clean:
	rm -f comp_y.c comp_l.c comp_y.h ${OBJECTS}

cleanAll: clean
	rm -f kompilator