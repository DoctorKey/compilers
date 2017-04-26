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
		struct {
			char *structname;
			ErrorInfo errorInfo;
			FieldList structure;
		};
	};
};
struct FieldList_ {
	char *name;
	Type type;
	FieldList tail;
	ErrorInfo errorInfo;
};

enum {Var, Func, Struct};

struct Var {
	Type type;
	int isDefine;
	int isDeclare;
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
		Type structure;
	};
	ErrorInfo errorInfo;
	Symbol next;
};

struct HashNode {
	int num;	//num of this hash node
	Symbol symNode;//the head of list
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

FieldList newFieldList(char *name, Type type, FieldList tail, ErrorInfo errorInfo); 
void freeType(Type type); 
void freeFieldList(FieldList fieldList); 

Symbol newStruct(char *name, Type type, ErrorInfo errorInfo); 
Symbol newVar(char *name, Type type, ErrorInfo errorInfo); 
Symbol newFunc(char *name, Type Return, FieldList argtype, ErrorInfo errorInfo); 

int freeSymNode(Symbol symNode); 

void showSymbol(Symbol symNode);
void showAllSymbol(void);

int insert(Symbol symNode); 
Symbol lookup(char *name);
Symbol lookupByType(char *name, int type); 
Type lookupFieldListElem(FieldList fieldList, char *name);
Type getSymType(Symbol symNode);

int cleanHashTable(void); 
void getHashTableInfo(void); 

#endif
