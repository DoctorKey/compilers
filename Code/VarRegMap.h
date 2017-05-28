#ifndef __VARREGMAP_H_
#define __VARREGMAP_H_

#include "IR.h"

typedef struct varregmap_* varregmap;
struct varregmap_ {
	struct Operand_* op;
	int reg;
	int num;
	int *varvec;
};
extern int dimension;

int getDimension();
void initmap(InterCodes IRhead);
void Free(int reg); 
int Ensure(Operand op); 
#endif
