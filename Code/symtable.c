#include "symtable.h"
#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct HashNode symTable[HASHSIZE];
struct HashTableInfo hashTableInfo;

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

struct SymNode *createSymNode(int type, ...) {
	struct SymNode *symNode;
	void **argc = (void **)&type + 1;
	symNode = malloc(sizeof(struct SymNode));
	if(!symNode) {
		fprintf(stderr, "out of space\n");
		exit(0);
	}
	symNode->type = type;
	symNode->name = malloc(sizeof(char) * strlen(*argc));
	strcpy(symNode->name, (char *)*argc);
	symNode->next = NULL;
	switch (symNode->type) {
	//TODO: add more symbol types
	case 1:
		return symNode;
		break;
	default:
		fprintf(stderr, "error type\n");
		freeSymNode(symNode);
		return NULL;
		break;
	}
}

int freeSymNode(struct SymNode *symNode) {
	if (symNode->next != NULL) {
		fprintf(stderr, "there is a symNode next to this, can't free this.\n");
		return 1;
	}
	free(symNode->name);
	switch (symNode->type) {
	//TODO: add more symbol types
	case 1:
		break;
	default:
		fprintf(stderr, "error type\n");
		break;
	}
	free(symNode);
	return 0;
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
 		hashTableInfo.num++;
		if (hashTableInfo.num > SYMBOLMAX) {
			fprintf(stderr, "too many symbols\n");
			exit(0);
		}
		return symTable[index].num;
	}
	while (list->next != NULL) {
		list = list->next;
	}
	list->next = symNode;
	symTable[index].num++;
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
		head = next;
	}
	return 0;
}

/*
	free all hashnode
*/
int cleanHashTable(void) {
	int i;
	for(i = hashTableInfo.num; i != 0; i--) {
		cleanSymList(symTable[i].symNode);
		hashTableInfo.num--;		
	}
	return 0;
}
int test(void) {
	char *name = "test";
	struct SymNode *symNode = NULL, *result = NULL;
	symNode = createSymNode(1, name);
	if (symNode == NULL) {
		fprintf(stderr, "creat fails\n");
		return 1;
	}
	fprintf(stdout, "symbol name %s\n", symNode->name);
	fprintf(stdout, "list %d\n", insert(symNode));
	result = lookup(name);
	if (result == NULL) {
		fprintf(stderr, "lookup fails\n");
		return 1;
	}
	else
		fprintf(stdout, "name is %s\n", result->name);
	return 0;
}
