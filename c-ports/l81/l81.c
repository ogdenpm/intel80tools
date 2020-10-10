/*
ml80 reconstructed from binary
*/
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
void showVersion(FILE *fp, bool full);

#define CPMEOF	0x1a


#define ERR_STRTOOLONG	0xf3
#define ERR_TOOMANYMACROS	0xf1
#define ERR_EOF	1
#define ERR_BADNUM	2
#define ERR_TOOBIG	3
#define ERR_SYNTAX	7
#define ERR_TOOMUCHNESTING	0xf2
#define ERR_M81ERROR	0xf5
#define ERR_FATALSYNTAX	0xf7
#define ERR_UNDEF	17
#define ERR_MACROHASPARAMS	18
#define ERR_TOOMANYPARAMS	19
#define ERR_NONNUMASSIGN	20
#define ERR_NONNUMERICEXPR	21

#define YERROR      256
#define IDENTIFIER	257
#define NUMBER	    258
#define STRING	    259
#define ASL	        260
#define TT_ADC	    261
#define TT_SBB	    262
#define TT_SWAP	    263
#define TT_RRC	    264
#define TT_RLC	    265
#define TT_CMP	    266
#define TT_XOR	    267



extern uint16_t lract[], lrpact[];
extern uint8_t llr1[], lrpgo[], lrgo[];
extern uint8_t llr2[];

FILE *inFp, *outFp, *symFp;
char *inFile, *pFile, *sFile;

uint8_t resword[333] = {
    12, 0, 0, 'E','O','F',0,
    13, 0, 0, 'D','E','C','L','A','R','E', 0,
    14, 0, 0, 'I','N','I','T','I','A','L', 0,
    15, 0, 0, 'E','N','D', 0,
    16, 0, 0, 'I','F', 0,
    17, 0, 0, 'T','H','E','N', 0,
    18, 0, 0, 'E','L','S','E', 0,
    19, 0, 0, 'Z','E','R','O', 0,
    20, 0, 0, 'C','Y', 0,
    21, 0, 0, 'P','Y', 0,
    22, 0, 0, 'O','D','D', 0,
    23, 0, 0, 'E','V','E','N', 0,
    24, 0, 0,'P','L','U','S', 0,
    25, 0, 0,'M','I','N','U','S', 0,
    26, 0, 0,'D','A','T','A', 0,
    27, 0, 0,'B','Y','T','E', 0,
    28, 0, 0,'L','A','B','E','L', 0,
    29, 0, 0,'E','X','T','E','R','N','A','L', 0,
    30, 0, 0,'C','O','M','M','O','N', 0,
    31, 0, 0,'D','O', 0,
    32, 0, 0,'B','Y', 0,
    33, 0, 0,'W','H','I','L','E', 0,
    34, 0, 0,'C','A','S','E', 0,
    35, 0, 0,'P','R','O','C','E','D','U','R','E',0,
    36, 0, 0,'R','E','T','U','R','N', 0,
    37, 0, 0,'C','A','L','L', 0,
    38, 0, 0,'G','O','T','O', 0,
    39, 0, 0,'H','L', 0,
    40, 0, 0,'U','N','T','I','L', 0,
    41, 0, 0,'R','E','P','E','A','T', 0,
    42, 0, 0,'H','A','L','T', 0,
    43, 0, 0,'N','O','P', 0,
    44, 0, 0,'D','I','S','A','B','L','E', 0,
    45, 0, 0,'E','N','A','B','L','E', 0,
    46, 0, 0,'B','C', 0,
    47, 0, 0,'D','E', 0,
    48, 0, 0,'S','P', 0,
    49, 0, 0,'S','T','A','C','K', 0,
    50, 0, 0,'P','S','W', 0,
    51, 0, 0,'I','N', 0,
    52, 0, 0,'O','U','T', 0 };

/* PARSER TABLES MASKS */
#define ACTMASK   0xF000
#define SYMBMASK  0x1000
#define SHIFTMASK 0x2000
#define REDMASK   0x3000
#define ERRORMASK 0x0000
#define DEFAULT   0xFFFF

/* PARSER ACTIONS OPCODES */
#define XREDUCE   1
#define XSHIFT    2
#define XACCEPT   3
#define XLINE     4

/* temporaries*/
//uint16_t i;
//uint8_t c = '\t';

/* PARSER GLOBAL VARIABLES */
#define STACKSIZE   50



