%option yylineno
/* not add default regular */
%option nodefault
%option noyywrap
/* %option location */

%{
#include "main.h"
#include "syntax.tab.h"
#include "tree.h"
	int yycolumn = 1;
	#define YY_USER_ACTION \
		yylloc.first_line = yylloc.last_line = yylineno; \
		yylloc.first_column = yycolumn; \
		yylloc.last_column = yycolumn + yyleng - 1; \
		yycolumn += yyleng;

%}
digit [0-9]
letter [_a-zA-Z]
space [ \t\n]

%%

(0|[1-9]+[0-9]*)		{ 
					yylval.type_int = atoi(yytext);
					printf("INT(%d) ", yylval.type_int);
					yylval.type_node = newtokenNode(INT, yylval.type_int);
					return INT;
				}

[1-9]*[0-9]"."[0-9]* 		{ 
					yylval.type_float = atof(yytext);
					printf("FLOAT(%f) ", yylval.type_float);
					yylval.type_node = newtokenNode(FLOAT, yylval.type_float);
					return FLOAT;
				}

";"				{ 	printf("SEMI  ");
					yylval.type_node = newtokenNode(SEMI, 0);
					return SEMI;	
				}

","				{ 
					printf("COMMA  "); 
					yylval.type_node = newtokenNode(COMMA, 0);
					return COMMA;
				}

(>|<|>=|<=|==|!=)		{ 	
					printf("RELOP  "); 
					yylval.type_node = newtokenNode(RELOP, 0);
					return RELOP;
				}

"="				{ 
					printf("ASSIGNOP  "); 
					yylval.type_node = newtokenNode(ASSIGNOP, 0);
					return ASSIGNOP;
				}

"+"				{ 
					printf("PLUS  "); 
					yylval.type_node = newtokenNode(PLUS, 0);
					return PLUS;
				}

"-"				{ 
					printf("MINUS  "); 
					yylval.type_node = newtokenNode(MINUS, 0);
					return MINUS;
				}

"*"				{ 
					printf("STAR  "); 
					yylval.type_node = newtokenNode(STAR, 0);
					return STAR;
				}

"/"				{ 
					printf("DIV  "); 
					yylval.type_node = newtokenNode(DIV, 0);
					return DIV;
				}
"&&"				{ 
					printf("AND  "); 
					yylval.type_node = newtokenNode(AND, 0);
					return AND;
				}
"||"				{ 
					printf("OR  "); 
					yylval.type_node = newtokenNode(OR, 0);
					return OR;
				}
"."				{ 
					printf("DOT  "); 
					yylval.type_node = newtokenNode(DOT, 0);
					return DOT;
				}
"!"				{ 
					printf("NOT  "); 
					yylval.type_node = newtokenNode(NOT, 0);
					return NOT;
				}
"int"|"float"			{ 
					printf("TYPE  "); 
					yylval.type_node = newtokenNode(TYPE, 0);
					return TYPE;
				}
"("				{ 
					printf("LP  "); 
					yylval.type_node = newtokenNode(LP, 0);
					return LP;
				}
")"				{ 
					printf("RP  "); 
					yylval.type_node = newtokenNode(RP, 0);
					return RP;}
"["				{ 
					printf("LB  "); 
					yylval.type_node = newtokenNode(LB, 0);
					return LB;}
"]"				{ 
					printf("RB  "); 
					yylval.type_node = newtokenNode(RB, 0);
					return RB;}
"{"				{ 
					printf("LC  "); 
					yylval.type_node = newtokenNode(LC, 0);
					return LC;}
"}"				{ 
					printf("RC  "); 
					yylval.type_node = newtokenNode(RC, 0);
					return RC;}
"struct"			{ 
					printf("STRUCT  "); 
					yylval.type_node = newtokenNode(STRUCT, 0);
					return STRUCT;}
"return"			{ 
					printf("RETURN  "); 
					yylval.type_node = newtokenNode(RETURN, 0);
					return RETURN;}
"if"				{ 
					printf("IF  "); 
					yylval.type_node = newtokenNode(IF, 0);
					return IF;}
"else"				{ 
					printf("ELSE  "); 
					yylval.type_node = newtokenNode(ELSE, 0);
					return ELSE;}
"while"				{ 
					printf("WHILE  "); 
					yylval.type_node = newtokenNode(WHILE, 0);
					return WHILE;}
{letter}+({digit}|{letter})*	{ 
					printf("ID  "); 
					yylval.type_node = newtokenNode(ID, 0);
					return ID;}
[ \t\r]				{ }
^.				{
					fprintf(yyout, "%4d %s", yylineno, yytext);
				}

^\n				{
					fprintf(yyout, "%4d %s", yylineno++, yytext);
				}

\n				{
					yycolumn = 1;
					ECHO; 
				}
. 				{ printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);	}
%%
