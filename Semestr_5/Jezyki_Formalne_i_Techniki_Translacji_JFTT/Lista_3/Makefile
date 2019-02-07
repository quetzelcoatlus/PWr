.PHONY = qshar run clean cleanall

cal: cal.y cal.l
	bison -o cal_y.c -d cal.y
	flex -o cal_l.c cal.l
	gcc -o cal cal_y.c cal_l.c

run:
	./cal < tests
	
clean:
	rm -f *.c *.h

cleanall: clean
	rm -f cal

