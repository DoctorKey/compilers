#include "tree.h"
#include "syntax.tab.h" 
#include <stdio.h>
#include <stdlib.h>

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
	void **argc = (void **)&num + 1;
	struct node *temp = malloc(sizeof(struct node));
	if(!temp) {
		printf("out of space");
		exit(0);
	}
	temp->nodetype = type;
	temp->depth = 0;
	for(i = 1;i <= num;i++) {
		temp->children[i - 1] = (struct node*)*argc;
		updateChildDepth(temp->children[i - 1]);
		argc++;
	}
	for(;i != CHILD_NUM + 1; i++) {
		temp->children[i - 1] = NULL;
	}
	return temp;
}
struct node *newtokenNode(int type,double num)
{
	int i;
	struct node *temp = malloc(sizeof(struct node));
	if(!temp) {
		printf("out of space");
		exit(0);
	}
	temp->nodetype = type;
	temp->depth = 0;
	for(i = 1;i != CHILD_NUM + 1; i++) {
		temp->children[i - 1] = NULL;
	}
	return temp;
}
void showTree(struct node *node)
{
	int i = 0;
	for(i = 0;i != node->depth; i++){
		printf("\t");
	}
	switch(node->nodetype){
	case TYPE:printf("TYPE");break;
	case ID:printf("ID");break;
	case LP:printf("LP");break;
	case RP:printf("RP");break;
	case LC:printf("LC");break;
	case RC:printf("RC");break;
	case Program:printf("Program");break;
	case ExtDefList:printf("ExtDefList");break;
	case ExtDef:printf("ExtDef");break;
	case Specifier:printf("Specifier");break;
	case FunDec:printf("FunDec");break;
	case CompSt:printf("CompSt");break;
	case StmtList:printf("StmtList");break;
	case DefList:printf("DefList");break;
	}
	printf("  %d\n", node->nodetype);
	for(i = 0;i != CHILD_NUM;i++) {
		if(node->children[i] != NULL){
			showTree(node->children[i]);
		}
	}
}
