#include "semantic.h"
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

int cmpType(Type type1, Type type2); 
void SetSpecifier(Type Specifier, Type *type); 

/*
	compare two FieldList
	return 0 if the same
	       1 if different
*/
int cmpFieldList(FieldList fieldList1, FieldList fieldList2) {
	if(fieldList1 == NULL && fieldList2 == NULL)
		return 0;
	if(fieldList1 == NULL || fieldList2 == NULL)
		return 1;
	if(!strcmp(fieldList1->name, fieldList2->name))
		return 1;
	if(!cmpType(fieldList1->type, fieldList2->type))
		return 1;
	return cmpFieldList(fieldList1->tail, fieldList2->tail);
}
/*
	compare two Type
	return 0 if the same
	       1 if different
*/
int cmpType(Type type1, Type type2) {
	if(type1 == NULL && type2 == NULL)
		return 0;
	if(type1 == NULL || type2 == NULL)
		return 1;
	if(type1->kind == BASIC && type2->kind == BASIC) {
		if(type1->basic == type2->basic)
			return 0;
	}
	if(type1->kind == ARRAY && type2->kind == ARRAY) {
		//don't compare the array size
		// just compare the dimension
		return cmpType(type1->array.elem, type2->array.elem);
	}
	if(type1->kind == STRUCTURE && type2->kind == STRUCTURE) {
		return cmpFieldList(type1->structure, type2->structure);
	}
}
void SetFieldListSpecifier(Type Specifier, FieldList *fieldList) {
	if(fieldList == NULL) {
		return;
	}
	SetSpecifier(Specifier, &((*fieldList)->type));
	SetFieldListSpecifier(Specifier, &((*fieldList)->tail));
}
/*
	SetSpecifier
	for
	ParamDec -> Specifier VarDec
	Def	-> Specifier DecList SEMI   
*/
void SetSpecifier(Type Specifier, Type *type) {
	if(type == NULL) {
		fprintf(stderr, "error at SetSpecifier, type is NULL\n");
		return;
	}	
	if(*type == NULL) {
		*type = Specifier;
		return;
	}
	if((*type)->kind == BASIC) {
		if(Specifier->kind != BASIC) {
			//Maybe error
			SemanticError(5, 0);			
		}
	}
	if((*type)->kind == STRUCTURE) {
		SetFieldListSpecifier(Specifier, &((*type)->structure));
	}
	if((*type)->kind == ARRAY) {
		
	}
}
/*
	High-level Definitions
*/
void ProgramAnalyze(struct node *parent, int num) {
}
void ExtDefListAnalyze(struct node *parent, int num) {
}
// update parent->type. add symbol
void ExtDefAnalyze(struct node *parent, int num) {
	struct node *specifier = NULL;
	struct node *midchild = NULL;
	struct SymNode *symNode = NULL;
	FieldList fieldList = NULL;

	specifier = parent->children[0];
	if (specifier == NULL) {
		fprintf(stderr, "ExtDef's child Specifier is NULL\n");
		return;
	}
	parent->type = specifier->type;
	if (num == 3) {
		midchild = parent->children[1];
		if (midchild == NULL) {
			fprintf(stderr, "ExtDef's child is NULL\n");
			return;
		}
		if (parent->children[2]->nodetype == SEMI) {
			//Specifier ExtDecList SEMI
			parent->fieldList = midchild->fieldList;

			// update all the fieldList
			fieldList = parent->fieldList;
			while (fieldList != NULL) {
				//TODO: fieldList->type is array should update
				symNode = newVar(fieldList->name, fieldList->type);
				if (!lookup(fieldList->name)) {
					SemanticError(3, parent->lineno);
				}else {
					insert(symNode);
				}
				if (debug2) {
					fprintf(stdout, "ExtDef add new symbol : \n");
					showSymbol(symNode);
					fprintf(stdout, "\n");
				}
				fieldList = fieldList->tail;
			}
		}else{
			// Specifier FunDec CompSt
			symNode = newFunc(midchild->nodevalue.str, parent->type, midchild->fieldList);
			if(debug2) {
				fprintf(stdout, "ExtDef add new symbol : \n");
			}
			if (lookup(symNode->name)) {
				SemanticError(3, parent->lineno);
			}else {
				insert(symNode);
			}
			if (debug2) {
				showSymbol(symNode);
				fprintf(stdout, "\n");
			}
		}
	}
	if(debug2) {
		fprintf(stdout, "ExtDef->type: \n");
		showType(parent->type);
		fprintf(stdout, "\n");
	}
}
// update parent->fieldList
void ExtDecListAnalyze(struct node *parent, int num) {
	struct node *vardec = NULL;
	struct node *extdeclist = NULL;
	FieldList fieldList = NULL;

	vardec = parent->children[0];
	if (vardec == NULL) {
		fprintf(stderr, "ExtDecList's child VarDec is NULL\n");
		return;
	}
	if (num == 1) {
		fieldList = NULL;
	}else {
		extdeclist = parent->children[2];
		if (extdeclist == NULL) {
			fprintf(stderr, "ExtDecList's child ExtDecList is NULL\n");
			return;
		}
		fieldList = extdeclist->fieldList;
	}
	parent->fieldList = newFieldList(vardec->nodevalue.str, vardec->type, fieldList);
	if(debug2) {
		fprintf(stdout, "ExtDecList->fieldList: \n");
		showFieldList(parent->fieldList);
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
		//maybe need change parent->type to error resume
		return;
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
		//maybe need change parent->type to error resume
		break;
	}
	Gspecifier = parent->type;
	if(debug2) {
		fprintf(stdout, "Specifier->type : \n");
		showType(parent->type);
		fprintf(stdout, "\n");
		fprintf(stdout, "Gspecifier: \n");
		showType(Gspecifier);
		fprintf(stdout, "\n");
	}
}
// new struct type; add to symTable; update parent->type
void StructSpecifierAnalyze(struct node *parent, int num) {
	struct node *child;
	struct SymNode *symNode = NULL;
	// STRUCT OptTag LC DefList RC
	if (num == 5) {
		parent->type = newType();
		parent->type->kind = STRUCTURE;
		child = parent->children[3];	// DefList
		if (child == NULL) {
			// DefList can be NULL!
//			fprintf(stderr, "StructSpecifier's child DefList is NULL!\n");
			parent->type->structure = NULL;
		}else {
			parent->type->structure = child->fieldList;	
		}
		child = parent->children[1];	//OptTag
		if (child == NULL) {
			// OptTag can be NULL!
			// no need to add it to symbol table
			return;
		}
		symNode = newNewType(child->nodevalue.str, parent->type);
		if (symNode == NULL) {
			fprintf(stderr, "can't create symNode\n");
			return;
		}
		if (insert(symNode)) {
			fprintf(stderr, "insert symNode error\n");
			return;
		}
	}
	// STRUCT Tag
	if (num == 2) {
		child = parent->children[1];	// Tag
		if (child == NULL) {
			fprintf(stderr, "StructSpecifier's child Tag is NULL!\n");
			//maybe need change parent->type to error resume
			return;
		}
		symNode = lookup(child->nodevalue.str);	
		if (symNode == NULL) {
			SemanticError(17, parent->lineno);
//			fprintf(stderr, "error no define struct %s\n", child->nodevalue.str); 
			//maybe need change parent->type to error resume
			return;
		}
		parent->type = symNode->specifier;
	}
	if(debug2) {
		fprintf(stdout, "StructSpecifierAnalyze->type : \n");
		showType(parent->type);
		fprintf(stdout, "\n symNode: \n");
		showSymbol(symNode);
		fprintf(stdout, "\n");
	}
}
// update parent->nodevalue.str
void OptTagAnalyze(struct node *parent, int num) {
	// only when OptTag -> ID , call this function
	// ID
	struct node *child = parent->children[0];
	if (child == NULL) {
		fprintf(stderr, "OptTag have no child\n");
		//maybe need change parent->type to error resume
		return;
	}
	parent->nodevalue.str = child->nodevalue.str;
	if(debug2) {
		fprintf(stdout, "OptTagAnalyze->nodevalue.str = %s\n", parent->nodevalue.str);
	}
}
//update parent->nodevalue.str
void TagAnalyze(struct node *parent, int num) {
	// ID
	struct node *child = parent->children[0];
	if (child == NULL) {
		fprintf(stderr, "Tag have no child\n");
		return;
	}
	parent->nodevalue.str = child->nodevalue.str;
	if(debug2) {
		fprintf(stdout, "TagAnalyze->nodevalue.str = %s\n", parent->nodevalue.str);
	}
}

