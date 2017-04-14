#include "semantic.h"
#include "name.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void ExtDefAnalyze(struct node *parent, int num) {
	
}
/*
	Specifiers
*/
void SpecifierAnalyze(struct node *parent, int num) {
	//only have one child
	struct node *child = parent->children[0];
	if (child == NULL) {
		fprintf(stderr, "Specifier have no child!\n");
		return;
	}
	switch (child->nodetype) {
	case TYPE:
		parent->type = newType();
		parent->type->kind = BASIC;
		if (!strcmp(parent->nodevalue.str, "int"))
			parent->type->basic = INT;
		else if (!strcmp(parent->nodevalue.str, "float"))
			parent->type->basic = FLOAT;
		else
			fprintf(stderr, "error type\n");
		break;
	case StructSpecifier:
		parent->type = child->type;
		break;
	default:
		fprintf(stderr, "Specifier child type error!\n");
		break;
	}
	
}
void StructSpecifierAnalyze(struct node *parent, int num) {
	struct node *child;
	struct SymNode *symNode = NULL;
	// STRUCT OptTag LC DefList RC
	if (num == 5) {
		child = parent->children[3];	// DefList
		parent->type = newType();
		parent->type->kind = STRUCTURE;
		parent->type->structure = child->fieldList;	
		child = parent->children[1];	//OptTag
		symNode = newNewType(child->nodevalue->str, parent->type);
		if (symNode == NULL) {
			fprintf(stderr, "can't create symNode\n");
			return;
		}
		if (insert(symNode)) {
			fprintf(stderr, "insert symNode error\n");
			return;
		}
	}
	// STRUCT Tag
	if (num == 2) {
		child = parent->children[1];	// Tag
		symNode = lookup(child->nodevalue->str);	
		if (symNode == NULL) {
			fprintf(stderr, "error no define struct %s\n", child->nodevalue->str); 
			return;
		}
		parent->type = symNode->var->type;
	}
}
void OptTagAnalyze(struct node *parent, int num) {
	// ID
	struct node *child = parent->children[0];
	if (child == NULL) {
		fprintf(stderr, "OptTag have no child\n");
		return;
	}
	parent->nodevalue->str = child->nodevalue->str;
}
void TagAnalyze(struct node *parent, int num) {
	// ID
	struct node *child = parent->children[0];
	if (child == NULL) {
		fprintf(stderr, "Tag have no child\n");
		return;
	}
	parent->nodevalue->str = child->nodevalue->str;
}

/*
	Declarators
*/
void VarDecAnalyze(struct node *parent, int num) {
	struct node *child = NULL;
	// ID
	if (num == 1) {
		child = parent->children[0];	// ID
		if (child == NULL) {
			fprintf(stderr, "VarDec's child ID is NULL\n");
			return;
		}
		parent->nodevalue->str = child->nodevalue->str;	
	}
	// VarDec LB INT RB
	if (num == 4) {
		child = parent->children[0];	// VarDec
		if (child == NULL) {
			fprintf(stderr, "VarDec's child VarDec is NULL\n");
			return;
		}
		parent->nodevalue->str = child->nodevalue->str;	
		parent->type = newType();
		parent->type->kind = ARRAY;
		parent->type->array.elem = child->type;
		child = parent->children[2];	// INT
		if (child == NULL) {
			fprintf(stderr, "VarDec's child INT is NULL\n");
			return;
		}
		parent->type->array.size = child->nodevalue->INT;
	}
}
/*
	Local Definitions
*/
void DefListAnalyze(struct node *parent, int num) {
	// Def DefList
	struct node *child = NULL;	
	child = parent->children[0];	// Def
	if (child == NULL) {
		fprintf(stderr, "DefList's child Def is NULL\n");
		return;
	}
	parent->fieldList = child->fieldList;
	//Maybe it is NULL ?
	child = parent->children[1];	// DefList
	// just link Def->tail = DefList
	parent->fieldList->tail = child->fieldList;
}
void DefAnalyze(struct node *parent, int num) {
	
}
void DecListAnalyze(struct node *parent, int num) {
	struct node *child = NULL;
	// Dec
	if (num == 1) {
	
	}
	// Dec COMMA DecList
	if (num == 3) {
		
	}
}
void DecAnalyze(struct node *parent, int num) {
	struct node *child = NULL;
	// VarDec
	if (num == 1) {
		child = parent->children[0];	//VarDec
		//TODO:
		parent->fieldList = newFieldList();	
	}
	// VarDec ASSIGNOP Exp
	if (num == 3) {
		//TODO: check VarDec type and Exp type
		//	but don't know VarDec type..
		//	can let VarDec type = Exp type temp. let Def to exam the type
	}
}
void semanticAnalyze(struct node *parent, int num) {
	switch (parent->nodetype) {
	case Program:	break;
	case ExtDefList:	break;
	case ExtDef:	break;
	case ExtDecList:	break;
	case Specifier:		SpecifierAnalyze(parent, num); 		break;
	case StructSpecifier:	StructSpecifierAnalyze(parent, num);	break;
	case OptTag:		OptTagAnalyze(parent, num);		break;
	case Tag:		TagAnalyze(parent, num);		break;
	case VarDec:	break;
	case FunDec:	break;
	case VarList:	break;
	case ParamDec:	break;
	case CompSt:	break;
	case StmtList:	break;
	case Stmt:	break; 
	case DefList:	break;
	case Def:	break;
	case DecList:	break;
	case Dec:	break;
	case Exp:	break;
	case Args:	break;
	}
}
