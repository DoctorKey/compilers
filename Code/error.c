#include "error.h"

void printfErrorRow(char *errmsg, int start, int end); 
void initerrorBuffer(char *);
/*-------------------------------------------------------------------
	Semantic analyze error

*--------------------------------------------------------------------
*/
extern int yylineno;
void SemanticError(int type, int line) {
	fprintf(stderr, "\033[31m\033[1m");
	fprintf(stderr, "Error ");
	fprintf(stderr, "\033[0m");
	fprintf(stderr, "type %d at Line %d: ", type, yylineno);
	switch(type) {
	case 1: fprintf(stderr, "Undefined variable "); break;
	case 2: fprintf(stderr, "Undefined function "); break;
	case 3: fprintf(stderr, "Redefined variable "); break;
	case 4: fprintf(stderr, "Redefined function "); break;
	case 5: fprintf(stderr, "Type mismatched for assignment "); break;
	case 6: fprintf(stderr, "The left-hand side of an assignment must be a variable "); break;
	case 7: fprintf(stderr, "Type mismatched for operands "); break;
	case 8: fprintf(stderr, "Type mismatched for return "); break;
	case 9: fprintf(stderr, "Type mismatched for function's args "); break;
	case 10: fprintf(stderr, "not an array "); break;
	case 11: fprintf(stderr, "not a function "); break;
	case 12: fprintf(stderr, "not an integer "); break;
	case 13: fprintf(stderr, "Illegal use of \".\" "); break;
	case 14: fprintf(stderr, "Non-existent field "); break;
	case 15: fprintf(stderr, "Redefined field "); break;
	case 16: fprintf(stderr, "Duplicated name "); break;
	case 17: fprintf(stderr, "Undefined structure "); break;
	case 18: fprintf(stderr, "Undefined function "); break;
	case 19: fprintf(stderr, "Inconsistent declaration of function "); break;
	}
	PrintError('C', NULL);
}
/*--------------------------------------------------------------------
 * MarkToken
 * 
 * marks the current read token
 *------------------------------------------------------------------*/
extern
void PrintError(char type, char *errorstring, ...) {
	static char errmsg[10000];
	va_list args;
	int start=curbuffer->nTokenStart;
	int end=start + curbuffer->nTokenLength - 1;

	initerrorBuffer(errmsg);

	if(type == 'A') {

		fprintf(stderr, "Mysterious characters "); 
		printfErrorRow(errmsg, start, end); 
		fprintf(stderr, "\n");

	}else if(type == 'B') {

		printfErrorRow(errmsg, start, end); 
		/* print it using variable arguments -----------------------------*/
		va_start(args, errorstring);
		vsprintf(errmsg, errorstring, args);
		va_end(args);
		fprintf(stderr, "%s\n", errmsg + 13);

	}else if(type == 'C') {
		printfErrorRow(errmsg, start, end); 
		fprintf(stderr, "\n");
	}else{
		fprintf(stderr, "unknow error type\n");
	}

	if(debug)
		fprintf(stderr, "start:%d, end:%d, lBuffer:%d\n", start, end, curbuffer->lBuffer);
	return;
}
void 
printfErrorRow(char *errmsg, int start, int end) {
	/*================================================================*/
	/* a bit more complicate version ---------------------------------*/
	fprintf(stderr, "\"%.*s", start - 1, errmsg);
	fprintf(stderr, "\033[31m\033[1m");
	fprintf(stderr, "%.*s", end - start + 1, errmsg + start - 1);
	fprintf(stderr, "\033[0m");
	//lBuffer - end - 1 need -1 and don't printf \n
	fprintf(stderr, "%.*s\"", curbuffer->lBuffer - end , errmsg + end);
	return;	
}
void
initerrorBuffer(char *errorbuffer) {
	int i;
	strcpy(errorbuffer, curbuffer->buffer);
	for(i = 0;i < curbuffer->lBuffer; i++) {
		if(errorbuffer[i] == '\t' || errorbuffer[i] == '\r' || errorbuffer[i] == '\n') 
			errorbuffer[i] = ' ';
	}
	return;
}