/*
	Declarators
*/
// update parent->type, parent->nodevalue.str
void VarDecAnalyze(struct node *parent, int num) {
	struct node *child = NULL;
	// ID
	if (num == 1) {
		child = parent->children[0];	// ID
		if (child == NULL) {
			fprintf(stderr, "VarDec's child ID is NULL\n");
			return;
		}
		parent->nodevalue.str = child->nodevalue.str;	
		// we can't get the ID type
		// this type should update later
		parent->type = NULL;
	}
	// VarDec LB INT RB
	if (num == 4) {
		child = parent->children[0];	// VarDec
		if (child == NULL) {
			fprintf(stderr, "VarDec's child VarDec is NULL\n");
			return;
		}
		parent->nodevalue.str = child->nodevalue.str;	
		parent->type = newType();
		parent->type->kind = ARRAY;
		parent->type->array.elem = child->type;
		child = parent->children[2];	// INT
		if (child == NULL) {
			fprintf(stderr, "VarDec's child INT is NULL\n");
			return;
		}
		parent->type->array.size = child->nodevalue.INT;
	}
	if(debug2) {
		fprintf(stdout, "VarDec->nodevalue.str = %s\n", parent->nodevalue.str);
		fprintf(stdout, "VarDec->type: \n");
		showType(parent->type);
		fprintf(stdout, "\n");
	}
}
// update parent->fieldList, parent->nodevalue.str, GFuncReturn
void FunDecAnalyze(struct node *parent, int num) {
	struct node *id = NULL;
	struct node *varlist = NULL;

	id = parent->children[0];
	if (id == NULL) {
		fprintf(stderr, "FunDec's child ID is NULL\n");
		return;
	}
	parent->nodevalue.str = id->nodevalue.str;

	if (num == 1) {
		parent->fieldList = NULL;
	}else {
		varlist = parent->children[2];
		if (varlist == NULL) {
			fprintf(stderr, "FunDec's child VarList is NULL\n");
			return;
		}
		parent->fieldList = varlist->fieldList;
	}
	GFuncReturn = Gspecifier;
	if(debug2) {
		fprintf(stdout, "FunDec->nodevalue.str = %s \n", parent->nodevalue.str);
		fprintf(stdout, "FunDec->fieldList : \n");
		showFieldList(parent->fieldList);
		fprintf(stdout, "\n");
		fprintf(stdout, "GFuncReturn : \n");
		showType(GFuncReturn);
		fprintf(stdout, "\n");
	}
	
}
// update parent->fieldList
void VarListAnalyze(struct node *parent, int num) {
	struct node *paramdec = NULL;
	struct node *varlist = NULL;
	FieldList fieldList = NULL;

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
		return;
	}
	if (varDec == NULL) {
		fprintf(stderr, "ParamDec's child VarDec is NULL\n");
		return;
	}

	type = varDec->type;
	if (type == NULL) {
		//just ID
		varDec->type = specifier->type;
	}else {
		//array
		while (type->array.elem != NULL) {
			type = type->array.elem;
		}
		type->array.elem = specifier->type;
	}

	parent->type = varDec->type;
	parent->nodevalue.str = varDec->nodevalue.str;

	if(!lookup(parent->nodevalue.str)) {
		SemanticError(3, parent->lineno);
	}else {
		symNode = newVar(parent->nodevalue.str, parent->type);
		if (insert(symNode)) {
			fprintf(stderr, "can't insert symNode\n");
		}
	}
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
/*
	struct node *stmtlist = NULL;
	stmtlist = parent->children[2];
	if (stmtlist == NULL) {
		fprintf(stderr, "CompSt's child StmtList is NULL\n");
		parent->returntype = newType();
		parent->returntype->kind = ERROR;
		return;
	}
	parent->returntype = stmtlist->returntype;
	if(debug2) {
		fprintf(stdout, "CompSt->returntype: \n");
		showType(parent->returntype);
		fprintf(stdout, "\n");
	}
*/
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
		if (!cmpType(GFuncReturn, exp->type)) {
			SemanticError(8, parent->lineno);
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
			fprintf(stderr, "assumption 2\n");
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
		return;
	}
	parent->fieldList = def->fieldList;
	
	fieldList = parent->fieldList;
	if (fieldList == NULL) {
		fprintf(stderr, "DefList's child Def's fieldList is NULL\n");
		return;
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
	if (debug2) {
		fprintf(stdout, "DefList->fieldList : \n");
		showFieldList(parent->fieldList);
		fprintf(stdout, "\n");
	}
}
// update parent->type, parent->fieldList, add new symbol
void DefAnalyze(struct node *parent, int num) {
	// Specifier DefList SEMI
	struct node *specifier = NULL;	
	struct node *declist = NULL;	
	FieldList fieldList = NULL;
	struct SymNode *symNode = NULL;
	
	specifier = parent->children[0];	// Specifier
	if (specifier == NULL) {
		fprintf(stderr, "Def's child Specifier is NULL\n");
		return;
	}
	parent->type = specifier->type;

	declist = parent->children[1];	// DecList
	if (declist == NULL) {
		fprintf(stderr, "Def's child DecList is NULL\n");
		return;
	}
	parent->fieldList = declist->fieldList;

	if (debug2) {
		fprintf(stdout, "Def add new symbol : \n");
	}
	// update all the fieldList
	fieldList = parent->fieldList;
	while (fieldList != NULL) {
		//TODO: fieldList->type is array should update
		symNode = newVar(fieldList->name, fieldList->type);
		if (lookup(fieldList->name)) {
			SemanticError(3, parent->lineno);
		}else {
			insert(symNode);
		}
		if (debug2) {
			showSymbol(symNode);
			fprintf(stdout, "\n");
		}
		fieldList = fieldList->tail;
	}
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
		return;
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
			return;
		}
		fieldList = declist->fieldList;
	}
	parent->fieldList = newFieldList(dec->nodevalue.str, dec->type, fieldList);	

	if (debug2) {
		fprintf(stdout, "DecList->fieldList :\n");
		showFieldList(parent->fieldList);
		fprintf(stdout, "\n");
	}
}
// update parent->nodevalue.str, parent->type
void DecAnalyze(struct node *parent, int num) {
	struct node *child = NULL;
	struct node *exp = NULL;

	// VarDec
	child = parent->children[0];	//VarDec
	if (child == NULL) {
		fprintf(stderr, "Dec's child VarDef is NULL\n");
		return;
	}
	parent->nodevalue.str = child->nodevalue.str;
	parent->type = child->type;
	// VarDec ASSIGNOP Exp
	if (num == 3) {
		//TODO: check VarDec type and Exp type
		//	but don't know VarDec type..
		//	can let VarDec type = Exp type temp. let Def to exam the type
		exp = parent->children[2];	//Exp
		if (exp == NULL) {
			fprintf(stderr, "Dec's child Exp is NULL\n");
			return;
		}
		/*
		if (!cmpType(parent->type, exp->type)) {
			SemanticError(5, parent->lineno);
		}
		*/
		//TODO: error 6 check VarDec can have value;
	}
	if (debug2) {
		fprintf(stdout, "Dec->nodevalue.str = %s\n", parent->nodevalue.str);
		fprintf(stdout, "Dec->type : \n");
		showType(parent->type);
		fprintf(stdout, "\n");
	}
}

