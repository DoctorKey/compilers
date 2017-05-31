#include "asm.h"
#include "mips32.h"
#include "VarRegMap.h"
#include "transAsm.h"

AsmCodes AsmHead = NULL;
AsmCodes AsmTail = NULL;
AsmCode newAsmCode(AsmCodeKind kind) {
	AsmCode result = NULL;
	result = (AsmCode) malloc(sizeof(struct AsmCode_));
	if(result == NULL) {
		fprintf(stderr, "can't malloc\n");
		return NULL;
	}
	result->kind = kind;
	result->x = 0;
	result->y = 0;
	result->z = 0;
	result->k = 0;
	result->label = NULL;
	return result;
}
AsmCodes newAsmCodes(AsmCode asmcode) {
	AsmCodes result = NULL;
	result = (AsmCodes) malloc(sizeof(struct AsmCodes_));
	if(result == NULL) {
		fprintf(stderr, "can't malloc\n");
		return NULL;
	}
	result->code = asmcode;
	result->prev = NULL;
	result->next = NULL;
	return result;
}
void addAsmCode(AsmCode code) {
	if(AsmHead == NULL) {
		AsmHead = newAsmCodes(code);
		AsmTail = AsmHead;
	}else {
		AsmTail->next = newAsmCodes(code);
		AsmTail->next->prev = AsmTail;
		AsmTail = AsmTail->next;
	}
#ifdef DEBUG4
	printfAsm(stdout, code); 
	fprintf(stdout, "\n");
#endif
}

//kind: A_LABEL, A_J, A_JAL, A_JR
AsmCode genlabel(int kind, char *label) {
	AsmCode code;
	code = newAsmCode(kind);
	code->label = label;
	addAsmCode(code);
	return code;
}
AsmCode genLI(int x, int k) {
	AsmCode code;
	code = newAsmCode(A_LI);
	code->x = x;
	code->k = k;
	addAsmCode(code);
	return code;
}
AsmCode genMOVE(int x, int y) {
	AsmCode code;
	code = newAsmCode(A_MOVE);
	code->x = x;
	code->y = y;
	addAsmCode(code);
	return code;
}
AsmCode genADDI(int x, int y, int k) {
	AsmCode code;
	code = newAsmCode(A_ADDI);
	code->x = x;
	code->y = y;
	code->k = k;
	addAsmCode(code);
	return code;
}
//kind: A_ADD, A_SUB, A_MUL
AsmCode genCompute(int kind, int x, int y, int z) {
	AsmCode code;
	code = newAsmCode(kind);
	code->x = x;
	code->y = y;
	code->z = z;
	addAsmCode(code);
	return code;
}
AsmCode genDIV(int y, int z) {
	AsmCode code;
	code = newAsmCode(A_DIV);
	code->y = y;
	code->z = z;
	addAsmCode(code);
	return code;
}
AsmCode genMFLO(int x) {
	AsmCode code;
	code = newAsmCode(A_MFLO);
	code->x = x;
	addAsmCode(code);
	return code;
}
AsmCode genLW(int x, int k, int y) {
	AsmCode code;
	code = newAsmCode(A_LW);
	code->x = x;
	code->k = k;
	code->y = y;
	addAsmCode(code);
	return code;
}
AsmCode genSW(int y, int k, int x) {
	AsmCode code;
	code = newAsmCode(A_SW);
	code->y = y;
	code->x = x;
	code->k = k;
	addAsmCode(code);
	return code;
}
//kind: A_BEQ, A_BNE, A_BGT, A_BLT, A_BGE, A_BLE
AsmCode genConJump(int kind, int x, int y, char *label) {
	AsmCode code;
	code = newAsmCode(kind);
	code->x = x;
	code->y = y;
	code->label = label;
	addAsmCode(code);
	return code;
}
void printfAsm(FILE *tag, AsmCode asmcode) {
	switch(asmcode->kind) {
	case A_LABEL:
		fprintf(tag, "%s:", asmcode->label);
		break;
	case A_LI:
		fprintf(tag, "\tli %s, %d", getRegName(asmcode->x), asmcode->k);
		break;
	case A_MOVE:
		fprintf(tag, "\tmove %s, %s", getRegName(asmcode->x), getRegName(asmcode->y));
		break;
	case A_ADDI:
		fprintf(tag, "\taddi %s, %s, %d", getRegName(asmcode->x), getRegName(asmcode->y), asmcode->k);
		break;
	case A_ADD:
		fprintf(tag, "\tadd %s, %s, %s", getRegName(asmcode->x), getRegName(asmcode->y), getRegName(asmcode->z));
		break;
	case A_SUB:
		fprintf(tag, "\tsub %s, %s, %s", getRegName(asmcode->x), getRegName(asmcode->y), getRegName(asmcode->z));
		break;
	case A_MUL:
		fprintf(tag, "\tmul %s, %s, %s", getRegName(asmcode->x), getRegName(asmcode->y), getRegName(asmcode->z));
		break;
	case A_DIV:
		fprintf(tag, "\tdiv %s, %s", getRegName(asmcode->y), getRegName(asmcode->z));
		break;
	case A_MFLO:
		fprintf(tag, "\tmflo %s", getRegName(asmcode->x));
		break;
	case A_LW:
		fprintf(tag, "\tlw %s, %d(%s)", getRegName(asmcode->x), asmcode->k, getRegName(asmcode->y));
		break;
	case A_SW:
		fprintf(tag, "\tsw %s, %d(%s)", getRegName(asmcode->y), asmcode->k, getRegName(asmcode->x));
		break;
	case A_J:
		fprintf(tag, "\tj %s", asmcode->label);
		break;
	case A_JAL:
		fprintf(tag, "\tjal %s", asmcode->label);
		break;
	case A_JR:
		fprintf(tag, "\tjr %s", getRegName(RA));
		break;
	case A_BEQ:
		fprintf(tag, "\tbeq %s, %s, %s", getRegName(asmcode->x), getRegName(asmcode->y), asmcode->label);
		break;
	case A_BNE:
		fprintf(tag, "\tbne %s, %s, %s", getRegName(asmcode->x), getRegName(asmcode->y), asmcode->label);
		break;
	case A_BGT:
		fprintf(tag, "\tbgt %s, %s, %s", getRegName(asmcode->x), getRegName(asmcode->y), asmcode->label);
		break;
	case A_BLT:
		fprintf(tag, "\tblt %s, %s, %s", getRegName(asmcode->x), getRegName(asmcode->y), asmcode->label);
		break;
	case A_BGE:
		fprintf(tag, "\tbge %s, %s, %s", getRegName(asmcode->x), getRegName(asmcode->y), asmcode->label);
		break;
	case A_BLE:
		fprintf(tag, "\tble %s, %s, %s", getRegName(asmcode->x), getRegName(asmcode->y), asmcode->label);
		break;
	}
}
void printfAllAsm(FILE *tag) {
	AsmCodes temp = AsmHead;
	printfAsmHead(tag);
	printfReadAsm(tag);
	printfWriteAsm(tag);
	while(temp) {
		printfAsm(tag, temp->code);
		fprintf(tag, "\n");
		temp = temp->next;
	}
}
