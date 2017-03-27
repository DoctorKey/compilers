#include "main.h"
#include "syntax.tab.h"
#include "error.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//extern void yyrestart  (FILE * input_file );
extern int yyparse ();
#if YYDEBUG == 1
extern int yydebug;
#endif
extern int newfile(char *fn);
extern char *curpwd;
extern void lexical_init();
extern void syntax_init();

extern int init_buffer();
extern void free_buffer();
char* getpwd(char *dest, char *str);

int debug = 0;

int main(int argc, char** argv)
{
	int i;
	if (argc <= 1) {
		printf("you must input file!\n");
		return 1;
	}
	if(init_buffer()) {
		return 1;
	}
	for(i = 1; i < argc; i++) {
		if (  strcmp(argv[i], "-debug") == 0  ) {
			printf("debugging activated\n");
			debug = 1;
		}
		else {
			lexical_init();
			syntax_init();

			curpwd = malloc(sizeof(char)*strlen(argv[i]));
			getpwd(curpwd, argv[i]);
			newfile(argv[i]);
#if YYDEBUG == 1
			yydebug = 1;
#endif
			if(getNextLine() == 0)
				yyparse();
			free(curpwd);
			//		fclose(f);	
		}
	}
  	free_buffer();
	return 0;
}

char* 
getpwd(char *dest, char *str)
{
	int i = strlen(str);
	while(i >= 0) {
		if(str[i] == '/')
			break;
		i--;
	}
	return strncpy(dest, str, i + 1);
}
