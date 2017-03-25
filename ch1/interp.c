#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "interp.h"

/* Constructor function for table */
Table_ Table(string id, int value, struct table *tail) {
	Table_ t = malloc(sizeof(*t));
	t->id = id;
	t->value = value;
	t->tail = tail;
	return t;
}

/* Constructor function for IntAndTable_ */
IntAndTable_ IntAndTable(int i, Table_ t) {
	IntAndTable_ it = malloc(sizeof(*it));
	it->i = i;
	it->t = t;
	return it;
}

Table_ update(Table_ t, string id, int value) {
	Table_ new_table = Table(id, value, t);
	return new_table;
}

int lookup(Table_ t, string id) {
	while (t != NULL) {
		if (t->id == id)
			return t->value;
		t = t->tail;
	}
	assert("lookup failed");
}

Table_ interpStm(A_stm stm, Table_ t) {

	IntAndTable_ it;

	switch (stm->kind) {
		case A_compoundStm:
			t = interpStm(stm->u.compound.stm1, t);
			t = interpStm(stm->u.compound.stm2, t);
			return t;

		case A_assignStm:
			it = interpExp(stm->u.assign.exp, t);
			t = update(it->t, stm->u.assign.id, it->i);
			return t;

		case A_printStm:
			it = interpExpList(stm->u.print.exps, t);
			return it->t;

		default:
			assert("ahhhhh!, can not interpret statement");
	}
	return t;
}

IntAndTable_ interpExp(A_exp exp, Table_ t) {
	switch (exp->kind) {
		case A_idExp:
			return IntAndTable(lookup(t, exp->u.id), t);

		case A_numExp:
			return IntAndTable(exp->u.num, t);

		case A_opExp:
		{
			int lval, rval;
			IntAndTable_ it_tmp;
			it_tmp = interpExp(exp->u.op.left, t);
			lval = it_tmp->i;
			it_tmp = interpExp(exp->u.op.right, it_tmp->t);
			rval = it_tmp->i;

			int value;
			switch (exp->u.op.oper) {
				case A_plus:
					value = lval + rval;
					break;
				case A_minus:
					value = lval - rval;
					break;
				case A_times:
					value = lval * rval;
					break;
				case A_div:
					value = lval/rval;
					break;
			}
			return IntAndTable(value, it_tmp->t);
		}

		case A_eseqExp:
			t = interpStm(exp->u.eseq.stm, t);
			return interpExp(exp->u.eseq.exp, t);

		default:
			assert("Wrong!");
	}
}

IntAndTable_ interpExpList(A_expList exps, Table_ t) {

	IntAndTable_ it;

	switch (exps->kind) {
		case A_pairExpList:
			it = interpExp(exps->u.pair.head, t);
			printf("%d\t", it->i);
			return interpExpList(exps->u.pair.tail, it->t);

		case A_lastExpList:
			it = interpExp(exps->u.last, t);
			printf("%d\n", it->i);
			return it;

		defaule:
			assert("interpExpList not working!!!!");
	}
}
