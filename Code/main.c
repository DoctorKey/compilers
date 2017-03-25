#include "main.h"
//#include "syntax.tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void yyrestart  (FILE * input_file );
extern int yyparse ();
extern int newfile(char *fn);
extern char *curpwd;
extern void lexical_init();
extern void syntax_init();

char* getpwd(char *dest, char *str);

int main(int argc, char** argv)
{
	int i;
	if (argc <= 1) {
		printf("you must input file!\n");
		return 1;
	}
	for(i = 1; i < argc; i++) {
//		FILE* f = fopen(argv[i], "r");
//		if(!f) {
//			perror(argv[i]);
//			return 1;
//		}
//		yyrestart(f);
		lexical_init();
		syntax_init();

		curpwd = malloc(sizeof(char)*strlen(argv[i]));
		getpwd(curpwd, argv[i]);
		newfile(argv[i]);
		yyparse();
		free(curpwd);
//		fclose(f);	
	}
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
