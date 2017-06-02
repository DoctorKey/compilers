#include "vec.h"
#include "main.h"
#include "mips32.h"
/*--------------------------------------------------
	VarVec
----------------------------------------------------*/
/*
	dimension is a constant
	only set once
*/
int vecdim = 0;
void setVecDim(int d) {
	vecdim = d;
}
int getVecDim() {
	return vecdim;
}
/*
	memory management
*/
int *newVec() {
	int *varvec = NULL;
	int dim = getVecDim();
	varvec = (int*) malloc(sizeof(int)*dim);
	if(varvec == NULL){
		fprintf(stderr, "can't malloc\n");
		return NULL;
	}
	clearVec(varvec);
	return varvec;
}
void freeVarVec(int *varvec) {
	if(varvec == NULL)
		return;
	free(varvec);
}
/*
	single compute
*/
void clearVec(int *varvec) {
	int dim = getVecDim();
	if(varvec == NULL)
		return;
	while(--dim >= 0) {
		varvec[dim] = 0;
	}
}
/*
	two vec compute
*/
int *VecCompute(int *varvec1, char compute, int *varvec2) {
	int i, vec;
	int *result = newVec();
	int dimension = getVecDim();
	if(varvec2 == 0) {
		varvec2 = newVec();
	}
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
int updateDimension(int varnum) {
	int count = varnum;
	int dim = 0;
	dim = 0;
	while(count != 0){
		count = count/32;
		dim++;
	}
	return dim;
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
int countVar(int *varvec) {
	int i, count = 0, vec, j;
	int dim = getVecDim();
	for(i = 0;i < dim; i++) {
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
int VecIs0(int *varvec) {
	int i;
	int dim = getVecDim();
	for(i = 0; i < dim; i++) {
		if(varvec[i] != 0)
			return false;
	}
	return true;
}
int my_pow(int n,int i) {
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
	int dim = getVecDim();
	for(i = 0; i < dim; i++) {
		j = getRegindex(varvec[i]);	
		result = result + my_pow(32, i) * j;
	}
	return result;
}
void printfVec(FILE *tag, int *varvec) {
	int dim = getVecDim();
	int i;
	for(i = 0; i < dim; i++) {
		fprintf(tag, "%08x ", varvec[i]);
	}
}
