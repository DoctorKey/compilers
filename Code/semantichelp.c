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
	//TODO: no need to compare the name?
//	if(!strcmp(fieldList1->name, fieldList2->name))
//		return 1;
	if(fieldList1->name != NULL && fieldList2->name != NULL) {
		if(strcmp(fieldList1->name, fieldList2->name))
			return 1;
	}
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
		return cmpFieldList(type1->structure, type2->structure);
	}
	//type1->kind != type2->kind
	return 1;
}

Type *specifierstack = NULL;
int index = 0;
int specifierstacksize = 0;
int InitSpecifierStack()
{
	if(specifierstack == NULL) {
		specifierstack = malloc(sizeof(Type) * STACKSIZE);
		if (specifierstack == NULL) {
			fprintf(stderr, "malloc fails\n");
			return 1;
		}
		specifierstacksize = STACKSIZE;
		return 0;
	}else {
		fprintf(stderr, "specifierstack isn't NULL, can't init\n");
		return 1;
	}
}
int IncreaseSpecifierStack() {
	int i;
	Type *pre = specifierstack;
	specifierstack = malloc(sizeof(Type) * (specifierstacksize + INCREASE));
	if (specifierstack == NULL) {
		fprintf(stderr, "malloc fails\n");
		return 1;
	}
	for (i = 0; i != specifierstacksize; i++ ) {
		specifierstack[i] = pre[i];
	}
	free(pre);
	pre = NULL;
	specifierstacksize += INCREASE;
	return 0;
}
void PushSpecifier(Type type) {
	if (specifierstack == NULL) {
		if(InitSpecifierStack) {
			fprintf(stderr, "push Specifier fails\n");
			return;
		}
	}
	if (index >= specifierstacksize) {
		if(IncreaseSpecifierStack) {
			fprintf(stderr, "increase Specifier fails\n");
			return;
		}
	}
	specifierstack[index] = type;
	index++;
	return;
}
Type GetSpecifierByIndex(int i) {
	if (i >= index || i < 0) {
		fprintf(stderr, "just have %d Specifier, can't get %d\n", index, i);
		return NULL;
	}
	return specifierstack[i];
}