/*
	Expressions
*/
// update parent->nodevalue.str, parent->type
void ExpAnalyze(struct node *parent, int num) {
	struct node *childleft, *childright;
	struct SymNode *symNode = NULL;
	childleft = parent->children[0];
	if (childleft == NULL) {
		fprintf(stderr, "Exp's child Exp is NULL\n");
		return;
	}
	// ID INT FLOAT
	if (num == 1) {
		switch(childleft->nodetype) {
		case ID:
			parent->type = getIDType(Var, childleft->nodevalue.str);
			parent->nodevalue.str = childleft->nodevalue.str;
			break;
		case INT:
			parent->type = newType();
			parent->type->kind = BASIC;
			parent->type->basic = INT;
			break;
		case FLOAT:
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
		parent->type = childleft->type;
		goto ExpDebug;
	}
	childright = parent->children[2];
	if (childright == NULL) {
		fprintf(stderr, "Exp's child Exp is NULL\n");
		return;
	}
	if (num == 3) {
		// LP Exp RP
		if (childleft->nodetype == LP && childright->nodetype == RP) {
			parent->type = parent->children[1]->type;
			goto ExpDebug;
		}
		// ID LP RP
		if (childleft->nodetype == ID) {
			symNode = lookup(childleft->nodevalue.str);
			if (symNode == NULL || symNode->type != Func) {
				SemanticError(2, parent->lineno);
			}
			if (!cmpFieldList(symNode->func->argtype, NULL)) {
				SemanticError(9, parent->lineno);
			}
			parent->type = symNode->func->Return;
			goto ExpDebug;
		}
		// Exp DOT ID
		if (childright->nodetype == ID) {
			if(childleft->type->kind != STRUCTURE){
				SemanticError(13, parent->lineno);
				parent->type = newType();
				parent->type->kind = ERROR;
				goto ExpDebug;
			}
			parent->type = lookupFieldListElem(childleft->type->structure, childright->nodevalue.str);
			if (parent->type == NULL) {
				SemanticError(14, parent->lineno);
				parent->type = newType();
				parent->type->kind = ERROR;
			}
			goto ExpDebug;
		}
		// other Exp
		if(!cmpType(childleft->type, childright->type))
			SemanticError(7, parent->lineno);
		//TODO: just take left type maybe error
		parent->type = childleft->type;
		goto ExpDebug;
	}
	//ID LP Args RP; Exp LB Exp RB
	if (num == 4) {
		// ID LP Args RP
		if(childleft->nodetype == ID) {
			symNode = lookup(childleft->nodevalue.str);
			if (symNode == NULL) {
				SemanticError(2, parent->lineno);
			}
			if (symNode->type != Func) {
				SemanticError(11, parent->lineno);
			}
			if (!cmpFieldList(symNode->func->argtype, childright->fieldList)) {
				SemanticError(9, parent->lineno);
			}
			parent->type = symNode->func->Return;
			goto ExpDebug;
		}	
		// Exp LB Exp RB
		if(childleft->type->kind != ARRAY){
			SemanticError(10, parent->lineno);
			parent->type = childleft->type;
			goto ExpDebug;
		}
		if(childright->type->kind != BASIC || childright->type->basic != INT) {
			SemanticError(12, parent->lineno);
		}
		parent->type = childleft->type->array.elem;
		goto ExpDebug;
	}
ExpDebug:
	if(debug2) {
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

	exp = parent->children[0];
	if (exp == NULL) {
		fprintf(stderr, "Args's child Exp is NULL\n");
		return;
	}
	// Exp
	if (num == 1) {
		fieldList = NULL;
	}
	// Exp COMMA Args
	if (num == 3) {
		args = parent->children[2];	
		if (args == NULL) {
			fprintf(stderr, "Args's child Args is NULL\n");
			return;
		}
		fieldList = args->fieldList;
	}
	parent->fieldList = newFieldList(exp->nodevalue.str, exp->type, fieldList);	

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
