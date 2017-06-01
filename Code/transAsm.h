#ifndef __TRANSASM_H_
#define __TRANSASM_H_

#include "IR.h"

void pareMem(int varindex);

void transAllAsm(InterCodes IRhead);
void printfAsmHead(FILE *tag);
void printfReadAsm(FILE *tag); 
void printfWriteAsm(FILE *tag); 
#endif
