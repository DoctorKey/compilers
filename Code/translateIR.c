#include "translateIR.h"
#include "main.h"
#include "symtable.h"
#include "syntax.tab.h"
#include "semantichelp.h"
#include "name.h"
#include "error.h"
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
/*
	struct FuncList *declist = NULL;
	Symbol symNode = NULL;
	Symbol tarNode = NULL;
	declist = getDecFuncList(); 
	while(declist != NULL) {
		symNode = declist->funcSymbol;	
		tarNode = lookup(symNode->name);
		if(tarNode == NULL || tarNode->func->isDefine == 0) {
			symNode->errorInfo->ErrorTypeNum = 18;
			SemanticError(symNode->errorInfo);
		}else {
			tarNode->func->isDeclare = 1;
		}
		declist = declist->next;
	}
	if(debug2){
		showAllSymbol();
		getHashTableInfo();
		ShowErrorInfoStack(IdErrorInfoStackHead);
		ShowErrorInfoStack(NumErrorInfoStackHead);
	}
//	cleanHashTable();
//	getHashTableInfo();
*/
}
void ExtDefListTrans(TreeNode parent, int num) {
}
// update Gspecifier = NULL;GFuncReturn = NULL;
void ExtDefTrans(TreeNode parent, int num) {
/*
	TreeNode specifier = NULL;
	TreeNode midchild = NULL;
	TreeNode rightchild = NULL;
	Symbol symNode = NULL;
	FieldList fieldList = NULL;
	FieldList param = NULL;
	ErrorInfo errorInfo = NULL;

	specifierLock = 0;
	specifier = parent->children[0];
	if (specifier == NULL) {
		fprintf(stderr, "ExtDef's child Specifier is NULL\n");
		goto ExtDefDebug;
	}
	midchild = parent->children[1];
	if (midchild == NULL) {
		fprintf(stderr, "ExtDef's child is NULL\n");
		goto ExtDefDebug;
	}
	//Specifier SEMI
	if (midchild->nodetype == SEMI) {
		goto ExtDefDebug;
	}
	rightchild = parent->children[2];
	if (rightchild == NULL) {
		fprintf(stderr, "ExtDef's right child is NULL\n");
		goto ExtDefDebug;
	}
	//Specifier ExtDecList SEMI
	if (midchild->nodetype == ExtDecList) {
		goto ExtDefDebug;
	}
	//Specifier FunDec SEMI
	if (rightchild->nodetype == SEMI) {
		param = Funcsymbol->func->argtype;
		while(param) {
			symNode = lookup(param->name);
			symNode->var->isDeclare = 1;
			param = param->tail;
		}
		symNode = lookupByType(Funcsymbol->name, Func);
		if(symNode) {
			if(symNode->func->isDefine || symNode->func->isDeclare) {
				//have been defined or declared	
				if(cmpFuncSym(symNode, Funcsymbol)) {
					//diff
					Funcsymbol->errorInfo->ErrorTypeNum = 19;
					SemanticError(Funcsymbol->errorInfo);
				}else {
					symNode->func->isDeclare = 1;			
				}
				//may free some types
//				freeSymNode(Funcsymbol);
			}else {
				//first declare
				symNode->func->isDeclare = 1;			
				Funcsymbol->func->isDeclare = 1;			
				addDecFunc(Funcsymbol);
			}
		}else {
			fprintf(stderr, "not insert the function!\n");
		}
		Funcsymbol = NULL;
	}
	//Specifier FunDec CompSt
	if (rightchild->nodetype == CompSt) {
		param = Funcsymbol->func->argtype;
		while(param) {
			symNode = lookup(param->name);
			if(symNode->var->isDefine == 1) {
				errorInfo = param->errorInfo;	
				errorInfo->ErrorTypeNum = 3;
				SemanticError(errorInfo);
			}else {
				symNode->var->isDefine = 1;
			}
			param = param->tail;
		}
		symNode = lookupByType(Funcsymbol->name, Func);
		if(symNode) {
			if(symNode->func->isDefine) {
				//have been defined
				Funcsymbol->errorInfo->ErrorTypeNum = 4;
				SemanticError(Funcsymbol->errorInfo);
				//may free some types
//				freeSymNode(Funcsymbol);
			}else if (symNode->func->isDeclare) {
				if(cmpFuncSym(symNode, Funcsymbol)) {
					//diff
					symNode->errorInfo->ErrorTypeNum = 19;
					SemanticError(symNode->errorInfo);
				}else { 
					symNode->func->isDefine = 1;			
				}
				//may free some types
//				freeSymNode(Funcsymbol);
			}else {
				//first define
				symNode->func->isDefine = 1;			
			}
		}else {
			fprintf(stderr, "not insert the function!\n");
		}
		Funcsymbol = NULL;
	}
	GFuncReturn = NULL;
	Gspecifier = NULL;
ExtDefDebug:
	if(debug2) {
		showDecFuncList();
	}
	*/
}
// update add symbol
void ExtDecListTrans(TreeNode parent, int num) {
/*
	TreeNode vardec = NULL;
	Symbol symNode = NULL;

	vardec = parent->children[0];
	if (vardec == NULL) {
		fprintf(stderr, "ExtDecList's child VarDec is NULL\n");
		goto ExtDecListDebug;
	}
	parent->errorInfo = vardec->errorInfo;
	symNode = lookup(vardec->nodevalue.str);
	if (symNode && (symNode->type == Var || symNode->type == Struct)) {
		parent->errorInfo->ErrorTypeNum = 3;
		SemanticError(parent->errorInfo);
	}else {
		symNode = newVar(vardec->nodevalue.str, Gspecifier, parent->errorInfo);
		insert(symNode);
	}
ExtDecListDebug:
	if(debug2) {
		fprintf(stdout, "ExtDecList add new symbol: \n");
		showSymbol(symNode);
		fprintf(stdout, "\n");
	}
*/
}
/*
	Specifiers
*/
// update parent->type , Gspecifier
void SpecifierTrans(TreeNode parent, int num) {
/*
	//TYPE; StructSpecifier
	TreeNode child = parent->children[0];
	if (child == NULL) {
		fprintf(stderr, "Specifier have no child!\n");
		parent->type = newType();
		parent->type->kind = ERROR;
		goto SpecifierDebug;
	}
	switch (child->nodetype) {
	case TYPE:
		parent->errorcount = 0;
		parent->type = newType();
		parent->type->kind = BASIC;
		if (!strcmp(child->nodevalue.str, "int"))
			parent->type->basic = INT;
		else if (!strcmp(child->nodevalue.str, "float"))
			parent->type->basic = FLOAT;
		else
			fprintf(stderr, "error type\n");
		break;
	case StructSpecifier:
		parent->errorcount = child->errorcount;
		parent->type = child->type;
		break;
	default:
		fprintf(stderr, "Specifier child type error!\n");
		parent->type = newType();
		parent->type->kind = ERROR;
		break;
	}
	Gspecifier = parent->type;
	if (specifierLock == 0) {
		GFuncReturn = parent->type;
		specifierLock = 1;
	}
SpecifierDebug:
	if(debug2) {
		fprintf(stdout, "Specifier->type : ");
		showType(parent->type);
		fprintf(stdout, "\n");
		fprintf(stdout, "Gspecifier: ");
		showType(Gspecifier);
		fprintf(stdout, "\n");
		fprintf(stdout, "GFuncReturn: ");
		showType(GFuncReturn);
		fprintf(stdout, "\n");
	}
*/
}
// new struct type; add to symTable; update parent->type, parent->nodevalue.str, structdefnum--
void StructSpecifierTrans(TreeNode parent, int num) {
/*
	TreeNode tag = NULL;
	TreeNode deflist = NULL;
	Symbol symNode = NULL;
	FieldList structItem = NULL;
	FieldList errortag = NULL;
	FieldList preerrortag = NULL;
	tag = parent->children[1];
	if (tag == NULL) {
		parent->nodevalue.str = NULL;
		parent->errorcount = 0;
	}else {
		parent->nodevalue.str = tag->nodevalue.str;
		parent->errorcount = tag->errorcount;
	}
	// STRUCT Tag
	if (num == 2) {
		if (tag == NULL) {
			fprintf(stderr, "StructSpecifier's child Tag is NULL!\n");
			parent->type = newType();
			parent->type->kind = ERROR;
			goto StructSpecifierDebug;
		}
		parent->type = tag->type;
	}
	// STRUCT OptTag LC DefList RC
	if (num == 5) {
		parent->type = newType();
		parent->type->kind = STRUCTURE;
		parent->type->structname = parent->nodevalue.str;
		deflist = parent->children[3];	
		if (deflist == NULL) {
			// DefList can be NULL!
			parent->type->structure = NULL;
		}else {
			parent->type->structure = deflist->fieldList;	
		}
		parent->errorcount += deflist->errorcount;
		
		structItem = parent->type->structure;
		while(structItem) {
			preerrortag = structItem;
			errortag = structItem->tail;
			while(errortag) {
				if(strcmp(structItem->name, errortag->name) == 0) {
					errortag->errorInfo->ErrorTypeNum = 15;
					SemanticError(errortag->errorInfo);
					preerrortag->tail = errortag->tail;
					//free(errortag);
					errortag = preerrortag->tail;
				}else {
					preerrortag = errortag;
					errortag = errortag->tail;
				}
			}
			structItem = structItem->tail;
		}
		
		//only when OptTag has name,add it to symTable 
		if (parent->nodevalue.str != NULL) {
			symNode = newStruct(parent->nodevalue.str, parent->type, tag->errorInfo);
			if (symNode == NULL) {
				fprintf(stderr, "can't create symNode\n");
				goto StructSpecifierDebug;
			}
			insert(symNode);
			goto StructSpecifierDebug;
		}
	}
StructSpecifierDebug:
	if(debug2) {
		fprintf(stdout, "StructSpecifier->nodevalue.str: %s\n", parent->nodevalue.str);
		fprintf(stdout, "StructSpecifier->type : \n");
		showType(parent->type);
		fprintf(stdout, "\n symNode: \n");
		showSymbol(symNode);
		fprintf(stdout, "\n");
	}
*/
}
// update parent->nodevalue.str, parent->errorInfo
void OptTagTrans(TreeNode parent, int num) {
/*
	// only when OptTag -> ID , call this function
	TreeNode child = parent->children[0];
	Symbol symNode = NULL;
	int totalErrorInfo = GetTotalErrorInfo();
	parent->errorInfo = GetErrorInfoByNum(IdErrorInfoStackHead, totalErrorInfo);
	parent->errorcount = 1;
	if (child == NULL) {
		fprintf(stderr, "OptTag have no child\n");
		parent->nodevalue.str = NULL;
		goto OptTagDebug;
	}
	parent->nodevalue.str = child->nodevalue.str;
	symNode = lookup(parent->nodevalue.str); 
	if (symNode != NULL && (symNode->type == Struct || symNode->type == Var)) {
		parent->errorInfo->ErrorTypeNum = 16;
		SemanticError(parent->errorInfo);
		goto OptTagDebug;
	}
OptTagDebug:
	if(debug2) {
		fprintf(stdout, "OptTag->nodevalue.str = %s\n", parent->nodevalue.str);
	}
*/
}
//update parent->nodevalue.str, parent->type, parent->errorInfo
void TagTrans(TreeNode parent, int num) {
/*
	// ID
	TreeNode child = parent->children[0];
	Symbol symNode = NULL;
	int totalErrorInfo = GetTotalErrorInfo();
	if (child == NULL) {
		fprintf(stderr, "Tag have no child\n");
		parent->nodevalue.str = NULL;
		parent->type = newType();
		parent->type->kind = ERROR;
		goto TagDebug;
	}
	//TODO: just -1 may error
	parent->errorInfo = GetErrorInfoByNum(IdErrorInfoStackHead, totalErrorInfo - 1);
	parent->errorcount = 1;
	parent->nodevalue.str = child->nodevalue.str;
	symNode = lookupByType(parent->nodevalue.str, Struct); 
	if (symNode == NULL) {
		parent->errorInfo->ErrorTypeNum = 17;
		SemanticError(parent->errorInfo);
		parent->type = newType();
		parent->type->kind = ERROR;
		goto TagDebug;
	}
	parent->type = symNode->structure;
TagDebug:
	if(debug2) {
		fprintf(stdout, "Tag->nodevalue.str = %s\n", parent->nodevalue.str);
		fprintf(stdout, "Tag->type: \n");
		showType(parent->type);
		fprintf(stdout, "\n");
	}
*/
}

