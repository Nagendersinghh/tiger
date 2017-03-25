a.out: main.o prog1.o slp.o util.o maxargs.o interp.o
	clang -g main.o prog1.o slp.o util.o maxargs.o interp.o

main.o: main.c slp.h util.h maxargs.h interp.h
	clang -g -c main.c

interp.o: interp.c interp.h slp.h util.h
	clang -g -c interp.c

persistent_tree: test_persistent_tree.c persistent_tree.c persistent_tree.h util.h
	clang test_persistent_tree.c persistent_tree.c util.c -o persistent_tree

persistent_tree.o: persistent_tree.c util.h
	clang -c persistent_tree.c util.c

maxargs.o: maxargs.c maxargs.h slp.h util.h
	clang -g -c maxargs.c

prog1.o: prog1.c slp.h util.h
	clang -g -c prog1.c

slp.o: slp.c slp.h util.h
	clang -g -c slp.c

util.o: util.c util.h
	clang -g -c util.c

clean:
	rm -f a.out util.o prog1.o slp.o main.o maxargs.o interp.o persistent_tree
