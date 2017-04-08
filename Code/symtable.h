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
#include "semantic.h"

#define SYMBOLMAX 100
#define HASHSIZE 0x3fff

enum {Var, Func};

struct Var {
	Type type;
};
struct Func {
	Type Return;
	int argc;
	Type *argtype;
};
struct SymNode {
	int type;
	char *name;
	union {
		struct Var *var;
		struct Func *func;
	};
	struct SymNode *next;
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

struct SymNode *newVar(char *name, Type type); 
void showVar(struct Var *var); 
struct SymNode *newFunc(char *name, Type Return, int argc, ...); 
void showFunc(struct Func *func);
int freeSymNode(struct SymNode *symNode); 
void showSymbol(struct SymNode *symNode);
void showAllSymbol(void);

int insert(struct SymNode *symNode); 
struct SymNode *lookup(char *name);
int cleanHashTable(void); 
void getHashTableInfo(void); 
int test(void);


#endif