/*
	Declarators
*/
// update parent->type, parent->nodevalue.str
void VarDecTrans(TreeNode parent, int num) {
/*
	TreeNode id = NULL;
	TreeNode vardec = NULL;
	TreeNode index = NULL;
	int totalErrorInfo = GetTotalErrorInfo();
	// ID
	if (num == 1) {
		parent->errorInfo = GetErrorInfoByNum(IdErrorInfoStackHead, totalErrorInfo);
		parent->errorcount = 1;
		id = parent->children[0];	// ID
		if (id == NULL) {
			fprintf(stderr, "VarDec's child ID is NULL\n");
			parent->nodevalue.str = NULL;
		}else {
			parent->nodevalue.str = id->nodevalue.str;	
		}
		// inh the specifier
		parent->type = Gspecifier;
	}
	// VarDec LB INT RB
	if (num == 4) {
		parent->type = newType();
		parent->type->kind = ARRAY;

		vardec = parent->children[0];	// VarDec
		if (vardec == NULL) {
			fprintf(stderr, "VarDec's child VarDec is NULL\n");
			parent->nodevalue.str = NULL;
			parent->type->array.elem = NULL;
		}else {
			parent->nodevalue.str = vardec->nodevalue.str;	
			parent->type->array.elem = vardec->type;
		}
		parent->errorInfo = vardec->errorInfo;
		parent->errorcount = vardec->errorcount + 1;

		// Error type B check it is INT
		index = parent->children[2];
		if (index == NULL) {
			fprintf(stderr, "VarDec's child INT is NULL\n");
			parent->type->array.size = 0;
		}else{
			parent->type->array.size = index->nodevalue.INT;
		}
	}
VarDecDebug:
	if(debug2) {
		fprintf(stdout, "VarDec->nodevalue.str = %s\n", parent->nodevalue.str);
		fprintf(stdout, "VarDec->type: \n");
		showType(parent->type);
		fprintf(stdout, "\n");
	}
*/
}
// update GFuncReturn, parent->errorInfo
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
		paramop->isParam = 1;
		paramop->type = String;
		paramop->str = strdup(fieldlist->name);
		paramIR = ParamIR(paramop);
		fieldlist = fieldlist->tail;	
	}
}
// update parent->fieldList
void VarListTrans(TreeNode parent, int num) {
/*
	TreeNode paramdec = NULL;
	TreeNode varlist = NULL;
	FieldList fieldList = NULL;

	paramdec = parent->children[0];
	if (paramdec == NULL) {
		fprintf(stderr, "VarList's child ParamDec is NULL\n");
		goto VarListDebug;
	}
	parent->errorcount = paramdec->errorcount;

	if (num == 1) {
		fieldList = NULL;
	}else {
		varlist = parent->children[2];
		if (varlist == NULL) {
			fprintf(stderr, "VarList's child VarList is NULL\n");
			goto VarListDebug;
		}
		fieldList = varlist->fieldList;
		parent->errorcount += varlist->errorcount;
	}	
	parent->fieldList = newFieldList(paramdec->nodevalue.str, paramdec->type, fieldList, paramdec->errorInfo);
VarListDebug:
	if(debug2) {
		fprintf(stdout, "VarList->fieldList : \n");
		showFieldList(parent->fieldList);
		fprintf(stdout, "\n");
	}
*/
}
//update parent->type, parent->nodevalue.str, add to symbol table
void ParamDecTrans(TreeNode parent, int num) {
/*
	TreeNode specifier = NULL;	
	TreeNode varDec = NULL;	
	Symbol symNode = NULL;

	specifier = parent->children[0];	// Specifier
	varDec = parent->children[1];	// VarDec
	if (specifier == NULL) {
		fprintf(stderr, "ParamDec's child Specifier is NULL\n");
		goto ParamDecDebug;
	}
	if (varDec == NULL) {
		fprintf(stderr, "ParamDec's child VarDec is NULL\n");
		goto ParamDecDebug;
	}
	parent->errorInfo = varDec->errorInfo;
	parent->errorcount = specifier->errorcount + varDec->errorcount;

	parent->type = varDec->type;
	parent->nodevalue.str = varDec->nodevalue.str;

	symNode = lookup(parent->nodevalue.str);
	//only insert once
	if(!symNode) {
		symNode = newVar(parent->nodevalue.str, parent->type, parent->errorInfo);
		insert(symNode);
	}
	
ParamDecDebug:
	if (debug2) {
		fprintf(stdout, "ParamDec->nodevalue.str = %s\n", parent->nodevalue.str);
		fprintf(stdout, "ParamDec->type : \n");
		showType(parent->type);
		fprintf(stdout, "\n add new symbol : \n");
		showSymbol(symNode);
		fprintf(stdout, "\n");
	}
*/
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
// update parent->fieldList 
void DefListTrans(TreeNode parent, int num) {
/*
	// Def DefList
	TreeNode def = NULL;	
	TreeNode deflist = NULL;	
	FieldList defFieldList = NULL;
	FieldList deflistFieldList = NULL;

	def = parent->children[0];	
	if (def == NULL) {
		fprintf(stderr, "DefList's child Def is NULL\n");
		goto DefListDebug;
	}
	defFieldList = def->fieldList;
	parent->errorcount = def->errorcount;
	
	if (defFieldList == NULL) {
		fprintf(stderr, "DefList's child Def's fieldList is NULL\n");
		goto DefListDebug;
	}
	//Maybe it is NULL ?
	deflist = parent->children[1];	// DefList
	if (deflist == NULL) {
		deflistFieldList = NULL;
	}else {
		deflistFieldList = deflist->fieldList;
		parent->errorcount += deflist->errorcount;
	}
	parent->fieldList = mergeFieldList(defFieldList, deflistFieldList);
DefListDebug:
	if (debug2) {
		fprintf(stdout, "DefList->fieldList : \n");
		showFieldList(parent->fieldList);
		fprintf(stdout, "\n");
	}
*/
}
// update parent->type, parent->fieldList, Gspecifier = NULL
void DefTrans(TreeNode parent, int num) {
/*
	// Specifier DefList SEMI
	TreeNode specifier = NULL;	
	TreeNode declist = NULL;	
	FieldList fieldList = NULL;
	Symbol symNode = NULL;
	
	Gspecifier = NULL;
	specifier = parent->children[0];	// Specifier
	if (specifier == NULL) {
		fprintf(stderr, "Def's child Specifier is NULL\n");
		goto DefDebug;
	}
	parent->type = specifier->type;

	declist = parent->children[1];	// DecList
	if (declist == NULL) {
		fprintf(stderr, "Def's child DecList is NULL\n");
		goto DefDebug;
	}
	parent->fieldList = declist->fieldList;
	parent->errorcount = specifier->errorcount + declist->errorcount;
	specifierLock = 0;
DefDebug:
	if (debug2) {
		fprintf(stdout, "Def->type : \n");
		showType(parent->type);
		fprintf(stdout, "\n Def->fieldList : \n");
		showFieldList(parent->fieldList);
		fprintf(stdout, "\n");
	}
*/
}
//update parent->fieldList 
void DecListTrans(TreeNode parent, int num) {
/*
	TreeNode dec = NULL;
	TreeNode declist = NULL;
	FieldList fieldList = NULL;

	dec = parent->children[0];	//Dec
	if (dec == NULL) {
		fprintf(stderr, "DecList's child Dec is NULL\n");
		goto DecListDebug;
	}
	parent->errorcount = dec->errorcount;
	// Dec
	if (num == 1) {
		fieldList = NULL;
	}
	// Dec COMMA DecList
	if (num == 3) {
		declist = parent->children[2];	// DecList
		if (declist == NULL) {
			fprintf(stderr, "DecList's child DecList is NULL\n");
			goto DecListDebug;
		}
		fieldList = declist->fieldList;
		parent->errorcount += declist->errorcount;
	}
	parent->fieldList = newFieldList(dec->nodevalue.str, dec->type, fieldList, dec->errorInfo);	
DecListDebug:
	if (debug2) {
		fprintf(stdout, "DecList->fieldList :\n");
		showFieldList(parent->fieldList);
		fprintf(stdout, "\n");
	}
*/
}
// update parent->nodevalue.str, parent->type, add symbol
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
			parent->irinfo->op->type = String;
			parent->irinfo->op->str = (char*)strdup(parent->nodevalue.str);
			if(parent->type->kind == ARRAY) {
				parent->irinfo->isArray = 1;
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
		switch(childmid->nodetype) {
		case PLUS:
			Assign3IR(op_tmp, childleft->irinfo->op, ADD_IR,childright->irinfo->op);
			break;
		case MINUS:
			Assign3IR(op_tmp, childleft->irinfo->op, SUB_IR,childright->irinfo->op);
			break;
		case STAR:
			Assign3IR(op_tmp, childleft->irinfo->op, MUL_IR,childright->irinfo->op);
			break;
		case DIV:
			Assign3IR(op_tmp, childleft->irinfo->op, DIV_IR,childright->irinfo->op);
			break;
		case AND:
			Mop = Mpop();
			Opbackpatch(childleft->irinfo->falselist, Mop);
			parent->irinfo->truelist = childright->irinfo->truelist;
			parent->irinfo->falselist = Opmerge(childleft->irinfo->falselist, childright->irinfo->falselist);
			break;
		case OR:
			Mop = Mpop();
			Opbackpatch(childleft->irinfo->truelist, Mop);
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
			result = IfIR(childleft->irinfo->op, childmid->nodevalue.str, childright->irinfo->op, trueop);
			GotoIR(falseop);
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
			}
			goto ExpDebug;
		}	
		// Exp LB Exp RB
		op_tmp = newTemp();
		parent->irinfo->op = newTemp();
		op_tmp2 = newOperand(CONSTANT_OP);
		op_tmp2->type = Int;
		op_tmp2->num_int = childleft->type->array.size;
		Assign3IR(op_tmp, childright->irinfo->op, MUL_IR, op_tmp2);
		if(childleft->irinfo->op->isParam == 1) {
			childleft->irinfo->op->kind = VALUEINADDR_OP;
		}else if(childleft->irinfo->op->kind == TEMP_OP) {
			childleft->irinfo->op->kind = TEMP_ADDR_OP;
		}else if(childleft->irinfo->op->kind == VARIABLE_OP) {
			childleft->irinfo->op->kind = ADDRESS_OP;
		}
		Assign3IR(parent->irinfo->op, childleft->irinfo->op, ADD_IR, op_tmp);
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
			result = IfIR(parent->irinfo->op, "!=", op_tmp, trueop);
			GotoIR(falseop);
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
			result = IfIR(thelastexp->irinfo->op, "!=", op_tmp, trueop);
			GotoIR(falseop);
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
