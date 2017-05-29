#ifndef __VARREGMAP_H_
#define __VARREGMAP_H_

#include "IR.h"
#include "mips32.h"

int getAllVarNum(); 
int getDimension(); 
int updateDimension(int varnum); 
int *newVarVec(); 
void freeVarVec(int *varvec); 
int Vec2Index(int *varvec); 
int countVar(int *varvec); 
int VecIs0(int *varvec); 
int* VecCompute(int *varvec1, char compute, int *varvec2); 
int getVarNum(int *varvec); 
int getDim(int num); 
int getVec(int num); 

typedef struct AddrDescrip_* AddrDescrip;
struct AddrDescrip_ {
	struct Operand_* op;
	int index;
	int *varvec;
	int reg;
	int memvilid;
	Mem mem;
};
int getMemk(int varindex); 
int getMemReg(int varindex); 

void printfAddrDescripTable(FILE *tag); 
void printfVarByVec(FILE *tag, int *varvec);
int getDimension();
void initmap(InterCodes IRhead);
//void Free(int reg); 
//int Ensure(Operand op); 

void spill(int varindex);
int getReg(int varindex);

void updateDesLW(int reg, int varindex);
void updateDesSW(int reg, int varindex);
void updateDesIR3(int reg, int varindex);
void updateDesIReq(int reg, int varindex);
#endif
