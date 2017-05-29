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
		return;
	}else {
		AsmTail->next = newAsmCodes(code);
		AsmTail->next->prev = AsmTail;
		AsmTail = AsmTail->next;
		return;
	}
}
void spill(int reg, Operand op) {
	AsmCode code;
	code = newAsmCode(A_SW);
	code->y = reg;
//	code->x = op->mem->reg;
//	code->k = op->mem->k;
	addAsmCode(code);
}
void naiveTransIR3(int kind, Operand x, Operand y, Operand z) {
	AsmCode code;
	int rx, ry, rz;
	rx = T0;
	ry = T1;
	rz = T2;
	code = newAsmCode(A_LW);
	code->x = ry;
	code->y = SP;
	code->k = 0;
	addAsmCode(code);
	code = newAsmCode(A_LW);
	code->x = rz;
	code->y = SP;
	code->k = 0;
	addAsmCode(code);
	code = newAsmCode(kind);
	code->x = rx;
	code->y = ry; 
	code->z = rz;
	addAsmCode(code);
	code = newAsmCode(A_SW);
	code->x = ry;
	code->y = SP;
	code->k = 0;
	addAsmCode(code);
	code = newAsmCode(A_SW);
	code->x = rz;
	code->y = SP;
	code->k = 0;
	addAsmCode(code);
}
void transIR3(int kind, Operand x, Operand y, Operand z) {
	AsmCode code;
	int rx, ry, rz;
	rx = getReg(x);
	ry = getReg(y);
	rz = getReg(z);
	if(isOpInReg(y, ry) == false){
		code = newAsmCode(A_LW);
		code->x = ry;
		code->y = SP;
		code->k = 0;
		addAsmCode(code);
	}
	if(isOpInReg(z, rz) == false){
		code = newAsmCode(A_LW);
		code->x = rz;
		code->y = SP;
		code->k = 0;
		addAsmCode(code);
	}
	code = newAsmCode(kind);
	code->x = rx;
	code->y = ry; 
	code->z = rz;
	addAsmCode(code);
}
void translabel(InterCode ir) {
	AsmCode code;
	code = newAsmCode(A_LABEL);
	code->label = Optostring(ir->op1.op1);
	addAsmCode(code);
}
void transfunc(InterCode ir) {
	AsmCode code;
	code = newAsmCode(A_LABEL);
	code->label = Optostring(ir->op1.op1);
	addAsmCode(code);
}
void transassign(InterCode ir) {
	AsmCode code;
	int rx, ry;
	rx = Ensure(ir->op2.result);
	if(ir->op2.right->kind == CONSTANT_OP) {
		code = newAsmCode(A_LI);
		code->x = rx;
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
#ifdef DEBUG4
	printfAsm(stdout, code); 
	fprintf(stdout, "\n");
	printfRegMap(stdout); 
	printfVar2RegTable(stdout); 
#endif
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
	code->label = Optostring(ir->op1.op1);
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
	code->label = Optostring(ir->op4.z);
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
int argnum = 0;
void transarg(InterCode ir) {
	AsmCode code;
	int k;
	argnum++;
	if(argnum <= 4) {
		code = newAsmCode(A_MOVE);
		switch(argnum) {
		case 1: code->x = A0; break;
		case 2: code->x = A1; break;
		case 3: code->x = A2; break;
		case 4: code->x = A3; break;
		}
		//code->y = ir->op1.op1->mem->reg;
		//code->k = ir->op1.op1->mem->k;
	}else {
		k = (argnum - 5) * 4;
		code = newAsmCode(A_SW);
		code->y = getReg(ir->op1.op1);
		code->k = k;
		code->x = SP;
	}
	addAsmCode(code);
}
void transcall(InterCode ir) {
	AsmCode code;
	int rx;
	argnum = 0;
	rx = Ensure(ir->op2.result);
	code = newAsmCode(A_JAL);
	code->label = strdup(ir->op2.right->str);
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
void printfAsmHead(FILE *tag) {
	fprintf(tag, ".data\n");
	fprintf(tag, "_prompt: .asciiz \"Enter an integer:\"\n");
	fprintf(tag, "_ret: .asciiz \"\\n\"\n");
	fprintf(tag, ".globl main\n");
	fprintf(tag, ".text\n");
}
void printfReadAsm(FILE *tag) {
	fprintf(tag, "read:\n");
	fprintf(tag, "\tli $v0, 4\n");
	fprintf(tag, "\tla $a0, _prompt\n");
	fprintf(tag, "\tsyscall\n");
	fprintf(tag, "\tli $v0, 5\n");
	fprintf(tag, "\tsyscall\n");
	fprintf(tag, "\tjr $ra\n");
}
void printfWriteAsm(FILE *tag) {
	fprintf(tag, "write:\n");
	fprintf(tag, "\tli $v0, 1\n");
	fprintf(tag, "\tsyscall\n");
	fprintf(tag, "\tli $v0, 4\n");
	fprintf(tag, "\tla $a0, _ret\n");
	fprintf(tag, "\tsyscall\n");
	fprintf(tag, "\tmove $v0, $0\n");
	fprintf(tag, "\tjr $ra\n");
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
