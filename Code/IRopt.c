#include "IRopt.h"

InterCodes ifreduce(InterCodes IRhead) {
	int labelnum = getLabelnum();	
	int i;
	int labeluse[1024] = {0};
	int index = 0;
	int nouse;
	InterCodes ir, tag, trueir, falseir;
	Operand truelabel, falselabel;
	ir = IRhead;
	while(ir) {
		if(ir->code->kind == IF_IR) {
			trueir = ir;
			truelabel = trueir->code->op4.z;		
			falseir = ir->next;
			if(falseir->code->kind == LABEL_IR)
				falselabel = falseir->code->op1.op1;
			if(falseir->next && falseir->next->code->kind == LABEL_IR 
				&& falseir->next->code->op1.op1->num_int == truelabel->num_int) {
				//TODO: add code to reduce if
			}
		} 
		ir = ir->next;
	}
	return IRhead;
}
InterCodes labelreduce(InterCodes IRhead) {
	int labelnum = getLabelnum();	
	int i;
	int labeluse[1024] = {0};
	int index = 0;
	int nouse;
	InterCodes ir, tag;
	Operand label;
	ir = IRhead;
	while(ir) {
		label = NULL;
		if(ir->code->kind == GOTO_IR) {
			label = ir->code->op1.op1;		
		} 
		if(ir->code->kind == IF_IR) {
			label = ir->code->op4.z;		
		} 
		if(label) {
			labeluse[index] = label->num_int;
			index++;
		}
		ir = ir->next;
	}
	ir = IRhead;
	while(ir) {
		if(ir->code->kind == LABEL_IR) {
			tag = ir;
			label = ir->code->op1.op1;
			nouse = 1;
			for(i = 0; i < index; i++) {
				if(label->num_int == labeluse[i]) {
					nouse = 0;	
					break;
				}
			}
			if(nouse == 1) {
				ir = ir->prev;
				if(tag->prev != NULL)
					tag->prev->next = tag->next;
				if(tag->next != NULL)
					tag->next->prev = tag->prev;
				freeIR(tag);
			}
		} 
		ir = ir->next;
	}
	return IRhead;
}
