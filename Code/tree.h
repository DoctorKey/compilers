
struct node {
	int nodetype;
	struct node *left;
	struct node *mid;
	struct node *right;
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

