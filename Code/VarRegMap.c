#include "VarRegMap.h"
#include "mips32.h"
#include "asm.h"

//#define DEBUG3 1
AddrDescrip AddrDescripTable = NULL;
/*--------------------------------------------------
	VarVec
----------------------------------------------------*/
int allvarnum = 0;
int dimension = 0;
int getAllVarNum() {
	return allvarnum;
#ifdef DEBUG3
	fprintf(stdout, "allvarnum is %d\n", allvarnum);
#endif
}
int getDimension() {
	return dimension;
#ifdef DEBUG3
	fprintf(stdout, "dimension is %d\n", dimension);
#endif
}
int updateDimension(int varnum) {
	int count = varnum;
	int dim = 0;
	dim = 0;
	while(count != 0){
		count = count/32;
		dim++;
	}
	return dim;
}
int *newVarVec() {
	int *varvec = NULL;
	int dim = getDimension();
	varvec = (int*) malloc(sizeof(int)*dim);
	if(varvec == NULL){
		fprintf(stderr, "can't malloc\n");
		return NULL;
	}
	while(--dim >= 0) {
		varvec[dim] = 0;
	}
	return varvec;
}
void freeVarVec(int *varvec) {
	if(varvec == NULL)
		return;
	free(varvec);
}
void clearVec(int *varvec) {
	int dim = getDimension();
	while(--dim >= 0) {
		varvec[dim] = 0;
	}
}
int getDim(int num) {
	int i = 0;
	while(num / 32 != 0) {
		num = num / 32;
		i++;
	}
	return i;
}
int getVec(int num) {
	int i;
	i = num % 32;
	return 1 << i;
}
int countVar(int *varvec) {
	int i, count = 0, vec, j;
	for(i = 0;i < dimension; i++) {
		vec = varvec[i];
		for(j = 0; j < 32; j++) {
			if(vec & 0x1 == 0x1) {
				count++;
			}
			vec = vec >> 1;
		}
	}
	return count;
}
int *VecCompute(int *varvec1, char compute, int *varvec2) {
	int i, vec;
	int *result = newVarVec();
	if(varvec2 == 0) {
		varvec2 = newVarVec();
	}
	if(compute == '=') {
		for(i = 0; i < dimension; i++) {
			varvec1[i] = varvec2[i];	
		}
	}else if(compute == '&') {
		for(i = 0; i < dimension; i++) {
			varvec1[i] = varvec1[i] & varvec2[i];	
		}
	}else if(compute == '|') {
		for(i = 0; i < dimension; i++) {
			varvec1[i] = varvec1[i] | varvec2[i];	
		}
	}else if(compute == '^') {
		for(i = 0; i < dimension; i++) {
			varvec1[i] = varvec1[i] ^ varvec2[i];	
		}
	}else if(compute == '$') {
	// 1 $ 1 = 0, 1 $ 0 = 1, 0 $ 0 = 0, 0 $ 1 = 0;
		for(i = 0; i < dimension; i++) {
			vec = varvec1[i];
			varvec1[i] = varvec1[i] ^ varvec2[i];	
			varvec1[i] = varvec1[i] & vec;
		}
	}
}
int VecIs0(int *varvec) {
	int i;
	for(i = 0; i < dimension; i++) {
		if(varvec[i] != 0)
			return false;
	}
	return true;
}
int my_pow(int n,int i) {
	int result = 1;
	if(i == 0)
		return 1;
	while(i != 0) {
		result = result * n;
		i--;
	}
	return result;
}
int Vec2Index(int *varvec) {
	int result = 0, i, j;
	for(i = 0; i < dimension; i++) {
		j = getRegindex(varvec[i]);	
		result = result + my_pow(32, i) * j;
	}
	return result;
}
void printfVec(FILE *tag, int *varvec) {
	int dim = getDimension();
	int i;
	for(i = 0; i < dim; i++) {
		fprintf(tag, "%08x ", varvec[i]);
	}
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
		AddrDescripTable[varnum].varvec = newVarVec();
		AddrDescripTable[varnum].reg = 0;
		AddrDescripTable[varnum].mem = newMem(0, 0);
		AddrDescripTable[varnum].varvec[getDim(varnum)] = getVec(varnum);
	}
}
int getMemk(int varindex) {
	return AddrDescripTable[varindex].mem->k;
}
int getMemReg(int varindex) {
	return AddrDescripTable[varindex].mem->reg;
}
void printfAddrDescripTable(FILE *tag) {
	int num = getAllVarNum();
	int dim = getDimension();
	int i;
	fprintf(tag, "------------------addr descrip table--------------------\n");
	fprintf(tag, "[index]\top\tmem\treg\n");
	for(i = 0; i < num; i++) {
		fprintf(tag, "[%d]", i);
		fprintf(tag, "\t%s\t", Optostring(AddrDescripTable[i].op));
		if(AddrDescripTable[i].memvilid = 1) {
			fprintf(tag, "\033[31m\033[1m");
			printfMem(tag, AddrDescripTable[i].mem);
			fprintf(tag, "\033[0m");
		}else {
			printfMem(tag, AddrDescripTable[i].mem);
		}
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
void spill(int varindex) {
	AddrDescrip addrdescrip = &(AddrDescripTable[varindex]);
	genSW(getOneReg(addrdescrip->reg), addrdescrip->mem->k, addrdescrip->mem->reg);	
	updateDesSW(addrdescrip->reg, varindex);
}
int getReg(int varindex) {
	int r = 1, i, min = 9999, this;
	AddrDescrip addrdescrip = NULL;
	if(varindex == -1) {
		// the op isn't var. it just a constant, but it needs a reg to save its value.
	}else {
		addrdescrip = &(AddrDescripTable[varindex]);
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
			AddrDescripTable[i].reg = AddrDescripTable[i].reg ^ reg;
		}
	}
}
void updateDesSW(int reg, int varindex) {
	AddrDescrip addrdescrip = &(AddrDescripTable[varindex]);
	addrdescrip->memvilid = 1;
}
void updateDesIR3(int reg, int varindex){
	int varnum = getAllVarNum(), i;
	AddrDescrip addrdescrip = &(AddrDescripTable[varindex]);
	setRegDes(reg, varindex);
	addrdescrip->reg = reg;
	addrdescrip->memvilid = 0;
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
	addVar2Reg(reg, varindex);
	addrdescrip->reg = reg;
	addrdescrip->memvilid = 0;
}
