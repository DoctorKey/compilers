#include "translateIR.h"
#include "main.h"
#include "symtable.h"
#include "syntax.tab.h"
#include "semantichelp.h"
#include "name.h"
#include "error.h"
#include "IRopt.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int pairscount = 0;
int beginbool = 0;
int noif = 0;
/*--------------------------------------------------------------------------

	High-level Definitions

--------------------------------------------------------------------------
*/
void ProgramTrans(TreeNode parent, int num) {
	InterCodes IRhead = getIRhead();
	labelreduce(IRhead);
	assignreduce(IRhead);
}
void ExtDefListTrans(TreeNode parent, int num) {
}
void ExtDefTrans(TreeNode parent, int num) {
}
void ExtDecListTrans(TreeNode parent, int num) {
}
/*
	Specifiers
*/
void SpecifierTrans(TreeNode parent, int num) {
}
void StructSpecifierTrans(TreeNode parent, int num) {
}
void OptTagTrans(TreeNode parent, int num) {
}
void TagTrans(TreeNode parent, int num) {
}

/*
	Declarators
*/
void VarDecTrans(TreeNode parent, int num) {
}
void FunDecTrans(TreeNode parent, int num) {
	FieldList fieldlist = NULL;
	char *funcname = NULL;
	InterCode funcIR = NULL, paramIR = NULL;
	Operand paramop = NULL;

	// maybe NULL
	funcname = parent->nodevalue.str;
	// maybe NULL, NULL is no param
	fieldlist = parent->fieldList;

	funcIR = FunctionIR(funcname);
	while(fieldlist) {
		// TODO: decide kind by type 
		paramop = newOperand(VARIABLE_OP);
		if(fieldlist->type->kind == ARRAY) 
			paramop->isAddr = 1;
		else	
			paramop->isAddr = 0;
		paramop->type = String;
		paramop->str = strdup(fieldlist->name);
		paramIR = ParamIR(paramop);
		fieldlist = fieldlist->tail;	
	}
}
void VarListTrans(TreeNode parent, int num) {
}
void ParamDecTrans(TreeNode parent, int num) {
}

