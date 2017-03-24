%option yylineno
/* not add default regular */
%option nodefault
%option noyywrap
/* %option location */

%{
	#include "syntax.tab.h"
	#include "tree.h"
	#include <string.h>
	#include <stdlib.h>
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
xdigit [0-9a-fA-F]

%%

(0|[1-9]+[0-9]*)		{ 
//					yylval.type_int = atoi(yytext);
//					printf("INT(%d) ", yylval.type_int);
					yylval.type_node = newtokenNode(INT);
					yylval.type_node->nodevalue.INT = atoi(yytext);
					return INT;
				}
(0[1-7]+[0-7]*)			{
					yylval.type_node = newtokenNode(INT);
					yylval.type_node->nodevalue.INT = strtol(yytext, NULL, 8);
					return INT;
				}
0[Xx]{xdigit}*			{
					yylval.type_node = newtokenNode(INT);
					yylval.type_node->nodevalue.INT = strtol(yytext, NULL, 16);
					return INT;
				}

[1-9]*[0-9]"."[0-9]* 		{ 
//					yylval.type_float = atof(yytext);
//					printf("FLOAT(%f) ", yylval.type_float);
					yylval.type_node = newtokenNode(FLOAT);
					yylval.type_node->nodevalue.FLOAT = atof(yytext);
					return FLOAT;
				}

";"				{ 
//					printf("SEMI  ");
					yylval.type_node = newtokenNode(SEMI);
					return SEMI;	
				}

","				{ 
//					printf("COMMA  "); 
					yylval.type_node = newtokenNode(COMMA);
					return COMMA;
				}

(>|<|>=|<=|==|!=)		{ 	
//					printf("RELOP  "); 
					yylval.type_node = newtokenNode(RELOP);
					return RELOP;
				}

"="				{ 
//					printf("ASSIGNOP  "); 
					yylval.type_node = newtokenNode(ASSIGNOP);
					return ASSIGNOP;
				}

"+"				{ 
//					printf("PLUS  "); 
					yylval.type_node = newtokenNode(PLUS);
					return PLUS;
				}

"-"				{ 
//					printf("MINUS  "); 
					yylval.type_node = newtokenNode(MINUS);
					return MINUS;
				}

"*"				{ 
//					printf("STAR  "); 
					yylval.type_node = newtokenNode(STAR);
					return STAR;
				}

"/"				{ 
//					printf("DIV  "); 
					yylval.type_node = newtokenNode(DIV);
					return DIV;
				}
"&&"				{ 
//					printf("AND  "); 
					yylval.type_node = newtokenNode(AND);
					return AND;
				}
"||"				{ 
//					printf("OR  "); 
					yylval.type_node = newtokenNode(OR);
					return OR;
				}
"."				{ 
//					printf("DOT  "); 
					yylval.type_node = newtokenNode(DOT);
					return DOT;
				}
"!"				{ 
//					printf("NOT  "); 
					yylval.type_node = newtokenNode(NOT);
					return NOT;
				}
"int"|"float"			{ 
//					printf("TYPE  "); 
					yylval.type_node = newtokenNode(TYPE);
					//strdup need to free
					yylval.type_node->nodevalue.str = strdup(yytext);
					return TYPE;
				}
"("				{ 
//					printf("LP  "); 
					yylval.type_node = newtokenNode(LP);
					return LP;
				}
")"				{ 
//					printf("RP  "); 
					yylval.type_node = newtokenNode(RP);
					return RP;}
"["				{ 
//					printf("LB  "); 
					yylval.type_node = newtokenNode(LB);
					return LB;}
"]"				{ 
//					printf("RB  "); 
					yylval.type_node = newtokenNode(RB);
					return RB;}
"{"				{ 
//					printf("LC  "); 
					yylval.type_node = newtokenNode(LC);
					return LC;}
"}"				{ 
//					printf("RC  "); 
					yylval.type_node = newtokenNode(RC);
					return RC;}
"struct"			{ 
//					printf("STRUCT  "); 
					yylval.type_node = newtokenNode(STRUCT);
					return STRUCT;}
"return"			{ 
//					printf("RETURN  "); 
					yylval.type_node = newtokenNode(RETURN);
					return RETURN;}
"if"				{ 
//					printf("IF  "); 
					yylval.type_node = newtokenNode(IF);
					return IF;}
"else"				{ 
//					printf("ELSE  "); 
					yylval.type_node = newtokenNode(ELSE);
					return ELSE;}
"while"				{ 
//					printf("WHILE  "); 
					yylval.type_node = newtokenNode(WHILE);
					return WHILE;}
{letter}+({digit}|{letter})*	{ 
//					printf("ID  "); 
					yylval.type_node = newtokenNode(ID);
					//strdup need to free
					yylval.type_node->nodevalue.str = strdup(yytext);
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
//					ECHO; 
				}
. 				{ printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);	}
%%
