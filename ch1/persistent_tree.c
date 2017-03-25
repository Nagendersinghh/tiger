#include <stdio.h>
#include <string.h>
#include "util.h"
#include "persistent_tree.h"

P_tree Tree(P_tree left, string key, P_tree right) {
	P_tree t = checked_malloc(sizeof(*t));
	t->left = left;
	t->right = right;
	t->key = key;
	return t;
}

P_tree insert(string key, P_tree t) {
	if (t == NULL)
		return Tree(NULL, key, NULL);

	if (strcmp(key, t->key) < 0) {
		return Tree(insert(key, t->left), t->key, t->right);
	} else if (strcmp(key, t->key) > 0) {
		return Tree(t->left, t->key, insert(key, t->right));
	} else
		return Tree(t->left, key, t->right);
}

int member(string key, P_tree t) {
	if (t == NULL)
		return FALSE;

	if (strcmp(key, t->key) == 0)
		return TRUE;
	else if (strcmp(key, t->key) < 0)
		return member(key, t->left);
	else	return member(key, t->right);
}
