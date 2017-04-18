#ifndef __SEMANTICHELP_H_
#define __SEMANTICHELP_H_

#include "symtable.h"

#define STACKSIZE 100
#define INCREASE 20

int cmpType(Type type1, Type type2); 
int cmpFieldList(FieldList fieldList1, FieldList fieldList2); 
void PushSpecifier(Type type); 
Type GetSpecifierByIndex(int i); 
#endif
