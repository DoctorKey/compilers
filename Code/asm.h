#ifndef __ASM_H_
#define __ASM_H_

#include "IR.h"

typedef enum {
	A_LABEL, A_LI, A_MOVE,
	A_ADDI, A_ADD, A_SUB, A_MUL, A_DIV,
	A_MFLO, A_LW, A_SW, A_J, A_JAL, A_JR,
	A_BEQ, A_BNE, A_BGT, A_BLT, A_BGE, A_BLE }AsmCodeKind;
typedef struct AsmCode_* AsmCode;
struct AsmCode_ {
	AsmCodeKind kind;
	int x;
	int y;
	int z;
	int k;
	char *f;
};

void transAllAsm(InterCodes IRhead);

typedef struct AsmCodes_* AsmCodes;
struct AsmCodes_ {
	AsmCode code;
	AsmCodes prev, next;
};
void printfAllAsm(FILE *tag); 
#endif

