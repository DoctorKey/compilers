#ifndef __VARREGMAP_H_
#define __VARREGMAP_H_

#include "IR.h"

typedef struct varregmap_* varregmap;
struct varregmap_ {
	struct Operand_* op;
	int num;
	int *varvec;
	int reg;
	Mem mem;
};
extern int dimension;

void printfVar2RegTable(FILE *tag); 
void printfVarByVec(FILE *tag, int *varvec);
int getDimension();
void initmap(InterCodes IRhead);
void Free(int reg); 
int Ensure(Operand op); 

typedef struct RegSet_* RegSet;
struct RegSet_ {
	int rx;
	int ry;
	int rz;
};
#endif
