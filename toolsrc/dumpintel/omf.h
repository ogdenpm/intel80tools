#ifndef _OMF_H
#define _OMF_H

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
extern char const *xstrdup(char const *s);
#define MAXTOKEN 10
#define MAXCOL 96

typedef struct {
    int type;
    union {
        unsigned ival;
        char name[256];
    };
} value_t;

typedef struct {
    char *label;
    uint16_t index;
    uint16_t size;
    char const **names;
} index_t;

extern value_t tokens[MAXTOKEN];
enum { Byte = 0, Word, DWord, Decimal, EIndex, String, Expr, Error };

extern bool atEnd;

#define MAXREC	64000
#define MAXEXTERN  2048
extern uint8_t rec[MAXREC];
extern unsigned int reclen;
extern int rectype;
extern unsigned int recptr;

enum {
    OMF85, OMF51, OMF96, OMF86
};
typedef struct {
    uint16_t len;
    uint32_t ival;
    char sval[256];
} var_t;
extern var_t vars[10];      // upto 10 vars
extern uint8_t varIdx;
uint16_t curPos;
#define MAXPOS      120
#define INDENT      8
#define MINGAP      2
#define BytesPerLine    16

extern int omfVer;
extern long fpos;
#define INDEXCHUNK   256

#define FIXED   0
#define REPEAT  1

uint8_t const *indexKeys;

void Log(char const *fmt, ...);

index_t *initIndex(uint8_t idx, uint8_t *label, uint16_t index, uint16_t size);

bool parseRule(FILE *fpout, uint8_t rectype);


void resetStrings();
char const *xstrdup(char const *s);
void *xmalloc(size_t size);
void *xrealloc(void *block, size_t size);
void *xcalloc(size_t count, size_t size);


uint8_t const *printRule(FILE *fpout, uint8_t const *s);
void printData(FILE *fpout, uint32_t addr);
void putch(char c, FILE *fpout);


uint8_t const *skipToTrue(uint8_t const *s, uint8_t ifType);
uint8_t const *skipToEnd(uint8_t const *s, uint8_t ifType);
uint32_t parseExpr(uint8_t const **ps);
void extendIndex(index_t *p, uint16_t size);
index_t *selectIndex(uint8_t type);
uint16_t getAllocatedIndex(uint8_t type);
bool format(char const **ps, var_t *pvar);
char *pad(char *s, uint16_t width);

bool parseRecNumber(uint8_t const **ps, var_t *pvar);
bool parseRecIndex(uint8_t const **ps, var_t *pvar);
bool parseIndexVal(uint8_t const **ps, var_t *pvar);
bool parseRecName(uint8_t const **ps, var_t *pvar);
bool parseUpdateIndex(uint8_t const **ps, char *s);

bool parseSegDef86(uint8_t const **ps, var_t *pvar);
bool parseBase86(uint8_t const **ps, var_t *pvar);
bool parseGroupComponent86(uint8_t const **ps, var_t *pvar);
bool parseFixup86(uint8_t const **ps, var_t *pvar);

bool getRecNumber(uint8_t size, uint32_t *val);
bool getRecIndex(uint32_t *val);
bool getRecName(uint32_t *len, uint8_t *str);
bool getRecRest(uint32_t *plen, uint8_t *str);


char const *getRecToEnd(uint32_t *len);
uint32_t getRecPos();
void setRecPos(uint32_t pos);
bool isRecEnd();

void fmtRegMask51(var_t *pvar);
void fmtSegInfo51(var_t *pvar);
void fmtSymInfo51(var_t *pvar);
void fmtSegId96(var_t *pvar);



char const *getIndexName(uint8_t type, uint32_t val);
bool fmtEnum(uint8_t type, uint32_t val, char *s);

char *padStr(char *s, int width);
char *wStrcat(char *s, int width, char const *str);
char *aSprintf(char *s, char const *fmt,  ...);
char *awSprintf(char *s, char const *fmt, int width, ...);

#endif