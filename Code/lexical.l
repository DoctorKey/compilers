%option yylineno
/* not add default regular */
%option nodefault
%option noyywrap
/* %option location */

%{
	#include "syntax.tab.h"
	#include "file.h"
	#include "main.h"
	#include "tree.h"
	int lexical_isError = 0;
	extern int pairscount;
	extern int beginbool;
	extern int expcount;
	extern int noif;
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

<<EOF>>				{
					if(debug)
						printf("eof of file!\n");
    					BeginToken(yytext); 
					return 0;
				}

(0|[1-9]+[0-9]*)		{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(INT);
					yylval.type_node->nodevalue.INT = atoi(yytext);
					initError(INT);
					return INT;
				}
(0+[1-7]+[0-7]*)		{
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(INT);
					yylval.type_node->nodevalue.INT = strtol(yytext, NULL, 8);
					initError(INT);
					return INT;
				}
0[Xx]{xdigit}*			{
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(INT);
					yylval.type_node->nodevalue.INT = strtol(yytext, NULL, 16);
					initError(INT);
					return INT;
				}

[1-9]*[0-9]"."[0-9]* 		{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(FLOAT);
					yylval.type_node->nodevalue.FLOAT = atof(yytext);
					initError(FLOAT);
					return FLOAT;
				}

";"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(SEMI);
					return SEMI;	
				}

","				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(COMMA);
					return COMMA;
				}

(>|<|>=|<=|==|!=)		{ 	
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(RELOP);
					yylval.type_node->nodevalue.str = strdup(yytext);
					return RELOP;
				}

"="				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(ASSIGNOP);
					return ASSIGNOP;
				}

"+"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(PLUS);
					return PLUS;
				}

"-"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(MINUS);
					return MINUS;
				}

"*"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(STAR);
					return STAR;
				}

"/"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(DIV);
					return DIV;
				}
"&&"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(AND);
					return AND;
				}
"||"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(OR);
					return OR;
				}
"."				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(DOT);
					return DOT;
				}
"!"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(NOT);
					return NOT;
				}
"int"|"float"			{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(TYPE);
					//strdup need to free
					yylval.type_node->nodevalue.str = strdup(yytext);
					return TYPE;
				}
"("				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(LP);
					pairscount++;
					return LP;
				}
")"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(RP);
					pairscount--;
					hurrytestif();
					return RP;}
"["				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(LB);
					return LB;}
"]"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(RB);
					return RB;}
"{"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(LC);
					return LC;}
"}"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(RC);
					return RC;}
"struct"			{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(STRUCT);
					return STRUCT;}
"return"			{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(RETURN);
					return RETURN;}
"if"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(IF);
					beginbool = 1;
					noif = 1;
					expcount = 0;
					return IF;}
"else"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(ELSE);
					return ELSE;}
"while"				{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(WHILE);
					beginbool = 1;
					noif = 1;
					expcount = 0;
					return WHILE;}
{letter}+({digit}|{letter})*	{ 
    					BeginToken(yytext); 
					yylval.type_node = newtokenNode(ID);
					yylval.type_node->nodevalue.str = strdup(yytext);
					//for semantic error show
					initError(ID);
					return ID;
				}
[ \t\r]				{ 
    					BeginToken(yytext); 
				}

\n				{
    					BeginToken(yytext); 
					if(debug)
						fprintf(stderr, "new line\n");
					yycolumn = 1;
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
	yycolumn = 1;
}

