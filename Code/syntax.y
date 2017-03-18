%locations
%{
	#include <stdio.h>
%}
/* declared types */
%union {
	int type_int;
	float type_float;
	double type_double;
	char* type_str;
}

/* declared tokens */
%token <type_int> INT
%token <type_float> FLOAT
%token <type_str> ID
%token SEMI COMMA
%token ASSIGNOP RELOP
%token PLUS MINUS STAR DIV
%token AND OR DOT NOT
%token TYPE
%token LP RP LB RB LC RC
%token STRUCT
%token RETURN IF ELSE WHILE

/* declared non-terminals */
/*%type <type_double> Exp Factor Term*/
%type <type_double> Program ExtDefList ExtDef ExtDecList
%type <type_str> Specifier StructSpecifier OptTag Tag
%type <type_str> VarDec FunDec VarList ParamDec
%type <type_int> CompSt StmtList Stmt 
%type <type_int> DefList Def DecList Dec
%type <type_double> Exp Args

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
	;
ExtDefList : 
	| ExtDef ExtDefList
	;
ExtDef : Specifier ExtDecList SEMI
	| Specifier SEMI
	| Specifier FunDec CompSt
	;
ExtDecList : VarDec
	| VarDec COMMA ExtDecList
	;

// Specifiers 
Specifier : TYPE
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
	| ID LP RP
	;
VarList : ParamDec COMMA VarList
	| ParamDec
	;
ParamDec : Specifier VarDec
	;

// Statements 
CompSt : LC DefList StmtList RC
	| error RC { yyerror("}"); }
	;
StmtList : 
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
DefList :
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
Exp : Exp ASSIGNOP Exp 
	| Exp AND Exp
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
