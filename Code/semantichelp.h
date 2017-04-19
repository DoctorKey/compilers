#ifndef __SEMANTICHELP_H_
#define __SEMANTICHELP_H_

#include "symtable.h"

int cmpType(Type type1, Type type2); 
int cmpFieldList(FieldList fieldList1, FieldList fieldList2); 

struct FuncList *getDecFuncList(); 
void showDecFuncList(); 
void addDecFunc(struct SymNode *symbol); 

int cmpFuncSym(struct SymNode *left, struct SymNode *right); 
int cmpFuncSymByName(struct SymNode *left, struct SymNode *right); 
int cmpFunc(struct Func *left, struct Func *right); 

#endif
