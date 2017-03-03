a.out: main.o prog1.o slp.o util.o maxargs.o
	clang -g main.o prog1.o slp.o util.o maxargs.o

main.o: main.c slp.h util.h maxargs.h
	clang -g -c main.c

maxargs.o: maxargs.c maxargs.h slp.h
	clang -g -c maxargs.c

prog1.o: prog1.c slp.h util.h
	clang -g -c prog1.c

slp.o: slp.c slp.h util.h
	clang -g -c slp.c

util.o: util.c util.h
	clang -g -c util.c

clean:
	rm -f a.out util.o prog1.o slp.o main.o maxargs.o
