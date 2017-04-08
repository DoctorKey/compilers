#ifndef __SEMANTIC_H_
#define __SEMANTIC_H_

#include "tree.h"

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;

struct Type_ {
	enum {BASIC, ARRAY, STRUCTURE} kind;
	union {
		int basic;
		struct {
			Type elem;
			int size;
		}array;
		FieldList structure;
	};
};
struct FieldList_ {
	char *name;
	Type type;
	FieldList tail;
};
void semanticAnalyze(struct node *parent, int num); 
void showType(Type type); 
void freeType(Type type); 
void showFieldList(FieldList fieldList); 
void freeFieldList(FieldList fieldList); 
#endif
