#include "semantic.h"
#include "name.h"
#include <stdio.h>

void semanticAnalyze(struct node *parent, int num) {
	switch (parent->nodetype) {
	case Program:	break;
	case ExtDefList:	break;
	case ExtDef:	break;
	case ExtDecList:	break;
	case Specifier:	break;
	case StructSpecifier:	break;
	case OptTag:	break;
	case Tag:	break;
	case VarDec:	break;
	case FunDec:	break;
	case VarList:	break;
	case ParamDec:	break;
	case CompSt:	break;
	case StmtList:	break;
	case Stmt:	break; 
	case DefList:	break;
	case Def:	break;
	case DecList:	break;
	case Dec:	break;
	case Exp:	break;
	case Args:	break;
	}
}
