a.out: parse.o prabsyn.o y.tab.o lex.yy.o errormsg.o absyn.o util.o symbol.o table.o semant.o env.o types.o
	cc -g parse.o prabsyn.o y.tab.o lex.yy.o errormsg.o absyn.o util.o symbol.o table.o semant.o env.o types.o

parse.o: parse.c parse.h util.h symbol.h absyn.h errormsg.h semant.h
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

env.o: env.c env.h types.h util.h symbol.h
	cc -g -c env.c

table.o: table.c table.h util.h
	cc -g -c table.c

types.o: types.c types.h
	cc -g -c types.c

symbol.o: symbol.c symbol.h util.h table.h
	cc -g -c symbol.c

semant.o: semant.c semant.h absyn.h env.h symbol.h trans.h
	cc -g -c semant.c

errormsg.o: errormsg.c errormsg.h util.h
	cc -g -c errormsg.c

lex.yy.o: lex.yy.c y.tab.h errormsg.h util.h symbol.h absyn.h
	cc -g -c lex.yy.c

lex.yy.c: tiger.lex
	lex tiger.lex

util.o: util.c util.h
	cc -g -c util.c

clean: 
	rm -f a.out util.o parsetest.o lex.yy.o lex.yy.c symbol.o prabsyn.o types.o table.o absyn.o parse.o types.o env.o semant.o errormsg.o y.tab.c y.tab.h y.tab.o