uint16_t pStack[STACKSIZE];
uint16_t pTop;		/* pad */
// uint8_t parsing = 1;
uint8_t recovering;
uint8_t searching;
uint16_t inputSy;
uint16_t tab;
uint16_t action;
// uint16_t it;
uint8_t errorCount;
#define SYMLSIZE 40000
uint8_t symList[SYMLSIZE];
uint16_t symlnNext;
uint16_t slFirst;
uint16_t hashTab[128];	// let system 0 fill
#define HASHMASK    0x7f
uint8_t hashCode;
uint8_t tokBuf[30];
#define TOKBUFTOP   (sizeof(tokBuf) - 1)
uint16_t tokVal;
uint16_t tokType;
uint8_t tokCont;
uint8_t tokError;
#define tokTop  tokBuf[0]
uint8_t ic = ' ';               // input character
uint8_t nc = ' ';               // next character
uint16_t lrlval;
uint16_t line = 1;
#define SPECIALC    1
#define EOFILE      0

#ifdef _TRACE
#include <conio.h>
uint8_t trace;
#endif

void Exit(int result) {
    puts("End L81\n");
    exit(result);
}

void Putc(uint8_t c) {
    if (putc(c, outFp) != c) {
        fprintf(stderr, "Write error to %s\n", pFile);
        Exit(1);
    }
}

uint8_t Getc() {
    int c;

    if ((c = getc(inFp)) == EOF) {
        if (ferror(inFp) != 0) {
            fprintf(stderr, "Read error %s\n", inFile);
            Exit(errorCount + 1);
        }
        c = CPMEOF;
    }
    return c;
}



char *GetExt(char *file)        // helper function for C-port locate position of .ext or return end of src if none
{
    char *s, *t;
    for (s = file; t = strpbrk(s, "/\\:"); s = t + 1)       // skip directory separators
        ;
    if (t = strrchr(s, '.'))    // we have an extent
        return t;
    else
        return strchr(s, '\0'); // return end of src
}

char *NewExt(char *src, size_t len, char *ext) {   // helper function for C-port
    char *s = malloc(len + strlen(ext) + 1);
    if (s == 0) {
        fprintf(stderr, "Out of memory in NewExt(%s, %zd, %s)\n", src, len, ext);
        exit(1);
    } else {
        strncpy(s, src, len);
        strcpy(s + len, ext);
    }
    return s;
}

void InitFiles(char *src) {     /* open input and output files, using common filename prefix */
    char *s = GetExt(src);

    inFile = NewExt(src, s - src, ".l80");  // add .l80

    if ((inFp = fopen(inFile, "rt")) == NULL) {
        fprintf(stderr, "Cannot open %s\n", inFile);
        Exit(1);
    }
    pFile = NewExt(src, s - src, ".80p");
    if ((outFp = fopen(pFile , "wb")) == NULL) {
        fprintf(stderr, "Cannot create %s\n", pFile);
        Exit(1);
    }
    sFile = NewExt(src, s - src, ".80s");
    if ((symFp = fopen(sFile, "wb")) == NULL) {
        fprintf(stderr, "Cannot create %s\n", sFile);
        Exit(1);
    }
}



bool Blank(uint8_t c) {         /* true if c is whitespace char */
    return c == ' ' || c == '\r' || c == '\n' || c == '\t';
}

bool Numeric(uint8_t c) {       /* true if c is a digit */
    return '0' <= c && c <= '9';
}

bool Alphabetic(uint8_t c) {    /* true if c is a letter or $ */
    return 'A' <= c && c <= 'Z' || 'a' <= c && c <= 'z' || c == '$';
}

bool Alphanumeric(uint8_t c) {  /* true if c is alphanumeric */
    return Alphabetic(c) || Numeric(c);
}

bool Hex(uint8_t c) {           /* true if c is alphanumeric */
    return Numeric(c) || 'A' <= c && c <= 'F';
}

uint8_t HexVal(uint8_t c) {     /* return value of hex character c */
    if (Numeric(c))
        return c - '0';
    return c - 'A' + 10;
}

void KError(uint8_t n) {
    char *errstr;

    switch (n) {
    case 0:	errstr = "syntax error near"; break;
    case 1:	errstr = "unexpected eof"; break;
    case 2: errstr = "mispelled number:";  break;
    case 3: errstr = "number too large:";  break;
    default: errstr = "unkown error"; break;
    }
    ++errorCount;
    printf("\nLine %04X: %s %.*s\n", line, errstr, tokTop, &tokBuf[1]);
}


#define WrPA1   Putc

void WrPA2(uint16_t a) {
    WrPA1(a & 0xff);
    WrPA1(a >> 8);
}


