#include "symtable.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct HashNode symTable[HASHSIZE];
struct HashTableInfo hashTableInfo;

struct SymNode *createSymNode(int type, char *name) {
	struct SymNode *symNode;
	symNode = malloc(sizeof(struct SymNode));
	if(!symNode) {
		fprintf(stderr, "out of space\n");
		exit(0);
	}
	hashTableInfo.allocTimes++;

	symNode->type = type;
	symNode->name = malloc(sizeof(char) * strlen(name));
	strcpy(symNode->name, name);
	symNode->next = NULL;
	return symNode;
}
struct SymNode *newVar(char *name, Type type) {
	struct Var *var = NULL;
	struct SymNode *symNode = NULL;
	var = malloc(sizeof(struct Var));
	if(!var) {
		fprintf(stderr, "out of space\n");
		exit(0);
	}
	var->type = type;
 	symNode = createSymNode(Var, name);
	symNode->var = var;
	return symNode;
}
void freeVar(struct Var *var) {
	if (var == NULL)
		return;
	freeType(var->type);
	var->type = NULL;
	free(var);
	var = NULL;
	return;
}
void showVar(struct Var *var) {
	fprintf(stdout, "var type: ");
	showType(var->type);	
	fprintf(stdout, "\n");
}
struct SymNode *newFunc(char *name, Type Return, int argc, ...) {
	int i;
	struct Func *func = NULL;
	struct SymNode *symNode = NULL;
	void **argv = (void **)&argc + 1;
	func = malloc(sizeof(struct Func));
	if(!func) {
		fprintf(stderr, "out of space\n");
		exit(0);
	}
	func->argtype = malloc(sizeof(Type) * argc);	
	if(!func->argtype) {
		fprintf(stderr, "out of space\n");
		exit(0);
	}
	func->Return = Return;
	func->argc = argc;
	for (i = 0; i != argc; i++, argv++) {
		func->argtype[i] = (Type) *argv;
	}
 	symNode = createSymNode(Func, name);
	return symNode;
}
void freeFunc(struct Func *func) {
	int i;
	if (func == NULL)
		return;
	for (i = 0; i != func->argc; i++) {
		freeType(func->argtype[i]);
		func->argtype[i] = NULL;
	}
	free(func->argtype);
	func->argtype = NULL;
	free(func);
	func = NULL;
	return;
}
void showFunc(struct Func *func) {
	int i;
	fprintf(stdout, "func return type: ");
	showType(func->Return);	
	fprintf(stdout, "\n");
	fprintf(stdout, "func argc: %d\n", func->argc);
	fprintf(stdout, "func args type: ");
	for (i = 0; i != func->argc; i++) {
		showType(func->argtype[i]);	
		fprintf(stdout, " ");
	}
	fprintf(stdout, "\n");
}

int freeSymNode(struct SymNode *symNode) {
	if (symNode == NULL) {
		return 0;
	}
	if (symNode->next != NULL) {
		fprintf(stderr, "there is a symNode next to this, can't free this.\n");
		return 1;
	}
	free(symNode->name);
	symNode->name = NULL;
	switch (symNode->type) {
	case Var:
		freeVar(symNode->var);
		break;
	case Func:
		freeFunc(symNode->func);
		break;
	default:
		fprintf(stderr, "error type\n");
		break;
	}
	free(symNode);
	symNode = NULL;
	hashTableInfo.freeTimes++;
	return 0;
}
void showSymbol(struct SymNode *symNode) {
	if (symNode == NULL)
		return;
	switch (symNode->type) {
	case Var:
		fprintf(stdout, "Var: ");
		fprintf(stdout, "%s\n", symNode->name);
		showVar(symNode->var);
		break;
	case Func:
		fprintf(stdout, "Func: ");
		fprintf(stdout, "%s\n", symNode->name);
		showFunc(symNode->func);
		break;
	default:
		fprintf(stderr, "error type\n");
		break;
	}
}
unsigned int hash_pjw(char *name) {
	unsigned int val = 0, i;
	char *temp = name;
	for (; *name; ++name) {
		val = (val << 2) + *name;
		if (i = val & ~HASHSIZE)
			val = (val ^ (i >> 12)) & HASHSIZE;
	}
	if (debug2 == 1) {
		fprintf(stdout, "%s hash val %d\n", temp, val);
	}
	return val;
}
/*
	insert a symbol in hashTable
	symNode: symNode to insert
	return: the num of symnode in HashNode
*/
int insert(struct SymNode *symNode) {
	unsigned int index;
	struct SymNode *list;
	index = hash_pjw(symNode->name);
	list = symTable[index].symNode;
	if (list == NULL) {
		symTable[index].symNode = symNode;
		symTable[index].num++;

		hashTableInfo.usedIndex[hashTableInfo.indexNum] = index;
 		hashTableInfo.indexNum++;
		if (hashTableInfo.indexNum > SYMBOLMAX) {
			fprintf(stderr, "too many symbols\n");
			exit(0);
		}
		hashTableInfo.symbolNum++;

		return symTable[index].num;
	}
	while (list->next != NULL) {
		list = list->next;
	}
	list->next = symNode;
	symTable[index].num++;
	hashTableInfo.symbolNum++;
	return symTable[index].num;
}

