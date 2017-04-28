#ifndef __IR_H_
#define __IR_H_

typedef struct Operand_* Operand;
struct Operand_ {
	enum { VARIABLE, CONSTANT, ADDRESS } kind;
	union {
		int var_no;
		int value;
	};
};

struct InterCode {
	enum { ASSIGN, ADD, SUB, MUL } kind;
	union {
		struct {
			Operand left, right;
		}assign;
		struct {
			Operand result, op1, op2;
		}binop;
	};
};

struct InterCodes {
	InterCode code;
	struct InterCodes *prev, *next;
}

#endif
