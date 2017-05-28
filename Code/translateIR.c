#include "translateIR.h"
#include "main.h"
#include "symtable.h"
#include "syntax.tab.h"
#include "semantichelp.h"
#include "name.h"
#include "error.h"
#include "IRopt.h"
/*--------------------------------------------------------------------------

	High-level Definitions

--------------------------------------------------------------------------
*/
void ProgramTrans(TreeNode parent, int num) {
	InterCodes IRhead = getIRhead();
	IRhead = ifreduce(IRhead);
	IRhead = labelreduce2(IRhead);
	IRhead = labelreduce(IRhead);
	IRhead = constantreduce(IRhead);
//	assignreduce(IRhead);

	IRhead = initbasicblock(IRhead);
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
	PrintError('D', NULL);
	exit(0);
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
	Symbol symNode = NULL;
	char *funcname = NULL;
	InterCode funcIR = NULL, paramIR = NULL;
	Operand paramop = NULL;

	// maybe NULL
	funcname = parent->nodevalue.str;
	// maybe NULL, NULL is no param
	fieldlist = parent->fieldList;

	funcIR = FunctionIR(funcname);
	while(fieldlist) {
		symNode = lookup(fieldlist->name);
		paramop = getOpBySymbol(symNode);
		if(fieldlist->type->kind == ARRAY) 
			paramop->isAddr = 1;
		else	
			paramop->isAddr = 0;
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
		result->isComputeAddr = isAddr(exp->type);
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
		result->isComputeAddr = isAddr(parent->type);
	}
}