void GetIC() {
    /* get one character from source program into ic;
       look ahead at next character (nc);
       end of file condicitons are denoted (at the exit of getic) by:
       ic=0, nc=0: eof;
       ic<>0, nc=0: almost eof;
       ic<>0, nc<>0: not eof */
    ic = nc;
    if (nc != 0)                /* if not eof get the new nc */
        nc = Getc();
    if (nc == '\n') {           /* new line so put line counter */
        WrPA1(XLINE);
        if ((line & 0xf) != 9)  // increment the number in bcd format
            line++;
        else if ((line & 0xff) != 0x99)
            line += 7;
        else if ((line & 0xfff) != 0x999)
            line += 0x67;
        else
            line += 0x667;

        WrPA2(line);
    }
    if (nc == CPMEOF)
        nc = 0;
}

void PutIC() {                  /* put input character ic into tokbuf */
    tokBuf[++tokTop] = ic;
}

void PgIC() {
    PutIC();
    GetIC();
}

void HashF(uint8_t c) {         /* update global hashcode with the input character c */
    hashCode = (hashCode + c) & HASHMASK;
}

bool Loading() {                /* return true if it is ok to keep on loading tokbuf */
    return tokBuf[0] < TOKBUFTOP && tokCont;
}


void GetId() {
    tokType = IDENTIFIER;
    while (Loading()) {
        HashF(ic);
        PgIC();
        tokCont = Alphanumeric(ic);
    }
}

void GetString() {              /* load a string int tokbuf */
    tokType = STRING;
    while (Loading()) {
        if (ic != '\'') {
            PgIC();
            if (ic == 0) {      /* unexpected eof */
                tokError = 1;
                tokCont = 0;
                return;
            }
        } else {                /* found a quote */
            GetIC();            // skip it
            if (ic != '\'')     // end of string
                tokCont = 0;
            else
                PgIC();         // no so put as single quote
        }
    }
}

bool NhNc() {                   /* true if nc is neither hex nor 'h' */
    return !Hex(nc) && nc != 'H';
}

void GetNum() {                 /* load a number into tokbuf, put its value into tokval */
    unsigned num = 0;

    uint8_t base, d;
    base = 0;
    tokType = NUMBER;
    while (Loading()) {
        PgIC();
        if (ic == 'O' || ic == 'Q')
            base = 8;
        else if (ic == 'H')
            base = 16;
        else if (ic == 'B' && NhNc())
            base = 2;
        else if (ic == 'D' && NhNc())
            base = 10;
        else if (!Hex(ic))
            base = 1;
        if (base > 1)	        /* gobble the char */
            GetIC();
        else if (base == 1)
            base = 10;
        tokCont = base == 0;    /* continue until base is known */
    }
    for (int i = 1; i <= tokTop; i++) {
        if ((d = HexVal(tokBuf[i])) >= base)
            tokError = ERR_BADNUM;
        num = num * base + d;
        if (num >= 0x10000)
            tokError = ERR_TOOBIG;
    }
    tokVal = (uint16_t)num;
}

void GetSpecial() {             /* load a special character into tokbuf */
    tokType = SPECIALC;
    tokVal = 0;
    if (ic == nc)	            /* double chars */
        if (ic == '+')
            tokVal = TT_ADC;
        else if (ic == '-')
            tokVal = TT_SBB;
        else if (ic == '=')
            tokVal = TT_SWAP;
        else if (ic == '>')
            tokVal = TT_RRC;
        else if (ic == '<')
            tokVal = TT_RLC;
        else if (ic == ':')
            tokVal = TT_CMP;
        else if (ic == '\\')
            tokVal = TT_XOR;
    if (tokVal != 0)            // skip one extra character
        PgIC();
    else
        tokVal = ic;
    PgIC();
    tokCont = 0;
}


void GetToken() {
    /* get a token and return the global variables
       toktype:  identifier, string, number, specialc, eofile
       tokval:   value of nubmer, itnernal no. of specialc
       tokerror: 0 if valid token, a message no. otherwise
       tokbuf:   array of characters (1 to toktop) with token name
       toptop:   index of last character in tokbuf
       hashcode: hashcode of identifiers, strings */
    /* ic: input character (already read in, but not yet used)
       nc: next character (look-ahead character) */

    tokError = tokTop = 0;
    if (tokCont == 0) {
        tokVal = 0;
        hashCode = 0;
        tokCont = 1;
        while (1) {                         // look for something
            while (Blank(ic))               // skip blanks
                GetIC();
            if (ic == '/' && nc == '*') {   // a comment
                GetIC();
                GetIC();
                while (ic != '*' || nc != '/') {
                    if (ic == 0) {          // eof - unfinished comment
                        tokType = EOFILE;
                        return;
                    }
                    GetIC();
                }
                GetIC();                    // skip the */
                GetIC();
            } else
                break;                      // got something
        }
        if (ic == 0)                        // EOF
            tokType = EOFILE;
        else if (Alphabetic(ic))
            GetId();
        else if (Numeric(ic))
            GetNum();
        else if (ic == '\'') {
            GetIC();
            GetString();
        } else
            GetSpecial();

    } else if (tokType == IDENTIFIER)
        GetId();
    else if (tokType == STRING)
        GetString();
    else if (tokType == NUMBER)
        GetNum();
    else
        GetSpecial();
}


