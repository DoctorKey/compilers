#ifndef __ERROR_H_
#define __ERROR_H_

#include <stdio.h>
#include <stdarg.h>
#include "main.h"
#include "file.h"
#include "syntax.tab.h"

struct ErrorInfo {
	int ErrorType;
	int ErrorLine;
	char *ErrorLineStr;
};
extern void PrintError(char type, char *s, ...);

void SemanticError(int type, struct ErrorInfo *errorInfo); 
struct ErrorInfo *initError(int type);
void FreeErrorInfo(struct ErrorInfo *errorInfo); 
void ShowErrorInfo(struct ErrorInfo *errorInfo); 

struct ErrorInfoStack {
	int num;
	struct ErrorInfo *errorInfo;
	struct ErrorInfoStack *last;
};

int GetTotalErrorInfo(); 
int pushErrorInfo(struct ErrorInfo *errorInfo, int type); 
void freeErrorInfoStack(struct ErrorInfoStack *head); 
struct ErrorInfo *GetErrorInfoByNum(struct ErrorInfoStack *head, int num); 
void ShowErrorInfoStack(struct ErrorInfoStack *head); 

#endif /*__ERROR_H_*/
