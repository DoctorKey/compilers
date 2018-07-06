#ifndef __VEC_H_
#define __VEC_H_

#include <stdio.h>

#define VARNUM_IN_ONE_VEC 8
typedef char vecType;

void setVecDim(int d); 
int getVecDim(); 

vecType *newVec(); 
void freeVarVec(vecType *varvec); 

void clearVec(vecType *varvec); 
//int* VecCompute(int *varvec1, char compute, int *varvec2); 

int updateDimension(int varnum); 
int getDim(int num); 
vecType getVec(int num); 
int countVar(vecType *varvec); 
int VecIs0(vecType *varvec); 
//int Vec2Index(vecType *varvec); 
void printfVec(FILE *tag, vecType *varvec); 

#endif
