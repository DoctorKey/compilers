%locations
%{
	#include <stdio.h>
	#include "tree.h"
	#include "name.h"
	#include "lex.yy.c"
	/* indicate grammar analyze is error or not*/
	int isError = 0;
%}
/* declared types */
%union {
//	int type_int;
//	float type_float;
//	char* type_str;
	struct node* type_node;
}

/* declared tokens */
%token <type_node> INT
%token <type_node> FLOAT
%token <type_node> ID
%token <type_node> SEMI COMMA
%token <type_node> ASSIGNOP RELOP
%token <type_node> PLUS MINUS STAR DIV
%token <type_node> AND OR DOT NOT
%token <type_node> TYPE
%token <type_node> LP RP LB RB LC RC
%token <type_node> STRUCT
%token <type_node> RETURN IF ELSE WHILE

/* declared non-terminals */
%type <type_node> Program ExtDefList ExtDef ExtDecList
%type <type_node> Specifier StructSpecifier OptTag Tag
%type <type_node> VarDec FunDec VarList ParamDec
%type <type_node> CompSt StmtList Stmt 
%type <type_node> DefList Def DecList Dec
%type <type_node> Exp Args

/* declared unite */
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT UMINUS 
%left DOT LP RP LB RB LC RC

%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE
%%
/* high-level Definitions */
Program : ExtDefList	
		{ 
			$$ = newNode(Program, 1, $1); 
			if(isError == 0)
				showTree($$); 
			clearTree($$); 
		}
	;
ExtDefList : 	
		{ $$ = NULL; }
	| ExtDef ExtDefList	
		{ $$ = newNode(ExtDefList, 2, $1, $2); }
	;
ExtDef : Specifier ExtDecList SEMI 
		{ $$ = newNode(ExtDef, 3, $1, $2, $3); }
	| Specifier SEMI	
		{ $$ = newNode(Specifier, 2, $1, $2); }
	| Specifier FunDec CompSt 
		{ $$ = newNode(ExtDef, 3, $1, $2, $3); }
	;
ExtDecList : VarDec	
		{ $$ = newNode(ExtDecList, 1, $1); }
	| VarDec COMMA ExtDecList	
		{ $$ = newNode(ExtDecList, 3, $1, $2, $3); }
	;

// Specifiers 
Specifier : TYPE 
		{ $$ = newNode(Specifier, 1, $1); }
	| StructSpecifier	
		{ $$ = newNode(Specifier, 1, $1); }
	;
StructSpecifier : STRUCT OptTag LC DefList RC	
		{ $$ = newNode(StructSpecifier, 5, $1, $2, $3, $4, $5);}
	| STRUCT Tag	
		{ $$ = newNode(StructSpecifier, 2, $1, $2); }
	;
OptTag : 	
		{ $$ = NULL; }
	| ID	
		{ $$ = newNode(OptTag, 1, $1); }
	;
Tag : ID	
		{ $$ = newNode(Tag, 1, $1); }
	;

// Declarators 
VarDec : ID	
		{ $$ = newNode(VarDec, 1, $1); }
	| VarDec LB INT RB 
		{ $$ = newNode(VarDec, 4, $1, $2, $3, $4); }
	;
FunDec : ID LP VarList RP 
		{ $$ = newNode(FunDec, 4, $1, $2, $3, $4); }
	| ID LP RP 
		{ $$ = newNode(FunDec, 3, $1, $2, $3); }
	;
VarList : ParamDec COMMA VarList	
		{ $$ = newNode(VarList, 3, $1, $2, $3); }
	| ParamDec	
		{ $$ = newNode(VarList, 1, $1); }
	;
ParamDec : Specifier VarDec	
		{ $$ = newNode(ParamDec, 2, $1, $2); }
	;

// Statements 
CompSt : LC DefList StmtList RC 
		{ $$ = newNode(CompSt, 4, $1, $2, $3, $4); }
	| error RC 
		{ }
	;
StmtList : 	
		{ $$ = NULL; }
	| Stmt StmtList	
		{ $$ = newNode(StmtList, 2, $1, $2); }
	;
