#ifndef __TREE_H_
#define __TREE_H_

//#include "name.h"
#include "symtable.h"

#define CHILD_NUM 7

union value {
	int INT;
	float FLOAT;
	int ID_index;
	char *str;
};
struct node {
	int nodetype;
	Type type;
	FieldList fieldList;
	struct SymNode *symNode;
	union value nodevalue;
	int depth;
	int lineno;
	struct node *children[CHILD_NUM];
};

struct node *newNode(int type,int num, ...);
struct node *newtokenNode(int type);
void clearTree(struct node *node);
void showTree(struct node *node);

#endif
