#include "semantic.h"
#include "name.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
			SemanticError(5);			
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
void ExtDefAnalyze(struct node *parent, int num) {
	
}
void ExtDecListAnalyze(struct node *parent, int num) {
}
/*
	Specifiers
*/
void SpecifierAnalyze(struct node *parent, int num) {
	//only have one child
	struct node *child = parent->children[0];
	if (child == NULL) {
		fprintf(stderr, "Specifier have no child!\n");
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
		break;
	}
	
}
void StructSpecifierAnalyze(struct node *parent, int num) {
	struct node *child;
	struct SymNode *symNode = NULL;
	// STRUCT OptTag LC DefList RC
	if (num == 5) {
		child = parent->children[3];	// DefList
		parent->type = newType();
		parent->type->kind = STRUCTURE;
		parent->type->structure = child->fieldList;	
		child = parent->children[1];	//OptTag
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
		symNode = lookup(child->nodevalue.str);	
		if (symNode == NULL) {
			fprintf(stderr, "error no define struct %s\n", child->nodevalue.str); 
			return;
		}
		parent->type = symNode->specifier;
	}
}
void OptTagAnalyze(struct node *parent, int num) {
	// ID
	struct node *child = parent->children[0];
	if (child == NULL) {
		fprintf(stderr, "OptTag have no child\n");
		return;
	}
	parent->nodevalue.str = child->nodevalue.str;
}
void TagAnalyze(struct node *parent, int num) {
	// ID
	struct node *child = parent->children[0];
	if (child == NULL) {
		fprintf(stderr, "Tag have no child\n");
		return;
	}
	parent->nodevalue.str = child->nodevalue.str;
}

/*
	Declarators
*/
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
}
void FunDecAnalyze(struct node *parent, int num) {
	
}
void VarListAnalyze(struct node *parent, int num) {
}
void ParamDecAnalyze(struct node *parent, int num) {
	struct node *child = NULL;	
	Type type, lastType;
	FieldList fieldList = NULL;
	struct SymNode *symNode = NULL;
	child = parent->children[0];	// Specifier
	if (child == NULL) {
		fprintf(stderr, "Def's child Specifier is NULL\n");
		return;
	}
	parent->type = child->type;	
	child = parent->children[1];	// VarDec
	if (child == NULL) {
		fprintf(stderr, "Def's child DecList is NULL\n");
		return;
	}
	/*
	symNode = newVar(fieldList->name, parent->type);
	if (insert(symNode)) {
		fprintf(stderr, "can't insert symNode\n");
	}
	if (debug2 == 1) {
		showSymbol(symNode);
	}
	*/
}

