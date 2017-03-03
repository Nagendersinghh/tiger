##ifndef INTERP_H
#define INTERP_H
#include "slp.h"

typedef struct table *Table_;
struct table {
	string id;
	int value;
	Table_ tail;
}

/* Constructor function for table */
Table_ Table(string id, int value, struct table *tail) {
	Table_ t = malloc(sizeof(*t));
	t->id = id;
	t->value = value;
	t->tail = tail;
}
#endif
