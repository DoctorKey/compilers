#include "main.h"
#include "syntax.tab.h"
#include "error.h"
#include "file.h"
#include "symtable.h"
#include "IR.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//extern void yyrestart  (FILE * input_file );
extern int yyparse ();
extern int yydebug;
extern void lexical_init();
extern void syntax_init();

int debug = 0;
int debug2 = 0;
int debug3 = 0;

int main(int argc, char** argv)
{
	int i;
	if (argc <= 1) {
		printf("you must input file!\n");
		return 1;
	}
	if(argc == 3) {
		i = 0;	
	}else if(argc == 4) {
		i = 1;
		if (  strcmp(argv[i], "-debug") == 0  ) {
			printf("debugging activated\n");
			debug = 1;
		}else if( strcmp(argv[i], "-yydebug") == 0  ) {
			printf("yydebug activated\n");
			yydebug = 1;
		}else if( strcmp(argv[i], "-debug2") == 0  ) {
			printf("debug2 activated\n");
//			test();
			debug2 = 1;
		}else if( strcmp(argv[i], "-debug3") == 0  ) {
			printf("debug3 activated\n");
//			test3();
			debug3 = 1;
		}
	
	}
	//init output file;
	if(newoutputfile(argv[i + 2])) {
		fprintf(stderr, "newfile(%s) error!\n", argv[i + 2]);
		return 0;
	}

	IR_init(); 

	lexical_init();
	syntax_init();

	if(newfile(argv[i + 1])) {
		fprintf(stderr, "newfile(%s) error!\n", argv[i + 1]);
		return 0;
	}else {
		if(getNextLine() == 0)
			yyparse();
	}

	printfallIR(stderr);
	printfallIRtoFile();
	closefile();
	closeoutputfile();
	/*
	for(i = 1; i < argc; i++) {
		if (  strcmp(argv[i], "-debug") == 0  ) {
			printf("debugging activated\n");
			debug = 1;
		}else if( strcmp(argv[i], "-yydebug") == 0  ) {
			printf("yydebug activated\n");
			yydebug = 1;
		}else if( strcmp(argv[i], "-debug2") == 0  ) {
			printf("debug2 activated\n");
//			test();
			debug2 = 1;
		}else if( strcmp(argv[i], "-debug3") == 0  ) {
			printf("debug3 activated\n");
			debug3 = 1;
		}
		else {
			lexical_init();
			syntax_init();

			if(newfile(argv[i])) {
				fprintf(stderr, "newfile(%s) error!\n", argv[i]);
			}else {
				if(getNextLine() == 0)
					yyparse();
			}

			closefile();
		}
	}
	*/
	return 0;
}

