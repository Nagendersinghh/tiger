#include <stdio.h>
#include "util.h"
#include "maxargs.h"
#include "interp.h"
#include "prog1.h"

int main() {
	printf("%d\n", maxargs(prog()));
	interpStm(prog(), NULL);
	return 0;
}
