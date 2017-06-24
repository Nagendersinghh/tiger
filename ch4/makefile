#a.out: parsetest.o y.tab.o lex.yy.o errormsg.o util.o
#	cc -g parsetest.o y.tab.o lex.yy.o errormsg.o util.o

#parsetest.o: parsetest.c errormsg.h util.h
#	cc -g -c parsetest.c
all: a.out

a.out: parse.o prabsyn.o y.tab.o lex.yy.o errormsg.o absyn.o util.o symbol.o table.o
	cc -g parse.o prabsyn.o y.tab.o lex.yy.o errormsg.o absyn.o util.o symbol.o table.o

parse.o: parse.c parse.h util.h symbol.h absyn.h errormsg.h
	cc -g -c parse.c

y.tab.o: y.tab.c
	cc -g -c y.tab.c

y.tab.c: tiger.grm
	yacc -dv tiger.grm

y.tab.h: y.tab.c
	echo "y.tab.h was created at the same time as y.tab.c"

absyn.o: absyn.c absyn.h util.h symbol.h
	cc -g -c absyn.c

prabsyn.o: prabsyn.c prabsyn.h util.h symbol.h absyn.h
	cc -g -c prabsyn.c

symbol.o: symbol.c symbol.h util.h table.h
	cc -g -c symbol.c

errormsg.o: errormsg.c errormsg.h util.h
	cc -g -c errormsg.c

lex.yy.o: lex.yy.c y.tab.h errormsg.h util.h symbol.h absyn.h
	cc -g -c lex.yy.c

lex.yy.c: tiger.lex
	lex tiger.lex

util.o: util.c util.h
	cc -g -c util.c

clean: 
	rm -f a.out util.o parsetest.o lex.yy.o lex.yy.c symbol.o absyn.o parse.o errormsg.o y.tab.c y.tab.h y.tab.o
