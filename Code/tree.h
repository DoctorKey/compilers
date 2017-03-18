#ifndef __TREE_H_
#define __TREE_H_

//#include "name.h"

#define CHILD_NUM 7
struct node {
	int nodetype;
	int depth;
	struct node *children[CHILD_NUM];
};

struct node *newNode(int type,int num, ...);
struct node *newtokenNode(int type,double num);
void clearTree(struct node *node);
void showTree(struct node *node);
#endif
