#include "main.h"
#include "mips32.h"
#include "VarRegMap.h"

struct RegMap regMap[REG_NUM];
int tempReg = 0|T0|T1|T2|T3|T4|T5|T6|T7|T8|T9|S0|S1|S2|S3|S4|S5|S6|S7;
int idleReg = 0|T0|T1|T2|T3|T4|T5|T6|T7|T8|T9|S0|S1|S2|S3|S4|S5|S6|S7;
void updateIdleReg() {
	int i, reg;
	for(i = 0; i < REG_NUM; i++) {
		reg = 1 << i;
		if((reg & tempReg) == 0)
			continue;
		if(VecIs0(regMap[i].varvec)) {
			idleReg = (idleReg | reg);
		}
	}
}
void initRegMap() {
	int i, j;
	for(i = 0; i < REG_NUM; i++) {
		regMap[i].reg = 1 << i;
		regMap[i].varvec = newVec();
	}
}
int isVarInReg(int var, int reg) {
	int varnum = var;
	int dim = getDim(varnum);
	int vec = getVec(varnum);
	vec = (vec & regMap[getRegindex(reg)].varvec[dim]);	
	if(vec == 0) {
		return false;
	}else {
		return true;
	}
}
int getRegindex(int reg) {
	int index = 0;
	while((reg & 0x1) != 0x1) {
		index++;
		reg = reg >> 1;
	}
	return index;
}
void addVar2Reg(int reg, int varindex) {
	int i = getRegindex(reg);	
	int j = getDim(varindex);
	regMap[i].varvec[j] = regMap[i].varvec[j] | getVec(varindex);
}
void clearVarInAllReg(int varindex) {
	int i;
	int j = getDim(varindex);
	for(i = 0; i < REG_NUM; i++) {
		if((regMap[i].varvec[j] & getVec(varindex)) != 0) {
			regMap[i].varvec[j] = (regMap[i].varvec[j] ^ getVec(varindex));
		}
	}
	updateIdleReg();
}
void setRegDes(int reg, int varindex) {
	int i = getRegindex(reg);	
	int dim = getVecDim();
	int j = getDim(varindex);
	clearVec(&(regMap[i].varvec[dim]));
	regMap[i].varvec[j] = getVec(varindex);
}
void clearRegDes(int reg) {
	int i = getRegindex(reg);	
	int dim = getVecDim();
	clearVec(regMap[i].varvec);
//	clearVec(&(regMap[i].varvec[dim]));
	updateIdleReg();
}
void clearRegMap() {
	int reg, i;
	for(i = 0; i < REG_NUM; i++) {
		reg = 1 << i;
		clearRegDes(reg);
	}
	updateIdleReg();
}
int getOneReg(int reg) {
	int result = 1;
	if(reg == 0) {
		fprintf(stderr, "no idle reg\n");
		return 0;
	}
	while((reg & result) == 0) {
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
		fprintf(tag, "[%d]", i);	
		if((regMap[i].reg & idleReg) == 0) {
			fprintf(tag, "*");
		}
		fprintf(tag, "\t%s", getRegName(regMap[i].reg));
		fprintf(tag, "\t");
		printfVarByVec(tag, regMap[i].varvec);
//		printfVec(tag, regMap[i].varvec);
		fprintf(tag, "\t");
		j = i + REG_NUM / 2;
		fprintf(tag, "[%d]", j);	
		if((regMap[j].reg & idleReg) == 0) {
			fprintf(tag, "*");
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
Mem newMem(int reg, int k) {
	Mem result = NULL;
	result = (Mem) malloc(sizeof(struct Mem_));
	if(result == NULL) {
		fprintf(stderr, "can't malloc\n");
		return NULL;
	}
	result->reg = reg;
	result->k = k;
	return result;
}
void printfMem(FILE *tag, Mem mem) {
	fprintf(tag, "%d(%s)", mem->k, getRegName(mem->reg));	
}
void spillAllReg() {
	int reg, i;
	for(i = 0; i < REG_NUM; i++) {
		reg = 1 << i;
		spillAll(reg);
	}
}
void spillAll(int reg) {
	int varnum = getAllVarNum();
	int *varvec = regMap[getRegindex(reg)].varvec;
	int i;
	for(i = 0; i < varnum; i++) {
		if((varvec[getDim(i)] & getVec(i)) != 0) {
			spill(i);
		}
	}
}
