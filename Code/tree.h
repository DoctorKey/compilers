#ifndef __TREE_H_
#define __TREE_H_
#define CHILD_NUM 7
enum {
	Program, ExtDefList, ExtDef, ExtDecList,
	Specifier, StructSpecifier, OptTag, Tag,
	VarDec, FunDec, VarList, ParamDec,
	CompSt, StmtList, Stmt, 
	DefList, Def, DecList, Dec,
	Exp, Args
};
struct node {
	int nodetype;
	int depth;
	struct node *children[CHILD_NUM];
};

struct node *newNode(int type,int num, ...);
void showTree(struct node *node);
/*
struct node *newNode_1(int, struct node*);
struct node *newNode_2(int, struct node*, struct node*);
struct node *newNode_3(int, struct node*, struct node*, struct node*);
struct node *newNode_4(int, struct node*, struct node*, struct node*, struct node*);
struct node *newNode_5(int, struct node*, struct node*, struct node*, struct node*, struct node*);
struct node *newNode_6(int, struct node*, struct node*, struct node*, struct node*, struct node*, struct node*);
struct node *newNode_7(int, struct node*, struct node*, struct node*, struct node*, struct node*, struct node*, struct node*);
*/
/*
struct ProgramNode {
	int nodetype;
	struct ExtDefListNode *mid;
};
struct ExtDefListNode {
	int nodetype;
	struct ExtDefNode *left;
	struct ExtDefListNode *right;
};
struct ExtDefNode {
	int nodetype;
	struct SpecifierNode *left;

};
struct stmtnode {
	int nodetype;

};

struct expnode {
	int nodetype;
	struct expnode *left;
	struct expnode *right;

};
struct symnode {
	int nodetype;
	char *s;
};

struct numnode {
	int nodetype;
	int int_num;
	double double_num;
};

struct ExpNode *newExpNode(int,struct ExpNode*,struct ExpNode*);
*/
#endif
