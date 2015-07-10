#include <stdio.h>
#include "getopt.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

FILE *openIsisFile(const char *isisName, char *mode);

#define MAX_ID		31
#define MAX_INCLUDE	10
#define MAXLINE		256


char line[MAXLINE];			/* inChut line */
char cline[MAXLINE];		/* used to rebuild control line with $IF $SET $RESET and possibly $INCLUDE removed */
char *ts;					/* token start pointer in line including leading space */
char *inCh;					/* inChut character pointer in buffer */
char *outCh;					/* current point in rebuild control line */

typedef unsigned char byte;
typedef unsigned short word;

enum types {
	EOL = 0,
	INCLUDE,
	SET, RESET,
	IF, ELSE, ELSEIF, ENDIF,
	LT, LE, EQ, NE, GE, GT,
	AND, OR, XOR, NOT,
	LP, RP, COMMA,
	ID, NUMBER, STRING,
	BAD
};


struct {
	char *str;
	int yyval;
} keywords[] = {
	{ "INCLUDE", INCLUDE }, { "SET", SET }, { "RESET", RESET },
	{ "IF", IF }, { "ELSE", ELSE }, { "ELSEIF", ELSEIF }, { "ENDIF", ENDIF },
	{ "AND", AND }, { "OR", OR }, { "XOR", XOR }, { "NOT", NOT }, { 0, 0}
};


typedef struct _symbol_t {
	struct _symbol_t *next;
	char *symbol;
	int value;
} symbol_t;

symbol_t *head;

struct {
	char *filename;
	FILE *fp;
} incFile[MAX_INCLUDE + 1];

int incDepth = -1;	/* first file makes this depth 0*/
int flatten;		/* max depth of includes allowed */
FILE *inFP;			/* current inChut file */
FILE *outFP;		/* file to write to */

symbol_t *lookup(char *symbol)
{
	symbol_t *p;

	for (p = head; p; p = p->next)
		if (_stricmp(symbol, p->symbol) == 0)
			return p;
	/* doesn't exist so create symbol */
	p = (symbol_t *)malloc(sizeof(symbol_t));
	p->next = head;
	p->symbol = _strdup(symbol);
	p->value = 0;
	head = p;
	return p;
}


void setSym(char *symbol, int value)
{
	lookup(symbol)->value = value;
}

int getSym(char *symbol)
{
	return lookup(symbol)->value;
}


int pushInc(char *filename)
{
	FILE *fp;

	if (incDepth >= flatten)
		return 0;
	if (!(fp = openIsisFile(filename, "r"))) {
		fprintf(stderr, "can't open %s\n", filename);
		return 0;
	}
	incFile[++incDepth].filename = _strdup(filename);
	inFP = incFile[incDepth].fp = fp;
	return 1;
}

int popInc()
{
	if (incDepth <= 0)		/* file 0 is original file so pop this and there is no file */
		return 0;
	fclose(inFP);
	free(incFile[incDepth--].filename);
	inFP = incFile[incDepth].fp;
	return 1;
}

char token[MAX_ID + 1];
int tokenVal;


int parseNum()
{
	char *t;
	char *s;
	int radix = 10;
	int val, digit;

	s = token;
	t = strchr(s, 0) - 1;	/* point to last character */

	if (*t == 'H')
		radix = 16;
	else if (*t == 'B')
		radix = 2;
	else if (*t == 'Q' || *t == 'O')
		radix = 8;
	else if (*t != 'D')
		t++;

	for (val = 0; s < t; s++) {
		if (isdigit(*s))
			digit = *s - '0';
		else if (isxdigit(*s))
			digit = toupper(*s) - 'A' + 10;
		if (digit >= radix || (val = val * radix + digit) > 255) {
			fprintf(stderr, "bad number %s\n", token);
			return 0xffff;
		}
	}
	return val;
}


