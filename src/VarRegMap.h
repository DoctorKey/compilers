#ifndef __VARREGMAP_H_
#define __VARREGMAP_H_

#include "IR.h"
#include "mips32.h"
#include "vec.h"
int getAllVarNum(); 

typedef struct AddrDescrip_* AddrDescrip;
struct AddrDescrip_ {
	struct Operand_* op;
	int index;
	vecType *varvec;
	int reg;
	int memactive;
	Mem mem;
};
void activeMem(int varindex); 
void unactiveMem(int varindex); 
int getMemactive(int varindex); 
void setMem(int varindex, int reg, int k); 
int getMemk(int varindex); 
int getMemReg(int varindex); 
int MemIsNull(int varindex); 

void clearAddrDesTable(); 
void printfAddrDescripTable(FILE *tag); 
void printfVarByVec(FILE *tag, vecType *varvec);
void initmap(InterCodes IRhead);

void spillAllVar(); 
void spill(int varindex);
int getReg(int varindex);

void updateDesLW(int reg, int varindex);
void updateDesSW(int reg, int varindex);
void updateDesIR3(int reg, int varindex);
void updateLITemp(int reg); 
void updateDesIReq(int reg, int varindex);
#endif
