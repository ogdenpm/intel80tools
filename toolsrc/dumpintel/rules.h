#pragma once

/* tags used in parsing for separating rule elements*/
#define FIXTOKTAG   '\x1'
#define FIXSTRTAG   '\x2'
#define REPTOKTAG   '\x3'
#define REPSTRTAG   '\x4'
#define USRTOKTAG   '\x5'
#define DMPDATTAG   '\x6'
#define DMPIDATTAG  '\x7'

#define SEGFULL     ('/' + 0x80)        // for seg names the <class,overlay> which follows should be added
#define SEGSHORT    (0x80)              // for seg names the <class,overlay> is omitted

/* tags & flags used for condiitonal processing */
#define IF0TAG          0x80
#define IF1TAG          0x81
#define ELSIFFLAG       4
#define ENDIFFLAG       8

/* tag for reformatting in wider field*/
#define WIDETAG           0x90

extern uint8_t const **rules;
extern uint8_t minRecType;
extern uint8_t maxRecType;


extern char const *enumAlign85[5];
extern char const *enumFixup85[5];
extern char const *enumRelType51[7];
extern char const *enumRefType51[9];
extern char const *enumScope51[7];
extern char const *enumUsage51[7];
extern char const *enumSegType51[6];
extern char const *enumModDat86[5];
char const *enumAlign86[8];
char const *enumCombine86[9];
char const *enumAlign96[4];

void init85();
bool isTrn85(uint8_t trn);
void fmtTrn85(var_t *pvar);
void init51();
bool isTrn51(uint8_t trn);
void fmtTrn51(var_t *pvar);
bool isTrn96(uint8_t trn);
void init96();
void fmtTrn96(var_t *pvar);
void init86();
void printComment86(FILE *fpout);
void printTypedef86(FILE *fpout);