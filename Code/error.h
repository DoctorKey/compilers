#ifndef __ERROR_H_
#define __ERROR_H_

#include <stdio.h>
#include <stdarg.h>
#include "main.h"
#include "file.h"
#include "syntax.tab.h"


extern void PrintError(char type, char *s, ...);


#endif /*__ERROR_H_*/
