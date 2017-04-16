#ifndef __ERROR_H_
#define __ERROR_H_

#include <stdio.h>
#include <stdarg.h>
#include "main.h"
#include "file.h"
#include "syntax.tab.h"

struct ErrorInfo {
	int ErrorLine;
	char *ErrorLineStr;
	int ErrorStart;
	int ErrorEnd;
};
extern void PrintError(char type, char *s, ...);
void SaveLine(); 

void SemanticError(int type, struct ErrorInfo *errorInfo); 
struct ErrorInfo *initError();
void FreeErrorInfo(struct ErrorInfo *errorInfo); 
void ShowErrorInfo(struct ErrorInfo *errorInfo); 

struct ErrorInfoStack {
	struct ErrorInfo *errorInfo;
	struct ErrorInfoStack *last;
};

int pushErrorInfo(struct ErrorInfo *errorInfo); 
void freeErrorInfoStack(struct ErrorInfoStack *head); 
struct ErrorInfo *GetErrorInfoByNum(struct ErrorInfoStack *head, int num); 
struct ErrorInfo *GetFirstErrorInfo(struct ErrorInfoStack *head); 
void ShowErrorInfoStack(struct ErrorInfoStack *head); 

#endif /*__ERROR_H_*/
