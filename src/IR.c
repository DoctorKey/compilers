#include "IR.h"
#include "name.h"

int tempnum = 0;
int varnum = 0;
int labelnum = 0;
InterCodes IRhead = NULL;
InterCodes IRtail = NULL;
int getTempnum() {
	return tempnum;
}
int getVarnum() {
	return varnum;
}
int getLabelnum() {
	return labelnum;
}
InterCodes getIRhead() {
	return IRhead;
}
InterCodes getIRtail() {
	return IRtail;
}
// add read write
void IR_init() {
	Type returntype, arg;
	Symbol write, read;
	returntype = newType();
	returntype->kind = BASIC;
	returntype->basic = INT;
	read = newFunc("read", returntype, NULL, NULL);
	read->func->isDefine = 1;
	insert(read);

	returntype = newType();
	returntype->kind = BASIC;
	returntype->basic = INT;
	arg = newType();
	arg->kind = BASIC;
	arg->basic = INT;
	write = newFunc("write", returntype, newFieldList(NULL, arg, NULL, NULL),NULL);
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
	result->isAddr = 0;
	result->varnum = -1;
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
Operandlist getFall() {
	Operand op;
	Operandlist result;
	op = newOperand(LABEL_OP);
	op->type = Int;
	op->num_int = -2;
	result = Opmakelist(op);
	return result;
}
int isFall(Operandlist fall) {
	if(fall == NULL)
		return 0;
	if(fall->op->kind == LABEL_OP) {
		if(fall->op->num_int == -2)
			return 1;
	}
	else
		return 0;
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
char *OptoAddrstring(Operand op) {
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
InterCodes newInterCodes(InterCode ir) {
	InterCodes new = NULL;
	new = (InterCodes) malloc(sizeof(struct InterCodes_));
	if(new == NULL) {
		fprintf(stderr, "can't malloc\n");
		return NULL;
	}
	new->code = ir;
	new->prev = NULL;
	new->next = NULL;
	return new;
}
// new function in lab4
InterCodes addInterCode(InterCodes head, InterCode ir) {
	InterCodes temp = head;
	InterCodes new = newInterCodes(ir);
	if(head == NULL)
		return new;
	else {
		new->next = head;
		head->prev = new;
		return new;
	}
}
void freeIRs(InterCodes irlist) {
	if(irlist == NULL)
		return;
	if(irlist->next)
		freeIRs(irlist->next);
	free(irlist);
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
void freeIR(InterCodes ir) {
	if(ir == NULL)
		return;
	free(ir);
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
	result->isComputeAddr = 0;
	result->op3.result = x;
	result->op3.right1 = y;
	result->op3.right2 = z;
	addIR(result);
	return result;
}
InterCode Assign3AddrIR(Operand x, Operand y, int kind, Operand z) {
	InterCode result = NULL;
	result = newInterCode();
	result->kind = kind;
	result->isComputeAddr = 1;
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
	result->isComputeAddr = 0;
	addIR(result);
	return result;
}
InterCode ReturnIR(Operand x) {
	InterCode result = NULL;
	result = newInterCode();
	result->kind = RETURN_IR;
	result->op1.op1 = x;
	result->isComputeAddr = 0;
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

char *formatStr(InterCode ir, Operand op) {
	char *result;
	static char buffer[64];
	if(ir->isComputeAddr == 0) {
		if(op->isAddr == 0) {
			sprintf(buffer, "%s", Optostring(op));	
		}else {
			sprintf(buffer, "*%s", Optostring(op));	
		}
	}else if(ir->isComputeAddr == 1) {
		if(op->isAddr == 0) {
			sprintf(buffer, "&%s", Optostring(op));	
		}else {
			sprintf(buffer, "%s", Optostring(op));	
		}
	}
	result = strdup(buffer);
	return result;
}
void printfExpIR(FILE *tag, InterCode ir, int type) {
	char comtype;
	switch(type) {
	case ADD_IR: comtype = '+'; break;
	case SUB_IR: comtype = '-'; break;
	case MUL_IR: comtype = '*'; break;
	case DIV_IR: comtype = '/'; break;
	}
	fprintf(tag, "%s := %s %c ", formatStr(ir, ir->op3.result), formatStr(ir, ir->op3.right1), comtype);
	if(ir->op3.right2->isAddr == 0) {
		fprintf(tag, "%s", Optostring(ir->op3.right2));
	}else {
		fprintf(tag, "*%s", Optostring(ir->op3.right2));
	}	
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
		fprintf(tag, "%s := %s", formatStr(ir, ir->op2.result), formatStr(ir, ir->op2.right));
		break;
	case ADD_IR:
		printfExpIR(tag, ir, ADD_IR); 
		break;
	case SUB_IR:
		printfExpIR(tag, ir, SUB_IR); 
		break;
	case MUL_IR:
		printfExpIR(tag, ir, MUL_IR); 
		break;
	case DIV_IR:
		printfExpIR(tag, ir, DIV_IR); 
		break;
	case GOTO_IR:
		fprintf(tag, "GOTO %s", Optostring(ir->op1.op1));	
		break;
	case IF_IR:
		fprintf(tag, "IF %s %s %s GOTO %s", 
			formatStr(ir, ir->op4.x), Optostring(ir->op4.relop), formatStr(ir, ir->op4.y), Optostring(ir->op4.z));
		break;
	case RETURN_IR:
		fprintf(tag, "RETURN %s", formatStr(ir, ir->op1.op1));	
		break;
	case DEC_IR:
		fprintf(tag, "DEC %s %s", Optostring(ir->op2.result), Optostring(ir->op2.right));	
		break;
	case ARG_IR:
		fprintf(tag, "ARG %s", formatStr(ir, ir->op1.op1));	
		break;
	case CALL_IR:
		fprintf(tag, "%s := CALL %s", formatStr(ir, ir->op2.result), Optostring(ir->op2.right));	
		break;
	case PARAM_IR:
		fprintf(tag, "PARAM %s", Optostring(ir->op1.op1));	
		break;
	case READ_IR:
		fprintf(tag, "READ %s", Optostring(ir->op1.op1));	
		break;
	case WRITE_IR:
		if(ir->op1.op1->isAddr == 1)
			fprintf(tag, "WRITE *%s", Optostring(ir->op1.op1));	
		else
			fprintf(tag, "WRITE %s", Optostring(ir->op1.op1));	
		break;
	}	
}
void printfallIR(FILE *tag) {
	InterCodes temp = IRhead;
	while(temp) {
		if(temp->isblockhead == 1) 
			fprintf(tag, "--------------------------\n");
		printfIR(tag, temp->code);
		fprintf(tag, "\n");
		temp = temp->next;
	}
}
