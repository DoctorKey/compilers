#include "asm.h"
#include "mips32.h"
#include "VarRegMap.h"
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
	result->f = NULL;
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
		return;
	}else {
		AsmTail->next = newAsmCodes(code);
		AsmTail->next->prev = AsmTail;
		AsmTail = AsmTail->next;
		return;
	}
}
void translabel(InterCode ir) {
	AsmCode code;
	code = newAsmCode(A_LABEL);
	code->x = ir->op1.op1->num_int;
	addAsmCode(code);
}
void transfunc(InterCode ir) {
}
void transassign(InterCode ir) {
}
void transadd(InterCode ir) {
}
void transsub(InterCode ir) {
}
void transmul(InterCode ir) {
}
void transdiv(InterCode ir) {
}
void transgoto(InterCode ir) {
}
void transif(InterCode ir) {
}
void transreturn(InterCode ir) {
}
void transdec(InterCode ir) {
}
void transarg(InterCode ir) {
}
void transcall(InterCode ir) {
}
void transparam(InterCode ir) {
}
void transread(InterCode ir) {
}
void transwrite(InterCode ir) {
}
void transAsm(InterCode ir) {
	switch(ir->kind) {
	case LABEL_IR:		translabel(ir);		break;
	case FUNCTION_IR:	transfunc(ir);		break;
	case ASSIGN_IR:		transassign(ir);	break;
	case ADD_IR:		transadd(ir);		break;
	case SUB_IR:		transsub(ir);		break;
	case MUL_IR:		transmul(ir);		break;
	case DIV_IR:		transdiv(ir);		break;
	case GOTO_IR:		transgoto(ir);		break;
	case IF_IR:		transif(ir);		break;
	case RETURN_IR:		transreturn(ir);	break;
	case DEC_IR:		transdec(ir);		break;
	case ARG_IR:		transarg(ir);		break;
	case CALL_IR:		transcall(ir);		break;
	case PARAM_IR:		transparam(ir);		break;
	case READ_IR:		transread(ir);		break;
	case WRITE_IR:		transwrite(ir);		break;
	}	
}
void transAllAsm(InterCodes IRhead) {
	InterCodes temp = IRhead;
	initmap(temp);
	while(temp) {
		transAsm(temp->code);
		temp = temp->next;
	}
}
void printfAsm(FILE *tag, AsmCode asmcode) {
	switch(asmcode->kind) {
	case A_LABEL:
		fprintf(tag, "label%d:", asmcode->x);
		break;
	case A_LI:
		fprintf(tag, "li %s, %d", getRegName(asmcode->x), asmcode->k);
		break;
	case A_MOVE:
		fprintf(tag, "move %s, %s", getRegName(asmcode->x), getRegName(asmcode->y));
		break;
	case A_ADDI:
		fprintf(tag, "addi %s, %s, %d", getRegName(asmcode->x), getRegName(asmcode->y), asmcode->k);
		break;
	case A_ADD:
		fprintf(tag, "add %s, %s, %s", getRegName(asmcode->x), getRegName(asmcode->y), getRegName(asmcode->z));
		break;
	case A_SUB:
		fprintf(tag, "sub %s, %s, %s", getRegName(asmcode->x), getRegName(asmcode->y), getRegName(asmcode->z));
		break;
	case A_MUL:
		fprintf(tag, "mul %s, %s, %s", getRegName(asmcode->x), getRegName(asmcode->y), getRegName(asmcode->z));
		break;
	case A_DIV:
		fprintf(tag, "div %s, %s", getRegName(asmcode->y), getRegName(asmcode->z));
		break;
	case A_MFLO:
		fprintf(tag, "mflo %s", getRegName(asmcode->x));
		break;
	case A_LW:
		fprintf(tag, "lw %s, 0(%s)", getRegName(asmcode->x), getRegName(asmcode->y));
		break;
	case A_SW:
		fprintf(tag, "sw %s, 0(%s)", getRegName(asmcode->y), getRegName(asmcode->x));
		break;
	case A_J:
		fprintf(tag, "j label%d", asmcode->x);
		break;
	case A_JAL:
		fprintf(tag, "jal %s", asmcode->f);
		break;
	case A_JR:
		fprintf(tag, "jr %s", getRegName(RA));
		break;
	case A_BEQ:
		fprintf(tag, "beq %s, %s, label%d", getRegName(asmcode->x), getRegName(asmcode->y), asmcode->z);
		break;
	case A_BNE:
		fprintf(tag, "bne %s, %s, label%d", getRegName(asmcode->x), getRegName(asmcode->y), asmcode->z);
		break;
	case A_BGT:
		fprintf(tag, "bgt %s, %s, label%d", getRegName(asmcode->x), getRegName(asmcode->y), asmcode->z);
		break;
	case A_BLT:
		fprintf(tag, "blt %s, %s, label%d", getRegName(asmcode->x), getRegName(asmcode->y), asmcode->z);
		break;
	case A_BGE:
		fprintf(tag, "bge %s, %s, label%d", getRegName(asmcode->x), getRegName(asmcode->y), asmcode->z);
		break;
	case A_BLE:
		fprintf(tag, "ble %s, %s, label%d", getRegName(asmcode->x), getRegName(asmcode->y), asmcode->z);
		break;
	}
}
void printfAllAsm(FILE *tag) {
	AsmCodes temp = AsmHead;
	while(temp) {
		printfAsm(tag, temp->code);
		fprintf(tag, "\n");
		temp = temp->next;
	}
}