/*
	Statements
*/
void CompStTrans(TreeNode parent, int num) {
	TreeNode stmtlist;
	stmtlist = parent->children[2];
	parent->irinfo = newIRinfo();
	parent->irinfo->nextlist = stmtlist->irinfo->nextlist;
}
void StmtListTrans(TreeNode parent, int num) {
	TreeNode stmt, stmtlist;
	Operand Mop;
	stmt = parent->children[0];
	stmtlist = parent->children[1];
	parent->irinfo = newIRinfo();
	Mop = Mpop();
	Opbackpatch(stmt->irinfo->nextlist, Mop);
	if(stmtlist == NULL)
		parent->irinfo->nextlist = NULL;
	else
		parent->irinfo->nextlist = stmtlist->irinfo->nextlist;
}
void StmtTrans(TreeNode parent, int num) {
	TreeNode exp = NULL;
	TreeNode stmtType = NULL;
	TreeNode stmt = NULL;
	TreeNode stmt2 = NULL;
	Operand truelabelop = NULL, falselabelop = NULL;
	Operand labelop, op_tmp;
	Operand M1op, M2op;
	Operandlist oplist, Noplist;
	InterCode result;
	parent->irinfo = newIRinfo();
	stmtType = parent->children[0];
	exp = parent->children[2];
	stmt = parent->children[4];
	stmt2 = parent->children[6];
	//Exp SEMI
	if (stmtType->nodetype == Exp) {
		parent->irinfo->nextlist = NULL;
	}
	//CompSt
	if (stmtType->nodetype == CompSt) {
		parent->irinfo->nextlist = stmtType->irinfo->nextlist;
	}
	//RETURN Exp SEMI
	if (stmtType->nodetype == RETURN) {
		exp = parent->children[1];
		result = ReturnIR(exp->irinfo->op);
		parent->irinfo->nextlist = NULL;
	}
	// if; while
	if (stmtType->nodetype == IF ) {
		// if lp exp rp stmt 
		if(num == 5) {
			M1op = Mpop();
			Opbackpatch(exp->irinfo->truelist, M1op);
			parent->irinfo->nextlist = Opmerge(exp->irinfo->falselist,stmt->irinfo->nextlist); 
		}else if(num == 7) {
			M2op = Mpop();
			M1op = Mpop();
			Opbackpatch(exp->irinfo->truelist, M1op);
			Opbackpatch(exp->irinfo->falselist, M2op);
			Noplist = Npop();
			oplist = Opmerge(stmt->irinfo->nextlist, Noplist);
			parent->irinfo->nextlist = Opmerge(oplist, stmt2->irinfo->nextlist);
		}
	}
	if (stmtType->nodetype == WHILE) {
		M2op = Mpop();
		M1op = Mpop();
		Opbackpatch(stmt->irinfo->nextlist, M1op);
		Opbackpatch(exp->irinfo->truelist, M2op);
		parent->irinfo->nextlist = exp->irinfo->falselist;
		result = GotoIR(M1op);
	}
}
void MTrans(TreeNode parent, int num) {
	parent->irinfo = newIRinfo();
	parent->irinfo->op = newLabel();
	LabelIR(parent->irinfo->op->num_int);
	Mpush(parent->irinfo->op);
}
void NTrans(TreeNode parent, int num) {
	parent->irinfo = newIRinfo();
	parent->irinfo->op = newOperand(LABEL_OP);
	parent->irinfo->op->type = Int;
	parent->irinfo->op->num_int = -1;
	parent->irinfo->nextlist = Opmakelist(parent->irinfo->op);
	Npush(parent->irinfo->nextlist);
	GotoIR(parent->irinfo->op);
}
/*
	Local Definitions
*/
void DefListTrans(TreeNode parent, int num) {
}
void DefTrans(TreeNode parent, int num) {
}
void DecListTrans(TreeNode parent, int num) {
}
void DecTrans(TreeNode parent, int num) {
	TreeNode exp = NULL;
	Operand op_tmp, op_size;
	InterCode result;

	// VarDec
	op_tmp = newOperand(VARIABLE_OP);
	op_tmp->type = String;
	op_tmp->str = (char*)strdup(parent->nodevalue.str);
	if(parent->type->kind == ARRAY) {
		op_size = newOperand(SIZE_OP);
		op_size->type = Int;
		op_size->num_int = parent->type->array.upbound * parent->type->array.size;
		result = DecIR(op_tmp, op_size->num_int);
	}
	//can't init array and struct
	// VarDec ASSIGNOP Exp
	if (num == 3) {
		exp = parent->children[2];
		result = Assign2IR(op_tmp, exp->irinfo->op);
	}
}

