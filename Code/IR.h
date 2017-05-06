#ifndef __IR_H_
#define __IR_H_

#include "symtable.h"

typedef enum {TEMP_OP, VARIABLE_OP, CONSTANT_OP, ADDRESS_OP, FUNC_OP, VALUEINADDR_OP, RELOP_OP, LABEL_OP, SIZE_OP} OP_KIND;
typedef struct Operand_* Operand;
typedef struct IRinfo_* IRinfo;
struct IRinfo_ {
	OP_KIND kind;
	char * addr;
	Operand op;
	IRinfo next;
};
IRinfo newIRinfo(); 

struct Operand_ {
	OP_KIND kind;
	enum {String, Int, Float} type;
	union {
		int num_int;
		float num_float;
		char *str;
	};
};
typedef enum { LABEL_IR, FUNCTION_IR, 
		ASSIGN_IR, ADD_IR, SUB_IR, MUL_IR, DIV_IR, 
		GOTO_IR, IF_IR, RETURN_IR, DEC_IR, 
		ARG_IR, CALL_IR, PARAM_IR, READ_IR, WRITE_IR } InterCodeKind;
typedef struct InterCode_* InterCode;
struct InterCode_ {
	InterCodeKind kind;
	int relop;
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
	InterCodes prev, next;
};

void IR_init(); 
Operand newOperand(int kind); 
Operand newTemp(); 
char *Optostring(Operand op); 
InterCode newInterCode(); 
void addIR(InterCode ir); 

InterCode LabelIR(int n); 
InterCode FunctionIR(char *funcname); 
InterCode Assign2IR(Operand x, Operand y); 
InterCode Assign3IR(Operand x, Operand y, int kind, Operand z); 
InterCode GotoIR(int n); 
InterCode IfIR(Operand x, char *relop, Operand y, int z); 
InterCode ReturnIR(Operand x); 
InterCode DecIR(Operand x, int size); 
InterCode ArgIR(Operand x); 
InterCode CallIR(Operand x, char *funcname); 
InterCode ParamIR(Operand x); 
InterCode ReadIR(Operand x); 
InterCode WriteIR(Operand x); 

void printfallIR(FILE *tag);
void test3(); 
#endif
