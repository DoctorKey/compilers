#include "error.h"
/*--------------------------------------------------------------------
 * MarkToken
 * 
 * marks the current read token
 *------------------------------------------------------------------*/
extern
void PrintError(char type, char *errorstring, ...) {
	static char errmsg[10000];
	va_list args;

	if(!curbuffer) {
		va_start(args, errorstring);
		vsprintf(errmsg, errorstring, args);
		va_end(args);

		fprintf(stderr, "%s\n", errmsg + 13);
		return;	
	}

	int start=curbuffer->nTokenStart;
	int end=start + curbuffer->nTokenLength - 1;
	int i;
	if(type == 'A') {
		fprintf(stderr, "Mysterious characters "); 
	}
	/*================================================================*/
	/* a bit more complicate version ---------------------------------*/
	cleanBuffer();
	fprintf(stderr, "\"%.*s", start - 1, curbuffer->buffer);
	fprintf(stderr, "\033[31m\033[1m");
	fprintf(stderr, "%.*s", end - start + 1, curbuffer->buffer + start - 1);
	fprintf(stderr, "\033[0m");
	//lBuffer - end - 1 need -1 and don't printf \n
	fprintf(stderr, "%.*s\"", curbuffer->lBuffer - end , curbuffer->buffer + end);

	/*================================================================*/
	/* print it using variable arguments -----------------------------*/
	va_start(args, errorstring);
	vsprintf(errmsg, errorstring, args);
	va_end(args);

	fprintf(stderr, "%s\n", errmsg + 13);
	if(debug)
		fprintf(stderr, "start:%d, end:%d, lBuffer:%d\n", start, end, curbuffer->lBuffer);
}
