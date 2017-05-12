#include "IRopt.h"
#include "name.h"

/*
	reduce 
		if x relop y GOTO label1
		GOTO label2
		LABEL label1 :
	to
		if x !relop y GOTO label2
*/
InterCodes ifreduce(InterCodes IRhead) {
	InterCodes ir, ir3, falseir;
	Operand truelabel, falselabel;
	ir = IRhead;
	while(ir) {
		if(ir->code->kind == IF_IR) {
			truelabel = ir->code->op4.z;		
			falseir = ir->next;
			if(falseir == NULL)
				break;
			ir3 = falseir->next;
			if(falseir->code->kind == GOTO_IR)
				falselabel = falseir->code->op1.op1;
			if(ir3 && ir3->code->kind == LABEL_IR 
				&& ir3->code->op1.op1->num_int == truelabel->num_int) {

				ir->code->op4.relop->str = getfalseRelop(ir->code->op4.relop->str);
				ir->code->op4.z = falselabel;
				ir->next = ir3->next;
				if(ir3->next) {
					ir3->next->prev = ir;
				}
				freeIR(falseir);
				freeIR(ir3);
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
/*
	reduce 
		GOTO label1
		LABEL label1 :
*/
InterCodes labelreduce2(InterCodes IRhead) {
	int labelnum = getLabelnum();	
	int i;
	int labeluse[1024] = {0};
	int index = 0;
	int nouse;
	InterCodes ir, irnext, tag;
	Operand label, taglabel;
	ir = IRhead;
	while(ir) {
		irnext = ir->next;
		if(irnext == NULL)
			break;
		label = NULL;
		if(ir->code->kind == GOTO_IR) {
			label = ir->code->op1.op1;		
			if(irnext->code->kind == LABEL_IR) {
				taglabel = irnext->code->op1.op1;	
				if(label->num_int == taglabel->num_int) {
					irnext->prev = ir->prev;
					if(ir->prev != NULL)
						ir->prev->next = irnext;
					freeIR(ir);
					ir = irnext->prev;
				}
			}
		} 
		ir = irnext;
	}
	return IRhead;
}
/*
	reduce 
		t10 := i + j
		t9 := t10
	to
		t9 := i + j
*/
InterCodes assignreduce(InterCodes IRhead) {
	int i;
	int templeft[1024] = {0};
	InterCodes tempIR[1024] = {0};
	int flag;
	InterCodes ir, tag;
	InterCode thiscode, tagcode;
	Operand leftop, right1, right2;
	ir = IRhead;
	while(ir) {
		thiscode = ir->code;
		leftop = NULL;
		if(thiscode->kind == ASSIGN_IR) {
			leftop = thiscode->op2.result;	
		}
		if(thiscode->kind == ADD_IR || thiscode->kind == SUB_IR 
			|| thiscode->kind == MUL_IR || thiscode->kind == DIV_IR) {
			leftop = thiscode->op3.result;
		}
		if(leftop && leftop->kind == TEMP_OP) {
			templeft[leftop->num_int]++;
			tempIR[leftop->num_int] = ir;
		}
		ir = ir->next;
	}
	ir = IRhead;
	while(ir) {
		thiscode = ir->code;
		right1 = right2 = NULL;
		if(thiscode->kind == ASSIGN_IR) {
			right1 = thiscode->op2.right;	
			flag = thiscode->isComputeAddr + thiscode->op2.result->isAddr;
			if((flag == 0 || flag == 2) && templeft[right1->num_int] == 1) {
				templeft[right1->num_int]--;
				tag = tempIR[right1->num_int];	
				tagcode = tag->code;
				thiscode->kind = tagcode->kind;
				switch(tagcode->kind) {
				case ASSIGN_IR:
					thiscode->op2.right = tagcode->op2.right;
					break;
				case ADD_IR:
				case SUB_IR:
				case MUL_IR:
				case DIV_IR:
					thiscode->op3.right1 = tagcode->op3.right1;
					thiscode->op3.right2 = tagcode->op3.right2;
					break;
				}
			}
		}
		if(thiscode->kind == ADD_IR || thiscode->kind == SUB_IR 
			|| thiscode->kind == MUL_IR || thiscode->kind == DIV_IR) {
			right1 = thiscode->op3.right1;
			right2 = thiscode->op3.right2;
		}
		ir = ir->next;
	}
	ir = IRhead;
	while(ir) {
		thiscode = ir->code;
		leftop = NULL;
		if(thiscode->kind == ASSIGN_IR) {
			leftop = thiscode->op2.result;	
		}
		if(thiscode->kind == ADD_IR || thiscode->kind == SUB_IR 
			|| thiscode->kind == MUL_IR || thiscode->kind == DIV_IR) {
			leftop = thiscode->op3.result;
		}
		if(leftop && leftop->kind == TEMP_OP) {
			if(templeft[leftop->num_int] == 0) {
				tag = ir;
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
