#ifndef __MIPS32_H_
#define __MIPS32_H_

#define ZERO 0x1
#define AT 0x2

#define V0 0x4
#define V1 0x8

#define A0 0x10
#define A1 0x20
#define A2 0x40
#define A3 0x80

#define T0 0x100
#define T1 0x200
#define T2 0x400
#define T3 0x800
#define T4 0x1000
#define T5 0x2000
#define T6 0x4000
#define T7 0x8000

#define S0 0x10000
#define S1 0x20000
#define S2 0x40000
#define S3 0x80000
#define S4 0x100000
#define S5 0x200000
#define S6 0x400000
#define S7 0x800000

#define T8 0x1000000
#define T9 0x2000000

#define K0 0x4000000
#define K1 0x8000000

#define GP 0x10000000
#define SP 0x20000000
#define FP 0x40000000
#define RA 0x80000000

#define REG_NUM 32
extern int idleReg;

struct RegMap {
	int reg;
	int *varvec;
};
extern struct RegMap regMap[REG_NUM];
void addVar2Reg(int reg, int varindex); 
void setRegDes(int reg, int varindex);
void initRegMap(); 
void printfRegMap(FILE *tag); 
int getOneReg(int reg);
int isVarInReg(int var, int reg); 
int getRegindex(int reg); 
char *getRegName(int reg); 
void printfAllReg(FILE *tag, int regvec); 

typedef struct Mem_* Mem;
struct Mem_ {
	int reg;
	int k;
};
Mem newMem(int reg, int k);
void spillAll(int reg); 
void printfMem(FILE *tag, Mem mem); 
#endif

