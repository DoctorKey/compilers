#ifndef __ERROR_H_
#define __ERROR_H_

#include <stdio.h>
#include <stdarg.h>
#include "main.h"
#include "file.h"
#include "syntax.tab.h"

typedef struct ErrorInfo_* ErrorInfo;

struct ErrorInfo_ {
	int ErrorTypeNum;
	int ErrorLineNum;
	char *ErrorLineStr;
	int ErrorStrType;
};
extern void PrintError(char type, char *s, ...);

void SemanticError(ErrorInfo errorInfo); 
ErrorInfo initError(int type);
void FreeErrorInfo(ErrorInfo errorInfo); 
void ShowErrorInfo(ErrorInfo errorInfo); 

struct ErrorInfoStack {
	int num;
	ErrorInfo errorInfo;
	struct ErrorInfoStack *last;
};

int GetTotalErrorInfo(); 
int pushErrorInfo(ErrorInfo errorInfo, int type); 
void freeErrorInfoStack(struct ErrorInfoStack *head); 
ErrorInfo GetErrorInfoByNum(struct ErrorInfoStack *head, int num); 
void ShowErrorInfoStack(struct ErrorInfoStack *head); 

#endif /*__ERROR_H_*/
