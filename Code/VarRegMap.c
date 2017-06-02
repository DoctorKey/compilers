#include "VarRegMap.h"
#include "mips32.h"
#include "asm.h"

//#define DEBUG3 1
AddrDescrip AddrDescripTable = NULL;
int allvarnum = 0;

int getAllVarNum() {
	return allvarnum;
#ifdef DEBUG3
	fprintf(stdout, "allvarnum is %d\n", allvarnum);
#endif
}
void printfVarByVec(FILE *tag, int *varvec) {
	int varnum = getAllVarNum();
	int i;
	for(i = 0; i < varnum; i++) {
		if((varvec[getDim(i)] & getVec(i)) != 0) {
			fprintf(tag, "%s,", Optostring(AddrDescripTable[i].op));
		}
	}
}
void initAddrDescripTable(int varnum) {
	AddrDescripTable = (AddrDescrip) malloc(sizeof(struct AddrDescrip_) * varnum);
	if(AddrDescripTable == NULL) {
		fprintf(stderr, "can't malloc\n");
		return;
	}
	while(--varnum >= 0) {
		AddrDescripTable[varnum].op = NULL;
		AddrDescripTable[varnum].index = varnum;
		AddrDescripTable[varnum].varvec = newVec();
		AddrDescripTable[varnum].reg = 0;
		AddrDescripTable[varnum].memactive = 0;
		AddrDescripTable[varnum].mem = newMem(0, 0);
		AddrDescripTable[varnum].varvec[getDim(varnum)] = getVec(varnum);
	}
}
void clearAddrDesTable() {
	int varnum = getAllVarNum();
	while(--varnum >= 0) {
		AddrDescripTable[varnum].reg = 0;
	}
}
int getVarReg(int varnum) {
	return AddrDescripTable[varnum].reg;
}
int MemIsNull(int varindex) {
	if(AddrDescripTable[varindex].mem->reg == 0)
		return 1;
	else
		return 0;
}
void activeMem(int varindex) {
	AddrDescripTable[varindex].memactive = 1;
}
void unactiveMem(int varindex) {
	AddrDescripTable[varindex].memactive = 0;
}
int getMemactive(int varindex) {
	return AddrDescripTable[varindex].memactive;
}
void setMem(int varindex, int reg, int k) {
	AddrDescripTable[varindex].mem->k = k;
	AddrDescripTable[varindex].mem->reg = reg;
}
int getMemk(int varindex) {
	return AddrDescripTable[varindex].mem->k;
}
int getMemReg(int varindex) {
	return AddrDescripTable[varindex].mem->reg;
}
void printfAddrDescripTable(FILE *tag) {
	int num = getAllVarNum();
	int i;
	fprintf(tag, "------------------addr descrip table--------------------\n");
	fprintf(tag, "[index]\top\tmem\treg\n");
	for(i = 0; i < num; i++) {
		fprintf(tag, "[%d]", i);
		fprintf(tag, "\t%s\t", Optostring(AddrDescripTable[i].op));
		printfMem(tag, AddrDescripTable[i].mem);
		if(AddrDescripTable[i].memactive == 1) {
			fprintf(tag, "(act)");
		}else {
			fprintf(tag, "(unact)");
		}
//		if(AddrDescripTable[i].memactive == 1) {
//			fprintf(tag, "\033[31m\033[1m");
//			printfMem(tag, AddrDescripTable[i].mem);
//			fprintf(tag, "\033[0m");
//		}else {
//			printfMem(tag, AddrDescripTable[i].mem);
//		}
		fprintf(tag, ",");
		printfAllReg(tag, AddrDescripTable[i].reg);
		fprintf(tag, "\n");
	}
	fprintf(tag, "------------------End-----------------------------------\n");
}
//TODO: this function isn't good
// shouldn't modify op in this step
void updatemap(Operand op) {
	if(op->kind != TEMP_OP && op->kind != VARIABLE_OP)
		return;
	if(op->varnum != -1)
		return;
	//from 0
	op->varnum = allvarnum;
	allvarnum++;
	return;
}
void updatemap2(Operand op) {
	if(op->kind != TEMP_OP && op->kind != VARIABLE_OP)
		return;
	AddrDescripTable[op->varnum].op = op;
	return;
}
void initmap(InterCodes IRhead) {
	int dimension;
	InterCodes temp = IRhead;
	InterCode ir;
	while(temp) {
		ir = temp->code;
		switch(ir->kind) {
			case ASSIGN_IR:
				updatemap(ir->op2.right);
				updatemap(ir->op2.result);
				break;
			case ADD_IR:
			case SUB_IR:
			case MUL_IR:
			case DIV_IR:
				updatemap(ir->op3.right1);
				updatemap(ir->op3.right2);
				updatemap(ir->op3.result);
				break;
			case IF_IR:
				updatemap(ir->op4.x);
				updatemap(ir->op4.y);
				break;
			case RETURN_IR:
				updatemap(ir->op1.op1);
				break;
			case DEC_IR:
				updatemap(ir->op2.result);
				break;
			case ARG_IR:
				updatemap(ir->op1.op1);
				break;
			case CALL_IR:
				updatemap(ir->op2.result);
				break;
			case PARAM_IR:
				updatemap(ir->op1.op1);
				break;
			case READ_IR:
				updatemap(ir->op1.op1);
				break;
			case WRITE_IR:
				updatemap(ir->op1.op1);
				break;
			default:
				break;
		}	
		temp = temp->next;
	}
	dimension = updateDimension(getAllVarNum());
	setVecDim(dimension);
	initAddrDescripTable(getAllVarNum());
	temp = IRhead;
	while(temp) {
		ir = temp->code;
		switch(ir->kind) {
			case ASSIGN_IR:
				updatemap2(ir->op2.right);
				updatemap2(ir->op2.result);
				break;
			case ADD_IR:
			case SUB_IR:
			case MUL_IR:
			case DIV_IR:
				updatemap2(ir->op3.right1);
				updatemap2(ir->op3.right2);
				updatemap2(ir->op3.result);
				break;
			case IF_IR:
				updatemap2(ir->op4.x);
				updatemap2(ir->op4.y);
				break;
			case RETURN_IR:
				updatemap2(ir->op1.op1);
				break;
			case DEC_IR:
				updatemap2(ir->op2.result);
				break;
			case ARG_IR:
				updatemap2(ir->op1.op1);
				break;
			case CALL_IR:
				updatemap2(ir->op2.result);
				break;
			case PARAM_IR:
				updatemap2(ir->op1.op1);
				break;
			case READ_IR:
				updatemap2(ir->op1.op1);
				break;
			case WRITE_IR:
				updatemap2(ir->op1.op1);
				break;
			default:
				break;
		}	
		temp = temp->next;
	}
}
//int vInOther(int reg) {
//	int *v = (int*) malloc(sizeof(int) * dimension);
//	int i;
//	VecCompute(v, '=', regMap[getRegindex(reg)].varvec);
//	for(i = 0; i < REG_NUM; i++) {
//		if(i == getRegindex(reg))
//			continue;
//		VecCompute(v, '$', regMap[i].varvec);
//	}
//	if(VecIs0(v)) {
//		free(v);
//		return true;
//	}else {
//		free(v);
//		return false;
//	}
//}
void spillAllVar() {
	int num = getAllVarNum();
	int i;
	for(i = 0; i < num; i++) {
		if(getVarReg(i) != 0 && getMemactive(i) == 0) {
			if(MemIsNull(i))
				pareMem(i);
			spill(i);
		}
	}
}
void spill(int varindex) {
	AddrDescrip addrdescrip = &(AddrDescripTable[varindex]);
	genSW(getOneReg(addrdescrip->reg), addrdescrip->mem->k, addrdescrip->mem->reg);	
	updateDesSW(addrdescrip->reg, varindex);
	activeMem(varindex);
}
int getReg(int varindex) {
	int r = 1, i, min = 9999, this;
	AddrDescrip addrdescrip = NULL;
	if(varindex == -1) {
		// the op isn't var. it just a constant, but it needs a reg to save its value.
	}else {
		addrdescrip = &(AddrDescripTable[varindex]);
//		if(MemIsNull(varindex))
//			pareMem(varindex);
	}
	if(addrdescrip != NULL && addrdescrip->reg != 0) 
		return getOneReg(addrdescrip->reg);
	else if(idleReg != 0) {
		r = getOneReg(idleReg);
		idleReg = idleReg ^ r;
		return r;
	}else{
//		for(i = 0; i < REG_NUM; i++) {
//			r = 1 << i;
//			if(vInOther(r))
//				return r;
//		}	
		for(i = 0; i < REG_NUM; i++) {
			this = countVar(regMap[i].varvec);
			if(this < min) {
				min = this;
				r = 1 << i;
			}
		}	
		spillAll(r);
		return r;
	}
}
//int Allocate(Operand op) {
//	if(idleReg != 0) 
//		return getOneReg(idleReg);
//	else {
//		
//	}
//}
//int Ensure(Operand op) {
//	int result;
//	AsmCode code;
//	return T0;
//	if(op->map->reg != 0) 
//		result = getOneReg(op->map->reg);
//	else {
////		result = Allocate(op);	
////		code = newAsmCode(A_LW);
////		code->x = result;
//////		code->y = 
////		addAsmCode(code);
//	}
//}
//void Free(int reg) {
//	idleReg = idleReg | reg;
//}
void updateDesLW(int reg, int varindex) {
	int varnum = getAllVarNum(), i;
	AddrDescrip addrdescrip = &(AddrDescripTable[varindex]);
	setRegDes(reg, varindex);
	addrdescrip->reg = addrdescrip->reg | reg;
	for(i = 0; i < varnum; i++) {
		if(i == varindex)
			continue;
		if((AddrDescripTable[i].reg & reg) != 0) {
			AddrDescripTable[i].reg = (AddrDescripTable[i].reg ^ reg);
		}
	}
}
void updateDesSW(int reg, int varindex) {
	AddrDescrip addrdescrip = &(AddrDescripTable[varindex]);
	activeMem(varindex);
}
void updateDesIR3(int reg, int varindex){
	int varnum = getAllVarNum(), i;
	AddrDescrip addrdescrip = &(AddrDescripTable[varindex]);
	setRegDes(reg, varindex);
	addrdescrip->reg = reg;
	unactiveMem(varindex);
	for(i = 0; i < varnum; i++) {
		if(i == varindex)
			continue;
		if((AddrDescripTable[i].reg & reg) != 0) {
			AddrDescripTable[i].reg = (AddrDescripTable[i].reg ^ reg);
		}
	}
}
void updateLITemp(int reg) {
	int varnum = getAllVarNum(), i;
	clearRegDes(reg);
	for(i = 0; i < varnum; i++) {
		if((AddrDescripTable[i].reg & reg) != 0) {
			AddrDescripTable[i].reg = (AddrDescripTable[i].reg ^ reg);
		}
	}
}
void updateDesIReq(int reg, int varindex) {
	AddrDescrip addrdescrip = &(AddrDescripTable[varindex]);
	clearVarInAllReg(varindex);
	addVar2Reg(reg, varindex);
	addrdescrip->reg = reg;
	unactiveMem(varindex);
}
