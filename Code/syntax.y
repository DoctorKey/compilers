%locations
%{
	#include <stdio.h>
	#include "tree.h"
%}
/* declared types */
%union {
	int type_int;
	float type_float;
	double type_double;
	char* type_str;
	struct node* type_node;
}

/* declared tokens */
%token <type_int> INT
%token <type_float> FLOAT
%token <type_str> ID
%token <type_node> SEMI COMMA
%token <type_node> ASSIGNOP RELOP
%token <type_node> PLUS MINUS STAR DIV
%token <type_node> AND OR DOT NOT
%token <type_node> TYPE
%token <type_node> LP RP LB RB LC RC
%token <type_node> STRUCT
%token <type_node> RETURN IF ELSE WHILE

/* declared non-terminals */
/*%type <type_double> Exp Factor Term*/
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
Program : ExtDefList	{ $$ = newNode(Program,1,$1); showTree($$);}
	;
ExtDefList : 	{ $$ = newNode(ExtDefList, 0); }
	| ExtDef ExtDefList	{ $$ = newNode(ExtDefList, 2, $1, $2); }
	;
ExtDef : Specifier ExtDecList SEMI
	| Specifier SEMI
	| Specifier FunDec CompSt { $$ = newNode(ExtDef, 3,$1, $2, $3); }
	;
ExtDecList : VarDec
	| VarDec COMMA ExtDecList
	;

// Specifiers 
Specifier : TYPE { $$ = newNode(Specifier, 1, newtokenNode(TYPE,0)); }
	| StructSpecifier
	;
StructSpecifier : STRUCT OptTag LC DefList RC
	| STRUCT Tag
	;
OptTag : 
	| ID
	;
Tag : ID
	;

// Declarators 
VarDec : ID
	| VarDec LB INT RB
	| error RB { yyerror("]"); }
	;
FunDec : ID LP VarList RP
	| ID LP RP { $$ = newNode(FunDec, 3, newtokenNode(ID,0), newtokenNode(LP,0), newtokenNode(RP,0)); }
	;
VarList : ParamDec COMMA VarList
	| ParamDec
	;
ParamDec : Specifier VarDec
	;

// Statements 
CompSt : LC DefList StmtList RC {$$ = newNode(CompSt, 4,newtokenNode(LC,0), $2,$3,newtokenNode(RC,0)); }
	| error RC { yyerror("}"); }
	;
StmtList : 	{ $$ = newNode(StmtList, 0); }
	| Stmt StmtList
	;
Stmt : Exp SEMI
	| CompSt
	| RETURN Exp SEMI
	| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
	| IF LP Exp RP Stmt ELSE Stmt
	| WHILE LP Exp RP Stmt
	| error ELSE { yyerror(";");}
	;

// Local Definitions 
DefList :	{$$ = newNode(DefList, 0);}
	| Def DefList
	;
Def : Specifier DecList SEMI
	;
DecList : Dec
	| Dec COMMA DecList
	;
Dec : VarDec
	| VarDec ASSIGNOP Exp
	;
// Expressions 
Exp : Exp ASSIGNOP Exp { $$ = newNode(ASSIGNOP,3,$1,$3);}
	| Exp AND Exp  { $$ = newNode(AND,3,$1,$3);}
	| Exp OR Exp  
	| Exp RELOP Exp
	| Exp PLUS Exp
	| Exp MINUS Exp
	| Exp STAR Exp
	| Exp DIV Exp
	| LP Exp RP
	| MINUS Exp %prec UMINUS
	| NOT Exp
	| ID LP Args RP
	| ID LP RP
	| Exp LB Exp RB
	| Exp DOT ID
	| ID
	| INT
	| FLOAT
	;
Args : Exp COMMA Args
	| Exp
	;
/*
Calc :
	| Exp {printf("= %lf \n", $1); }
	;

Exp : Term
	| Exp PLUS Exp { $$ = $1 + $3; }
	| Exp MINUS Exp { $$ = $1 - $3; }
	| Exp STAR Exp { $$ = $1 * $3; }
	| Exp DIV Exp { $$ = $1 / $3; }
	;
Factor : Term
	;
Term : INT { printf(" @1 %d %d \n", @$.first_column, @$.last_column);$$ = $1; }
	| FLOAT { printf(" %d \n", @1);$$ = $1;}
	;
Calc :
	| Exp {printf("= %lf \n", $1); }
	;

Exp : Factor
	| Exp PLUS Factor { $$ = $1 + $3; }
	| Exp MINUS Factor { $$ = $1 - $3; }
	;

Factor : Term
	| Factor STAR Term { $$ = $1 * $3; }
	| Factor DIV Term { $$ = $1 / $3; }
	;

Term : INT { printf(" @1 %d %d \n", @$.first_column, @$.last_column);$$ = $1; }
	| FLOAT { printf(" %d \n", @1);$$ = $1;}
	;
*/
%%
#include "lex.yy.c"
/*
int main() {
	yyparse();
}
*/
yyerror(char *msg) {
//	fprintf(stderr, "error: %s\n", msg);
	fprintf(stderr, "Error type B at Line %d column %d: Missing \"%s\"\n",yylineno,yycolumn,msg);
}