void SLink(uint16_t i, uint16_t link) {         /* set the link field of symbol i of symlist to l */
    symList[i - 2] = link % 256;
    symList[i - 1] = link / 256;
}

uint16_t Link(uint16_t i) {                     /* return link field of symbol i in symlist */
    return symList[i - 1] * 256 + symList[i - 2];
}


bool Compar(uint8_t *s1, uint8_t *s2) {
    return strcmp(s1, s2) == 0;
}

uint16_t LookupSym(uint16_t k) {
    /* check whether the token(identifier or string) at symlist(k)
       is already in symlist, return pointer to symlist entry if
       yes, 0 otherwise */
    
    for (uint16_t i = hashTab[hashCode]; i > 0; i = Link(i))
        if (Compar(symList + k, symList + i))
            return i;
    return 0;
}

uint16_t SaveTok(uint16_t i) {
    /* move current token to symlist, starting at location i of
       symlist; return pointer to next character in symlist */
    if (tokBuf[0] + i > SYMLSIZE) {
        fprintf(stderr, "Symbol list overflow\n");
        Exit(1);
    }
    for (uint8_t j = 1; j <= tokTop; i++, j++)
        symList[i] = tokBuf[j];

    symList[i] = 0;                 // end marker
    return i;
}

uint16_t LrLex() {                  /* lexical analyzer */
    /* return the internal number of a reserved word, or:
       INDENTIFIER: lrlval = pointer to indentifier in symlist
       STRING:      lrlval = pointer to string in symlist
       NUMBER:      lrlval = value of hte number
       0:           EOFILE */
    static uint8_t regs[] = "BCDEHLMA";
    uint8_t *s;

    GetToken();
    while (tokError != 0) {
        KError(tokError);
        GetToken();
    }
    if (tokType == IDENTIFIER && tokTop == 1 && (s = strchr(regs, tokBuf[1]))) {
        if (*s == 'M')
            return 'M';
        else {
            lrlval = (uint16_t)(s - regs);
            return ASL;
        }
    }
    if (tokType == IDENTIFIER || tokType == STRING) {
        uint16_t k = symlnNext + 2;
        uint16_t j = SaveTok(k);        /* move token to top of symlist */
        while (tokCont != 0) {          // get the remainder of the token
            GetToken();
            j = SaveTok(j);
        }
        lrlval = LookupSym(k);          // search symlist for this token
        if (lrlval != 0) {              // already there
            if (tokType == IDENTIFIER && lrlval < slFirst)
                return symList[lrlval - 3] + 256;
            else
                k = lrlval;             // k points to symbol
        } else {                        // new identifier or string
            SLink(k, hashTab[hashCode]); 
            hashTab[hashCode] = k;
            symlnNext = j + 1;
        }
        /* adjust lrlval since the slfirst bytes of
           symlist won't be passed to l82 */
        lrlval = k - slFirst;
        return tokType;                 // IDENTIFIER or STRING
    }
    lrlval = tokVal;
    if (tokType == NUMBER)	            /* number */
        return NUMBER;
    if (tokType == SPECIALC)	        /* special */
        return tokVal;
    return EOFILE;
}


uint16_t Table(uint16_t s, uint16_t i) { /* return ith entry in the action table of state s */
    return lract[lrpact[s + 1] + i];
}



void Pop(uint8_t n) {                   /* pop n topmost states of the parse stack */ 
    if (pTop < n) {
        fprintf(stderr, "Stack underflow\n");
        Exit(1);
    }
    pTop -= n;
}

void Push(uint16_t s) {
    if (++pTop >= STACKSIZE) {
        fprintf(stderr, "Stack overflow\n");
        Exit(1);
    }
    pStack[pTop] = s;
}

uint16_t GotoF(uint8_t state, uint8_t nonTerm) {        /* return next state after a reduction */
    int i;
    // rpgo[nonTerm] is pointer to goto table in lrgo
    for (i = lrpgo[nonTerm]; lrgo[i] != 0 && lrgo[i] != state; i += 2)
        ;
    return lrgo[i + 1];
}


