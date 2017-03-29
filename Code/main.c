#include "main.h"
#include "syntax.tab.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//extern void yyrestart  (FILE * input_file );
extern int yyparse ();
extern int yydebug;
extern int newfile(char *fn);
extern char *curpwd;
extern void lexical_init();
extern void syntax_init();

int debug = 0;

int main(int argc, char** argv)
{
	int i;
	if (argc <= 1) {
		printf("you must input file!\n");
		return 1;
	}
	for(i = 1; i < argc; i++) {
		if (  strcmp(argv[i], "-debug") == 0  ) {
			printf("debugging activated\n");
			debug = 1;
		}else if( strcmp(argv[i], "-yydebug") == 0  ) {
			printf("yydebug activated\n");
			yydebug = 1;
		}
		else {
			lexical_init();
			syntax_init();

			curpwd = malloc(sizeof(char)*strlen(argv[i]));
			getpwd(curpwd, argv[i]);

			if(newfile(argv[i])) {
				fprintf(stderr, "newfile(%s) error!\n", argv[i]);
			}else {
				if(getNextLine() == 0)
					yyparse();
			}

			free(curpwd);
			curpwd = NULL;
		}
	}
	return 0;
}

