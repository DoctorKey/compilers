#include "transAsm.h"
#include "asm.h"
#include "mips32.h"
#include "VarRegMap.h"
int pareReg(Operand op) {
	int r;
	int varnum = op->varnum;
	r = getReg(varnum);
	if(varnum == -1) {
		// var is constant	
		genLI(r, op->num_int);
		updateLITemp(r);
	}else if(isVarInReg(varnum, r) == false){
		genLW(r, getMemk(varnum), getMemReg(varnum));
		updateDesLW(r, varnum);
	}
	return r;
}
AsmCode transADDI(Operand x, Operand y, int k) {
	AsmCode code;
	int xindex, yindex;
	int rx, ry;
	xindex = x->varnum;
	yindex = y->varnum;
	rx = getReg(xindex);
	ry = pareReg(y); 
	genADDI(rx, ry, k);
	updateDesIR3(rx, xindex);
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
}
//void naiveTransIR3(int kind, Operand x, Operand y, Operand z) {
//	AsmCode code;
//	int rx, ry, rz;
//	rx = T0;
//	ry = T1;
//	rz = T2;
//	code = newAsmCode(A_LW);
//	code->x = ry;
//	code->y = SP;
//	code->k = 0;
//	addAsmCode(code);
//	code = newAsmCode(A_LW);
//	code->x = rz;
//	code->y = SP;
//	code->k = 0;
//	addAsmCode(code);
//	code = newAsmCode(kind);
//	code->x = rx;
//	code->y = ry; 
//	code->z = rz;
//	addAsmCode(code);
//	code = newAsmCode(A_SW);
//	code->x = ry;
//	code->y = SP;
//	code->k = 0;
//	addAsmCode(code);
//	code = newAsmCode(A_SW);
//	code->x = rz;
//	code->y = SP;
//	code->k = 0;
//	addAsmCode(code);
//}
void transIR3(int kind, Operand x, Operand y, Operand z) {
	AsmCode code;
	int xindex, yindex, zindex;
	int rx, ry, rz;
	xindex = x->varnum;
	yindex = y->varnum;
	zindex = z->varnum;
	rx = getReg(xindex);
	ry = pareReg(y);
	rz = pareReg(z);
	genCompute(kind, rx, ry, rz);
	updateDesIR3(rx, xindex);
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
}
void translabel(InterCode ir) {
	genlabel(A_LABEL, Optostring(ir->op1.op1));
}
void transfunc(InterCode ir) {
	genlabel(A_LABEL, Optostring(ir->op1.op1));
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
}
void transassign(InterCode ir) {
	AsmCode code;
	int rx, ry;
	int xindex, yindex;
	if(ir->op2.right->kind == CONSTANT_OP) {
		xindex = ir->op2.result->varnum;
		rx = getReg(xindex);
		genLI(rx, ir->op2.right->num_int);
		updateDesIR3(rx, xindex);
	}else {
		xindex = ir->op2.result->varnum;
		yindex = ir->op2.right->varnum;
		ry = pareReg(ir->op2.right);
		updateDesIReq(ry, xindex);
	}
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
}
void transadd(InterCode ir) {
	if(ir->op3.right2->kind == CONSTANT_OP) {
		transADDI(ir->op3.result, ir->op3.right1, ir->op3.right2->num_int);
	}else {
		transIR3(A_ADD, ir->op3.result, ir->op3.right1, ir->op3.right2); 
	}
}
void transsub(InterCode ir) {
	if(ir->op3.right2->kind == CONSTANT_OP) {
		transADDI(ir->op3.result, ir->op3.right1, -ir->op3.right2->num_int);
	}else {
		transIR3(A_SUB, ir->op3.result, ir->op3.right1, ir->op3.right2); 
	}
}
void transmul(InterCode ir) {
	transIR3(A_MUL, ir->op3.result, ir->op3.right1, ir->op3.right2); 
}
void transdiv(InterCode ir) {
	int xindex, yindex, zindex;
	int rx, ry, rz;
	xindex = ir->op3.result->varnum;
	yindex = ir->op3.right1->varnum;
	zindex = ir->op3.right2->varnum;
	rx = getReg(xindex);
	ry = pareReg(ir->op3.right1);
	rz = pareReg(ir->op3.right2);
	genDIV(ry, rz);
	genMFLO(rx);
	updateDesIR3(rx, xindex);
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
}
void transgoto(InterCode ir) {
	genlabel(A_J, Optostring(ir->op1.op1));
}
void transif(InterCode ir) {
	int rx, ry;
	int kind;
	rx = pareReg(ir->op4.x);
	ry = pareReg(ir->op4.y);

	if(strcmp(ir->op4.relop->str, "==") == 0) {
		kind = A_BEQ;
	}else if(strcmp(ir->op4.relop->str, "!=") == 0) {
		kind = A_BNE;
	}else if(strcmp(ir->op4.relop->str, ">") == 0) {
		kind = A_BGT;
	}else if(strcmp(ir->op4.relop->str, "<") == 0) {
		kind = A_BLT;
	}else if(strcmp(ir->op4.relop->str, ">=") == 0) {
		kind = A_BGE;
	}else if(strcmp(ir->op4.relop->str, "<=") == 0) {
		kind = A_BLE;
	}
	genConJump(kind, rx, ry, Optostring(ir->op4.z));
}
void transreturn(InterCode ir) {
	int rx;
	rx = getReg(ir->op1.op1->varnum);
	genMOVE(V0, rx);
	genlabel(A_JR, NULL);
}
void transdec(InterCode ir) {
}
int argnum = 0;
void transarg(InterCode ir) {
	AsmCode code;
	int k;
	argnum++;
	if(argnum <= 4) {
//		code = newAsmCode(A_MOVE);
//		switch(argnum) {
//		case 1: code->x = A0; break;
//		case 2: code->x = A1; break;
//		case 3: code->x = A2; break;
//		case 4: code->x = A3; break;
//		}
		//code->y = ir->op1.op1->mem->reg;
		//code->k = ir->op1.op1->mem->k;
	}else {
//		k = (argnum - 5) * 4;
//		code = newAsmCode(A_SW);
////		code->y = getReg(ir->op1.op1);
//		code->k = k;
//		code->x = SP;
	}
}
void transcall(InterCode ir) {
	int rx;
	argnum = 0;
	rx = getReg(ir->op2.result->varnum);
	genlabel(A_JAL, strdup(ir->op2.right->str));
	genMOVE(rx, V0);
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
