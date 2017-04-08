#include "semantic.h"
#include "name.h"
#include <stdio.h>
#include <stdlib.h>

void semanticAnalyze(struct node *parent, int num) {
	switch (parent->nodetype) {
	case Program:	break;
	case ExtDefList:	break;
	case ExtDef:	break;
	case ExtDecList:	break;
	case Specifier:	break;
	case StructSpecifier:	break;
	case OptTag:	break;
	case Tag:	break;
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
void showType(Type type) {
	if (type == NULL)
		return;
	switch (type->kind) {
	case BASIC:
		fprintf(stdout, "basic: %d", type->basic);
		break;
	case ARRAY:
		fprintf(stdout, "array ");
		showType(type->array.elem);
		fprintf(stdout, "size: %d", type->array.size);
		break;
	case STRUCTURE:
		fprintf(stdout, "structure ");
		showFieldList(type->structure);
		break;
	}
}
void freeType(Type type) {
	if (type == NULL)
		return;
	switch (type->kind) {
	case BASIC:
		break;
	case ARRAY:
		freeType(type->array.elem);
		type->array.elem = NULL;
		break;
	case STRUCTURE:
		freeFieldList(type->structure);
		type->structure = NULL;
		break;
	}
	free(type);
	type = NULL;
}
void showFieldList(FieldList fieldList) {
	if (fieldList == NULL)
		return;
	fprintf(stdout, "name: ", fieldList->name);
	showType(fieldList->type);
	showFieldList(fieldList->tail);
}
void freeFieldList(FieldList fieldList) {
	if (fieldList == NULL)
		return;
	freeType(fieldList->type);
	freeFieldList(fieldList->tail);
	free(fieldList);
	fieldList = NULL;
}
