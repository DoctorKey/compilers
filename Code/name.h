#ifndef __NAME_H_
#define __NAME_H_

enum {
	Program, ExtDefList, ExtDef, ExtDecList,
	Specifier, StructSpecifier, OptTag, Tag,
	VarDec, FunDec, VarList, ParamDec,
	CompSt, StmtList, Stmt, 
	DefList, Def, DecList, Dec,
	Exp, Args
};

char *getName(int type);

#endif
