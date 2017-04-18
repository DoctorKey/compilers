#include "semantic.h"
#include "semantichelp.h"
#include "name.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
	here are some global var
	let them be inherit attribute
*/
Type Gspecifier = NULL;
Type GFuncReturn = NULL;
extern struct ErrorInfoStack *IdErrorInfoStackHead;
extern struct ErrorInfoStack *NumErrorInfoStackHead;
struct SymNode *Funcsymbol = NULL;
int structdefnum = 0;
int argsnum = 0;
int varlistnum = 0;
int specifierLock = 0;

/*--------------------------------------------------------------------------

	High-level Definitions

--------------------------------------------------------------------------
*/
void ProgramAnalyze(struct node *parent, int num) {
	struct FuncList *errorlist = NULL;
	int errortype;
	errorlist = checkDecFuncList();
	while(errorlist != NULL) {
		errortype = errorlist->funcSymbol->errorInfo->ErrorType;
		SemanticError(errortype, errorlist->funcSymbol->errorInfo);
		errorlist = errorlist->next;
	}
}
void ExtDefListAnalyze(struct node *parent, int num) {
}
// update Gspecifier = NULL;GFuncReturn = NULL;
void ExtDefAnalyze(struct node *parent, int num) {
	struct node *specifier = NULL;
	struct node *midchild = NULL;
	struct node *rightchild = NULL;
	struct SymNode *symNode = NULL;
	FieldList fieldList = NULL;

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
	//Specifier ExtDecList SEMI
	if (midchild->nodetype == ExtDecList) {
	}
	if (midchild->nodetype == FunDec) {
		rightchild = parent->children[2];
		if (rightchild == NULL) {
			fprintf(stderr, "ExtDef's right child is NULL\n");
			goto ExtDefDebug;
		}
		switch(rightchild->nodetype) {
		//Specifier FunDec SEMI
		case SEMI:
			Funcsymbol->func->isDeclare = 1;			
 			if(lookupDecFuncByName(Funcsymbol)) {
				if(!lookupDecFunc(Funcsymbol)) {
				//name is the same, type is diff
					SemanticError(19, Funcsymbol->errorInfo);
				}
			}else {
				insert(Funcsymbol);
				addDecFunc(Funcsymbol);
			}
			Funcsymbol = NULL;
			break;
		//Specifier FunDec CompSt
		case CompSt:
			Funcsymbol->func->isDefine = 1;			
 			if(lookupDefFuncByName(Funcsymbol)) {
				SemanticError(4, Funcsymbol->errorInfo);
			}else {
				insert(Funcsymbol);
				addDefFunc(Funcsymbol);
			}
			Funcsymbol = NULL;
			break;
		default:
			break;
		}
		GFuncReturn = NULL;
	}
	Gspecifier = NULL;
	specifierLock = 0;
ExtDefDebug:
	if(debug2) {
		showDefFuncList();
		showDecFuncList();
	}
}
// update add symbol
void ExtDecListAnalyze(struct node *parent, int num) {
	struct node *vardec = NULL;
	struct SymNode *symNode = NULL;

	vardec = parent->children[0];
	if (vardec == NULL) {
		fprintf(stderr, "ExtDecList's child VarDec is NULL\n");
		goto ExtDecListDebug;
	}
	parent->errorInfo = vardec->errorInfo;
	symNode = lookup(vardec->nodevalue.str);
	if (symNode) {
		SemanticError(3, parent->errorInfo);
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
}
/*
	Specifiers
*/
// update parent->type , Gspecifier
void SpecifierAnalyze(struct node *parent, int num) {
	//TYPE; StructSpecifier
	struct node *child = parent->children[0];
	if (child == NULL) {
		fprintf(stderr, "Specifier have no child!\n");
		parent->type = newType();
		parent->type->kind = ERROR;
		goto SpecifierDebug;
	}
	switch (child->nodetype) {
	case TYPE:
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
	}
}
// new struct type; add to symTable; update parent->type, parent->nodevalue.str, structdefnum--
void StructSpecifierAnalyze(struct node *parent, int num) {
	struct node *tag = NULL;
	struct node *deflist = NULL;
	struct SymNode *symNode = NULL;
	//finish define
	structdefnum--;
	tag = parent->children[1];
	if (tag == NULL) {
		parent->nodevalue.str = NULL;
	}else {
		parent->nodevalue.str = tag->nodevalue.str;
	}
	// STRUCT OptTag LC DefList RC
	if (num == 5) {
		parent->type = newType();
		parent->type->kind = STRUCTURE;
		deflist = parent->children[3];	
		if (deflist == NULL) {
			// DefList can be NULL!
			parent->type->structure = NULL;
		}else {
			parent->type->structure = deflist->fieldList;	
		}
		//only when OptTag has name,add it to symTable 
		if (parent->nodevalue.str != NULL) {
			symNode = newNewType(parent->nodevalue.str, parent->type, tag->errorInfo);
			if (symNode == NULL) {
				fprintf(stderr, "can't create symNode\n");
				goto StructSpecifierDebug;
			}
			insert(symNode);
			goto StructSpecifierDebug;
		}
	}
	// STRUCT Tag
	if (num == 2) {
		tag = parent->children[1];	// Tag
		if (tag == NULL) {
			fprintf(stderr, "StructSpecifier's child Tag is NULL!\n");
			parent->type = newType();
			parent->type->kind = ERROR;
			goto StructSpecifierDebug;
		}
		parent->type = tag->type;
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
}
// update parent->nodevalue.str, parent->errorInfo
void OptTagAnalyze(struct node *parent, int num) {
	// only when OptTag -> ID , call this function
	struct node *child = parent->children[0];
	struct SymNode *symNode = NULL;
	int totalErrorInfo = GetTotalErrorInfo();
	parent->errorInfo = GetErrorInfoByNum(IdErrorInfoStackHead, totalErrorInfo);
	if (child == NULL) {
		fprintf(stderr, "OptTag have no child\n");
		parent->nodevalue.str = NULL;
		goto OptTagDebug;
	}
	parent->nodevalue.str = child->nodevalue.str;
	symNode = lookup(parent->nodevalue.str); 
	if (symNode != NULL) {
		SemanticError(16, parent->errorInfo);
		goto OptTagDebug;
	}
OptTagDebug:
	if(debug2) {
		fprintf(stdout, "OptTag->nodevalue.str = %s\n", parent->nodevalue.str);
	}
}
//update parent->nodevalue.str, parent->type, parent->errorInfo
void TagAnalyze(struct node *parent, int num) {
	// ID
	struct node *child = parent->children[0];
	struct SymNode *symNode = NULL;
	int totalErrorInfo = GetTotalErrorInfo();
	// not define
	structdefnum--;
	if (child == NULL) {
		fprintf(stderr, "Tag have no child\n");
		parent->nodevalue.str = NULL;
		parent->type = newType();
		parent->type->kind = ERROR;
		goto TagDebug;
	}
	//TODO: just -1 may error
	parent->errorInfo = GetErrorInfoByNum(IdErrorInfoStackHead, totalErrorInfo - 1);
	parent->nodevalue.str = child->nodevalue.str;
	symNode = lookup(parent->nodevalue.str); 
	if (symNode == NULL) {
		SemanticError(17, parent->errorInfo);
		parent->type = newType();
		parent->type->kind = ERROR;
		goto TagDebug;
	}
	if (symNode->type != NewType) {
		SemanticError(16, parent->errorInfo);
		parent->type = newType();
		parent->type->kind = ERROR;
		goto TagDebug;
	}
	parent->type = symNode->specifier;
TagDebug:
	if(debug2) {
		fprintf(stdout, "Tag->nodevalue.str = %s\n", parent->nodevalue.str);
		fprintf(stdout, "Tag->type: \n");
		showType(parent->type);
		fprintf(stdout, "\n");
	}
}

/*
	Declarators
*/
// update parent->type, parent->nodevalue.str
void VarDecAnalyze(struct node *parent, int num) {
	struct node *id = NULL;
	struct node *vardec = NULL;
	struct node *index = NULL;
	int totalErrorInfo = GetTotalErrorInfo();
	// ID
	if (num == 1) {
		parent->errorInfo = GetErrorInfoByNum(IdErrorInfoStackHead, totalErrorInfo);
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
}
// update GFuncReturn, parent->errorInfo
void FunDecAnalyze(struct node *parent, int num) {
	struct node *id = NULL;
	struct node *varlist = NULL;
	struct SymNode *symNode = NULL;
	struct ErrorInfo *idErrorInfo = NULL;
	int totalErrorInfo = GetTotalErrorInfo();

	parent->errorInfo = GetErrorInfoByNum(IdErrorInfoStackHead, totalErrorInfo - varlistnum);
	id = parent->children[0];
	if (id == NULL) {
		fprintf(stderr, "FunDec's child ID is NULL\n");
		parent->nodevalue.str = NULL;
	}else {
		parent->nodevalue.str = id->nodevalue.str;
	}

	if (num == 3) {
		parent->fieldList = NULL;
	}else {
		varlist = parent->children[2];
		if (varlist == NULL) {
			fprintf(stderr, "FunDec's child VarList is NULL\n");
			parent->fieldList = NULL;
		}else {
			parent->fieldList = varlist->fieldList;
		}
	}

	varlistnum = 0;
	/*
	symNode = lookup(parent->nodevalue.str);
	if(symNode) {
		//may def may dec
//		SemanticError(4, idErrorInfo);
		symNode = newFunc(parent->nodevalue.str, GFuncReturn, parent->fieldList, parent->errorInfo);
	}else {
		symNode = newFunc(parent->nodevalue.str, GFuncReturn, parent->fieldList, parent->errorInfo);
		insert(symNode);
	}
	*/
	symNode = newFunc(parent->nodevalue.str, GFuncReturn, parent->fieldList, parent->errorInfo);
 	Funcsymbol = symNode;

FunDecDebug:
	if(debug2) {
		fprintf(stdout, "FunDec add new func symbol: \n");
		showSymbol(symNode);
		fprintf(stdout, "\n");
	}
}
// update parent->fieldList
void VarListAnalyze(struct node *parent, int num) {
	struct node *paramdec = NULL;
	struct node *varlist = NULL;
	FieldList fieldList = NULL;

	varlistnum++;
	paramdec = parent->children[0];
	if (paramdec == NULL) {
		fprintf(stderr, "VarList's child ParamDec is NULL\n");
		return;
	}

	if (num == 1) {
		fieldList = NULL;
	}else {
		varlist = parent->children[2];
		if (varlist == NULL) {
			fprintf(stderr, "VarList's child VarList is NULL\n");
			return;
		}
		fieldList = varlist->fieldList;
	}	
	parent->fieldList = newFieldList(paramdec->nodevalue.str, paramdec->type, fieldList);
	if(debug2) {
		fprintf(stdout, "VarList->fieldList : \n");
		showFieldList(parent->fieldList);
		fprintf(stdout, "\n");
	}
}
//update parent->type, parent->nodevalue.str, add to symbol table
void ParamDecAnalyze(struct node *parent, int num) {
	struct node *specifier = NULL;	
	struct node *varDec = NULL;	
	Type type, lastType;
	FieldList fieldList = NULL;
	struct SymNode *symNode = NULL;

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

	parent->type = varDec->type;
	parent->nodevalue.str = varDec->nodevalue.str;

	if(lookup(parent->nodevalue.str)) {
		SemanticError(3, parent->errorInfo);
	}else {
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
}

/*
	Statements
*/
void CompStAnalyze(struct node *parent, int num) {
}
void StmtListAnalyze(struct node *parent, int num) {
}
void StmtAnalyze(struct node *parent, int num) {
	struct node *exp = NULL;
	struct node *stmtType = NULL;
	stmtType = parent->children[0];
	if (stmtType == NULL) {
		fprintf(stderr, "Stmt's child is NULL\n");
		return;
	}
	//RETURN Exp SEMI
	if (stmtType->nodetype == RETURN) {
		exp = parent->children[1];
		if (exp == NULL) {
			fprintf(stderr, "Stmt's child Exp is NULL\n");
			return;
		}
		parent->errorInfo = exp->errorInfo;
		if (cmpType(GFuncReturn, exp->type)) {
			SemanticError(8, parent->errorInfo);
		}
	}
	// if; while
	if (stmtType->nodetype == IF || stmtType->nodetype == WHILE) {
		exp = parent->children[2];
		if (exp == NULL) {
			fprintf(stderr, "Stmt's child Exp is NULL\n");
			return;
		}
		if (exp->type->kind != BASIC || exp->type->basic != INT) {
			//fprintf(stderr, "assumption 2\n");
		}
	}
}
/*
	Local Definitions
*/
// update parent->fieldList 
void DefListAnalyze(struct node *parent, int num) {
	// Def DefList
	struct node *def = NULL;	
	struct node *deflist = NULL;	
	FieldList fieldList = NULL;

	def = parent->children[0];	// Def
	if (def == NULL) {
		fprintf(stderr, "DefList's child Def is NULL\n");
		goto DefListDebug;
	}
	parent->fieldList = def->fieldList;
	
	fieldList = parent->fieldList;
	if (fieldList == NULL) {
		fprintf(stderr, "DefList's child Def's fieldList is NULL\n");
		goto DefListDebug;
	}
	while(fieldList->tail != NULL) {
		fieldList = fieldList->tail;
	}
	//Maybe it is NULL ?
	deflist = parent->children[1];	// DefList
	if (deflist == NULL) {
		fieldList->tail = NULL;
	}else {
		// just link Def->tail = DefList
		fieldList->tail = deflist->fieldList;
	}
DefListDebug:
	if (debug2) {
		fprintf(stdout, "DefList->fieldList : \n");
		showFieldList(parent->fieldList);
		fprintf(stdout, "\n");
	}
}
// update parent->type, parent->fieldList, Gspecifier = NULL
void DefAnalyze(struct node *parent, int num) {
	// Specifier DefList SEMI
	struct node *specifier = NULL;	
	struct node *declist = NULL;	
	FieldList fieldList = NULL;
	struct SymNode *symNode = NULL;
	
//	freeErrorInfoStack(IdErrorInfoStackHead);
//	IdErrorInfoStackHead = NULL;
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
	specifierLock = 0;
DefDebug:
	if (debug2) {
		fprintf(stdout, "Def->type : \n");
		showType(parent->type);
		fprintf(stdout, "\n Def->fieldList : \n");
		showFieldList(parent->fieldList);
		fprintf(stdout, "\n");
	}
}
//update parent->fieldList 
void DecListAnalyze(struct node *parent, int num) {
	struct node *dec = NULL;
	struct node *declist = NULL;
	FieldList fieldList = NULL;

	dec = parent->children[0];	//Dec
	if (dec == NULL) {
		fprintf(stderr, "DecList's child Dec is NULL\n");
		goto DecListDebug;
	}
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
	}
	parent->fieldList = newFieldList(dec->nodevalue.str, dec->type, fieldList);	
DecListDebug:
	if (debug2) {
		fprintf(stdout, "DecList->fieldList :\n");
		showFieldList(parent->fieldList);
		fprintf(stdout, "\n");
	}
}
// update parent->nodevalue.str, parent->type, add symbol
void DecAnalyze(struct node *parent, int num) {
	struct node *vardec = NULL;
	struct node *exp = NULL;
	struct SymNode *symNode = NULL;

	// VarDec
	vardec = parent->children[0];	//VarDec
	if (vardec == NULL) {
		fprintf(stderr, "Dec's child VarDef is NULL\n");
		goto DecDebug;
	}
	parent->errorInfo = vardec->errorInfo;

	parent->nodevalue.str = vardec->nodevalue.str;
	parent->type = vardec->type;

	symNode = lookup(parent->nodevalue.str);
	if (symNode) {
		if (structdefnum > 0)
			SemanticError(15, parent->errorInfo);
		else
			SemanticError(3, parent->errorInfo);
	}else {
		symNode = newVar(parent->nodevalue.str, parent->type, parent->errorInfo);
		insert(symNode);
	}
	// VarDec ASSIGNOP Exp
	if (num == 3) {
		if (structdefnum > 0) {
			SemanticError(15, parent->errorInfo);
			goto DecDebug;
		}
		exp = parent->children[2];
		if (exp == NULL) {
			fprintf(stderr, "Dec's child Exp is NULL\n");
			goto DecDebug;
		}
		if (cmpType(vardec->type, exp->type)) {
			SemanticError(5, parent->errorInfo);
		}
	}
DecDebug:
	if (debug2) {
		fprintf(stdout, "Dec->nodevalue.str = %s\n", parent->nodevalue.str);
		fprintf(stdout, "Dec->type : \n");
		showType(parent->type);
		fprintf(stdout, "\nDec add new Var symbol: \n");
		showSymbol(symNode);
		fprintf(stdout, "\n");
	}
}

/*
	Expressions
*/
// update parent->nodevalue.str, parent->type
void ExpAnalyze(struct node *parent, int num) {
	struct node *childleft, *childright, *childmid;
	struct SymNode *symNode = NULL;
	struct ErrorInfo *idErrorInfo = NULL;
	int totalErrorInfo = GetTotalErrorInfo();
	childleft = parent->children[0];
	if (childleft == NULL) {
		fprintf(stderr, "Exp's child Exp is NULL\n");
		return;
	}
	// ID INT FLOAT
	if (num == 1) {
		switch(childleft->nodetype) {
		case ID:
			symNode = lookup(childleft->nodevalue.str);
			parent->errorInfo = GetErrorInfoByNum(IdErrorInfoStackHead, totalErrorInfo);
			if (symNode == NULL) {
				SemanticError(1, parent->errorInfo);
				parent->type = newType();
				parent->type->kind = ERROR;
			}else {
				parent->type = getSymType(symNode);
			}
			parent->nodevalue.str = childleft->nodevalue.str;
			break;
		case INT:
			parent->errorInfo = GetErrorInfoByNum(NumErrorInfoStackHead, totalErrorInfo);
			parent->type = newType();
			parent->type->kind = BASIC;
			parent->type->basic = INT;
			break;
		case FLOAT:
			parent->errorInfo = GetErrorInfoByNum(NumErrorInfoStackHead, totalErrorInfo);
			parent->type = newType();
			parent->type->kind = BASIC;
			parent->type->basic = FLOAT;
			break;
		}
		goto ExpDebug;
	}
	// MINUS Exp; NOT Exp
	if (num == 2) {
		childleft = parent->children[1];
		if (childleft == NULL) {
			fprintf(stderr, "Exp's child Exp is NULL\n");
			return;
		}
		parent->errorInfo = childleft->errorInfo;
		parent->type = childleft->type;
		goto ExpDebug;
	}
	childright = parent->children[2];
	if (childright == NULL) {
		fprintf(stderr, "Exp's child Exp is NULL\n");
		return;
	}
	childmid = parent->children[1];
	if (childmid == NULL) {
		fprintf(stderr, "Exp's child mid is NULL\n");
		return;
	}
	if (num == 3) {
		// LP Exp RP
		if (childleft->nodetype == LP && childright->nodetype == RP) {
			parent->errorInfo = childmid->errorInfo;
			parent->type = childmid->type;
			goto ExpDebug;
		}
		// ID LP RP
		if (childleft->nodetype == ID) {
			idErrorInfo = GetErrorInfoByNum(IdErrorInfoStackHead, totalErrorInfo - argsnum);
			argsnum = 0;
			parent->errorInfo = idErrorInfo;
			symNode = lookup(childleft->nodevalue.str);
			if (symNode == NULL || symNode->type != Func) {
				SemanticError(2, parent->errorInfo);
			}
			if (cmpFieldList(symNode->func->argtype, NULL)) {
				SemanticError(9, parent->errorInfo);
			}
			parent->type = symNode->func->Return;
			goto ExpDebug;
		}
		// Exp DOT ID
		if (childright->nodetype == ID) {
			parent->errorInfo = childleft->errorInfo;
			if(childleft->type->kind != STRUCTURE){
				SemanticError(13, parent->errorInfo);
				parent->type = newType();
				parent->type->kind = ERROR;
				goto ExpDebug;
			}
			parent->errorInfo = GetErrorInfoByNum(IdErrorInfoStackHead, totalErrorInfo);
			parent->type = lookupFieldListElem(childleft->type->structure, childright->nodevalue.str);
			if (parent->type == NULL) {
			//	parent->errorInfo = GerrorInfo;
				SemanticError(14, parent->errorInfo);
				parent->type = newType();
				parent->type->kind = ERROR;
			}else {
				parent->nodevalue.str = childright->nodevalue.str;
			}
			goto ExpDebug;
		}
		// Exp ASSIGNOP Exp
		if (childmid->nodetype == ASSIGNOP) {
			parent->errorInfo = childleft->errorInfo;
			if (childleft->type->kind == ERROR)
				goto ExpDebug;
			if(cmpType(childleft->type, childright->type))
				SemanticError(5, parent->errorInfo);
			if(!childleft->nodevalue.str)
				SemanticError(6, parent->errorInfo);
			goto ExpDebug;
		}
		// other Exp
		parent->errorInfo = childleft->errorInfo;
		if(cmpType(childleft->type, childright->type))
			SemanticError(7, parent->errorInfo);
		//TODO: just take left type maybe error
		parent->type = childleft->type;
		goto ExpDebug;
	}
	//ID LP Args RP; Exp LB Exp RB
	if (num == 4) {
		// ID LP Args RP
		if(childleft->nodetype == ID) {
			idErrorInfo = GetErrorInfoByNum(IdErrorInfoStackHead, totalErrorInfo - argsnum);
			argsnum = 0;
			parent->errorInfo = idErrorInfo;
			symNode = lookup(childleft->nodevalue.str);
			if (symNode == NULL) {
				SemanticError(2, parent->errorInfo);
				goto ExpDebug;
			}
			if (symNode->type != Func) {
				SemanticError(11, parent->errorInfo);
				goto ExpDebug;
			}
			if (cmpFieldList(symNode->func->argtype, childright->fieldList)) {
				SemanticError(9, parent->errorInfo);
			}
			parent->type = symNode->func->Return;
			goto ExpDebug;
		}	
		// Exp LB Exp RB
		parent->errorInfo = childleft->errorInfo;
		if(childleft->type->kind != ARRAY){
			SemanticError(10, parent->errorInfo);
			parent->type = childleft->type;
			goto ExpDebug;
		}
		if(childright->type->kind != BASIC || childright->type->basic != INT) {
			SemanticError(12, childright->errorInfo);
		}
		parent->type = childleft->type->array.elem;
		parent->nodevalue.str = childleft->nodevalue.str;
		goto ExpDebug;
	}
ExpDebug:
	if(debug2) {
		fprintf(stdout, "Exp: ");
		ShowErrorInfo(parent->errorInfo);
		fprintf(stdout, "Exp->type: \n");
		showType(parent->type);
		fprintf(stdout, "\n");
	}
}
// update parent->fieldList 
void ArgsAnalyze(struct node *parent, int num) {
	struct node *exp = NULL;
	struct node *args = NULL;
	FieldList fieldList = NULL;
	int totalErrorInfo = GetTotalErrorInfo();

	argsnum++;
	parent->errorInfo = GetErrorInfoByNum(IdErrorInfoStackHead, totalErrorInfo);
	// Exp
	if (num == 1) {
		fieldList = NULL;
	}
	// Exp COMMA Args
	if (num == 3) {
		args = parent->children[2];	
		if (args == NULL) {
			fprintf(stderr, "Args's child Args is NULL\n");
		}else {
			fieldList = args->fieldList;
		}
	}
	exp = parent->children[0];
	if (exp == NULL) {
		fprintf(stderr, "Args's child Exp is NULL\n");
		parent->fieldList = fieldList;
	}else {
		//parent->fieldList = newFieldList(exp->nodevalue.str, exp->type, fieldList);	
		parent->fieldList = newFieldList(" ", exp->type, fieldList);	
	}
ArgsDebug:
	if (debug2) {
		fprintf(stdout, "Args->fieldList :\n");
		showFieldList(parent->fieldList);
		fprintf(stdout, "\n");
	}
}

void semanticAnalyze(struct node *parent, int num) {
	switch (parent->nodetype) {
	case Program:		ProgramAnalyze(parent, num);		break;
	case ExtDefList:	ExtDefListAnalyze(parent, num);		break;
	case ExtDef:		ExtDefAnalyze(parent, num);		break;
	case ExtDecList:	ExtDecListAnalyze(parent, num);		break;
	case Specifier:		SpecifierAnalyze(parent, num); 		break;
	case StructSpecifier:	StructSpecifierAnalyze(parent, num);	break;
	case OptTag:		OptTagAnalyze(parent, num);		break;
	case Tag:		TagAnalyze(parent, num);		break;
	case VarDec:		VarDecAnalyze(parent, num);		break;
	case FunDec:		FunDecAnalyze(parent, num);		break;
	case VarList:		VarListAnalyze(parent, num);		break;
	case ParamDec:		ParamDecAnalyze(parent, num);		break;
	case CompSt:		CompStAnalyze(parent, num);		break;
	case StmtList:		StmtListAnalyze(parent, num);		break;
	case Stmt:		StmtAnalyze(parent, num);		break; 
	case DefList:		DefListAnalyze(parent, num);		break;
	case Def:		DefAnalyze(parent, num);		break;
	case DecList:		DecListAnalyze(parent, num);		break;
	case Dec:		DecAnalyze(parent, num);		break;
	case Exp:		ExpAnalyze(parent, num);		break;
	case Args:		ArgsAnalyze(parent, num);		break;
	}
}
