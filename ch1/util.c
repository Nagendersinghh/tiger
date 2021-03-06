/*
 * util.c - Commonly used utility functions
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "util.h"

void *checked_malloc(int len) {
	void *p = malloc(len);
	if (!p) {
		fprintf(stderr, "\nOut of memeory!\n");
		exit(1);
	}
	return p;
}

string String(char *s) {
	string p = checked_malloc(strlen(s) + 1);
	strcpy(p, s);
	return p;
}
