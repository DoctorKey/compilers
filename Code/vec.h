#ifndef __VEC_H_
#define __VEC_H_

#include <stdio.h>

void setVecDim(int d); 
int getVecDim(); 

int *newVec(); 
void freeVarVec(int *varvec); 

void clearVec(int *varvec); 
int* VecCompute(int *varvec1, char compute, int *varvec2); 

int updateDimension(int varnum); 
int getDim(int num); 
int getVec(int num); 
int countVar(int *varvec); 
int VecIs0(int *varvec); 
int Vec2Index(int *varvec); 
void printfVec(FILE *tag, int *varvec); 

#endif