/*
	Expressions
*/
void genArgsIR(IRinfo irinfo) {
	if(irinfo == NULL) 
		return;
	genArgsIR(irinfo->next);
	ArgIR(irinfo->op);
}
TreeNode thelastexp = NULL;
int expcount = 0;
// update parent->type, parent->errorInfo, parent->nodevalue.str
void ExpTrans(TreeNode parent, int num) {
	TreeNode childleft, childright, childmid;
	Symbol symNode = NULL;
	Operand op_tmp, op_tmp2;
	Operand trueop, falseop, Mop;
	InterCode result;	
	parent->irinfo = newIRinfo();
	childleft = parent->children[0];
	// ID INT FLOAT
	if (num == 1) {
		switch(childleft->nodetype) {
		case ID:
			parent->irinfo->op = newOperand(VARIABLE_OP);
			parent->irinfo->op->isBool = 0;
			parent->irinfo->op->isConstant = 0;
			parent->irinfo->op->type = String;
			parent->irinfo->op->str = (char*)strdup(parent->nodevalue.str);
			symNode = lookup(parent->nodevalue.str);
			if(symNode->var->isParam == 1) {
				if(parent->type->kind == ARRAY) {
					parent->irinfo->op->isAddr = 1;
				}
			}
			break;
		case INT:
			parent->nodevalue.INT = childleft->nodevalue.INT;
			parent->irinfo->op = newOperand(CONSTANT_OP);
			parent->irinfo->op->isBool = 0;
			parent->irinfo->op->isConstant = 1;
			parent->irinfo->op->type = Int;
			parent->irinfo->op->num_int = parent->nodevalue.INT;
			break;
		case FLOAT:
			parent->nodevalue.FLOAT = childleft->nodevalue.FLOAT;
			parent->irinfo->op = newOperand(CONSTANT_OP);
			parent->irinfo->op->isBool = 0;
			parent->irinfo->op->isConstant = 1;
			parent->irinfo->op->type = Float;
			parent->irinfo->op->num_float = parent->nodevalue.FLOAT;
			break;
		}
		goto ExpDebug;
	}
	// MINUS Exp; NOT Exp
	childmid = parent->children[1];
	if (num == 2) {
		if(childleft->nodetype == MINUS) {
			op_tmp = newTemp();
			op_tmp2 = newOperand(CONSTANT_OP);
			op_tmp2->type = Int;
			op_tmp2->num_int = 0;
			result = Assign3IR(op_tmp, op_tmp2, SUB_IR, childmid->irinfo->op);
			parent->irinfo->op = op_tmp;
			parent->irinfo->op->isBool = childmid->irinfo->op->isBool;
			parent->irinfo->op->isConstant = childmid->irinfo->op->isConstant;
		}
		if(childleft->nodetype == NOT) {
			parent->irinfo->truelist = childmid->irinfo->falselist;		
			parent->irinfo->falselist = childmid->irinfo->truelist;		
		}
		goto ExpDebug;
	}
	childright = parent->children[2];
	if (num == 3) {
		// LP Exp RP
		if (childleft->nodetype == LP && childright->nodetype == RP) {
			parent->irinfo->op = childmid->irinfo->op;
			parent->irinfo->truelist = childmid->irinfo->truelist;		
			parent->irinfo->falselist = childmid->irinfo->falselist;		
			goto ExpDebug;
		}
		// ID LP RP
		if (childleft->nodetype == ID) {
			op_tmp = newTemp();
			if(strcmp(childleft->nodevalue.str, "read") == 0) {
				result = ReadIR(op_tmp);
			}else {
				result = CallIR(op_tmp, childleft->nodevalue.str);
			}
			parent->irinfo->op = op_tmp;
			parent->irinfo->op->isBool = 0;
			parent->irinfo->op->isConstant = 0;
			parent->irinfo->op->isAddr = 0;
			goto ExpDebug;
		}
		// Exp DOT ID
		if (childright->nodetype == ID) {
//			parent->errorInfo = childleft->errorInfo;
//			if(childleft->type->kind != STRUCTURE){
//				parent->errorInfo->ErrorTypeNum = 13;
//				SemanticError(parent->errorInfo);
//				parent->type = newType();
//				parent->type->kind = ERROR;
//				goto ExpDebug;
//			}
//			parent->errorInfo = GetErrorInfoByNum(IdErrorInfoStackHead, totalErrorInfo);
//			parent->type = lookupFieldListElem(childleft->type->structure, childright->nodevalue.str);
//			if (parent->type == NULL) {
//				parent->errorInfo->ErrorTypeNum = 14;
//				SemanticError(parent->errorInfo);
//				parent->type = newType();
//				parent->type->kind = ERROR;
//			}else {
//				parent->nodevalue.str = childright->nodevalue.str;
//			}
			goto ExpDebug;
		}
		// Exp ASSIGNOP Exp
		if(childmid->nodetype == ASSIGNOP) {
			Assign2IR(childleft->irinfo->op, childright->irinfo->op);	
			parent->irinfo->op = childleft->irinfo->op;
			goto ExpDebug;
		}
		// other Exp
		op_tmp = newTemp();
		op_tmp->isAddr = 0;
		op_tmp->isBool = 0;
		op_tmp->isConstant = 0;
		switch(childmid->nodetype) {
		case PLUS:
			if(childleft->irinfo->op->isConstant == 1 && childright->irinfo->op->isConstant == 1) {
				op_tmp->isConstant = 1;
				op_tmp->type = childleft->irinfo->op->type;
				switch(op_tmp->type) {
				case Int: 
					op_tmp->num_int = childleft->irinfo->op->num_int + childright->irinfo->op->num_int;
					break;
				case Float: 
					op_tmp->num_float = childleft->irinfo->op->num_float + childright->irinfo->op->num_float;
					break;
				}
			}else {
				Assign3IR(op_tmp, childleft->irinfo->op, ADD_IR,childright->irinfo->op);
			}
			break;
		case MINUS:
			if(childleft->irinfo->op->isConstant == 1 && childright->irinfo->op->isConstant == 1) {
				op_tmp->isConstant = 1;
				op_tmp->type = childleft->irinfo->op->type;
				switch(op_tmp->type) {
				case Int: 
					op_tmp->num_int = childleft->irinfo->op->num_int - childright->irinfo->op->num_int;
					break;
				case Float: 
					op_tmp->num_float = childleft->irinfo->op->num_float - childright->irinfo->op->num_float;
					break;
				}
			}else {
				Assign3IR(op_tmp, childleft->irinfo->op, SUB_IR,childright->irinfo->op);
			}
			break;
		case STAR:
			if(childleft->irinfo->op->isConstant == 1 && childright->irinfo->op->isConstant == 1) {
				op_tmp->isConstant = 1;
				op_tmp->type = childleft->irinfo->op->type;
				switch(op_tmp->type) {
				case Int: 
					op_tmp->num_int = childleft->irinfo->op->num_int * childright->irinfo->op->num_int;
					break;
				case Float: 
					op_tmp->num_float = childleft->irinfo->op->num_float * childright->irinfo->op->num_float;
					break;
				}
			}else {
				Assign3IR(op_tmp, childleft->irinfo->op, MUL_IR,childright->irinfo->op);
			}
			break;
		case DIV:
			if(childleft->irinfo->op->isConstant == 1 && childright->irinfo->op->isConstant == 1) {
				op_tmp->isConstant = 1;
				op_tmp->type = childleft->irinfo->op->type;
				switch(op_tmp->type) {
				case Int: 
					op_tmp->num_int = childleft->irinfo->op->num_int / childright->irinfo->op->num_int;
					break;
				case Float: 
					op_tmp->num_float = childleft->irinfo->op->num_float / childright->irinfo->op->num_float;
					break;
				}
			}else {
				Assign3IR(op_tmp, childleft->irinfo->op, DIV_IR,childright->irinfo->op);
			}
			break;
		case AND:
			op_tmp->isBool = 1;
			Mop = Mpop();
			Opbackpatch(childleft->irinfo->falselist, Mop);
			parent->irinfo->truelist = childright->irinfo->truelist;
			parent->irinfo->falselist = Opmerge(childleft->irinfo->falselist, childright->irinfo->falselist);
			break;
		case OR:
			op_tmp->isBool = 1;
			Mop = Mpop();
			Opbackpatch(childleft->irinfo->truelist, Mop);
			parent->irinfo->falselist = childright->irinfo->falselist;
			parent->irinfo->truelist = Opmerge(childleft->irinfo->truelist, childright->irinfo->truelist);
			break;
		case RELOP:
			op_tmp->isBool = 1;
			trueop = newOperand(LABEL_OP);
			trueop->type = Int;
			trueop->num_int = -1;
			falseop = newOperand(LABEL_OP);
			falseop->type = Int;
			falseop->num_int = -1;
//			result = IfIR(childleft->irinfo->op, childmid->nodevalue.str, childright->irinfo->op, trueop);
//			GotoIR(falseop);
			result = IffalseIR(childleft->irinfo->op, childmid->nodevalue.str, childright->irinfo->op, falseop);
			parent->irinfo->truelist = Opmakelist(trueop);
			parent->irinfo->falselist = Opmakelist(falseop);
			noif = 0;
			beginbool = 0;
			break;
		}
		parent->irinfo->op = op_tmp;
		goto ExpDebug;
	}
	//ID LP Args RP; Exp LB Exp RB
	if (num == 4) {
		// ID LP Args RP
		if(childleft->nodetype == ID) {
			if(strcmp(childleft->nodevalue.str, "write") == 0) {
				result = WriteIR(childright->irinfo->op);			
			}else {
				genArgsIR(childright->irinfo);
				op_tmp = newTemp();
				result = CallIR(op_tmp, childleft->nodevalue.str);
				parent->irinfo->op = op_tmp;
				parent->irinfo->op->isAddr = 0;
				parent->irinfo->op->isBool = 0;
			}
			goto ExpDebug;
		}	
		// Exp LB Exp RB
		op_tmp = newTemp();
		parent->irinfo->op = newTemp();
		parent->irinfo->op->isAddr = 1;
		op_tmp2 = newOperand(CONSTANT_OP);
		op_tmp2->type = Int;
		op_tmp2->isConstant = 1;
		op_tmp2->num_int = childleft->type->array.size;
		if(childright->irinfo->op->isConstant == 1) {
			op_tmp->isConstant = 1;
			op_tmp->type = childright->irinfo->op->type;
			op_tmp->num_int = childright->irinfo->op->num_int * op_tmp2->num_int;
		}else {
			Assign3IR(op_tmp, childright->irinfo->op, MUL_IR, op_tmp2);
		}
		Assign3AddrIR(parent->irinfo->op, childleft->irinfo->op, ADD_IR, op_tmp);
		goto ExpDebug;
	}
ExpDebug:
	thelastexp = parent;
	expcount++;
	if(beginbool == 1) {
		if(pairscount == 0 && noif == 1) {
			trueop = newOperand(LABEL_OP);
			trueop->type = Int;
			trueop->num_int = -1;
			falseop = newOperand(LABEL_OP);
			falseop->type = Int;
			falseop->num_int = -1;
			op_tmp = newOperand(CONSTANT_OP);
			op_tmp->type = Int;
			op_tmp->num_int = 0;
//			result = IfIR(parent->irinfo->op, "!=", op_tmp, trueop);
//			GotoIR(falseop);
			result = IfIR(parent->irinfo->op, "==", op_tmp, falseop);
			parent->irinfo->truelist = Opmakelist(trueop);
			parent->irinfo->falselist = Opmakelist(falseop);
			beginbool = 0;
			noif = 0;
		}
	}
}
void hurrytestif() {
	Operand op_tmp;
	Operand trueop, falseop;
	InterCode result;	
	if(beginbool == 1 && expcount == 1) {
		if(pairscount == 0 && noif == 1) {
			trueop = newOperand(LABEL_OP);
			trueop->type = Int;
			trueop->num_int = -1;
			falseop = newOperand(LABEL_OP);
			falseop->type = Int;
			falseop->num_int = -1;
			op_tmp = newOperand(CONSTANT_OP);
			op_tmp->type = Int;
			op_tmp->num_int = 0;
//			result = IfIR(thelastexp->irinfo->op, "!=", op_tmp, trueop);
//			GotoIR(falseop);
			result = IfIR(thelastexp->irinfo->op, "==", op_tmp, falseop);
			thelastexp->irinfo->truelist = Opmakelist(trueop);
			thelastexp->irinfo->falselist = Opmakelist(falseop);
			beginbool = 0;
			noif = 0;
		}
	}
}
// update parent->fieldList 
void ArgsTrans(TreeNode parent, int num) {
	TreeNode exp = NULL;
	TreeNode args = NULL;

	exp = parent->children[0];
	parent->irinfo = newIRinfo();
	parent->irinfo->op = exp->irinfo->op;
	if(exp->type->kind == ARRAY) {
		parent->irinfo->op->isArray = 1;
	}
	// Exp
	if (num == 1) {
	}
	// Exp COMMA Args
	if (num == 3) {
		args = parent->children[2];	
		parent->irinfo->next = args->irinfo;
	}
}
void translateIR(TreeNode parent, int num) {
	switch (parent->nodetype) {
	case Program:		ProgramTrans(parent, num);		break;
	case ExtDefList:	ExtDefListTrans(parent, num);		break;
	case ExtDef:		ExtDefTrans(parent, num);		break;
	case ExtDecList:	ExtDecListTrans(parent, num);		break;
	case Specifier:		SpecifierTrans(parent, num); 		break;
	case StructSpecifier:	StructSpecifierTrans(parent, num);	break;
	case OptTag:		OptTagTrans(parent, num);		break;
	case Tag:		TagTrans(parent, num);			break;
	case VarDec:		VarDecTrans(parent, num);		break;
	case FunDec:		FunDecTrans(parent, num);		break;
	case VarList:		VarListTrans(parent, num);		break;
	case ParamDec:		ParamDecTrans(parent, num);		break;
	case CompSt:		CompStTrans(parent, num);		break;
	case StmtList:		StmtListTrans(parent, num);		break;
	case Stmt:		StmtTrans(parent, num);			break; 
	case DefList:		DefListTrans(parent, num);		break;
	case Def:		DefTrans(parent, num);			break;
	case DecList:		DecListTrans(parent, num);		break;
	case Dec:		DecTrans(parent, num);			break;
	case Exp:		ExpTrans(parent, num);			break;
	case Args:		ArgsTrans(parent, num);			break;
	case M:			MTrans(parent, num);			break;
	case N:			NTrans(parent, num);			break;
	}
}
