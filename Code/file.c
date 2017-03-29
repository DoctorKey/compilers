#include "file.h"
#include <ctype.h>
#include <stdarg.h>
#include "main.h"
#include "file.h"
#include "error.h"
#include "syntax.tab.h"

struct Filestack *curfile = NULL;
struct Buffer *curbuffer = NULL;
char *curfilename;
char *curpwd;
extern int yylineno;
extern int yycolumn;

int lMaxBuffer = 1000;

int
newfile(char *fn)
{
	FILE *f;
	struct Filestack *filestack = malloc(sizeof(struct Filestack));
	if(!filestack) {
		fprintf(stderr, "malloc filestack failed!\n");
		return 1;
	}
	if(curfile){
		curfile->lineno = yylineno;
		filestack->filename = malloc(sizeof(char)*(strlen(fn) + strlen(curpwd)));
		strcpy(filestack->filename, curpwd);
		strcat(filestack->filename, fn);
	}else{
		filestack->filename = malloc(sizeof(char)*(strlen(fn)));
		strcpy(filestack->filename, fn);
	}
	f = fopen(filestack->filename, "r");

	if(!f) {
		fprintf(stderr, "can't open %s\n", filestack->filename);
		return 1;
	}
	filestack->f = f;
	curfilename = fn;

	filestack->prev = curfile;
	curfile = filestack;

	filestack->buffer = init_buffer();
	//yy_switch_to_buffer(filestack->buffer);
	curbuffer = filestack->buffer;
	yylineno = 1;
	return 0;
}

int 
popfile(void)
{
	struct Filestack *filestack = curfile;
	struct Filestack *prevfilestack;

	if(!filestack)
		return 1;

	fclose(filestack->f);
	free(filestack->filename);
	free_buffer(filestack->buffer);
	curfile = NULL;
	curbuffer = NULL;

	prevfilestack = filestack->prev;
	free(filestack);

	if(!prevfilestack) {
		return 1;
	}

//	yy_switch_to_buffer(prevfilestack->buffer);
	curfile = prevfilestack;
	curbuffer = curfile->buffer;
	yylineno = curfile->lineno;
	curfilename = curfile->filename;
	return 0;
}

char* 
getpwd(char *dest, char *str)
{
	int i = strlen(str);
	while(i >= 0) {
		if(str[i] == '/')
			break;
		i--;
	}
	return strncpy(dest, str, i + 1);
}

FILE *getCurFile()
{
	if(curfile)
		return curfile->f;
	else
		return NULL;
}

struct Buffer*
init_buffer(){
	struct Buffer *buffer;
	buffer = malloc(sizeof(struct Buffer));
	if (buffer == NULL) {
		fprintf(stderr, "mallocate buffer failed!\n");
		return NULL;
	}
	buffer->buffer = malloc(lMaxBuffer);
	if ((buffer->buffer) == NULL) {
		fprintf(stderr, "cannot allocate %d bytes of memory\n", lMaxBuffer);
		return NULL;
	}
	buffer->eof = 0;
	buffer->nRow = 0;
	buffer->nBuffer = 0;
	buffer->lBuffer = 0;
	buffer->nTokenStart = 0;
	buffer->nTokenLength = 0;
	buffer->nTokenNextStart = 0;
	return buffer;
}

void 
free_buffer(struct Buffer *buffer){
	if(buffer->buffer)
		free(buffer->buffer);
	if(buffer)
		free(buffer);
}
/*--------------------------------------------------------------------
 * dumpChar
 * 
 * printable version of a char
 *------------------------------------------------------------------*/
static
char dumpChar(char c) {
	if (  isprint(c)  )
		return c;
	return '@';
}
/*--------------------------------------------------------------------
 * dumpString
 * 
 * printable version of a string upto 100 character
 *------------------------------------------------------------------*/
