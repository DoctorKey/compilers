#ifndef __SEMANTICHELP_H_
#define __SEMANTICHELP_H_

#include "symtable.h"

int cmpType(Type type1, Type type2); 
int cmpFieldList(FieldList fieldList1, FieldList fieldList2); 

int cmpFuncSym(Symbol left, Symbol right); 
int cmpFuncSymByName(Symbol left, Symbol right); 
int cmpFunc(struct Func *left, struct Func *right); 

struct FuncList {
	struct SymNode *funcSymbol;
	struct FuncList *next;
};
struct FuncList *getDecFuncList(); 
void showDecFuncList(); 
void addDecFunc(Symbol symbol); 
void freeDecFuncList(); 

#endif
