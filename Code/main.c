#include "main.h"
#include <stdio.h>
//extern FILE* yyin;

int main(int argc, char** argv)
{
/*
	if(argc > 1) {
		if(!(yyin = fopen(argv[1], "r"))) {
			perror(argv[1]);
			return 1;
		}
	}
	while(yylex() != 0);
	return 0;
*/
	yyparse();
/*
	if (argc <= 1) return 1;
	FILE* f = fopen(argv[1], "r");
	if(!f) {
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	yyparse();
	return 0;
*/
/*
	if(argc < 2) {
		fprintf(stderr, "need filename\n");
		return 1;
	}
	if(newfile(argv[1]))
		yylex();
	*/
}
/*
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
*/
