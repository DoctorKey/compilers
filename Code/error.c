#include "error.h"
#include <string.h>

void printfErrorRow(char *errmsg, int start, int end); 
void initerrorBuffer(char *);
/*-------------------------------------------------------------------
	Semantic analyze error
*--------------------------------------------------------------------
*/
extern int yylineno;
struct ErrorInfo *GerrorInfo = NULL;
static char errbuf[100];
void SemanticError(int type, struct ErrorInfo *errorInfo) {
	fprintf(stderr, "\033[31m\033[1m");
	fprintf(stderr, "Error ");
	fprintf(stderr, "\033[0m");
	fprintf(stderr, "type %d at Line %d: ", type, errorInfo->ErrorLine);
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
	ShowErrorInfo(errorInfo);
}
/*--------------------------------------------------------------------
 * MarkToken
 * 
 * marks the current read token
 *------------------------------------------------------------------*/
extern int getErrorLine() {
	return yylineno;
}
extern int getErrorStart() {
	return curbuffer->nTokenStart;
}
extern int getErrorEnd() {
	return curbuffer->nTokenStart + curbuffer->nTokenLength - 1;
}
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

struct ErrorInfo *initError(int type) {
	//errbuf
	struct ErrorInfo *errorInfo = NULL;
	char errbuftemp[100];
	char temp[100];
	int start = curbuffer->nTokenStart;
	int end = start + curbuffer->nTokenLength - 1;
	initerrorBuffer(errbuftemp);
	sprintf(errbuf, "\"%.*s", start - 1, errbuftemp);
	strcat(errbuf, "\033[31m\033[1m");
	sprintf(temp, "%.*s", end - start + 1, errbuftemp + start - 1);
	strcat(errbuf, temp);
	strcat(errbuf, "\033[0m");
	sprintf(temp, "%.*s\"", curbuffer->lBuffer - end , errbuftemp + end);
	strcat(errbuf, temp);
	errorInfo = malloc(sizeof(struct ErrorInfo));
	errorInfo->ErrorType = type;
	errorInfo->ErrorLine = getErrorLine();	
	errorInfo->ErrorLineStr = malloc(sizeof(char) * (strlen(errbuf)+1));	
	strcpy(errorInfo->ErrorLineStr, errbuf);
	if (debug2) {
		fprintf(stderr, "line %d: %s\n", errorInfo->ErrorLine, errorInfo->ErrorLineStr);
	}

	pushErrorInfo(errorInfo, type);

	return errorInfo;
}
void deleteErrorInfoStack(int type, int num); 
void FreeErrorInfo(struct ErrorInfo *errorInfo) {
	if (errorInfo == NULL)
		return;
	if (errorInfo->ErrorLineStr != NULL) {
		free(errorInfo->ErrorLineStr);
		errorInfo->ErrorLineStr = NULL;
	}
	deleteErrorInfoStack(errorInfo->ErrorType, errorInfo->ErrorLine);
	free(errorInfo);
	errorInfo = NULL;
}
void ShowErrorInfo(struct ErrorInfo *errorInfo) {
	if (errorInfo == NULL) {
		fprintf(stderr, "errorInfo is NULL\n");
	}
	fprintf(stderr, "Line %d: %s\n", errorInfo->ErrorLine, errorInfo->ErrorLineStr);
}

struct ErrorInfoStack *IdErrorInfoStackHead = NULL;
struct ErrorInfoStack *NumErrorInfoStackHead = NULL;
static int totalErrorInfo = 0;

int GetTotalErrorInfo() {
	return totalErrorInfo;
}
int pushErrorInfo(struct ErrorInfo *errorInfo, int type) {
	struct ErrorInfoStack *temp = NULL;
	struct ErrorInfoStack *head = NULL;
	totalErrorInfo++;
	temp = malloc(sizeof(struct ErrorInfoStack));
	temp->errorInfo = errorInfo;
	temp->num = totalErrorInfo;
	if (type == INT || type == FLOAT) {
		temp->last = NumErrorInfoStackHead;
		NumErrorInfoStackHead = temp;
	}else if (type == ID) {
		temp->last = IdErrorInfoStackHead;
		IdErrorInfoStackHead = temp;
	}
	if(debug2) {
		fprintf(stderr, "push ErrorInfo into %s\n", type == ID? "ID":"Num");
		fprintf(stderr, "num %d: ", temp->num);
		ShowErrorInfo(temp->errorInfo);
	}
}
void deleteErrorInfoStack(int type, int num) {
	struct ErrorInfoStack *temp = NULL;
	if (type == INT || type == FLOAT) {
		temp = NumErrorInfoStackHead;
	}else if(type == ID) {
		temp = IdErrorInfoStackHead;
	}
	while(temp != NULL) {
		if(temp->errorInfo) {
			if(temp->errorInfo->ErrorLine == num) {
				temp->errorInfo = NULL;
				break;
			}
		}
		temp = temp->last;
	}
}
//only free Stack. not free errorInfo
void freeErrorInfoStack(struct ErrorInfoStack *head) {
	if(head->last == NULL) {
		free(head);
		head = NULL;	
	}
	else
		freeErrorInfoStack(head->last);
}
struct ErrorInfo *GetErrorInfoByNum(struct ErrorInfoStack *head, int num) {
	struct ErrorInfoStack *temp = head;
	int i;
	if (temp == NULL)
		return NULL;
	while (temp != NULL) {
		if (temp->num == num)
			return temp->errorInfo;
		else
			temp = temp->last;
	}
	return NULL;
}
void ShowErrorInfoStack(struct ErrorInfoStack *head) {
	fprintf(stdout, "-----------ErrorInfoStack----------------\n");
	while (head != NULL) {
		fprintf(stderr, "num %d: ", head->num);
		if(head->errorInfo)
			ShowErrorInfo(head->errorInfo);
		else 
			fprintf(stderr, "\n");
		head = head->last;
	}
	fprintf(stdout, "-----------------------------------------\n");
}