/*
	Expressions
*/
void genArgsIR(FieldList argtype, IRinfo irinfo) {
	InterCode result;	
	if(irinfo == NULL || argtype == NULL) 
		return;
	genArgsIR(argtype->tail, irinfo->next);
	result = ArgIR(irinfo->op);
	result->isComputeAddr = isAddr(argtype->type);
}
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
//			parent->irinfo->op = newOperand(VARIABLE_OP);
//			parent->irinfo->op->type = String;
//			parent->irinfo->op->str = (char*)strdup(parent->nodevalue.str);
			symNode = lookup(parent->nodevalue.str);
			parent->irinfo->op = getOpBySymbol(symNode);
			if(symNode->var->isParam == 1) {
				if(parent->type->kind == ARRAY) {
					parent->irinfo->op->isAddr = 1;
				}
			}
			break;
		case INT:
			parent->nodevalue.INT = childleft->nodevalue.INT;
			parent->irinfo->op = newOperand(CONSTANT_OP);
			parent->irinfo->op->type = Int;
			parent->irinfo->op->num_int = parent->nodevalue.INT;
			break;
		case FLOAT:
			parent->nodevalue.FLOAT = childleft->nodevalue.FLOAT;
			parent->irinfo->op = newOperand(CONSTANT_OP);
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
		}
		if(childleft->nodetype == NOT) {
			parent->irinfo->truelist = childmid->irinfo->falselist;		
			parent->irinfo->falselist = childmid->irinfo->truelist;		
			parent->irinfo->op = childmid->irinfo->op;
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
			symNode = lookup(childleft->nodevalue.str);
			op_tmp = newTemp();
			if(strcmp(childleft->nodevalue.str, "read") == 0) {
				result = ReadIR(op_tmp);
			}else {
				result = CallIR(op_tmp, childleft->nodevalue.str);
			}
			result->isComputeAddr = isAddr(symNode->func->Return);
			parent->irinfo->op = op_tmp;
			parent->irinfo->op->isAddr = result->isComputeAddr;
			goto ExpDebug;
		}
		// Exp DOT ID
		if (childright->nodetype == ID) {
			PrintError('D', NULL);
			exit(0);
			goto ExpDebug;
		}
		// Exp ASSIGNOP Exp
		if(childmid->nodetype == ASSIGNOP) {
			result = Assign2IR(childleft->irinfo->op, childright->irinfo->op);	
			result->isComputeAddr = isAddr(childleft->type);
			parent->irinfo->op = childleft->irinfo->op;
			goto ExpDebug;
		}
		// other Exp
		op_tmp = newTemp();
		op_tmp->isAddr = 0;
		switch(childmid->nodetype) {
		case PLUS:
			result = Assign3IR(op_tmp, childleft->irinfo->op, ADD_IR,childright->irinfo->op);
			result->isComputeAddr = isAddr(childleft->type);
			break;
		case MINUS:
			result = Assign3IR(op_tmp, childleft->irinfo->op, SUB_IR,childright->irinfo->op);
			result->isComputeAddr = isAddr(childleft->type);
			break;
		case STAR:
			result = Assign3IR(op_tmp, childleft->irinfo->op, MUL_IR,childright->irinfo->op);
			result->isComputeAddr = isAddr(childleft->type);
			break;
		case DIV:
			result = Assign3IR(op_tmp, childleft->irinfo->op, DIV_IR,childright->irinfo->op);
			result->isComputeAddr = isAddr(childleft->type);
			break;
		case AND:
			Mop = Mpop();
			Opbackpatch(childleft->irinfo->truelist, Mop);
			parent->irinfo->truelist = childright->irinfo->truelist;
			parent->irinfo->falselist = Opmerge(childleft->irinfo->falselist, childright->irinfo->falselist);
			break;
		case OR:
			Mop = Mpop();
			Opbackpatch(childleft->irinfo->falselist, Mop);
			parent->irinfo->falselist = childright->irinfo->falselist;
			parent->irinfo->truelist = Opmerge(childleft->irinfo->truelist, childright->irinfo->truelist);
			break;
		case RELOP:
			trueop = newOperand(LABEL_OP);
			trueop->type = Int;
			trueop->num_int = -1;
			falseop = newOperand(LABEL_OP);
			falseop->type = Int;
			falseop->num_int = -1;
//			result = IfIR(childleft->irinfo->op, getfalseRelop(childmid->nodevalue.str), childright->irinfo->op, trueop);
			result = IfIR(childleft->irinfo->op, childmid->nodevalue.str, childright->irinfo->op, trueop);
			result->isComputeAddr = isAddr(childleft->type);
			GotoIR(falseop);
//			parent->irinfo->truelist = getFall();
			parent->irinfo->truelist = Opmakelist(trueop);
			parent->irinfo->falselist = Opmakelist(falseop);
			break;
		}
		parent->irinfo->op = op_tmp;
		goto ExpDebug;
	}
	//ID LP Args RP; Exp LB Exp RB
	if (num == 4) {
		// ID LP Args RP
		if(childleft->nodetype == ID) {
			symNode = lookup(childleft->nodevalue.str);
			if(strcmp(childleft->nodevalue.str, "write") == 0) {
				result = WriteIR(childright->irinfo->op);			
				result->isComputeAddr = isAddr(symNode->func->Return);
			}else {
				genArgsIR(symNode->func->argtype, childright->irinfo);
				op_tmp = newTemp();
				result = CallIR(op_tmp, childleft->nodevalue.str);
				result->isComputeAddr = isAddr(symNode->func->Return);
				parent->irinfo->op = op_tmp;
				parent->irinfo->op->isAddr = result->isComputeAddr;
			}
			goto ExpDebug;
		}	
		// Exp LB Exp RB
		op_tmp = newTemp();
		parent->irinfo->op = newTemp();
		parent->irinfo->op->isAddr = 1;
		op_tmp2 = newOperand(CONSTANT_OP);
		op_tmp2->type = Int;
		op_tmp2->num_int = childleft->type->array.size;
		result = Assign3IR(op_tmp, childright->irinfo->op, MUL_IR, op_tmp2);
		result->isComputeAddr = 0;
		result = Assign3IR(parent->irinfo->op, childleft->irinfo->op, ADD_IR, op_tmp);
		result->isComputeAddr = 1;
		goto ExpDebug;
	}
ExpDebug:
	return;
}
void ArgsTrans(TreeNode parent, int num) {
	TreeNode exp = NULL;
	TreeNode args = NULL;

	exp = parent->children[0];
	parent->irinfo = newIRinfo();
	parent->irinfo->op = exp->irinfo->op;

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