int getToken()
{
	int i = 0;

	ts = inCh;
	while (*inCh == ' ' || *inCh == '\t')
		inCh++;
	if (!*inCh)
		return EOL;
	if (isalpha(*inCh)) {
		while (isalnum(*inCh) || *inCh == '$') {
			if (*inCh != '$' && i < MAX_ID)
				token[i++] = toupper(*inCh);
			inCh++;
		}
		token[i] = 0;
		for (i = 0; keywords[i].str; i++)
			if (strcmp(keywords[i].str, token) == 0)
				return keywords[i].yyval;
		return ID;
	}
	if (isdigit(*inCh)) {
		while (isxdigit(*inCh) || *inCh == '$') {
			if (*inCh != '$' && i < MAX_ID)
				token[i++] = toupper(*inCh);
			inCh++;
		}
		if (toupper(*inCh) == 'H' || toupper(*inCh) == 'Q' || toupper(*inCh) == 'O')
			token[i++] = toupper(*inCh);
		token[i] = 0;
		tokenVal = parseNum();
		return NUMBER;
 	}
	switch (*inCh++) {
	case '\'':
		while (*inCh && (*inCh != '\'' || *++inCh == '\''))
			inCh++;
		return STRING;
	case ',': return COMMA;
	case '(': return LP;
	case ')': return RP;
	case '=': return EQ;
	case '<':
		if (*inCh == '>') {
			inCh++;
			return NE;
		}
		if (*inCh != '=')
			return LT;
		inCh++;
		return LE;
	case '>':
		if (*inCh != '=')
			return GT;
		inCh++;
		return GE;

	}
	return BAD;
}



byte chkNot()
{
	byte notStatus = 0;

	while (getToken() == NOT)
		notStatus = !notStatus;
	inCh = ts;
	return notStatus;
}

word getIFVal()
{
	int type;

	if ((type = getToken()) == NUMBER) {
		if (tokenVal > 255)
			fprintf(stderr, "invalid conditional compliation constant\n");
		return tokenVal;
	}
	else if (type == ID)
		return getSym(token);
	fprintf(stderr, "invalid conditional compilation parameter\n");
	return 0xffff;
}


byte parseIfCond()
{
	byte andFactor = 0xff, orFactor = 0, xorFactor = 0;
	byte not1, not2, relOp;
	word val1, val2;

	for (;;) {
		not1 = chkNot();
		if ((val1 = getIFVal()) > 255)
			return 255;

		relOp = getToken();
		if (relOp < LT || relOp > GT)
			inCh = ts;	/* prime for reparse */
		else {			/* we have a rel op so get second value */
			not2 = chkNot();
			if ((val2 = getIFVal()) > 255)
				return 255;
			val2 ^= not2;
			switch (relOp) {
			case EQ: val1 = (val1 == val2); break;
			case LT: val1 = (val1 < val2);  break;
			case LE: val1 = (val1 <= val2); break;
			case GT: val1 = (val1 > val2);  break;
			case GE: val1 = (val1 >= val2); break;
			case NE: val1 = (val1 != val2); break;
			}
		}
		val1 = andFactor & (not1 ^ val1);
		andFactor = 0xff;	// reset mask
		switch (getToken()) { 
		case EOL:
			return xorFactor ^ (orFactor | val1);
		case OR:
			orFactor = xorFactor ^ (orFactor | val1);
			break;
		case AND:
			andFactor = (byte)val1;
			break;
		case XOR:
			xorFactor = xorFactor ^ (orFactor | val1);
			break;
		default:
			return 255;
		}
	}
}


void skiptoRPorEOL()
{
	while (*inCh && *inCh != ')')
		inCh++;
	if (*inCh)
		inCh++;
}


void doSetReset(int isSet)
{
	char varName[MAX_ID + 1];
	int tok;

	if (getToken() != LP) {
		fprintf(stderr, "missing control parameter %s\n", line);
		inCh = ts;
		return;
	}


	for (;;) {
		if (getToken() != ID) {
			fprintf(stderr, "missing or invalid conditional compilation parameter %s\n", line);
			skiptoRPorEOL();
			return;
		}
		if (getToken() == EQ && isSet) {
			strcpy(varName, token);
			if (getToken() != NUMBER || tokenVal > 255) {
				fprintf(stderr, "missing of invalid conditional compilation parameter %s\n", line);
				skiptoRPorEOL();
				return;
			}
			setSym(varName, tokenVal);
		}
		else {
			inCh = ts;		/* reparse later */
			setSym(token, isSet ? 255 : 0);
		}
		if ((tok = getToken()) != COMMA) {
			if (tok != RP) {
				fprintf(stderr, "bad conditional %s missing , or )\n", line);
				skiptoRPorEOL();
			}
			return;
		}
	}

}



