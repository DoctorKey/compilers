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
	if(count != 0){
		count = count/32;
		dimension++;
	}
}
void updatemap(Operand op) {
	if(op->kind != TEMP_OP && op->kind != VARIABLE_OP)
		return;
	if(op->varnum != 0)
		return;
	allvarnum++;
	op->varnum = allvarnum;
	return;
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
	op->map->varvec = (int*) malloc(sizeof(int)*dimension);
	temp = op->map->num;
	for(i = 0; i < dimension; i++) {
		vec = temp % 32 - 1;
		temp = temp / 32;
		if(vec >= 0)
			op->map->varvec[i] = 1 << vec; 
		else
			op->map->varvec[i] = 0;
	}
#ifdef DEBUG3
	fprintf(stdout, "%s num:%d ", Optostring(op), op->map->num);	
	for(i = 0; i < dimension; i++) {
		fprintf(stdout, "%8x ", op->map->varvec[i]);
	}
	fprintf(stdout, "\n");
#endif
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
//int getReg(Operand op) {
//	int r = 1, i;
//	if(op->map->reg != 0) 
//		return getOneReg(op->map->reg);
//	else if(idleReg != 0) 
//		return getOneReg(idleReg);
//	else{
//		for(i = 0; i < REG_NUM; i++) {
//			r = 1 << i;
//		}	
//	}
//}
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
