#include "transAsm.h"
#include "asm.h"
#include "mips32.h"
#include "VarRegMap.h"

#define CALLER_REG_NUM 10
#define CALLEE_REG_NUM 9
int sp = 0;
int fp = 0;

char getAddrOpType(InterCode ir, Operand op) {
	if(ir->isComputeAddr == 0) {
		if(op->isAddr == 0) {
			return '0';
		}else {
			return '*';
		}
	}else if(ir->isComputeAddr == 1) {
		if(op->isAddr == 0) {
			return '&';
		}else {
			return '0';
		}
	}

}
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
int pareReg_plus(Operand op, char type) {
	int r, rt;
	int base, offset;
	int varnum = op->varnum;
	if(type == '&') {
		rt = getReg(-1);	
		base = getMemReg(op->varnum);
		offset = getMemk(op->varnum);
		genADDI(rt, base, offset);
		updateLITemp(rt);
		return rt;
	}else if(type == '*') {
		rt = pareReg(op);
		r = getReg(-1);
		genLW(r, 0, rt);
		updateLITemp(r);
		return r;
	}
	r = getReg(varnum);
	if(varnum == -1) {
		// var is constant	
		genLI(r, op->num_int);
		updateLITemp(r);
	}else if(getMemactive(varnum) == 1 && isVarInReg(varnum, r) == false){
	// if var's mem isn't active, it is of cause that it don't need lw
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
void transIR3_regy(int kind, Operand x, int y, Operand z) {
	AsmCode code;
	int xindex, yindex, zindex;
	int rx, ry, rz;
	xindex = x->varnum;
//	yindex = y->varnum;
	zindex = z->varnum;
	rx = getReg(xindex);
//	ry = pareReg(y);
	rz = pareReg(z);
	genCompute(kind, rx, y, rz);
	updateDesIR3(rx, xindex);
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
}
void transIR3_plus(int kind, InterCode ir) {
	Operand x, y, z;
	AsmCode code;
	int xindex, yindex, zindex;
	int rx, ry, rz;
	char xtype, ytype, ztype;
	x = ir->op3.result;
	y = ir->op3.right1;
	z = ir->op3.right2;
	xindex = x->varnum;
	yindex = y->varnum;
	zindex = z->varnum;
	xtype = getAddrOpType(ir, x);
	ytype = getAddrOpType(ir, y);
	ztype = getAddrOpType(ir, z);
	rx = getReg(xindex);
	ry = pareReg_plus(y, ytype);
	rz = pareReg_plus(z, ztype);
	genCompute(kind, rx, ry, rz);
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
	int rx, ry, rt;
	int xindex, yindex;
	char xtype, ytype;
	if(ir->op2.right->kind == CONSTANT_OP) {
		xtype = getAddrOpType(ir, ir->op2.result);
		switch(xtype) {
		case '0':
			xindex = ir->op2.result->varnum;
			rx = getReg(xindex);
			genLI(rx, ir->op2.right->num_int);
			updateDesIR3(rx, xindex);
			break;
		case '*':
			rt = pareReg(ir->op2.right);
			xindex = ir->op2.result->varnum;
			rx = getReg(xindex);
			genSW(rt, 0, rx);
			updateDesSW(rx, xindex);
			//TODO may need kill all reg
			break;
		case '&':
			break;
		}
	}else {
		xtype = getAddrOpType(ir, ir->op2.result);
		ytype = getAddrOpType(ir, ir->op2.right);
		if(xtype == '0' && ytype == '0') {
			// x = y
			xindex = ir->op2.result->varnum;
			yindex = ir->op2.right->varnum;
			ry = pareReg(ir->op2.right);
			updateDesIReq(ry, xindex);
		}else if(xtype == '*' && ytype == '0') {
			// *x = y
			xindex = ir->op2.result->varnum;
			yindex = ir->op2.right->varnum;
			ry = pareReg(ir->op2.right);
			rx = getReg(xindex);
			genSW(ry, 0, rx);
			updateDesSW(rx, xindex);
		}else if(xtype == '0' && ytype == '*') {
			// x = *y
			xindex = ir->op2.result->varnum;
			yindex = ir->op2.right->varnum;
			ry = pareReg(ir->op2.right);
			rx = getReg(xindex);
			genLW(rx, 0, ry);
			updateDesLW(rx, xindex);
		}
	}
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
}
void transadd(InterCode ir) {
	int rt, base, offset;
	char xtype, ytype, ztype;
	xtype = getAddrOpType(ir, ir->op3.result);
	ytype = getAddrOpType(ir, ir->op3.right1);
	ztype = getAddrOpType(ir, ir->op3.right2);
	if(ir->op3.right2->kind == CONSTANT_OP) {
		transADDI(ir->op3.result, ir->op3.right1, ir->op3.right2->num_int);
	}else {
//		transIR3_plus(A_ADD, ir);
		if(ytype == '0') {
			transIR3(A_ADD, ir->op3.result, ir->op3.right1, ir->op3.right2); 
		}else if(ytype == '&') {
			rt = getReg(-1);	
			base = getMemReg(ir->op3.right1->varnum);
			offset = getMemk(ir->op3.right1->varnum);
			genADDI(rt, base, offset);
			updateLITemp(rt);
			transIR3_regy(A_ADD, ir->op3.result, rt, ir->op3.right2); 
		}
	}
}
void transsub(InterCode ir) {
	if(ir->op3.right2->kind == CONSTANT_OP) {
		transADDI(ir->op3.result, ir->op3.right1, -ir->op3.right2->num_int);
	}else {
		transIR3_plus(A_SUB, ir);
//		transIR3(A_SUB, ir->op3.result, ir->op3.right1, ir->op3.right2); 
	}
}
void transmul(InterCode ir) {
//	transIR3(A_MUL, ir->op3.result, ir->op3.right1, ir->op3.right2); 
	transIR3_plus(A_MUL, ir);
}
void transdiv(InterCode ir) {
	int xindex, yindex, zindex;
	int rx, ry, rz;
	char ytype, ztype;
	xindex = ir->op3.result->varnum;
	rx = getReg(xindex);
	ytype = getAddrOpType(ir, ir->op3.right1);
	ztype = getAddrOpType(ir, ir->op3.right2);
	ry = pareReg_plus(ir->op3.right1, ytype);
	rz = pareReg_plus(ir->op3.right2, ztype);
//	ry = pareReg(ir->op3.right1);
//	rz = pareReg(ir->op3.right2);
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
	rx = pareReg(ir->op1.op1);
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
	int size = ir->op2.right->num_int;
	int varindex = ir->op2.result->varnum;
	sp -= size;
	genADDI(SP, SP, -size);
	setMem(varindex, FP, sp-fp);
	// just get a mem, wait for init mem
	unactiveMem(varindex);
#ifdef DEBUG4
	printfRegMap(stdout); 
	printfAddrDescripTable(stdout); 
#endif
}
InterCodes arglist = NULL;
void transarg(InterCode ir) {
	arglist = addInterCode(arglist, ir);
}
InterCodes transferArg2A(InterCodes arglist, int reg) {
	InterCodes prev = NULL;
	if(arglist) {
		arglist->prev = NULL;
		if(arglist->code->op1.op1->kind == CONSTANT_OP) {
			genLI(reg, arglist->code->op1.op1->num_int);	
			updateLITemp(A0);
		}else{
			genMOVE(reg, pareReg(arglist->code->op1.op1));	
		}
		prev = arglist;
		arglist = arglist->next;
		freeIR(prev);
		return arglist;
	}
	return NULL;
}
void transcall(InterCode ir) {
	int rx, tbegin;
	int argr, memk, memreg;
	InterCodes prev = NULL;
	spillAllVar();
	tbegin = pushT();
	arglist = transferArg2A(arglist, A0);
	arglist = transferArg2A(arglist, A1);
	arglist = transferArg2A(arglist, A2);
	arglist = transferArg2A(arglist, A3);
	if(arglist) {
		while(arglist->next != NULL){
			arglist = arglist->next;
		}
		while(arglist) {
			freeIR(arglist->next);
			arglist->next = NULL;
			if(arglist->code->op1.op1->kind == CONSTANT_OP) {
				argr = pareReg(arglist->code->op1.op1);
				genADDI(SP, SP, -4);
				sp -= 4;
				genSW(argr, 0, SP);
			}else{
				argr = pareReg(arglist->code->op1.op1);
				memk = getMemk(arglist->code->op1.op1->varnum); 
				memreg = getMemReg(arglist->code->op1.op1->varnum);
				genSW(argr, memk, memreg);
			}
			arglist = arglist->prev;
		}
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
	if(ir->op1.op1->isAddr == 1) {
		// write *op
		genLW(A0, 0, pareReg(ir->op1.op1));	
		updateDesLW(A0, ir->op1.op1->varnum); 
	}else {
		// write op
		genMOVE(A0, pareReg(ir->op1.op1));	
		updateDesLW(A0, ir->op1.op1->varnum); 
	}
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
	// have fun
	updateIdleReg();
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
