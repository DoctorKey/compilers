%option yylineno
/* not add default regular */
%option nodefault
%option noyywrap
/* %option location */
%x IFILE

%{
	#include "main.h"
	#include "syntax.tab.h"
	#include "tree.h"
	#include "file.h"
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	int lexical_isError = 0;

	int yycolumn = 1;
#define YY_USER_ACTION \
	yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; \
	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;

#define YY_INPUT(buf,result,max_size)  {\
	result = GetNextChar(buf, max_size); \
	if (  result <= 0  ) \
	result = YY_NULL; \
}

%}
digit [0-9]
letter [_a-zA-Z]
space [ \t\n]
xdigit [0-9a-fA-F]

%%

"#include"[ \t]*[\"<]		{	
//					fprintf(yyout, "begin include\n");
    					BeginToken(yytext); 
					BEGIN IFILE; 
				}

<IFILE>[^ \t\n\">]+		{
					int c;
    					BeginToken(yytext); 
					while((c = input()) && c != '\n');
//					fprintf(yyout, "file name %s\n", yytext);
					if(debug)
						printf("begin file!\n");
					if(newfile(yytext))
						yyterminate();
					BEGIN INITIAL;
				}

<IFILE>.|\n			{
    					BeginToken(yytext); 
					fprintf(stderr, "%4d bad include line\n", yylineno);
					yyterminate();
				}

<<EOF>>				{
					if(debug)
						printf("eof of file!\n");
    					BeginToken(yytext); 
					if(popfile()) {
						yyterminate();
					}
				}

(0|[1-9]+[0-9]*)		{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(INT);
					yylval.type_node->nodevalue.INT = atoi(yytext);
					return INT;
				}
(0[1-7]+[0-7]*)			{
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(INT);
					yylval.type_node->nodevalue.INT = strtol(yytext, NULL, 8);
					return INT;
				}
0[Xx]{xdigit}*			{
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(INT);
					yylval.type_node->nodevalue.INT = strtol(yytext, NULL, 16);
					return INT;
				}

[1-9]*[0-9]"."[0-9]* 		{ 
    					BeginToken(yytext); 
//					yylval.type_float = atof(yytext);
//					printf("FLOAT(%f) ", yylval.type_float);
					yylval.type_node = newtokenNode(FLOAT);
					yylval.type_node->nodevalue.FLOAT = atof(yytext);
					return FLOAT;
				}

";"				{ 
    					BeginToken(yytext); 
//					printf("SEMI  ");
					yylval.type_node = newtokenNode(SEMI);
					return SEMI;	
				}

","				{ 
    					BeginToken(yytext); 
//					printf("COMMA  "); 
					yylval.type_node = newtokenNode(COMMA);
					return COMMA;
				}

(>|<|>=|<=|==|!=)		{ 	
    					BeginToken(yytext); 
//					printf("RELOP  "); 
					yylval.type_node = newtokenNode(RELOP);
					return RELOP;
				}

"="				{ 
    					BeginToken(yytext); 
//					printf("ASSIGNOP  "); 
					yylval.type_node = newtokenNode(ASSIGNOP);
					return ASSIGNOP;
				}

"+"				{ 
    					BeginToken(yytext); 
//					printf("PLUS  "); 
					yylval.type_node = newtokenNode(PLUS);
					return PLUS;
				}

"-"				{ 
    					BeginToken(yytext); 
//					printf("MINUS  "); 
					yylval.type_node = newtokenNode(MINUS);
					return MINUS;
				}

"*"				{ 
    					BeginToken(yytext); 
//					printf("STAR  "); 
					yylval.type_node = newtokenNode(STAR);
					return STAR;
				}

"/"				{ 
    					BeginToken(yytext); 
//					printf("DIV  "); 
					yylval.type_node = newtokenNode(DIV);
					return DIV;
				}
"&&"				{ 
    					BeginToken(yytext); 
//					printf("AND  "); 
					yylval.type_node = newtokenNode(AND);
					return AND;
				}
"||"				{ 
    					BeginToken(yytext); 
//					printf("OR  "); 
					yylval.type_node = newtokenNode(OR);
					return OR;
				}
"."				{ 
    					BeginToken(yytext); 
//					printf("DOT  "); 
					yylval.type_node = newtokenNode(DOT);
					return DOT;
				}
"!"				{ 
    					BeginToken(yytext); 
//					printf("NOT  "); 
					yylval.type_node = newtokenNode(NOT);
					return NOT;
				}
"int"|"float"			{ 
    					BeginToken(yytext); 
//					printf("TYPE  "); 
					yylval.type_node = newtokenNode(TYPE);
					//strdup need to free
					yylval.type_node->nodevalue.str = strdup(yytext);
					return TYPE;
				}
"("				{ 
    					BeginToken(yytext); 
//					printf("LP  "); 
					yylval.type_node = newtokenNode(LP);
					return LP;
				}
")"				{ 
    					BeginToken(yytext); 
//					printf("RP  "); 
					yylval.type_node = newtokenNode(RP);
					return RP;}
"["				{ 
    					BeginToken(yytext); 
//					printf("LB  "); 
					yylval.type_node = newtokenNode(LB);
					return LB;}
"]"				{ 
    					BeginToken(yytext); 
//					printf("RB  "); 
					yylval.type_node = newtokenNode(RB);
					return RB;}
"{"				{ 
    					BeginToken(yytext); 
//					printf("LC  "); 
					yylval.type_node = newtokenNode(LC);
					return LC;}
"}"				{ 
    					BeginToken(yytext); 
//					printf("RC  "); 
					yylval.type_node = newtokenNode(RC);
					return RC;}
"struct"			{ 
    					BeginToken(yytext); 
//					printf("STRUCT  "); 
					yylval.type_node = newtokenNode(STRUCT);
					return STRUCT;}
"return"			{ 
    					BeginToken(yytext); 
//					printf("RETURN  "); 
					yylval.type_node = newtokenNode(RETURN);
					return RETURN;}
"if"				{ 
    					BeginToken(yytext); 
//					printf("IF  "); 
					yylval.type_node = newtokenNode(IF);
					return IF;}
"else"				{ 
    					BeginToken(yytext); 
//					printf("ELSE  "); 
					yylval.type_node = newtokenNode(ELSE);
					return ELSE;}
"while"				{ 
    					BeginToken(yytext); 
//					printf("WHILE  "); 
					yylval.type_node = newtokenNode(WHILE);
					return WHILE;}
{letter}+({digit}|{letter})*	{ 
    					BeginToken(yytext); 
//					printf("ID  "); 
					yylval.type_node = newtokenNode(ID);
					//strdup need to free
					yylval.type_node->nodevalue.str = strdup(yytext);
					return ID;}
[ \t\r]				{ 
    					BeginToken(yytext); 
				}

\n				{
    					BeginToken(yytext); 
					//may be can not detect \n I don't know why
					if(debug)
						fprintf(stderr, "new line\n");
					yycolumn = 1;
//					ECHO; 
				}

. 				{ 
    					BeginToken(yytext); 
					lexical_isError = 1;
					fprintf(stderr, "\033[31m\033[1m");
					fprintf(stderr, "Error ");
					fprintf(stderr, "\033[0m");
					fprintf(stderr, "type A at Line %d: ", yylineno);
					PrintError('A', yytext);
				}
%%

void lexical_init()
{
	lexical_isError = 0;
	curfile = NULL;
	curbuffer = NULL;
	yycolumn = 1;
}

