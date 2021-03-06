#include "IRopt.h"
#include "name.h"
#include "IR.h"

#define LABEL_NUM 1024
int labelcount[LABEL_NUM] = {0};
InterCodes updatelabelcount(InterCodes IRhead) {
	int labelnum = getLabelnum();	
	int i;
	InterCodes ir;
	Operand label;
	if(labelnum >= LABEL_NUM) {
		fprintf(stderr, "too many labels\n");
		return IRhead;
	}
	for(i = 0; i < LABEL_NUM; i++) {
		labelcount[i] = 0;
	}
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
			labelcount[label->num_int]++;
		}
		ir = ir->next;
	}
	return IRhead;
}
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
	IRhead = updatelabelcount(IRhead);
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
				&& ir3->code->op1.op1->num_int == truelabel->num_int
				&& labelcount[truelabel->num_int] == 1) {

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
	InterCodes ir, tag;
	Operand label;
	IRhead = updatelabelcount(IRhead);
	ir = IRhead;
	while(ir) {
		if(ir->code->kind == LABEL_IR) {
			tag = ir;
			label = ir->code->op1.op1;
			if(labelcount[label->num_int] == 0) {
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
				if(label->num_int == taglabel->num_int && labelcount[label->num_int] == 1) {
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

/*
	reduce 
		t1 = #4 + #3
	to 
		t1 = #7
*/
InterCodes constantreduce(InterCodes IRhead) {
	InterCodes ir;
	Operand right1, right2, right;
	ir = IRhead;
	while(ir) {
		right1 = right2 = NULL;
		if(ir->code->kind == ADD_IR || ir->code->kind == SUB_IR 
			|| ir->code->kind == MUL_IR || ir->code->kind == DIV_IR) {
			right1 = ir->code->op3.right1;
			right2 = ir->code->op3.right2;
		}
		if(right1 && right2 && right1->kind == CONSTANT_OP && right2->kind == CONSTANT_OP) {
			right = newOperand(CONSTANT_OP);
			right->type = right1->type;
			right->isAddr = 0;
//			right->isArray = 0;
			switch(ir->code->kind) {
				case ADD_IR:
					if(right->type == Int)
						right->num_int = right1->num_int + right2->num_int;
					else
						right->num_float = right1->num_float + right2->num_float;
					break;
				case SUB_IR:
					if(right->type == Int)
						right->num_int = right1->num_int - right2->num_int;
					else
						right->num_float = right1->num_float - right2->num_float;
					break;
				case MUL_IR:
					if(right->type == Int)
						right->num_int = right1->num_int * right2->num_int;
					else
						right->num_float = right1->num_float * right2->num_float;
					break;
				case DIV_IR:
					if(right->type == Int)
						right->num_int = right1->num_int / right2->num_int;
					else
						right->num_float = right1->num_float / right2->num_float;
					break;
			}
			ir->code->op2.result = ir->code->op3.result;
			ir->code->op2.right = right;
			ir->code->kind = ASSIGN_IR;
		}
		ir = ir->next;
	}
	return IRhead;
}
InterCodes initbasicblock(InterCodes IRhead) {
	int funcnum = 0, basicblock = 0;
	InterCodes ir;
	ir = IRhead;
	while(ir) {
		if(ir->code->kind == FUNCTION_IR) {
			funcnum++;
			basicblock++;
			ir->isblockhead = 1;
		}
		ir->funcnum = funcnum;
		if(ir->code->kind == LABEL_IR) {
			ir->isblockhead = 1;
		}
		if(ir->code->kind == IF_IR || ir->code->kind == GOTO_IR || ir->code->kind == RETURN_IR) {
			if(ir->next) {
				ir->next->isblockhead = 1;
			}
		}
		ir = ir->next;
	}
	if(debug3) {
		ir = IRhead;
		while(ir) {
			if(ir->isblockhead == 1) 
				fprintf(stdout, "---------------------\n");
			printfIR(stdout, ir->code);
			fprintf(stdout, "\n");
			ir = ir->next;
		}
	}
	return IRhead;
}
InterCodes blocktempopt(InterCodes blockhead) {
	int i;
	int templeft[1024] = {0};
	int tempright[1024] = {0};
	InterCodes tempIR[1024] = {0};
	int flag;
	InterCodes ir, tag;
	InterCode thiscode, tagcode;
	Operand leftop, right1, right2;
	ir = blockhead;
	while(ir && ir->isblockhead == 0) {
		thiscode = ir->code;
		leftop = NULL;
		if(thiscode->kind == ASSIGN_IR) {
			leftop = thiscode->op2.result;	
			right1 = thiscode->op2.right;
		}
		if(thiscode->kind == ADD_IR || thiscode->kind == SUB_IR 
			|| thiscode->kind == MUL_IR || thiscode->kind == DIV_IR) {
			leftop = thiscode->op3.result;
			right1 = thiscode->op3.right1;
			right2 = thiscode->op3.right2;
		}
		if(thiscode->kind == IF_IR) {
			right1 = thiscode->op4.x;
			right2 = thiscode->op4.y;
		}
		if(leftop && leftop->kind == TEMP_OP) {
			templeft[leftop->num_int]++;
			tempIR[leftop->num_int] = ir;
		}
		ir = ir->next;
	}
	return blockhead;

}
InterCodes blockinteropt(InterCodes IRhead) {
	InterCodes ir;
	ir = IRhead;
	while(ir) {
		if(ir->isblockhead == 1) {
			blocktempopt(ir);
		}
		ir = ir->next;
	}
	return IRhead;
}