void Reduce() {
    uint16_t pn;                    // production number
    uint8_t nrs;                    // no. elements right hand side
    uint8_t ls;                     // id. no of left hand side

    pn = tab & 0xfff;
    if (pn != 6)                    /* clear recovery flag if not <stmt> ::= <error> */
        recovering = 0;
    nrs = llr2[pn];
    ls = llr1[pn];
    WrPA1(XREDUCE);
    WrPA1(nrs);
    WrPA1((uint8_t)pn);
#ifdef _TRACE
    if (trace == 'Y') {
        printf("R: %04X,%04X", nrs, pn);
        getche();
    }
#endif
    // update the parse stack
    Pop(nrs);                       /* pop right hand side */
    Push(GotoF((uint8_t)pStack[pTop], ls));  /* next state */
}

void Shift() {                      /* execute a shift action */
    Push(tab & 0xfff);              /* push next state */
    WrPA1(XSHIFT);
    WrPA2(lrlval);
#ifdef _TRACE
    if (trace == 'Y') {
        printf("S: %04X,%04X", inputSy, lrlval);
        getche();
    }
#endif
    inputSy = LrLex();
}

void Accept() {
    WrPA1(XACCEPT);
//    parsing = 0;                    // break the parser loop
}

void Error() {                      /* parsing error handler */
#ifdef _TRACE
    if (trace == 'Y') {
        printf("E. ");
        getche();
    }
#endif
    if (recovering) {               /* already given an error message */
        if (pTop <= 0) {
            KError(0);
            fprintf(stderr, "Unrecoverable error\n");
            Exit(1);
        }
        Pop(1);
        return;
    }
    KError(0);
    inputSy = YERROR;               // <ERROR>
    recovering = 1;
}

void Parse() {                      /* parse source program */
    inputSy = LrLex();              // get first input symbol
    while (1) {
        for (uint16_t it = 0; ; it += 2) {
            tab = Table(pStack[pTop], it);  /* current table entry */
            action = tab & ACTMASK;         /* decode the action */
            if (action == REDMASK)
                Reduce();
            else if (action == ERRORMASK)
                Error();
            else if ((inputSy | SYMBMASK) == tab) { /* entry found for this input */
                tab = Table(pStack[pTop], it + 1);  /* look for corresponding action */
                action = tab & ACTMASK;
                if (action == REDMASK)
                    Reduce();
                else if (action == SHIFTMASK)
                    Shift();
                else {
                    Accept();
                    return;         // return explicit rather than use parsing flag
                }
            } else
                continue;           // with for loop
            break;                  // from for loop
        }
    }
}


void SlDump() {                     /* dump symlist on the sl file */
    /* the portion of symlist containign reserved words
       (bytes 0 through slfirst-1) is not passed to l82 */
    for (uint16_t i = slFirst; i < symlnNext; i++) {
        putc(0, symFp);             // clear link field for l82
        putc(0, symFp);
        i += 2;                     /* index of 1st char of next symbol */
        while (symList[i])
            putc(symList[i++], symFp);
        putc(0, symFp);             /* end of symbol */
    }
    putc(CPMEOF, symFp);            /* end of symbol list */
}


void InitTab() {

    // install the reserved keywords
    symlnNext = slFirst = sizeof(resword);
    memcpy(symList, resword, slFirst);
    /* initialise the hash table, first reserved word starts at location 3 */
    /* skip trailing 0 and 3 info bytes of next entry */
    for (uint16_t j = 3; j < slFirst; j += 4) {
        uint16_t entry = j;
        
        hashCode = 0;
        while (symList[j])
            HashF(symList[j++]);
        SLink(entry, hashTab[hashCode]);        // link in previous hash entry
        hashTab[hashCode] = entry;
    }
}

void main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: l81 [-v] | [-V] | file\n");
        Exit(1);
    }
    if (stricmp(argv[1], "-v") == 0) {
        showVersion(stdout, argv[1][1] == 'V');
        exit(0);
    }
    
    InitTab();

    InitFiles(argv[1]);

    Parse();
    SlDump();
    if (fclose(inFp) != 0) {
        fprintf(stderr, "Error closing source file %s\n", inFile);
        errorCount++;
    }
    if (fclose(outFp) != 0) {
        fprintf(stderr, "Error closing token file %s\n", pFile);
        errorCount++;
    }
    if (fclose(symFp) != 0) {
        fprintf(stderr, "Error closing symbol file %s\n", sFile);
        errorCount++;
    }
    if (errorCount > 0)
        fprintf(stderr, "\nErrors: %d\n", errorCount);
    Exit(errorCount);
}



