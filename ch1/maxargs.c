#include "maxargs.h"

int maxargs(A_stm stm) {
	switch (stm->kind) {
		case A_compoundStm:
			return max(maxargs(stm->u.compound.stm1),
				   maxargs(stm->u.compound.stm2));
		case A_assignStm:
			return maxargs_exp(stm->u.assign.exp);
		case A_printStm:
			return max(count_args(stm->u.print.exps),
				   maxargs_expList(stm->u.print.exps));
		default:
			assert("!Wrong value for maxargs!");
	}
}

int max (int first, int second) {
	return first > second ? first : second;
}
int count_args(A_expList exps) {
	/* return 1 if 'kind' of expList is A_lastExpList */
	if (exps->kind == A_lastExpList)
		return 1;
	return 1 + count_args(exps->u.pair.tail);
}

int maxargs_expList(A_expList exps) {
	switch (exps->kind) {
		case A_pairExpList:
			return max(maxargs_exp(exps->u.pair.head), maxargs_expList(exps->u.pair.tail));
		case A_lastExpList:
			return maxargs_exp(exps->u.last);
		default:
			assert("ahhhhh!");
	}
}

int maxargs_exp(A_exp exp) {
	switch (exp->kind) {
		case A_idExp:
		case A_numExp:
			return 0;
		case A_opExp:
			return max(maxargs_exp(exp->u.op.left), maxargs_exp(exp->u.op.right));
		case A_eseqExp:
			return max(maxargs(exp->u.eseq.stm), maxargs_exp(exp->u.eseq.exp));
	}
}