void doInclude()
{

	while (*inCh == ' ' || *inCh == '\t')
		inCh++;
	if (*inCh != '(') {
		fprintf(stderr, "bad include %s\n", line);
		return;
	}
	*ts = 0;	/* remove the include from the control line */
	ts = inCh + 1;
	while (*inCh && *inCh != ')')
		inCh++;
	if (*inCh != ')')
		fprintf(stderr, "warning missing ) on include(%s\n", ts);
	*inCh = 0;	/* will truncate rest of line */
	pushInc(ts);
}





int ifDepth;
int skipping;
int skippingDepth;
int inIFpart;

void doControl(char *line)
{
	char *s;
	outCh = cline;
	inCh = line + 1;		/* past the $ */
	int tok;

	s = strchr(line, 0) - 1;
	if (*s == '\n')
		*s = 0;

	if (skipping) {
		tok = getToken();
		if (tok == IF)
			ifDepth++;
		else if (tok == ENDIF) {
			if (--ifDepth < skippingDepth)
				skipping = 0;
		}
		else if (ifDepth == skippingDepth && inIFpart)
			if (tok == ELSE)
				skipping = 0;
			else if (tok == ELSEIF)
				skipping = (parseIfCond() & 1) == 0;
		*line = 0;
		return;
	}




	while ((tok = getToken()) != EOL) {
		switch (tok) {
		case SET: doSetReset(1); break;
		case RESET: doSetReset(0); break;
		case IF:
			ifDepth++;
			if ((parseIfCond() & 1) == 0) {
				skipping = 1;
				inIFpart = 1;
				skippingDepth = ifDepth;
			}
			*line = 0;
			return;
		case ELSE:
		case ELSEIF:
			if (ifDepth == 0)
				fprintf(stderr, "misplaced else or elseif option\n");
			else {
				skipping = 1;
				inIFpart = 0;
				skippingDepth = ifDepth;
			}
			*line = 0;
			return;
		case ENDIF:
			if (ifDepth == 0)
				fprintf(stderr, "misplaced endif option\n");
			else
				--ifDepth;
			*line = 0;
			return;
		case INCLUDE:
			if (incDepth < flatten) {
				doInclude();
				break;
			}
		default: 	/* unused so copy to cline */
			strncpy(outCh, ts, inCh - ts);
			outCh += inCh - ts;
		}
	}
	if (outCh == cline)	/* all consumed so no line to outChut */
		*line = 0;
	else {				/* copy back the unused elements after the $ in line*/
		strcpy(outCh, "\n");
		strcpy(line + 1, cline);
	}
}


pp()
{

	for (;;) {
		while (fgets(line, MAXLINE, inFP)) {
			if (line[0] == '$')
				doControl(line);
			if (!skipping && *line)
				fputs(line, outFP);
		}
		if (!popInc())
			break;
	}
}


int main(int argc, char **argv)
{

	char *outChutfile = NULL;
	int c;

	strcpy(line, "$");			/* initialise the line so that set / reset can reuse common code*/

	while ((c = getopt(argc, argv, "fFo:s:r:")) != -1)
		switch (c) {
		case 'f':
			if (++flatten >= MAX_INCLUDE)
				flatten = MAX_INCLUDE;
			break;
		case 'F':
			flatten = MAX_INCLUDE;
			break;
		case 'o':
			outChutfile = optarg;
			break;
		case 's':
			strcat(line, "SET(");
			strcat(line, optarg);
			strcat(line, ")");
		break;
		case 'r':
			strcat(line, "RESET(");
			strcat(line, optarg);
			strcat(line, ")");
			break;
		case '?':
			if (optopt == 'c')
				fprintf(stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint(optopt))
				fprintf(stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf(stderr,	"Unknown option character `\\x%x'.\n", optopt);
			return 1;
		default:
			abort();

		}

	if (argc - optind != 1) {
		fprintf(stderr, "usage: plmpp [-f] [-F] [-SVAR[=val]] [-RVAR] [-o outfile] srcfile\n");
		return 1;
	}

	if (!pushInc(argv[optind])) {
		fprintf(stderr, "no file to process\n");
		return 1;
	}
	if (outChutfile)
		outFP = openIsisFile(outChutfile, "w");
	else
		outFP = stdout;
	doControl(line);

	pp();

	return 0;
}