#ifndef INTERP_H
#define INTERP_H
#include <stdlib.h>
#include "slp.h"

typedef struct table *Table_;
struct table {
	string id;
	int value;
	Table_ tail;
};

/* Constructor function for table */
Table_ Table(string id, int value, struct table *tail);

typedef struct intAndTable *IntAndTable_;
struct intAndTable {
	int i;
	Table_ t;
};
IntAndTable_ IntAndTable(int i, Table_ t);

/*
 * Produce a new table from the specified table. The new table is same as
 * the original one except some identifiers map to different values as result
 * of executing the given statement.
 */
Table_ interpStm(A_stm stm, Table_ t);

IntAndTable_ interpExp(A_exp exp, Table_ t);

IntAndTable_ interpExpList(A_expList exps, Table_ t);

Table_ update(Table_ t, string id, int value);

int lookup(Table_ t, string id);
#endif
