#ifndef __IR_H_
#define __IR_H_

#include "symtable.h"

typedef enum {TEMP_OP, VARIABLE_OP, CONSTANT_OP, FUNC_OP,
		RELOP_OP, LABEL_OP, SIZE_OP} OP_KIND;
typedef struct Operand_* Operand;
typedef struct Operandlist_* Operandlist;
typedef struct IRinfo_* IRinfo;
struct IRinfo_ {
	OP_KIND kind;
	Operand op;
	IRinfo next;
	Operandlist truelist, falselist, nextlist;
};
IRinfo newIRinfo(); 

struct Operand_ {
	OP_KIND kind;
	enum {String, Int, Float} type;
	int isAddr;
	int isArray;
	union {
		int num_int;
		float num_float;
		char *str;
	};
};

struct Operandlist_ {
	Operand op;
	Operandlist next;
};
Operandlist Opmakelist(Operand op); 
Operandlist Opmerge(Operandlist oplist1, Operandlist oplist2); 
void Opbackpatch(Operandlist oplist, Operand label); 
void showOplist(Operandlist oplist);

void Mpush(Operand M); 
Operand Mpop(); 
void Npush(Operandlist N); 
Operandlist Npop(); 

typedef enum { LABEL_IR, FUNCTION_IR, 
		ASSIGN_IR, ADD_IR, SUB_IR, MUL_IR, DIV_IR, 
		GOTO_IR, IF_IR, RETURN_IR, DEC_IR, 
		ARG_IR, CALL_IR, PARAM_IR, READ_IR, WRITE_IR } InterCodeKind;
typedef struct InterCode_* InterCode;
struct InterCode_ {
	InterCodeKind kind;
	int isComputeAddr;
	union {
		struct {
			Operand op1;
		}op1;
		struct {
			Operand result, right;
		}op2;
		struct {
			Operand result, right1, right2;
		}op3;
		struct {
			Operand x, relop, y, z;
		}op4;
	};
};
typedef struct InterCodes_* InterCodes;
struct InterCodes_ {
	InterCode code;
	int isblockhead;
	int funcnum;
	InterCodes prev, next;
};

void IR_init(); 
Operand newOperand(int kind); 
Operand newTemp(); 
Operand newLabel(); 
char *Optostring(Operand op); 
InterCode newInterCode(); 
InterCodes newInterCodes(InterCode ir); 
void addIR(InterCode ir); 
void freeIR(InterCodes ir);

InterCode LabelIR(int n); 
InterCode FunctionIR(char *funcname); 
InterCode Assign2IR(Operand x, Operand y); 
InterCode Assign3IR(Operand x, Operand y, int kind, Operand z); 
InterCode Assign3AddrIR(Operand x, Operand y, int kind, Operand z); 
InterCode GotoIR(Operand n); 
InterCode IfIR(Operand x, char *relop, Operand y, Operand z); 
InterCode ReturnIR(Operand x); 
InterCode DecIR(Operand x, int size); 
InterCode ArgIR(Operand x); 
InterCode CallIR(Operand x, char *funcname); 
InterCode ParamIR(Operand x); 
InterCode ReadIR(Operand x); 
InterCode WriteIR(Operand x); 

int getTempnum(); 
int getVarnum();
int getLabelnum();
InterCodes getIRhead();
InterCodes getIRtail();
void printfIR(FILE *tag, InterCode ir); 
void printfallIR(FILE *tag);
#endif
