#include "IR.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int tempnum = 0;
int varnum = 0;
int labelnum = 0;
InterCodes IRhead = NULL;
InterCodes IRtail = NULL;
// add read write
void IR_init() {
	Type tmp, arg;
	Symbol write, read;
	tmp = newType();
	tmp->kind = BASIC;
	tmp->basic = INT;
	read = newFunc("read", tmp, NULL, NULL);
	read->func->isDefine = 1;
	insert(read);

	tmp = newType();
	tmp->kind = BASIC;
	tmp->basic = INT;
	arg = newType();
	arg->kind = BASIC;
	arg->basic = INT;
	write = newFunc("write", tmp, newFieldList(NULL, arg, NULL, NULL),NULL);
	write->func->isDefine = 1;
	insert(write);
}

IRinfo newIRinfo() {
	IRinfo result = NULL;
	result = (IRinfo) malloc(sizeof(struct IRinfo_));
	if(result == NULL) {
		fprintf(stderr, "can't malloc\n");
		return NULL;
	}
	result->kind = 0;
	result->addr = NULL;
	result->op = NULL;
	result->next = NULL;
	result->truelist = NULL;
	result->falselist = NULL;
	result->nextlist = NULL;
	return result;
}
Operand newOperand(int kind) {
	Operand result = NULL;
	result = (Operand) malloc(sizeof(struct Operand_));
	if(result == NULL){
		fprintf(stderr, "can't malloc\n");
		return NULL;
	}
	result->kind = kind;
	return result;
}
Operand newTemp() {
	Operand result = NULL;
	result = newOperand(TEMP_OP);
	tempnum++;
	result->type = Int;
	result->num_int = tempnum;
	return result;
}
Operand newLabel() {
	Operand result = NULL;
	result = newOperand(LABEL_OP);
	labelnum++;
	result->type = Int;
	result->num_int = labelnum;
	return result;
}
Operandlist Mstack = NULL;
void Mpush(Operand M) {
	Operandlist new = NULL;
	if(Mstack == NULL)
		Mstack = Opmakelist(M);
	else {
		new = Opmakelist(M);
		new->next = Mstack;
		Mstack = new;
	}
}
Operand Mpop() {
	Operand result;
	if(Mstack == NULL)
		return NULL;
	else {
		result = Mstack->op;
		Mstack = Mstack->next;
		return result;
	}	
}
struct Nlist_ {
	Operandlist oplist;
	struct Nlist_ *next;
};
struct Nlist_ *Nstack = NULL;
void Npush(Operandlist N) {
	struct Nlist_ *new = NULL;
	new = (struct Nlist_*)malloc(sizeof(struct Nlist_));
	new->oplist = N;
	new->next = NULL;
	if(Nstack == NULL) {
		Nstack = new;
	}else {
		new->next = Nstack;
		Nstack = new;
	}
}
Operandlist Npop() {
	Operandlist result;
	if(Nstack == NULL)
		return NULL;
	else {
		result = Nstack->oplist;
		Nstack = Nstack->next;
		return result;
	}	
}
Operandlist Opmakelist(Operand op) {
	Operandlist result = NULL;
	result = (Operandlist) malloc(sizeof(struct Operandlist_));
	if(result == NULL) {
		fprintf(stderr, "can't malloc\n");
		return NULL;
	}
	result->op = op;
	result->next = NULL;
	return result;
}
Operandlist Opmerge(Operandlist oplist1, Operandlist oplist2) {
	Operandlist tmp = oplist1;
	if(tmp == NULL)
		return oplist2;
	while(tmp->next)
		tmp = tmp->next;
	tmp->next = oplist2;
	return oplist1;
}
void Opbackpatch(Operandlist oplist, Operand label) {
	if(label == NULL)
		return;
	while(oplist) {
		oplist->op->num_int = label->num_int;
		oplist = oplist->next;
	}
}
void showOplist(Operandlist oplist) {
	while(oplist) {
		fprintf(stdout, "%s\n", Optostring(oplist->op));
		oplist = oplist->next;
	}
}
char *Opvaluetostring(Operand op) {
	static char buf[20];
	char *result = NULL;
	switch(op->type) {
	case String:
		sprintf(buf, "%s", op->str);
		break;
	case Int:
		sprintf(buf, "%d", op->num_int);
		break;
	case Float:
		sprintf(buf, "%f", op->num_float);
		break;
	}
	result = (char*)strdup(buf);
	return result;
}
char *Optostring(Operand op) {
	static char buf[20];
	char *result = NULL;
	switch(op->kind) {
	case TEMP_OP:
		sprintf(buf, "t%s", Opvaluetostring(op));
		break;
	case VARIABLE_OP:
		sprintf(buf, "%s", Opvaluetostring(op));
		break;
	case CONSTANT_OP:
		sprintf(buf, "#%s", Opvaluetostring(op));
		break;
	case FUNC_OP:
		sprintf(buf, "%s", Opvaluetostring(op));
		break;
	case ADDRESS_OP:
		sprintf(buf, "&%s", Opvaluetostring(op));
		break;
	case VALUEINADDR_OP:
		sprintf(buf, "*%s", Opvaluetostring(op));
		break;
	case TEMP_ADDR_OP:
		sprintf(buf, "&t%s", Opvaluetostring(op));
		break;
	case TEMP_VALUE_OP:
		sprintf(buf, "*t%s", Opvaluetostring(op));
		break;
	case RELOP_OP:
		sprintf(buf, "%s", Opvaluetostring(op));
		break;
	case LABEL_OP:
		sprintf(buf, "label%s", Opvaluetostring(op));
		break;
	case SIZE_OP:
		sprintf(buf, "%s", Opvaluetostring(op));
		break;
	}
	result = (char*)strdup(buf);
	return result;
}


