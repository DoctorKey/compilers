#include "main.h"
#include "mips32.h"
#include "VarRegMap.h"

struct RegMap regMap[REG_NUM];
int idleReg = 0xffffffff;
void initRegMap() {
	int i, j;
	for(i = 0; i < REG_NUM; i++) {
		regMap[i].reg = 1 << i;
		regMap[i].varvec = (int*) malloc(sizeof(int)*dimension);
		for(j = 0; j < dimension; j++) {
			regMap[i].varvec[j] = 0;
		}
	}
}
int isOpInReg(Operand op, int reg) {
	int varnum = op->varnum;
	int dim = getDim(varnum);
	int vec = getVec(varnum);
	vec = vec & regMap[getRegindex(reg)].varvec[dim];	
	if(vec == 0) {
		return false;
	}else {
		return true;
	}
}
int getRegindex(int reg) {
	int index = 1;
	while(reg & 0x1 != 0x1) {
		index++;
		reg = reg >> 1;
	}
	return index;
}
void addVar2Reg(int reg, Operand op) {
	int i = getRegindex(reg);	
	int j;
	for(j = 0; j < dimension; j++) {
		regMap[i].varvec[j] = regMap[i].varvec[j] | op->map->varvec[j];
	}
}
int getOneReg(int reg) {
	int result = 1;
	while(reg & result == 0) {
		result = result << 1;
	}
	return result;
}
char *getRegName(int reg) {
	switch(reg) {
	case ZERO: return strdup("$zero");
	case AT: return strdup("$at");
	case V0: return strdup("$v0");
	case V1: return strdup("$v1");
	case A0: return strdup("$a0");
	case A1: return strdup("$a1");
	case A2: return strdup("$a2");
	case A3: return strdup("$a3");
	case T0: return strdup("$t0");
	case T1: return strdup("$t1");
	case T2: return strdup("$t2");
	case T3: return strdup("$t3");
	case T4: return strdup("$t4");
	case T5: return strdup("$t5");
	case T6: return strdup("$t6");
	case T7: return strdup("$t7");
	case S0: return strdup("$s0");
	case S1: return strdup("$s1");
	case S2: return strdup("$s2");
	case S3: return strdup("$s3");
	case S4: return strdup("$s4");
	case S5: return strdup("$s5");
	case S6: return strdup("$s6");
	case S7: return strdup("$s7");
	case T8: return strdup("$t8");
	case T9: return strdup("$t9");
	case K0: return strdup("$k0");
	case K1: return strdup("$k1");
	case GP: return strdup("$gp");
	case SP: return strdup("$sp");
	case FP: return strdup("$fp");
	case RA: return strdup("$ra");
	default: return strdup("null");
	}
}
void printfAllReg(FILE *tag, int regvec) {
	int i, reg;
	for(i = 0; i < REG_NUM; i++) {
		reg = 1 << i;
		if((reg & regvec) != 0) {
			fprintf(tag, "%s,", getRegName(reg));
		}
	}
}
void printfRegMap(FILE *tag) {
	int i, j;
	fprintf(tag, "------------------------Reg Map-------------------------\n");
	fprintf(tag, "idle Reg: %08x\n", idleReg);
	fprintf(tag, "[index]\tReg\tvec\t[index]\tReg\tvec\n");
	for(i = 0; i < REG_NUM / 2; i++) {
		if(regMap[i].reg & idleReg == 0) {
			fprintf(tag, "\033[31m\033[1m");
			fprintf(tag, "[%d]", i);	
			fprintf(tag, "\033[0m");
		}else {
			fprintf(tag, "[%d]", i);	
		}
		fprintf(tag, "\t%s", getRegName(regMap[i].reg));
		fprintf(tag, "\t");
		printfVarByVec(tag, regMap[i].varvec);
		fprintf(tag, "\t");
		j = i + REG_NUM / 2;
		if(regMap[j].reg & idleReg == 0) {
			fprintf(tag, "\033[31m\033[1m");
			fprintf(tag, "[%d]", j);	
			fprintf(tag, "\033[0m");
		}else {
			fprintf(tag, "[%d]", j);	
		}
		fprintf(tag, "\t%s", getRegName(regMap[j].reg));
		fprintf(tag, "\t");
		printfVarByVec(tag, regMap[j].varvec);
		fprintf(tag, "\n");
	}
	fprintf(tag, "------------------------End-----------------------------\n");
}
/*---------------------------------------------------
	MEM

-----------------------------------------------------
*/
void printfMem(FILE *tag, Mem mem) {
	fprintf(tag, "%d(%s)", mem->k, getRegName(mem->reg));	
}
