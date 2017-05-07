%locations
%{
	#include <stdio.h>
	#include "lex.yy.c"
	#include "tree.h"
	#include "name.h"
	#define YYERROR_VERBOSE 1
	#define YYDEBUG 1
	/* indicate grammar analyze is error or not*/
	int isError = 0;
	extern int lexical_isError;
	extern int structdefnum;
%}

/* declared types */
%union {
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

%destructor {
//			printf("free at node\n");
			clearTree($$);
			$$ = NULL;
		} <type_node>

%%
/* high-level Definitions */
Program : ExtDefList	
		{ 
			$$ = newNode(Program, 1, $1); 
			#if	LAB_1 == true
			if(isError == 0 && lexical_isError == 0)
				showTree($$);
			#endif
			clearTree($$); 
			$$ = NULL;
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
		{ $$ = newNode(ExtDef, 2, $1, $2); }
	| Specifier FunDec SEMI 
		{ $$ = newNode(ExtDef, 3, $1, $2, $3); }
	| Specifier FunDec CompSt 
		{ $$ = newNode(ExtDef, 3, $1, $2, $3); }
	| error SEMI 
		{ $$ = newNode(ExtDef, 2, NULL, $2); yyerrok; }
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
		{ $$ = newNode(StructSpecifier, 5, $1, $2, $3, $4, $5); structdefnum--; }
	| STRUCT Tag	
		{ $$ = newNode(StructSpecifier, 2, $1, $2); }
	| STRUCT OptTag LC error RC 
		{ $$ = newNode(StructSpecifier, 5, $1, $2, $3, NULL, $5); yyerrok; structdefnum--; }
	| error LC DefList RC 
		{ $$ = newNode(StructSpecifier, 4, NULL, $2, $3, $4); yyerrok; }
	| error LC error RC 
		{ $$ = newNode(StructSpecifier, 4, NULL, $2, NULL, $4); yyerrok; }
	;
OptTag : 	
		{ $$ = NULL; structdefnum++; }
	| ID	
		{ $$ = newNode(OptTag, 1, $1); structdefnum++; }
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
	| error LP VarList RP 
		{ $$ = newNode(FunDec, 4, NULL, $2, $3, $4); yyerrok; }
	| error LP error RP 
		{ $$ = newNode(FunDec, 4, NULL, $2, NULL, $4); yyerrok; }
	| ID LP error RP
		{ $$ = newNode(FunDec, 4, $1, $2, NULL, $4); yyerrok; }
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
	| LC error RC 
		{ $$ = newNode(CompSt, 3, $1, NULL, $3); yyerrok; }
	;
StmtList : 	
		{ $$ = NULL; }
	| Stmt M StmtList	
		{ $$ = newNode(StmtList, 2, $1, $3); }
	;
Stmt : Exp SEMI	
		{ $$ = newNode(Stmt, 2, $1, $2); }
	| CompSt	
		{ $$ = newNode(Stmt, 1, $1); }
	| RETURN Exp SEMI	
		{ $$ = newNode(Stmt, 3, $1, $2, $3); }
	| IF LP Exp RP M Stmt %prec LOWER_THAN_ELSE 
		{ $$ = newNode(Stmt, 5, $1, $2, $3, $4, $6); }
	| IF LP Exp RP M Stmt ELSE N M Stmt
		{ $$ = newNode(Stmt, 7, $1, $2, $3, $4, $6, $7, $10); }
	| WHILE M LP Exp RP M Stmt
		{ $$ = newNode(Stmt, 5, $1, $3, $4, $5, $7); }
	| error SEMI 
		{ $$ = newNode(Stmt, 2, NULL, $2); yyerrok; }
	| IF LP error RP M Stmt %prec LOWER_THAN_ELSE 
		{ $$ = newNode(Stmt, 5, $1, $2, NULL, $4, $6); yyerrok; }
	| IF LP error RP M Stmt ELSE M Stmt
		{ $$ = newNode(Stmt, 7, $1, $2, NULL, $4, $6, $7, $9); yyerrok; }
	| WHILE M LP error RP M Stmt
		{ $$ = newNode(Stmt, 5, $1, $3, NULL, $5, $7); yyerrok; }
	;
N : 		{ newNode(N, 0); }
	;
M : 		{ newNode(M, 0); }
	;
// Local Definitions 
DefList :	
		{$$ = NULL;}
	| Def DefList	
		{ $$ = newNode(DefList, 2, $1, $2); }
	;
Def : Specifier DecList SEMI 
		{ $$ = newNode(Def, 3, $1, $2, $3); }
	| error SEMI 
		{ $$ = newNode(Def, 2, NULL, $2); yyerrok; }
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
	| Exp AND M Exp  
		{ $$ = newNode(Exp, 3, $1, $2, $4);}
	| Exp OR M Exp  
		{ $$ = newNode(Exp, 3, $1, $2, $4);}
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
%%

yyerror(char *msg) {
	isError = 1;
	fprintf(stderr, "\033[31m\033[1m");
	fprintf(stderr, "Error ");
	fprintf(stderr, "\033[0m");
	fprintf(stderr, "type B at Line %d: ", yylineno);
	PrintError('B', msg);
}

void syntax_init()
{
	isError = 0;
}