InterCode newInterCode() {
	InterCode result = NULL;
	result = (InterCode) malloc(sizeof(struct InterCode_));
	if(result == NULL) {
		fprintf(stderr, "can't malloc\n");
		return NULL;
	}
	return result;
}
void addIR(InterCode ir) {
	InterCodes new = NULL;
	new = (InterCodes) malloc(sizeof(struct InterCodes_));
	if(new == NULL) {
		fprintf(stderr, "can't malloc\n");
		return;
	}
	new->code = ir;
	new->prev = NULL;
	new->next = NULL;
	if(IRhead == NULL) {
		IRhead = new;
		IRtail = new;
		return;
	}else {
		IRtail->next = new;
		new->prev = IRtail;
		IRtail = new;
		return;
	}
}
InterCode LabelIR(int n) {
	InterCode result = NULL;
	Operand op = NULL;
	result = newInterCode();
	op = newOperand(LABEL_OP);
	op->type = Int;
	op->num_int = n;
	result->kind = LABEL_IR;
	result->op1.op1 = op;
	addIR(result);
	return result;
}
InterCode FunctionIR(char *funcname) {
	InterCode result = NULL;
	Operand op = NULL;
	result = newInterCode();
	op = newOperand(FUNC_OP);
	op->type = String;
	op->str = (char*)strdup(funcname);
	result->kind = FUNCTION_IR;
	result->op1.op1 = op;
	addIR(result);
	return result;
}
InterCode Assign2IR(Operand x, Operand y) {
	InterCode result = NULL;
	result = newInterCode();
	result->kind = ASSIGN_IR;
	result->op2.result = x;
	result->op2.right = y;
	addIR(result);
	return result;
}
InterCode Assign3IR(Operand x, Operand y, int kind, Operand z) {
	InterCode result = NULL;
	result = newInterCode();
	result->kind = kind;
	result->op3.result = x;
	result->op3.right1 = y;
	result->op3.right2 = z;
	addIR(result);
	return result;
}
InterCode GotoIR(Operand n) {
	InterCode result = NULL;
	result = newInterCode();
	result->kind = GOTO_IR;
	result->op1.op1 = n;
	addIR(result);
	return result;
}
InterCode IfIR(Operand x, char *relop, Operand y, Operand z) {
	InterCode result = NULL;
	Operand op = NULL;
	op = newOperand(RELOP_OP);
	op->type = String; 
	op->str = (char*)strdup(relop);
	result = newInterCode();
	result->kind = IF_IR;
	result->op4.x = x;
	result->op4.relop = op;
	result->op4.y = y;
	result->op4.z = z;
	addIR(result);
	return result;
}
InterCode ReturnIR(Operand x) {
	InterCode result = NULL;
	result = newInterCode();
	result->kind = RETURN_IR;
	result->op1.op1 = x;
	addIR(result);
	return result;
}
InterCode DecIR(Operand x, int size) {
	InterCode result = NULL;
	Operand op = NULL;
	op = newOperand(SIZE_OP);
	op->type = Int;
	op->num_int = size;
	result = newInterCode();
	result->kind = DEC_IR;
	result->op2.result = x;
	result->op2.right = op;
	addIR(result);
	return result;
}
InterCode ArgIR(Operand x) {
	InterCode result = NULL;
	result = newInterCode();
	result->kind = ARG_IR;
	result->op1.op1 = x;
	addIR(result);
	return result;
}
InterCode CallIR(Operand x, char *funcname) {
	InterCode result = NULL;
	Operand op = NULL;
	op = newOperand(FUNC_OP);
	op->type = String;
	op->str = (char *)strdup(funcname);
	result = newInterCode();
	result->kind = CALL_IR;
	result->op2.result = x;
	result->op2.right = op;
	addIR(result);
	return result;
}
InterCode ParamIR(Operand x) {
	InterCode result = NULL;
	result = newInterCode();
	result->kind = PARAM_IR;
	result->op1.op1 = x;
	addIR(result);
	return result;
}
InterCode ReadIR(Operand x) {
	InterCode result = NULL;
	result = newInterCode();
	result->kind = READ_IR;
	result->op1.op1 = x;
	addIR(result);
	return result;
}
InterCode WriteIR(Operand x) {
	InterCode result = NULL;
	result = newInterCode();
	result->kind = WRITE_IR;
	result->op1.op1 = x;
	addIR(result);
	return result;
}

