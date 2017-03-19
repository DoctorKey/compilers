#include "tree.h"
#include "syntax.tab.h" 
#include "name.h"
#include <stdio.h>
#include <stdlib.h>

extern int yylineno;
void updateChildDepth(struct node *node)
{
	int i;
	node->depth++;
	for(i = 0;i != CHILD_NUM; i++) {
		if(node->children[i] != NULL){
			updateChildDepth(node->children[i]);
		}
	}
}
struct node *newNode(int type,int num, ...)
{
	int i;
	/* if the file has more than minlineno lines, change the init value of minlineno */
	int minlineno = 9999;
	void **argc = (void **)&num + 1;
	struct node *temp = malloc(sizeof(struct node));
	if(!temp) {
		printf("out of space");
		exit(0);
	}
	temp->nodetype = type;
	temp->depth = 0;
	for(i = 0;i < num;i++, argc++) {
		if(*argc == NULL)
			continue;
		temp->children[i] = (struct node*)*argc;
		updateChildDepth(temp->children[i]);
		minlineno = temp->children[i]->lineno < minlineno? temp->children[i]->lineno: minlineno;
	}
	temp->lineno = minlineno;
	for(;i != CHILD_NUM; i++) {
		temp->children[i] = NULL;
	}
	return temp;
}
struct node *newtokenNode(int type,float nodevalue)
{
	int i;
	struct node *temp = malloc(sizeof(struct node));
	if(!temp) {
		printf("out of space");
		exit(0);
	}
	temp->nodetype = type;
	temp->lineno = yylineno;
	temp->depth = 0;
	if(type == INT){
		temp->nodevalue.INT = (int)nodevalue;
	}
	if(type == FLOAT){
		temp->nodevalue.FLOAT = nodevalue;
	}
/*	if(type == ID){
		temp->nodevalue.ID_index = (int)nodevalue;
	}
	*/
	for(i = 0;i != CHILD_NUM; i++) {
		temp->children[i] = NULL;
	}
	return temp;
}
void clearTree(struct node *node)
{
	int i = 0;
	for(i = 0;i != CHILD_NUM;i++) {
		if(node->children[i] != NULL) {
			clearTree(node->children[i]);
		}
	}
	free(node);
}
void showTree(struct node *node)
{
	int i = 0;
	for(i = 0;i != node->depth; i++){
		printf("    ");
	}
	printf("%s", getName(node->nodetype));
	if(node->nodetype >= INT){
		if(node->nodetype == INT){
			printf(": %d", node->nodevalue.INT);
		}
		if(node->nodetype == FLOAT){
			printf(": %f", node->nodevalue.FLOAT);
		}
		if(node->nodetype == ID){
			printf(": %s", node->nodevalue.str);
		}
		if(node->nodetype == TYPE){
			printf(": %s", node->nodevalue.str);
		}
	}else{
		printf(" (%d)", node->lineno);
	}
	printf("\n");
	for(i = 0;i != CHILD_NUM;i++) {
		if(node->children[i] != NULL){
			showTree(node->children[i]);
		}
	}
}
