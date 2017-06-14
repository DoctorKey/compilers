#include "vec.h"
#include "main.h"
#include "mips32.h"
#include "VarRegMap.h"
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
vecType *newVec() {
	vecType *varvec = NULL;
	int dim = getVecDim();
	varvec = (vecType*) malloc(sizeof(vecType)*dim);
	if(varvec == NULL){
		fprintf(stderr, "can't malloc\n");
		return NULL;
	}
	clearVec(varvec);
	return varvec;
}
void freeVarVec(vecType *varvec) {
	if(varvec == NULL)
		return;
	free(varvec);
}
/*
	single compute
*/
void clearVec(vecType *varvec) {
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
//int *VecCompute(vecType *varvec1, char compute, vecType *varvec2) {
//	int i;
//	vecType vec;
//	vecType *result = newVec();
//	int dimension = getVecDim();
//	if(varvec2 == 0) {
//		varvec2 = newVec();
//	}
//	if(compute == '=') {
//		for(i = 0; i < dimension; i++) {
//			varvec1[i] = varvec2[i];	
//		}
//	}else if(compute == '&') {
//		for(i = 0; i < dimension; i++) {
//			varvec1[i] = varvec1[i] & varvec2[i];	
//		}
//	}else if(compute == '|') {
//		for(i = 0; i < dimension; i++) {
//			varvec1[i] = varvec1[i] | varvec2[i];	
//		}
//	}else if(compute == '^') {
//		for(i = 0; i < dimension; i++) {
//			varvec1[i] = varvec1[i] ^ varvec2[i];	
//		}
//	}else if(compute == '$') {
//	// 1 $ 1 = 0, 1 $ 0 = 1, 0 $ 0 = 0, 0 $ 1 = 0;
//		for(i = 0; i < dimension; i++) {
//			vec = varvec1[i];
//			varvec1[i] = varvec1[i] ^ varvec2[i];	
//			varvec1[i] = varvec1[i] & vec;
//		}
//	}
//}
int updateDimension(int varnum) {
//	int count = varnum;
//	int dim = 0;
//	dim = 0;
//	while(count != 0){
//		count = count / VARNUM_IN_ONE_VEC;
//		dim++;
//	}
	int dim = varnum / VARNUM_IN_ONE_VEC + 1;
	return dim;
}
int getDim(int num) {
//	int i = 0;
//	while(num / 32 != 0) {
//		num = num / 32;
//		i++;
//	}
//	return i;
	return num / VARNUM_IN_ONE_VEC;
}
vecType getVec(int num) {
	vecType i;
//	i = num % 32;
	i = num % VARNUM_IN_ONE_VEC;
	return 1 << i;
}
int countVar(vecType *varvec) {
	int i, count = 0, j;
	vecType vec;
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
#ifdef DEBUG4
	printfVarByVec(stdout, varvec); 
	printfVec(stdout, varvec);	
	fprintf(stdout, "count is %d\n", count);
#endif
	return count;
}
int VecIs0(vecType *varvec) {
	int i;
	int dim = getVecDim();
	for(i = 0; i < dim; i++) {
		if(varvec[i] != 0)
			return false;
	}
	return true;
}
//int my_pow(int n,int i) {
//	int result = 1;
//	if(i == 0)
//		return 1;
//	while(i != 0) {
//		result = result * n;
//		i--;
//	}
//	return result;
//}
//int Vec2Index(vecType *varvec) {
//	int result = 0, i, j;
//	int dim = getVecDim();
//	for(i = 0; i < dim; i++) {
//		j = getRegindex(varvec[i]);	
//		result = result + my_pow(32, i) * j;
//	}
//	return result;
//}
void printfVec(FILE *tag, vecType *varvec) {
	int dim = getVecDim();
	int i;
	for(i = 0; i < dim; i++) {
		fprintf(tag, "%x ", varvec[i]);
	}
}
