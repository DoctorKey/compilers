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
	#include <stdio.h>
	#include <string.h>
	#include <stdlib.h>
	int lexical_isError = 0;
	
	struct bufstack { struct bufstack *prev;
		YY_BUFFER_STATE bs;
		int lineno;
		char *filename;
		FILE *f;
	};
	struct bufstack *curbs = NULL;
	char *curfilename;
	char *curpwd;

	int newfile(char *fn);
	int popfile(void);

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

"#include"[ \t]*[\"<]		{	
//					fprintf(yyout, "begin include\n");
					BEGIN IFILE; 
				}

<IFILE>[^ \t\n\">]+		{
					int c;
					while((c = input()) && c != '\n');

//					yylineno++;
//					fprintf(yyout, "file name %s\n", yytext);
					if(!newfile(yytext))
						yyterminate();
					BEGIN INITIAL;
				}

<IFILE>.|\n			{
					fprintf(stderr, "%4d bad include line\n", yylineno);
					yyterminate();
				}

<<EOF>>				{
					if(!popfile())
						yyterminate();
				}

(0|[1-9]+[0-9]*)		{ 
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

\n				{
					yycolumn = 1;
//					ECHO; 
				}

. 				{ 
					lexical_isError = 1;
					printf("Error type A at Line %d: Mysterious characters \'%s\' in %s\n", 
						yylineno, yytext, curbs->filename);
				}
%%

void lexical_init()
{
	lexical_isError = 0;
	curbs = NULL;
	yycolumn = 1;
}

int
newfile(char *fn)
{
	FILE *f;
	struct bufstack *bs = malloc(sizeof(struct bufstack));
	if(!bs) {
		perror("malloc");
		exit(1);
	}
	if(curbs){
		curbs->lineno = yylineno;
		bs->filename = malloc(sizeof(char)*(strlen(fn) + strlen(curpwd)));
		strcpy(bs->filename, curpwd);
		strcat(bs->filename, fn);
	}else{
		bs->filename = malloc(sizeof(char)*(strlen(fn)));
		strcpy(bs->filename, fn);
	}
	f = fopen(bs->filename, "r");

	if(!f) {
		printf("can't open %s\n", bs->filename);
		perror(bs->filename);
		return 0;
	}

	bs->prev = curbs;

	bs->bs = yy_create_buffer(f, YY_BUF_SIZE);
	bs->f = f;
	yy_switch_to_buffer(bs->bs);
	curbs = bs;
	yylineno = 1;
	curfilename = fn;
	return 1;
}

int 
popfile(void)
{
	struct bufstack *bs = curbs;
	struct bufstack *prevbs;

	if(!bs)
		return 0;

	fclose(bs->f);
	free(bs->filename);
	yy_delete_buffer(bs->bs);

	prevbs = bs->prev;
	free(bs);

	if(!prevbs) {
		return 0;
	}

	yy_switch_to_buffer(prevbs->bs);
	curbs = prevbs;
	yylineno = curbs->lineno;
	curfilename = curbs->filename;
	return 1;
}
