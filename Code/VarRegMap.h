#ifndef __VARREGMAP_H_
#define __VARREGMAP_H_

#include "IR.h"

typedef struct varregmap_* varregmap;
struct varregmap_ {
	struct Operand_* op;
	int num;
	int *varvec;
};

void initmap(InterCodes IRhead);
#endif
