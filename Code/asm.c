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
	AsmCode code;
	int rx, ry;
	rx = Ensure(ir->op2.result);
	if(ir->op2.right->kind == CONSTANT_OP) {
		code = newAsmCode(A_LI);
		code->x = rx;
		//TODO: may be float
		code->k = ir->op2.right->num_int;
	}else {
		ry = Ensure(ir->op2.right);
		code = newAsmCode(A_LI);
		code->x = rx;
		code->y = ry; 
	}
	Free(rx);
	Free(ry);
	addAsmCode(code);
}
void transadd(InterCode ir) {
	AsmCode code;
	int rx, ry, rz;
	rx = Ensure(ir->op3.result);
	ry = Ensure(ir->op3.right1);
	if(ir->op3.right2->kind == CONSTANT_OP) {
		code = newAsmCode(A_ADDI);
		code->x = rx;
		code->y = ry; 
		//TODO: may be float
		code->k = ir->op3.right2->num_int;
	}else {
		rz = Ensure(ir->op3.right2);
		code = newAsmCode(A_ADD);
		code->x = rx;
		code->y = ry; 
		code->z = rz;
		Free(rz);
	}
	Free(rx);
	Free(ry);
	addAsmCode(code);
}
void transsub(InterCode ir) {
	AsmCode code;
	int rx, ry, rz;
	rx = Ensure(ir->op3.result);
	ry = Ensure(ir->op3.right1);
	if(ir->op3.right2->kind == CONSTANT_OP) {
		code = newAsmCode(A_ADDI);
		code->x = rx;
		code->y = ry; 
		//TODO: may be float
		code->k = -ir->op3.right2->num_int;
	}else {
		rz = Ensure(ir->op3.right2);
		code = newAsmCode(A_SUB);
		code->x = rx;
		code->y = ry; 
		code->z = rz;
		Free(rz);
	}
	Free(rx);
	Free(ry);
	addAsmCode(code);
}
void transmul(InterCode ir) {
	AsmCode code;
	int rx, ry, rz;
	rx = Ensure(ir->op3.result);
	ry = Ensure(ir->op3.right1);
	rz = Ensure(ir->op3.right2);
	code = newAsmCode(A_MUL);
	code->x = rx;
	code->y = ry; 
	code->z = rz;
	Free(rz);
	Free(rx);
	Free(ry);
	addAsmCode(code);
}
void transdiv(InterCode ir) {
	AsmCode code;
	int rx, ry, rz;
	rx = Ensure(ir->op3.result);
	ry = Ensure(ir->op3.right1);
	rz = Ensure(ir->op3.right2);
	code = newAsmCode(A_DIV);
	code->x = rx;
	code->y = ry; 
	code->z = rz;
	Free(rz);
	Free(rx);
	Free(ry);
	addAsmCode(code);
}
void transgoto(InterCode ir) {
	AsmCode code;
	code = newAsmCode(A_J);
	code->x = ir->op1.op1->num_int;
	addAsmCode(code);
}
void transif(InterCode ir) {
	AsmCode code;
	int rx, ry;
	rx = Ensure(ir->op4.x);
	ry = Ensure(ir->op4.y);
	if(strcmp(ir->op4.relop->str, "==") == 0) {
		code = newAsmCode(A_BEQ);
	}else if(strcmp(ir->op4.relop->str, "!=") == 0) {
		code = newAsmCode(A_BNE);
	}else if(strcmp(ir->op4.relop->str, ">") == 0) {
		code = newAsmCode(A_BGT);
	}else if(strcmp(ir->op4.relop->str, "<") == 0) {
		code = newAsmCode(A_BLT);
	}else if(strcmp(ir->op4.relop->str, ">=") == 0) {
		code = newAsmCode(A_BGE);
	}else if(strcmp(ir->op4.relop->str, "<=") == 0) {
		code = newAsmCode(A_BLE);
	}
	code->x = rx;
	code->y = ry;
	code->z = ir->op4.z->num_int;
	addAsmCode(code);
	Free(rx);
	Free(ry);
}
void transreturn(InterCode ir) {
	AsmCode code;
	int rx;
	rx = Ensure(ir->op1.op1);
	code = newAsmCode(A_MOVE);
	code->x = V0;
	code->y = rx; 
	addAsmCode(code);
	code = newAsmCode(A_JR);
	Free(rx);
	addAsmCode(code);
}
void transdec(InterCode ir) {
}
void transarg(InterCode ir) {
}
void transcall(InterCode ir) {
	AsmCode code;
	int rx;
	rx = Ensure(ir->op2.result);
	code = newAsmCode(A_JAL);
	code->f = strdup(ir->op2.right->str);
	addAsmCode(code);
	code = newAsmCode(A_MOVE);
	code->x = rx;
	code->y = V0;
	Free(rx);
	addAsmCode(code);
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
	initRegMap();
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
		fprintf(tag, "lw %s, %d(%s)", getRegName(asmcode->x), asmcode->k, getRegName(asmcode->y));
		break;
	case A_SW:
		fprintf(tag, "sw %s, %d(%s)", getRegName(asmcode->y), asmcode->k, getRegName(asmcode->x));
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
