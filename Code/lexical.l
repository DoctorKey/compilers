%option yylineno
/* not add default regular */
%option nodefault
%option noyywrap
/* %option location */
%x IFILE

%{
#include "main.h"
#include "syntax.tab.h"
/*
	enum yytokentype {
		INT = 258,
		FLOAT = 259,
		ID = 260,
		SEMI = 261,
		COMMA = 262,
		ASSIGNOP = 263,
		RELOP = 264,
		PLUS = 265,
		MINUS = 266,
		STAR = 267,
		DIV = 268,
		AND = 269,
		OR = 270,
		DOT = 271,
		NOT = 272,
		TYPE = 273,
		LP = 274, RP = 275,
		LB = 276, RB = 277,
		LC = 278, RC = 279,
		STRUCT = 280,
		RETURN = 281,
		IF = 282,
		ELSE = 283,
		WHILE = 284
	};
*/
//	int yylval;
	YYSTYPE yylval;


	int yycolumn = 1;
	#define YY_USER_ACTION \
		yylloc.first_line = yylloc.last_line = yylineno; \
		yylloc.first_column = yycolumn; \
		yylloc.last_column = yycolumn + yyleng - 1; \
		yycolumn += yyleng;

	struct bufstack {
		struct bufstack *prev;
		YY_BUFFER_STATE bs;
		int lineno;
		char *filename;
		FILE *f;
	}*curbs = 0;
	char *curfilename;
//	extern int newfile(char *fn);
//	extern int popfile(void);
%}
digit [0-9]
letter [_a-zA-Z]
space [ \t\n]

%%
^"#"[ \t]*"include"[ \t]*\[\"<]	{ 
					printf("include ");
					BEGIN IFILE; 
				}

<IFILE>[^ \t\n\">]+		{ 
					{	
						int c;
						while((c = input()) && c != '\n') ;
					}
					yylineno++;
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
					yylval = (YYSTYPE)atoi(yytext);
					printf("INT(%d) ", yylval);
					return INT;
				}

[1-9]*[0-9]"."[0-9]* 		{ 
					yylval = (YYSTYPE)atof(yytext);
					printf("FLOAT(%f) ", yylval);
					return FLOAT;
				}
";"				{ printf("SEMI  "); }
","				{ printf("COMMA  "); }
(>|<|>=|<=|==|!=)		{ printf("RELOP  "); }
"="				{ printf("ASSIGNOP  "); }

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
"&&"				{ printf("AND  "); }
"||"				{ printf("OR  "); }
"."				{ printf("DOT  "); }
"!"				{ printf("NOT  "); }
"int"|"float"			{ printf("TYPE  "); }
"("				{ printf("LP  "); }
")"				{ printf("RP  "); }
"["				{ printf("LB  "); }
"]"				{ printf("RB  "); }
"{"				{ printf("LC  "); }
"}"				{ printf("RC  "); }
"struct"			{ printf("STRUCT  "); }
"return"			{ printf("RETURN  "); }
"if"				{ printf("IF  "); }
"else"				{ printf("ELSE  "); }
"while"				{ printf("WHILE  "); }
{letter}+({digit}|{letter})*	{ printf("ID  "); }
[ \t\r]				{ }
^.				{
					fprintf(yyout, "%4d %s", yylineno, yytext);
				}

^\n				{
					fprintf(yyout, "%4d %s", yylineno++, yytext);
				}

\n				{
				//	yycolumn = 1;
					ECHO; 
				//	yylineno++;
				}
. 				{ printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);	}
%%

int newfile(char *fn)
{
	FILE *f = fopen(fn, "r");
	struct bufstack *bs = malloc(sizeof(struct bufstack));

	if(!f) {
		perror(fn);
		return 0;
	}
	if(!bs) {
		perror("malloc");
		exit(1);
	}

	if(curbs)
		curbs->lineno = yylineno;
	bs->prev = curbs;

	bs->bs = yy_create_buffer(f, YY_BUF_SIZE);
	bs->f = f;
	bs->filename = fn;
	yy_switch_to_buffer(bs->bs);
	curbs = bs;
	yylineno = 1;
	curfilename = fn;
	return 1;
}

int popfile(void)
{
	struct bufstack *bs = curbs;
	struct bufstack *prevbs;
	if(!bs)
		return 0;
	
	fclose(bs->f);
	yy_delete_buffer(bs->bs);

	prevbs = bs->prev;
	free(bs);

	if(!prevbs)
		return 0;
	
	yy_switch_to_buffer(prevbs->bs);
	curbs = prevbs;
	yylineno = curbs->lineno;
	curfilename = curbs->filename;
	return 1;
}
