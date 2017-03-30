#ifndef __FILE_H_
#define __FILE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Buffer {
	char *buffer;
	int eof;
	int nRow;
	int nBuffer;
	int lBuffer;
	int nTokenStart;
	int nTokenLength;
	int nTokenNextStart;
};
struct Filestack { 
	struct Filestack *prev;
	struct Buffer *buffer;
	int lineno;
	char *filename;
	FILE *f;
};

extern struct Filestack *curfile;
extern struct Buffer *curbuffer;
extern char *curfilename;
extern char *curpwd;

int newfile(char *fn);
int popfile(void);
void freecur(void);
char *getpwd(char *dest, char *str);
FILE *getCurFile(void);
struct Buffer *init_buffer(void);
void free_buffer(struct Buffer *buffer);

void DumpRow(void); 
int getNextLine();
int GetNextChar(char *b, int maxBuffer);
void BeginToken(char*);

#endif
