#include "semantichelp.h"

/*
	compare two FieldList
	return 0 if the same
	       1 if different
*/
int cmpFieldList(FieldList fieldList1, FieldList fieldList2) {
	if(fieldList1 == NULL && fieldList2 == NULL)
		return 0;
	if(fieldList1 == NULL || fieldList2 == NULL)
		return 1;
	/*
	if(fieldList1->name != NULL && fieldList2->name != NULL) {
		if(strcmp(fieldList1->name, fieldList2->name))
			return 1;
	}*/
	if(cmpType(fieldList1->type, fieldList2->type))
		return 1;
	return cmpFieldList(fieldList1->tail, fieldList2->tail);
}
/*
	compare two Type
	return 0 if the same
	       1 if different
*/
int cmpType(Type type1, Type type2) {
	if(type1 == NULL && type2 == NULL)
		return 0;
	if(type1 == NULL || type2 == NULL)
		return 1;
	if(type1->kind == BASIC && type2->kind == BASIC) {
		if(type1->basic == type2->basic)
			return 0;
		else
			return 1;
	}
	if(type1->kind == ARRAY && type2->kind == ARRAY) {
		//don't compare the array size
		// just compare the dimension
		return cmpType(type1->array.elem, type2->array.elem);
	}
	if(type1->kind == STRUCTURE && type2->kind == STRUCTURE) {
		if(strcmp(type1->structname, type2->structname))
			return 1;
//		return cmpFieldList(type1->structure, type2->structure);
	}
	//type1->kind != type2->kind
	return 1;
}
int cmpFunc(struct Func *left, struct Func *right) {
	if(cmpType(left->Return, right->Return)) {
		return 1;
	} 
	if(cmpFieldList(left->argtype, right->argtype)) {
		return 1;
	} 
	return 0;
}
int cmpFuncSymByName(Symbol left, Symbol right) {
	if(left->type != Func || right->type != Func) {
		return 1;
	}	
	if(strcmp(left->name, right->name)) {
		return 1;
	}
	return 0;
}
int cmpFuncSym(Symbol left, Symbol right) {
	if(left->type != Func || right->type != Func) {
		return 1;
	}	
	if(strcmp(left->name, right->name)) {
		return 1;
	}
	if(cmpFunc(left->func, right->func)) {
		return 1;
	}
	return 0;
}

/*-----------------------------------------------------------------
	Define function
	Declare function
------------------------------------------------------------------
*/
struct FuncList *DecFuncList = NULL;

struct FuncList *getDecFuncList() {
	return DecFuncList;
}
void addFunc(struct FuncList **funcList, Symbol symbol) {
	struct FuncList *new = NULL;
	new = malloc(sizeof(struct FuncList));
	if (!new) {
		fprintf(stderr, "malloc fails\n");
		exit(0);
	}
	new->funcSymbol = symbol;
	new->next = NULL;
	if ((*funcList) == NULL) {
		(*funcList) = new;
	}else {
		(*funcList)->next = new;
	}
}
void addDecFunc(Symbol symbol) {
	addFunc(&DecFuncList, symbol);
}
void showDecFuncList() {
	struct FuncList *temp = DecFuncList;
	fprintf(stdout, "----------------DecFuncList-------------------\n");
	while (temp != NULL) {
		showSymbol(temp->funcSymbol);
		temp = temp->next;
	}
	fprintf(stdout, "----------------------------------------------\n");
}
void freeDecFuncList() {
	struct FuncList *temp = DecFuncList;
	struct FuncList *next = NULL;
	while(temp) {
		next = temp->next;
		free(temp);
		temp = next;
	}
	DecFuncList = NULL;
}
