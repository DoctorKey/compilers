#ifndef __SYMTABLE_H_
#define __SYMTABLE_H_
/*
	to use this symtable
	you should call createSymNode first to have a symNode;
	then you can call insert to add the symNode in hashTable;

	you can call lookup with name to check a symNode is in the hashTable;

	to free memory you need call freeSymNode or cleanHashTable
*/

#define SYMBOLMAX 100
#define HASHSIZE 0x3fff

//TODO: add more infomation
struct SymNode {
	int type;
	char *name;

	struct SymNode *next;
};
struct HashNode {
	//num of this hash node
	int num;
	//the head of list
	struct SymNode *symNode;
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

struct SymNode *createSymNode(int type, ...); 
int freeSymNode(struct SymNode *symNode); 
int insert(struct SymNode *symNode); 
struct SymNode *lookup(char *name);
int cleanHashTable(void); 
void getHashTableInfo(void); 
int test(void);


#endif
