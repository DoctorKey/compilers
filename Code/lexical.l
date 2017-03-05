%option yylineno
%option noyywrap
%{
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
	int yylval;
%}
digit [0-9]
letter [_a-zA-Z]
space [ \t\n]

%%
(0|[1-9]+[0-9]*)	{ printf("INT %d\n", atoi(yytext));}
[1-9]*[0-9]"."[0-9]* { printf("FLOAT %f\n", atof(yytext)); }
";"	{ printf("SEMI\n"); }
","	{ printf("COMMA\n"); }
"="	{ printf("ASSIGNOP\n"); }
[>|<|>=|<=|==|!=]	{ printf("RELOP\n"); }
"+"	{ printf("PLUS\n"); }
"-"	{ printf("MINUS\n"); }
"*"	{ printf("STAR\n"); }
"/"	{ printf("DIV\n"); }
"&&"	{ printf("AND\n"); }
"||"	{ printf("OR\n"); }
"."	{ printf("DOT\n"); }
"!"	{ printf("NOT\n"); }
"int"|"float"	{ printf("TYPE\n"); }
"("	{ printf("LP\n"); }
")"	{ printf("RP\n"); }
"["	{ printf("LB\n"); }
"]"	{ printf("RB\n"); }
"{"	{ printf("LC\n"); }
"}"	{ printf("RC\n"); }
"struct"	{ printf("STRUCT\n"); }
"return"	{ printf("RETURN\n"); }
"if"	{ printf("IF\n"); }
"else"	{ printf("ELSE\n"); }
"while"	{ printf("WHILE\n"); }
{letter}+({digit}|{letter})*	{ printf("ID\n"); }
[ \t\r\n]	{ }
. 	{ printf("Error type A at Line %d: Mysterious characters \'%s\'\n", yylineno, yytext);	}
%%
//extern FILE* yyin;
