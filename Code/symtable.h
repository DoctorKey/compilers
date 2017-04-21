#ifndef __SYMTABLE_H_
#define __SYMTABLE_H_
/*
	to use this symtable
	you should call newVar or newFunc first to have a symNode;
	then you can call insert to add the symNode in hashTable;

	you can call lookup with name to check a symNode is in the hashTable;

	to free memory you need call freeSymNode or cleanHashTable

	to show some infomation you need call showSymbol or showAllSymbol or getHashTableInfo
*/

#include "error.h"

#define SYMBOLMAX 100
#define HASHSIZE 0x3fff

typedef struct Type_* Type;
typedef struct FieldList_* FieldList;
typedef struct SymNode* Symbol;

struct Type_ {
	enum {BASIC, ARRAY, STRUCTURE, ERROR} kind;
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

enum {Var, Func, NewType};

struct Var {
	Type type;
	int isDefine;
	int isDeclare;
	union {
		int intValue;
		float floatValue;
	};
};

struct Func {
	Type Return;
	int argc;
	int isDefine;
	int isDeclare;
	FieldList argtype;
};
struct SymNode {
	int type;
	char *name;
	union {
		struct Var *var;
		struct Func *func;
		Type specifier;
	};
	struct ErrorInfo *errorInfo;
	struct SymNode *next;
};

struct FuncList {
	struct SymNode *funcSymbol;
	struct FuncList *next;
};

struct HashNode {
	int num;	//num of this hash node
	struct SymNode *symNode;//the head of list
};
struct HashTableInfo {
	// alloc symbols times
	int allocTimes;
	// free symbols times
	int freeTimes;
	//num of symbol in table, every insert plus 1
	int symbolNum;
	// num of used index
	int indexNum;
	//all used index
	int usedIndex[SYMBOLMAX];
};
extern struct HashNode symTable[HASHSIZE];

Type newType(); 
void showType(Type type); 
void showFieldList(FieldList fieldList); 

FieldList newFieldList(char *name, Type type, FieldList tail); 
void freeType(Type type); 
void freeFieldList(FieldList fieldList); 

struct SymNode *newNewType(char *name, Type type, struct ErrorInfo *errorInfo); 
struct SymNode *newVar(char *name, Type type, struct ErrorInfo *errorInfo); 
struct SymNode *newFunc(char *name, Type Return, FieldList argtype, struct ErrorInfo *errorInfo); 

int freeSymNode(struct SymNode *symNode); 

void showSymbol(struct SymNode *symNode);
void showAllSymbol(void);

int insert(struct SymNode *symNode); 
int removeSymbol(char *name); 
struct SymNode *lookup(char *name);
Type lookupFieldListElem(FieldList fieldList, char *name);
Type getSymType(struct SymNode *symNode);

int cleanHashTable(void); 
void getHashTableInfo(void); 

#endif
