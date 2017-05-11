#include "main.h"
#include "name.h"
#include "syntax.tab.h" 

const char *name[] = {
	"Program", "ExtDefList", "ExtDef", "ExtDecList",
	"Specifier", "StructSpecifier", "OptTag", "Tag",
	"VarDec", "FunDec", "VarList", "ParamDec",
	"CompSt", "StmtList", "Stmt", 
	"DefList", "Def", "DecList", "Dec",
	"Exp", "Args"
};
const char *tokenName[] = {
     "INT", 
     "FLOAT", 
     "ID",
     "SEMI",
     "COMMA",
     "ASSIGNOP",
     "RELOP",
     "PLUS",
     "MINUS",
     "STAR",
     "DIV",
     "AND",
     "OR",
     "DOT",
     "NOT",
     "TYPE",
     "LP",
     "RP",
     "LB",
     "RB",
     "LC",
     "RC",
     "STRUCT",
     "RETURN",
     "IF",
     "ELSE",
     "WHILE",
     "UMINUS",
     "LOWER_THAN_ELSE"
};
const char *getName(int type)
{
	if(type >= INT){
		return tokenName[type - INT];
	}else{
		return name[type];
	}
}

char *getfalseRelop(const char *relop) {
	if(strcmp(relop, ">") == 0) {
		return strdup("<=");
	}else if(strcmp(relop, "<") == 0) {
		return strdup(">=");
	}else if(strcmp(relop, ">=") == 0) {
		return strdup("<");
	}else if(strcmp(relop, "<=") == 0) {
		return strdup(">");
	}else if(strcmp(relop, "==") == 0) {
		return strdup("!=");
	}else if(strcmp(relop, "!=") == 0) {
		return strdup("==");
	}	
}
