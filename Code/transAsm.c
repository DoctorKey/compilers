#include "transAsm.h"
#include "asm.h"
#include "mips32.h"
#include "VarRegMap.h"

#define CALLER_REG_NUM 10
#define CALLEE_REG_NUM 9
int sp = 0;
int fp = 0;

int pareReg(Operand op) {
	int r;
	int varnum = op->varnum;
	r = getReg(varnum);
	if(varnum == -1) {
		// var is constant	
		genLI(r, op->num_int);
		updateLITemp(r);
	}else if(getMemactive(varnum) == 1 && isVarInReg(varnum, r) == false){
	// if var's mem isn't active, it is of course that it don't need lw
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
int pushS() {
	int begin = sp;
	sp -= 4;
	genSW(S0, sp-fp, FP);
	sp -= 4;
	genSW(S1, sp-fp, FP);
	sp -= 4;
	genSW(S2, sp-fp, FP);
	sp -= 4;
	genSW(S3, sp-fp, FP);
	sp -= 4;
	genSW(S4, sp-fp, FP);
	sp -= 4;
	genSW(S5, sp-fp, FP);
	sp -= 4;
	genSW(S6, sp-fp, FP);
	sp -= 4;
	genSW(S7, sp-fp, FP);
	genADDI(SP, SP, sp-fp);
	return begin;
}
void popS(int begin) {
	int i = fp;
	i -= 4;
	genLW(S0, i-fp, FP);
	i -= 4;
	genLW(S1, i-fp, FP);
	i -= 4;
	genLW(S2, i-fp, FP);
	i -= 4;
	genLW(S3, i-fp, FP);
	i -= 4;
	genLW(S4, i-fp, FP);
	i -= 4;
	genLW(S5, i-fp, FP);
	i -= 4;
	genLW(S6, i-fp, FP);
	i -= 4;
	genLW(S7, i-fp, FP);
}
int pushT() {
	int begin = sp;
	sp -= 4;
	genSW(T0, sp-fp, FP);
	sp -= 4;
	genSW(T1, sp-fp, FP);
	sp -= 4;
	genSW(T2, sp-fp, FP);
	sp -= 4;
	genSW(T3, sp-fp, FP);
	sp -= 4;
	genSW(T4, sp-fp, FP);
	sp -= 4;
	genSW(T5, sp-fp, FP);
	sp -= 4;
	genSW(T6, sp-fp, FP);
	sp -= 4;
	genSW(T7, sp-fp, FP);
	sp -= 4;
	genSW(T8, sp-fp, FP);
	sp -= 4;
	genSW(T9, sp-fp, FP);
	genADDI(SP, SP, sp-fp);
	return begin;
}
void popT(int begin) {
	int i = begin;
	i -= 4;
	genLW(T0, i-fp, FP);
	i -= 4;
	genLW(T1, i-fp, FP);
	i -= 4;
	genLW(T2, i-fp, FP);
	i -= 4;
	genLW(T3, i-fp, FP);
	i -= 4;
	genLW(T4, i-fp, FP);
	i -= 4;
	genLW(T5, i-fp, FP);
	i -= 4;
	genLW(T6, i-fp, FP);
	i -= 4;
	genLW(T7, i-fp, FP);
	i -= 4;
	genLW(T8, i-fp, FP);
	i -= 4;
	genLW(T9, i-fp, FP);
}
void pareMem(int varindex) {
	sp -= 4;
	genADDI(SP, SP, -4);
	setMem(varindex, FP, sp-fp);
	// just get a mem, wait for init mem
	unactiveMem(varindex);
}

void translabel(InterCode ir) {
	// new basic block
	// spill all
	spillAllVar();
	genlabel(A_LABEL, Optostring(ir->op1.op1));
	clearRegMap();
	clearAddrDesTable();
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
}
int param = 0;
void transfunc(InterCode ir) {
	fp = sp = 0;
	genlabel(A_LABEL, Optostring(ir->op1.op1));
	genADDI(SP, SP, -4);
	sp = sp - 4;
	genSW(FP, 0, SP);
	genMOVE(FP, SP);
	fp = sp;
	pushS();
	clearRegMap();
	clearAddrDesTable();
	param = 0;
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
	spillAllVar();
	genlabel(A_J, Optostring(ir->op1.op1));
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
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
	spillAllVar();
	genConJump(kind, rx, ry, Optostring(ir->op4.z));
	// next is basic block
	clearRegMap();
	clearAddrDesTable();
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
}
void transreturn(InterCode ir) {
	int rx;
	rx = getReg(ir->op1.op1->varnum);
	genMOVE(V0, rx);
	// this arg is no sence
	popS(0);
	spillAllVar();
	genADDI(SP, FP, 4);
	genLW(FP, 0, FP);
	genlabel(A_JR, NULL);
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
}
void transdec(InterCode ir) {
}
InterCodes arglist = NULL;
void transarg(InterCode ir) {
	arglist = addInterCode(arglist, ir);
}
void transcall(InterCode ir) {
	int rx, tbegin;
	InterCodes prev = NULL;
	spillAllVar();
	tbegin = pushT();
	if(arglist) {
		genMOVE(A0, pareReg(arglist->code->op1.op1));	
		prev = arglist;
		arglist = arglist->next;
		freeIR(prev);
	}
	if(arglist) {
		genMOVE(A1, pareReg(arglist->code->op1.op1));	
		prev = arglist;
		arglist = arglist->next;
		freeIR(prev);
	}
	if(arglist) {
		genMOVE(A2, pareReg(arglist->code->op1.op1));	
		prev = arglist;
		arglist = arglist->next;
		freeIR(prev);
	}
	if(arglist) {
		genMOVE(A3, pareReg(arglist->code->op1.op1));	
		prev = arglist;
		arglist = arglist->next;
		freeIR(prev);
	}
	while(arglist) {
		genSW(pareReg(arglist->code->op1.op1), getMemk(arglist->code->op1.op1->varnum), getMemReg(arglist->code->op1.op1->varnum));
		prev = arglist;
		arglist = arglist->next;
		freeIR(prev);
	}
	genADDI(SP, SP, -4);
	sp -= 4;
	genSW(RA, 0, SP);
	genlabel(A_JAL, strdup(ir->op2.right->str));
	genLW(RA, 0, SP);
	genADDI(SP, SP, 4);
	sp += 4;
	popT(tbegin);
	clearRegMap();
	clearAddrDesTable();
	rx = getReg(ir->op2.result->varnum);
	genMOVE(rx, V0);
	updateDesIR3(rx, ir->op2.result->varnum);
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
}
void transparam(InterCode ir) {
	param++;
	switch(param) {
	case 1:updateDesIR3(A0, ir->op1.op1->varnum);break;
	case 2:updateDesIR3(A1, ir->op1.op1->varnum);break;
	case 3:updateDesIR3(A2, ir->op1.op1->varnum);break;
	case 4:updateDesIR3(A3, ir->op1.op1->varnum);break;
	}
	if(param > 4) {
		setMem(ir->op1.op1->varnum, FP, 4+4*(param - 4));	
		activeMem(ir->op1.op1->varnum);
	}
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
}
void transread(InterCode ir) {
	int rx;
	spillAllVar();
	genADDI(SP, SP, -4);
	sp -= 4;
	genSW(RA, 0, SP);
	genlabel(A_JAL, "read");
	genLW(RA, 0, SP);
	genADDI(SP, SP, 4);
	sp += 4;
	clearRegMap();
	clearAddrDesTable();
	rx = pareReg(ir->op1.op1);
	genMOVE(rx, V0);
	updateDesIR3(rx, ir->op1.op1->varnum);
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
}
void transwrite(InterCode ir) {
	spillAllVar();
	genMOVE(A0, pareReg(ir->op1.op1));	
	updateDesLW(A0, ir->op1.op1->varnum); 
	genADDI(SP, SP, -4);
	sp -= 4;
	genSW(RA, 0, SP);
	genlabel(A_JAL, "write");
	genLW(RA, 0, SP);
	genADDI(SP, SP, 4);
	sp += 4;
	clearRegMap();
	clearAddrDesTable();
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
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
#ifdef DEBUG4
		fprintf(stdout, "ir:"); 
		printfIR(stdout, temp->code);
		fprintf(stdout, "\n"); 
#endif
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
