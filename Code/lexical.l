%option yylineno
/* not add default regular */
%option nodefault
%option noyywrap
/* %option location */
%x IFILE

%{
#include "main.h"
#include "syntax.tab.h"
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
					return INT;
				}

[1-9]*[0-9]"."[0-9]* 		{ 
					yylval.type_double = atof(yytext);
					printf("FLOAT(%f) ", yylval.type_double);
					return FLOAT;
				}

";"				{ 	printf("SEMI  ");
					return SEMI;	
				}

","				{ 
					printf("COMMA  "); 
					return COMMA;
				}

(>|<|>=|<=|==|!=)		{ 	
					printf("RELOP  "); 
					return RELOP;
				}

"="				{ 
					printf("ASSIGNOP  "); 
					return ASSIGNOP;
				}

"+"				{ 
					printf("PLUS  "); 
					return PLUS;
				}

"-"				{ 
					printf("MINUS  "); 
					return MINUS;
				}

"*"				{ 
					printf("STAR  "); 
					return STAR;
				}

"/"				{ 
					printf("DIV  "); 
					return DIV;
				}
"&&"				{ printf("AND  "); return AND;}
"||"				{ printf("OR  "); return OR;}
"."				{ printf("DOT  "); return DOT;}
"!"				{ printf("NOT  "); return NOT;}
"int"|"float"			{ printf("TYPE  "); return TYPE;}
"("				{ printf("LP  "); return LP;}
")"				{ printf("RP  "); return RP;}
"["				{ printf("LB  "); return LB;}
"]"				{ printf("RB  "); return RB;}
"{"				{ printf("LC  "); return LC;}
"}"				{ printf("RC  "); return RC;}
"struct"			{ printf("STRUCT  "); return STRUCT;}
"return"			{ printf("RETURN  "); return RETURN;}
"if"				{ printf("IF  "); return IF;}
"else"				{ printf("ELSE  "); return ELSE;}
"while"				{ printf("WHILE  "); return WHILE;}
{letter}+({digit}|{letter})*	{ printf("ID  "); return ID;}
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
				//	yylineno++;
				}
. 				{ printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);	}
%%