Stmt : Exp SEMI	
		{ $$ = newNode(Stmt, 2, $1, $2); }
	| CompSt	
		{ $$ = newNode(Stmt, 1, $1); }
	| RETURN Exp SEMI	
		{ $$ = newNode(Stmt, 3, $1, $2, $3); }
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE 
		{ $$ = newNode(Stmt, 5, $1, $2, $3, $4, $5); }
	| IF LP Exp RP Stmt ELSE Stmt
		{ $$ = newNode(Stmt, 7, $1, $2, $3, $4, $5, $6, $7); }
	| WHILE LP Exp RP Stmt
		{ $$ = newNode(Stmt, 5, $1, $2, $3, $4, $5); }
	| error SEMI {} 
	;

// Local Definitions 
DefList :	
		{$$ = NULL;}
	| Def DefList	
		{ $$ = newNode(DefList, 2, $1, $2); }
	;
Def : Specifier DecList SEMI 
		{ $$ = newNode(Def, 3, $1, $2, $3); }
	| error SEMI {} 
	;
DecList : Dec	
		{ $$ = newNode(DecList, 1, $1); }
	| Dec COMMA DecList	
		{ $$ = newNode(DecList, 3, $1, $2, $3); }
	;
Dec : VarDec	
		{ $$ = newNode(Dec, 1, $1); }
	| VarDec ASSIGNOP Exp	
		{ $$ = newNode(Dec, 3, $1, $2, $3); }
	;
// Expressions 
Exp : Exp ASSIGNOP Exp 
		{ $$ = newNode(Exp, 3, $1, $2, $3);}
	| Exp AND Exp  
		{ $$ = newNode(Exp, 3, $1, $2, $3);}
	| Exp OR Exp  
		{ $$ = newNode(Exp, 3, $1, $2, $3);}
	| Exp RELOP Exp
		{ $$ = newNode(Exp, 3, $1, $2, $3);}
	| Exp PLUS Exp
		{ $$ = newNode(Exp, 3, $1, $2, $3);}
	| Exp MINUS Exp
		{ $$ = newNode(Exp, 3, $1, $2, $3);}
	| Exp STAR Exp
		{ $$ = newNode(Exp, 3, $1, $2, $3);}
	| Exp DIV Exp
		{ $$ = newNode(Exp, 3, $1, $2, $3);}
	| LP Exp RP
		{ $$ = newNode(Exp, 3, $1, $2, $3);}
	| MINUS Exp %prec UMINUS
		{ $$ = newNode(Exp, 2, $1, $2);}
	| NOT Exp	
		{ $$ = newNode(Exp, 2, $1, $2);}
	| ID LP Args RP	
		{ $$ = newNode(Exp, 4, $1, $2, $3, $4); }
	| ID LP RP	
		{ $$ = newNode(Exp, 3, $1, $2, $3); }
	| Exp LB Exp RB	
		{ $$ = newNode(Exp, 4, $1, $2, $3, $4); }
	| Exp DOT ID	
		{ $$ = newNode(Exp, 3, $1, $2, $3); }
	| ID		
		{ $$ = newNode(Exp, 1, $1); }
	| INT		
		{ $$ = newNode(Exp, 1, $1); }
	| FLOAT		
		{ $$ = newNode(Exp, 1, $1); }
	;
Args : Exp COMMA Args	
		{ $$ = newNode(Args, 3, $1, $2, $3); }
	| Exp		
		{ $$ = newNode(Args, 1, $1); }
	;
/*
Term : INT { printf(" @1 %d %d \n", @$.first_column, @$.last_column);$$ = $1; }
	| FLOAT { printf(" %d \n", @1);$$ = $1;}
	;
Term : INT { printf(" @1 %d %d \n", @$.first_column, @$.last_column);$$ = $1; }
	| FLOAT { printf(" %d \n", @1);$$ = $1;}
	;
*/
%%
yyerror(char *msg) {
	isError = 1;
//	fprintf(stderr, "Error type B at Line %d:column %d Missing \"%s\" source is \"%s\"\n",yylineno,yycolumn,msg, yytext);
	fprintf(stderr, "Error type B at Line %d: \"%s\" in column %d is error!\n", yylineno, yytext, yycolumn);
}