/*
	Statements
*/
void CompStAnalyze(struct node *parent, int num) {
}
void StmtListAnalyze(struct node *parent, int num) {
}
void StmtAnalyze(struct node *parent, int num) {
}
/*
	Local Definitions
*/
void DefListAnalyze(struct node *parent, int num) {
	// Def DefList
	struct node *child = NULL;	
	child = parent->children[0];	// Def
	if (child == NULL) {
		fprintf(stderr, "DefList's child Def is NULL\n");
		return;
	}
	parent->fieldList = child->fieldList;
	//Maybe it is NULL ?
	child = parent->children[1];	// DefList
	if (child == NULL) {
		parent->fieldList->tail = NULL;
	}else {
		// just link Def->tail = DefList
		parent->fieldList->tail = child->fieldList;
	}
}
void DefAnalyze(struct node *parent, int num) {
	// Specifier DefList SEMI
	struct node *child = NULL;	
	FieldList fieldList = NULL;
	struct SymNode *symNode = NULL;
	child = parent->children[0];	// Specifier
	if (child == NULL) {
		fprintf(stderr, "Def's child Specifier is NULL\n");
		return;
	}
	parent->type = child->type;	
	child = parent->children[1];	// DecList
	if (child == NULL) {
		fprintf(stderr, "Def's child DecList is NULL\n");
		return;
	}
	parent->fieldList = child->fieldList;
	// update all the fieldList
	fieldList = child->fieldList;
	while (fieldList != NULL) {
//		fieldList->type = parent->type;
		symNode = newVar(fieldList->name, fieldList->type);
		if (insert(symNode)) {
			fprintf(stderr, "can't insert symNode\n");
		}
		if (debug2 == 1) {
			showSymbol(symNode);
		}
		fieldList = fieldList->tail;
	}
}
void DecListAnalyze(struct node *parent, int num) {
	struct node *child = NULL;
	FieldList fieldList = NULL;
	// Dec
	if (num == 1) {
		child = parent->children[0];	//Dec
		if (child == NULL) {
			fprintf(stderr, "DecList's child Dec is NULL\n");
			return;
		}
		parent->fieldList = newFieldList(child->nodevalue.str, child->type, NULL);	
	}
	// Dec COMMA DecList
	if (num == 3) {
		child = parent->children[2];	// DecList
		if (child == NULL) {
			fprintf(stderr, "DecList's child Dec is NULL\n");
			return;
		}
		fieldList = child->fieldList;
		child = parent->children[0];	//Dec
		if (child == NULL) {
			fprintf(stderr, "DecList's child Dec is NULL\n");
			return;
		}
		parent->fieldList = newFieldList(child->nodevalue.str, child->type, fieldList);
	}
}
void DecAnalyze(struct node *parent, int num) {
	struct node *child = NULL;
	// VarDec
	if (num == 1) {
		child = parent->children[0];	//VarDec
		if (child == NULL) {
			fprintf(stderr, "Dec's child VarDef is NULL\n");
			return;
		}
		parent->nodevalue.str = child->nodevalue.str;
		parent->type = child->type;
	}
	// VarDec ASSIGNOP Exp
	if (num == 3) {
		//TODO: check VarDec type and Exp type
		//	but don't know VarDec type..
		//	can let VarDec type = Exp type temp. let Def to exam the type
		child = parent->children[0];	//VarDec
		if (child == NULL) {
			fprintf(stderr, "Dec's child VarDef is NULL\n");
			return;
		}
		parent->nodevalue.str = child->nodevalue.str;
		parent->type = child->type;
	}
}

/*
	Expressions
*/
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
		return;
	}
	// MINUS Exp; NOT Exp
	if (num == 2) {
		childleft = parent->children[1];
		if (childleft == NULL) {
			fprintf(stderr, "Exp's child Exp is NULL\n");
			return;
		}
		parent->type = childleft->type;
		return;
	}
	childright = parent->children[2];
	if (childright == NULL) {
		fprintf(stderr, "Exp's child Exp is NULL\n");
		return;
	}
	if (num == 3) {
		// LP Exp RP
		if (childleft->nodetype == LP && childright == RP) {
			parent->type = parent->children[1]->type;
			return;
		}
		// ID LP RP
		if (childleft->nodetype == ID) {
			//TODO: error  9
			parent->type = getIDType(Func, childleft->nodevalue.str);
			return;
		}
		// Exp DOT ID
		if (childright->nodetype == ID) {
			//TODO: error type 13
			return;
		}
		if(!cmpType(childleft->type, childright->type))
			SemanticError(7);
		//TODO: just take left type maybe error
		parent->type = childleft->type;
		return;
	}
	//ID LP Args RP; Exp LB Exp RB
	if (num == 4) {
		// ID LP Args RP
		if(childleft->nodetype == ID) {
			//TODO: error 9
			parent->type = getIDType(Func, childleft->nodevalue.str);
			return;
		}	
		// Exp LB Exp RB
		if(childleft->type->kind != ARRAY){
			SemanticError(10);
			parent->type = childleft->type;
			return;
		}
		if(childright->type->kind != BASIC || childright->type->basic != INT) {
			SemanticError(12);
		}
		parent->type = childleft->type->array.elem;
		return;
	}
}
void ArgsAnalyze(struct node *parent, int num) {

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