static
char *dumpString(char *s) {
	static char buf[101];
	int i;
	int n = strlen(s);

	if (  n > 100  )
		n = 100;

	for (i=0; i<n; i++)
		buf[i] = dumpChar(s[i]);
	buf[i] = 0;
	return buf;
}
/*--------------------------------------------------------------------
 * DumpRow
 * 
 * dumps the contents of the current row
 *------------------------------------------------------------------*/
extern
void DumpRow(void) {
	if (  curbuffer->nRow == 0  ) {
	}
	else 
		fprintf(stdout, "%6d |%.*s", curbuffer->nRow, curbuffer->lBuffer, curbuffer->buffer);
}
void
cleanBuffer(void) {
	int i;
	errorbuffer = malloc(curbuffer->lBuffer);
	strcpy(errorbuffer, curbuffer->buffer);
	for(i = 0;i < curbuffer->lBuffer; i++) {
		if(errorbuffer[i] == '\t' || errorbuffer[i] == '\r' || errorbuffer[i] == '\n') 
			errorbuffer[i] = ' ';
		//if(curbuffer->buffer[i] == '\t' || curbuffer->buffer[i] == '\r' || curbuffer->buffer[i] == '\n') 
		//	curbuffer->buffer[i] = ' ';
	}
}
/*--------------------------------------------------------------------
 * getNextLine
 * 
 * reads a line into the buffer
 *------------------------------------------------------------------*/
extern
int getNextLine() {
	int i;
	char *p;
	FILE *f;

	if( debug )
		fprintf(stderr, "getNextLine()\n");
	/*================================================================*/
	/*----------------------------------------------------------------*/
	curbuffer->nBuffer = 0;
	curbuffer->nTokenStart = -1;
	curbuffer->nTokenNextStart = 1;
	curbuffer->eof = false;

	f = getCurFile();
	if(f == NULL)
		return 1;
	/*================================================================*/
	/* read a line ---------------------------------------------------*/
	p = fgets(curbuffer->buffer, lMaxBuffer, f);
	if (  p == NULL  ) {
		if (  ferror(f)  )
			return -1;
		curbuffer->eof = true;
		return 1;
	}

	yycolumn = 1;
	curbuffer->nRow += 1;
	yylineno = curbuffer->nRow;
	curbuffer->lBuffer = strlen(curbuffer->buffer);

	return 0;
}
/*--------------------------------------------------------------------
 * GetNextChar
 * 
 * reads a character from input for flex
 *------------------------------------------------------------------*/
extern
int GetNextChar(char *b, int maxBuffer) {
	int frc;

	/*================================================================*/
	/*----------------------------------------------------------------*/
	if (  curbuffer->eof  )
		return 0;

	/*================================================================*/
	/* read next line if at the end of the current -------------------*/
	while (  curbuffer->nBuffer >= curbuffer->lBuffer  ) {
		frc = getNextLine();
		if (  frc != 0  )
			return 0;
	}

	/*================================================================*/
	/* ok, return character ------------------------------------------*/
	b[0] = curbuffer->buffer[curbuffer->nBuffer];
	curbuffer->nBuffer += 1;

	if (  debug  )
		printf("GetNextChar() => '%c'0x%02x at %d\n",
				dumpChar(b[0]), b[0], curbuffer->nBuffer);
	return b[0]==0?0:1;
}
/*--------------------------------------------------------------------
 * BeginToken
 * 
 * marks the beginning of a new token
 *------------------------------------------------------------------*/
extern
void BeginToken(char *t) {
	/*================================================================*/
	/* remember last read token --------------------------------------*/
	curbuffer->nTokenStart = curbuffer->nTokenNextStart;
	curbuffer->nTokenLength = strlen(t);
	//  nTokenNextStart = nBuffer; // + 1;
	curbuffer->nTokenNextStart = yylloc.last_column + 1; 

	/*================================================================*/
	/* location for bison --------------------------------------------*/
	if (  debug  ) {
		printf("Token '%s' at %d:%d next at %d\n", dumpString(t),
				yylloc.first_column,
				yylloc.last_column, curbuffer->nTokenNextStart);
	}
}