void printfIR(FILE *tag, InterCode ir) {
	switch(ir->kind) {
	case LABEL_IR:
		fprintf(tag, "LABEL %s :", Optostring(ir->op1.op1));	
		break;
	case FUNCTION_IR:
		fprintf(tag, "FUNCTION %s :", Optostring(ir->op1.op1));	
		break;
	case ASSIGN_IR:
		fprintf(tag, "%s := %s", Optostring(ir->op2.result), Optostring(ir->op2.right));	
		break;
	case ADD_IR:
		fprintf(tag, "%s := %s + %s", 
			Optostring(ir->op3.result), Optostring(ir->op3.right1), Optostring(ir->op3.right2));	
		break;
	case SUB_IR:
		fprintf(tag, "%s := %s - %s", 
			Optostring(ir->op3.result), Optostring(ir->op3.right1), Optostring(ir->op3.right2));	
		break;
	case MUL_IR:
		fprintf(tag, "%s := %s * %s", 
			Optostring(ir->op3.result), Optostring(ir->op3.right1), Optostring(ir->op3.right2));	
		break;
	case DIV_IR:
		fprintf(tag, "%s := %s / %s", 
			Optostring(ir->op3.result), Optostring(ir->op3.right1), Optostring(ir->op3.right2));	
		break;
	case GOTO_IR:
		fprintf(tag, "GOTO %s", Optostring(ir->op1.op1));	
		break;
	case IF_IR:
		fprintf(tag, "IF %s %s %s GOTO %s", 
			Optostring(ir->op4.x), Optostring(ir->op4.relop), Optostring(ir->op4.y), Optostring(ir->op4.z));	
		break;
	case RETURN_IR:
		fprintf(tag, "RETURN %s", Optostring(ir->op1.op1));	
		break;
	case DEC_IR:
		fprintf(tag, "DEC %s %s", Optostring(ir->op2.result), Optostring(ir->op2.right));	
		break;
	case ARG_IR:
		if(ir->op1.op1->isArray == 1)
			fprintf(tag, "ARG &%s", Optostring(ir->op1.op1));	
		else
			fprintf(tag, "ARG %s", Optostring(ir->op1.op1));	
		break;
	case CALL_IR:
		fprintf(tag, "%s := CALL %s", Optostring(ir->op2.result), Optostring(ir->op2.right));	
		break;
	case PARAM_IR:
		fprintf(tag, "PARAM %s", Optostring(ir->op1.op1));	
		break;
	case READ_IR:
		fprintf(tag, "READ %s", Optostring(ir->op1.op1));	
		break;
	case WRITE_IR:
		fprintf(tag, "WRITE %s", Optostring(ir->op1.op1));	
		break;
	}	
}
void printfallIR(FILE *tag) {
	InterCodes temp = IRhead;
	while(temp) {
		printfIR(tag, temp->code);
		fprintf(tag, "\n");
		temp = temp->next;
	}
}
/*
void test3() {
	Operand x,y,z;
	InterCode temp;
	FILE *tag = stderr;
	temp = LabelIR(1);
	printfIR(tag, temp);
	fprintf(tag, "\n");
	temp = FunctionIR("main");
	printfIR(tag, temp);
	fprintf(tag, "\n");
	x = newOperand(VARIABLE_OP);
	x->type = String;
	x->str = "var";
	y = newTemp();
	z = newTemp();
	temp = Assign2IR(x, y);
	printfIR(tag, temp);
	fprintf(tag, "\n");
	temp = Assign3IR(x, y, ADD_IR, z);
	printfIR(tag, temp);
	fprintf(tag, "\n");
	temp = GotoIR(1);
	printfIR(tag, temp);
	fprintf(tag, "\n");
	temp = IfIR(x, ">", y, 2);
	printfIR(tag, temp);
	fprintf(tag, "\n");
	temp = ReturnIR(x);
	printfIR(tag, temp);
	fprintf(tag, "\n");
	temp = DecIR(x, 4);
	printfIR(tag, temp);
	fprintf(tag, "\n");
	temp = ArgIR(x);
	printfIR(tag, temp);
	fprintf(tag, "\n");
	temp = CallIR(x, "func");
	printfIR(tag, temp);
	fprintf(tag, "\n");
	temp = ParamIR(x);
	printfIR(tag, temp);
	fprintf(tag, "\n");
	temp = ReadIR(x);
	printfIR(tag, temp);
	fprintf(tag, "\n");
	temp = WriteIR(x);
	printfIR(tag, temp);
	fprintf(tag, "\n");
	fprintf(tag, "---------------all--------------\n");
	printfallIR(tag);
}
*/
