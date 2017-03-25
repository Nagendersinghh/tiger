#ifndef PERSISTENT_H
#define PERSISTENT_H

typedef struct tree *P_tree;
struct tree {
	P_tree left;
	P_tree right;
	string key;
};

P_tree Tree(P_tree left, string key, P_tree right);

P_tree insert(string key, P_tree t);

int member(string key, P_tree t);

#endif
