#include "main.h"
//#include "syntax.tab.h"
#include <stdio.h>
#include <string.h>

extern void yyrestart  (FILE * input_file );
extern int yyparse ();
extern int newfile(char *fn);
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
		newfile(argv[i]);
		yyparse();
//		fclose(f);	
	}
	return 0;
}


