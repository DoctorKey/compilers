#include "VarRegMap.h"
#include "mips32.h"
#include "asm.h"

#define DEBUG3 1
int allvarnum = 0;
int dimension = 0;
varregmap var2regtable = NULL;

int getDimension() {
	return dimension;
}
void initVar2RegTable() {
	var2regtable = (varregmap) malloc(sizeof(struct varregmap_) * allvarnum);
	if(var2regtable == NULL) {
		fprintf(stderr, "can't malloc\n");
		return;
	}
}
void getdimension() {
	int count = allvarnum;
	dimension = 0;
	while(count != 0){
		count = count/32;
		dimension++;
	}
#ifdef DEBUG3
	fprintf(stdout, "allvarnum is %d\n", allvarnum);
	fprintf(stdout, "dimension is %d\n", dimension);
#endif
}
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
void updatemap2(Operand op) {
	int temp = 0, i, vec;
	if(op->kind != TEMP_OP && op->kind != VARIABLE_OP)
		return;
	if(op->map != NULL)
		return;
	op->map = var2regtable + op->varnum;
	op->map->op = op;
	op->map->reg = 0;
	op->map->num = op->varnum;
	op->map->mem = (Mem) malloc(sizeof(struct Mem_));
	op->map->mem->reg = 0;
	op->map->mem->k = 0;
	op->map->varvec = (int*) malloc(sizeof(int)*dimension);
	for(i = 0; i < dimension; i++) {
		op->map->varvec[i] = 0;
	}
	temp = op->map->num;
	op->map->varvec[getDim(temp)] = getVec(temp);
#ifdef DEBUG3
	fprintf(stdout, "%s num:%d ", Optostring(op), op->map->num);	
	printfVec(stdout, op->map->varvec);
	fprintf(stdout, "\n");
#endif
	return;
}
void printfVec(FILE *tag, int *varvec) {
	int i;
	for(i = 0; i < dimension; i++) {
		fprintf(tag, "%08x ", varvec[i]);
	}
}
void printfVarByVec(FILE *tag, int *varvec) {
	int i;
	for(i = 0; i < allvarnum; i++) {
		if((varvec[getDim(i)] & getVec(i)) == 0) {
		
		}else {
			fprintf(tag, "%s,", Optostring(var2regtable[i].op));
		}
	}
}
int getVarNum(int *varvec) {
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
void VecCompute(int *varvec1, char compute, int *varvec2) {
	int i, vec;
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
int pow(int n,int i) {
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
		result = result + pow(32, i) * j;
	}
	return result;
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
	getdimension();
	initVar2RegTable();
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
int vInOther(int reg) {
	int *v = (int*) malloc(sizeof(int) * dimension);
	int i;
	VecCompute(v, '=', regMap[getRegindex(reg)].varvec);
	for(i = 0; i < REG_NUM; i++) {
		if(i == getRegindex(reg))
			continue;
		VecCompute(v, '$', regMap[i].varvec);
	}
	if(VecIs0(v)) {
		free(v);
		return true;
	}else {
		free(v);
		return false;
	}
}
RegSet getReg1(Operand op) {
	static struct RegSet_ regset;
	int r = 1, i, min = 9999, this;
	if(op->map->reg != 0) {
		regset.rx = getOneReg(op->map->reg);
		return &regset;
	}else if(idleReg != 0) {
		regset.rx = getOneReg(idleReg);
		idleReg = idleReg ^ regset.rx;
		return &regset;
	}else{
		for(i = 0; i < REG_NUM; i++) {
			r = 1 << i;
			if(vInOther(r)) {
				regset.rx = r;
				return &regset;
			}
		}	
		for(i = 0; i < REG_NUM; i++) {
			this = getVarNum(regMap[i].varvec);
			if(this < min) {
				min = this;
				r = 1 << i;
			}
		}	
//		spillAll(r);
		regset.rx = r;
		return &regset;
	}
}
RegSet getReg(Operand op) {
	static struct RegSet_ regset;
	int r = 1, i, min = 9999, this;
	if(op->map->reg != 0) 
		return getOneReg(op->map->reg);
	else if(idleReg != 0) 
		return getOneReg(idleReg);
	else{
		for(i = 0; i < REG_NUM; i++) {
			r = 1 << i;
			if(vInOther(r))
				return r;
		}	
		for(i = 0; i < REG_NUM; i++) {
			this = getVarNum(regMap[i].varvec);
			if(this < min) {
				min = this;
				r = 1 << i;
			}
		}	
//		spill(r);
		return r;
	}
}
int Allocate(Operand op) {
	if(idleReg != 0) 
		return getOneReg(idleReg);
	else {
		
	}
}
int Ensure(Operand op) {
	int result;
	AsmCode code;
	return T0;
	if(op->map->reg != 0) 
		result = getOneReg(op->map->reg);
	else {
		result = Allocate(op);	
		code = newAsmCode(A_LW);
		code->x = result;
//		code->y = 
		addAsmCode(code);
	}
}
void Free(int reg) {
	idleReg = idleReg | reg;
}
void printfVar2RegTable(FILE *tag) {
	int num = allvarnum, dim = dimension;
	int i, j;
	fprintf(tag, "------------------Var to Reg----------------------------\n");
	fprintf(tag, "[index]\top\tmem\treg\n");
	for(i = 0; i < num; i++) {
		fprintf(tag, "[%d]", i);
		fprintf(tag, "\t%s\t", Optostring(var2regtable[i].op));
//		fprintf(tag, "\t%d\t", var2regtable[i].num);
//		printfVec(tag, var2regtable[i].varvec);
		printfMem(tag, var2regtable[i].mem);
//		fprintf(tag, "\t%08x\n", var2regtable[i].reg);
		printfAllReg(tag, var2regtable[i].reg);
		fprintf(tag, "\n");
	}
	fprintf(tag, "------------------End-----------------------------------\n");
}
