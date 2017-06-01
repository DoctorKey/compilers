#ifndef __ASM_H_
#define __ASM_H_

#include "stdio.h"

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
	char *label;
};

//kind: A_LABEL, A_J, A_JAL, A_JR
AsmCode genlabel(int kind, char *label);
AsmCode genLI(int x, int k);
AsmCode genMOVE(int x, int y);
AsmCode genADDI(int x, int y, int k);
//kind: A_ADD, A_SUB, A_MUL
AsmCode genCompute(int kind, int x, int y, int z);
AsmCode genDIV(int y, int z);
AsmCode genMFLO(int x);
AsmCode genLW(int x, int k, int y);
// *x = y // genSW(y, 0, x);
AsmCode genSW(int y, int k, int x);
//kind: A_BEQ, A_BNE, A_BGT, A_BLT, A_BGE, A_BLE
AsmCode genConJump(int kind, int x, int y, char *label);

void printfAsm(FILE *tag, AsmCode asmcode); 

typedef struct AsmCodes_* AsmCodes;
struct AsmCodes_ {
	AsmCode code;
	AsmCodes prev, next;
};
void printfAllAsm(FILE *tag); 
#endif

