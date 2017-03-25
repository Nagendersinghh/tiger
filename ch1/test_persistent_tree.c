#include <stdio.h>
#include <stdlib.h>
#include "util.h"
#include "persistent_tree.h"

int main() {
	P_tree t = insert(String("what"), NULL);
	free(t);
}
