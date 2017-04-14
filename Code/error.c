#include "error.h"

void printfErrorRow(char *errmsg, int start, int end); 
void initerrorBuffer(char *);
/*-------------------------------------------------------------------
	Semantic analyze error

*--------------------------------------------------------------------
*/
void SemanticError(int type) {
	fprintf(stderr, "\033[31m\033[1m");
	fprintf(stderr, "Error ");
	fprintf(stderr, "\033[0m");
	fprintf(stderr, "type %d at Line : ", type);
		
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
		/*================================================================*/
		/* print it using variable arguments -----------------------------*/
		va_start(args, errorstring);
		vsprintf(errmsg, errorstring, args);
		va_end(args);
		fprintf(stderr, "%s\n", errmsg + 13);

	}else {
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
