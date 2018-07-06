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

extern struct Buffer *curbuffer;
extern FILE *curFile;
extern char *curfilename;

int newfile(char *fn);
int closefile(void);
int newoutputfile(char *fn);
void printfallIRtoFile(); 
void printfallAsmtoFile(); 
int closeoutputfile(void);
struct Buffer *init_buffer(void);
void free_buffer(struct Buffer *buffer);

void DumpRow(void); 
int getNextLine();
int GetNextChar(char *b, int maxBuffer);
void BeginToken(char*);

#endif