/*
	name: name of symNode
	return: if hit return symNode
		else return null
*/
struct SymNode *lookup(char *name) {
	unsigned int index;
	struct SymNode *result, *list;
	index = hash_pjw(name);
	list = symTable[index].symNode;
	if (list == NULL) {
		return NULL;
	}
	while (list != NULL) {
		if (strcmp(list->name, name) == 0)
			return list;
		list = list->next;
	}
	return NULL;
}

/*
	free all symnodes in a hashnode
*/
int cleanSymList(struct SymNode *head) {
	struct SymNode *next;
	if (head == NULL)
		return 0;
	while (head != NULL) {
		next = head->next;
		head->next = NULL;
		freeSymNode(head);
		hashTableInfo.symbolNum--;
		head = next;
	}
	return 0;
}
int showSymList(struct SymNode *head) {
	if (head == NULL)
		return 0;
	while (head != NULL) {
		showSymbol(head);
		head = head->next;
	}
	return 0;
}

/*
	free all hashnode
*/
int cleanHashTable(void) {
	int i,index;
	for(i = hashTableInfo.indexNum - 1; i >= 0; i--) {
		index = hashTableInfo.usedIndex[i];
		cleanSymList(symTable[index].symNode);
		hashTableInfo.usedIndex[i] = 0;
		hashTableInfo.indexNum--;		
	}
	return 0;
}
void showAllSymbol(void) {
	int i, index;
	for(i = hashTableInfo.indexNum - 1; i >= 0; i--) {
		index = hashTableInfo.usedIndex[i];
		showSymList(symTable[index].symNode);
	}
}
void getHashTableInfo(void) {
	int i;
	fprintf(stdout, "--------------------------------------\n");
	fprintf(stdout, "hashTableInfo:\n");
	fprintf(stdout, "\tallocTimes:%d", hashTableInfo.allocTimes);
	fprintf(stdout, "\tfreeTimes:%d\n", hashTableInfo.freeTimes);
	fprintf(stdout, "\tsymbolNum:%d", hashTableInfo.symbolNum);
	fprintf(stdout, "\tindexNum:%d\n", hashTableInfo.indexNum);
	fprintf(stdout, "\tindex: ");
	for (i = 0; i != hashTableInfo.indexNum; i++) {
		fprintf(stdout, "%d ", hashTableInfo.usedIndex[i]);
	}
	fprintf(stdout, "\n--------------------------------------\n");
}
int test(void) {
	char *name = "test", *name2 = "test2";
	struct SymNode *symNode = NULL, *result = NULL;
	Type type;
	
	type = malloc(sizeof(Type));
	type->kind = BASIC;
	type->basic = 5;

	symNode = newVar(name, type);
	getHashTableInfo();
	if (symNode == NULL) {
		fprintf(stderr, "creat fails\n");
		return 1;
	}
	showSymbol(symNode);
	fprintf(stdout, "symbol name %s\n", symNode->name);
	fprintf(stdout, "list %d\n", insert(symNode));
	getHashTableInfo();

	symNode = newVar(name2, type);
	showSymbol(symNode);
	getHashTableInfo();
	if (symNode == NULL) {
		fprintf(stderr, "creat fails\n");
		return 1;
	}
	fprintf(stdout, "symbol name %s\n", symNode->name);
	fprintf(stdout, "list %d\n", insert(symNode));
	getHashTableInfo();

	fprintf(stdout, "show all symbols\n");
	showAllSymbol();
	result = lookup(name);
	if (result == NULL) {
		fprintf(stderr, "lookup fails\n");
		return 1;
	}
	else
		fprintf(stdout, "name is %s\n", result->name);
	cleanHashTable();
	getHashTableInfo();
	return 0;
}
