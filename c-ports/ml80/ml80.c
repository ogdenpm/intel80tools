
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <stdint.h>
#include <stdbool.h>

void showVersion(FILE *fp, bool full);
/*
      ml80 reconstructed from binary
*/
#define CPMEOF  0x1A

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


// yacc-assigned terminal numbers
#define YERROR      256
#define IDENTIFIER  257
#define NUMBER      258
#define STRING      259



char *inFile, *outFile;
FILE *inFp, *outFp;

/* PARSER TABLES MASKS */
#define ACTMASK   0xF000
#define SYMBMASK  0x1000
#define SHIFTMASK 0x2000
#define REDMASK   0x3000
#define ERRORMASK 0x0000
#define DEFAULT   0xFFFF





#define STACKSIZE   30
uint16_t pStack[STACKSIZE]; // parse stack
uint16_t vStack[STACKSIZE]; // value stack
uint8_t pTop;               // parse stack pointer
uint8_t parsing = { 1 };
uint8_t recovering;
uint8_t searching;
uint16_t inputSy;
uint16_t tab;
uint16_t action;
uint8_t it;
uint16_t errorCount;

/* the main parser tables */
uint16_t lrAct[268] = {
    0, 0x105b, 0x2002, 0, 0x1000, 0x4000, 0x105b, 0x2003,
    0, 0x1100, 0x200b, 0x1101, 0x200d, 0x1108, 0x200c, 0x1109,
    0x2012, 0x110a, 0x2013, 0x110b, 0x2014, 0x110c, 0x2015, 0x110d,
    0x2016, 0x1110, 0x2017, 0, 0x105d, 0x2019, 0, 0x1101,
    0x201a, 0x3003, 0x3004, 0x3005, 0x3006, 0x3007, 0x3008, 0x3009,
    0x1101, 0x201b, 0, 0x105f, 0x201c, 0x3016, 0x1101, 0x2025,
    0x1102, 0x2026, 0x1021, 0x2021, 0x102d, 0x2028, 0x1028, 0x2027,
    0, 0x1101, 0x202a, 0x1103, 0x2029, 0, 0x1103, 0x202b,
    0x3015, 0x300e, 0x300f, 0x3010, 0x3011, 0x1101, 0x202d, 0,
    0x301a, 0x105d, 0x202e, 0, 0x3001, 0x300b, 0x300a, 0x1104,
    0x2031, 0x105c, 0x2030, 0x300d, 0x1026, 0x2032, 0x301b, 0x301e,
    0x3020, 0x1101, 0x2025, 0x1102, 0x2026, 0x102d, 0x2028, 0x1028,
    0x2027, 0, 0x1105, 0x203a, 0x1106, 0x203b, 0x1107, 0x203c,
    0x103d, 0x2037, 0x103c, 0x2038, 0x103e, 0x2039, 0x102b, 0x2035,
    0x102d, 0x2036, 0x3022, 0x102a, 0x203d, 0x102f, 0x203e, 0x1025,
    0x203f, 0x302a, 0x302d, 0x3031, 0x3032, 0x1101, 0x2042, 0x1102,
    0x2026, 0x1021, 0x2021, 0x102d, 0x2028, 0x1028, 0x2027, 0,
    0x1102, 0x2043, 0, 0x3012, 0x3014, 0x3017, 0x1104, 0x2031,
    0x110e, 0x2044, 0x105c, 0x2030, 0, 0x3013, 0x3002, 0x1104,
    0x2031, 0x105c, 0x2030, 0x300c, 0x1101, 0x2025, 0x1102, 0x2026,
    0x1021, 0x2021, 0x102d, 0x2028, 0x1028, 0x2027, 0, 0x1101,
    0x2025, 0x1102, 0x2026, 0x1021, 0x2021, 0x102d, 0x2028, 0x1028,
    0x2027, 0, 0x3021, 0x1101, 0x2025, 0x1102, 0x2026, 0x102d,
    0x2028, 0x1028, 0x2027, 0, 0x1101, 0x2025, 0x1102, 0x2026,
    0x102d, 0x2028, 0x1028, 0x2027, 0, 0x3024, 0x3025, 0x3026,
    0x3027, 0x3028, 0x3029, 0x1101, 0x2025, 0x1102, 0x2026, 0x102d,
    0x2028, 0x1028, 0x2027, 0, 0x1029, 0x204e, 0, 0x1104,
    0x2031, 0x105c, 0x2030, 0x1029, 0x204f, 0, 0x105f, 0x201c,
    0x3031, 0x3035, 0x1103, 0x2050, 0, 0x1026, 0x2032, 0x301c,
    0x1026, 0x2032, 0x301d, 0x301f, 0x102b, 0x2035, 0x102d, 0x2036,
    0x3023, 0x102a, 0x203d, 0x102f, 0x203e, 0x1025, 0x203f, 0x302b,
    0x102a, 0x203d, 0x102f, 0x203e, 0x1025, 0x203f, 0x302c, 0x302e,
    0x302f, 0x3030, 0x3033, 0x3034, 0x110f, 0x2051, 0x3018, 0x1103,
    0x2052, 0, 0x3019, 0xffff };

uint16_t lrPAct[85] = {
    0, 1, 4, 9, 9, 0x1c, 0x1f, 0x22,
    0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x2b, 0x2e,
    0x39, 0x3e, 0x2e, 0x41, 0x42, 0x43, 0x44, 0x45,
    0x48, 0x49, 0x4c, 0x4d, 0x4e, 0x2e, 0x4f, 0x54,
    0x57, 0x58, 0x59, 0x62, 0x73, 0x7a, 0x7b, 0x7c,
    0x7d, 0x88, 0x8b, 0x8c, 0x8d, 0x8e, 0x95, 0x96,
    0x97, 0x9c, 0x9c, 0xa7, 0xb2, 0xb3, 0xbc, 0xbc,
    0xc5, 0xc6, 0xc7, 0xc8, 0xc9, 0xca, 0xcb, 0xcb,
    0xcb, 0xd4, 0xd7, 0xde, 0xe1, 0xe2, 0xe5, 0xe8,
    0xeb, 0xec, 0xf1, 0xf8, 0xff, 0x100, 0x101, 0x102,
    0x103, 0x104, 0x107, 0x10a, 0xffff };

uint8_t lrR1[55] = {
    0, 1, 1, 2, 2, 2, 2, 2, 2, 2, 3, 3, 4, 5, 0xa, 0xa,
    0xa, 0xa, 6, 0xb, 0xb, 7, 0xc, 0xc, 8, 8, 0xd, 9, 9,
    9, 0xe, 0xe, 0xf, 0xf, 0x10, 0x10, 0x12, 0x12, 0x12, 0x12,
    0x12, 0x12, 0x11, 0x11, 0x11, 0x13, 0x13, 0x13, 0x13, 0x14, 0x14,
    0x14, 0x14, 0x14, 0xff };

uint8_t lrR2[55] = {
    0, 3, 4, 1, 1, 1, 1, 1, 1, 1, 2, 2, 3, 2, 1, 1, 1,
    1, 2, 2, 2, 1, 1, 2, 4, 6, 1, 1, 3, 3, 1, 3, 1, 2,
    1, 3, 1, 1, 1, 1, 1, 1, 1, 3, 3, 1, 3, 3, 3, 1, 1,
    3, 3, 2, 0xff };

uint16_t lrGo[72] = {
    0, 0xffff, 1, 3, 0x18, 0xffff, 4, 0xffff, 5, 0x27, 0x40,
    0xffff, 6, 0xffff, 7, 0xffff, 8, 0xffff, 9, 0xffff,
    0xa, 0x11, 0x2c, 0x1c, 0x2f, 0x27, 0x41, 0xffff, 0x1d, 0xffff,
    0xe, 0xffff, 0xf, 0xffff, 0x10, 0xffff, 0x11, 0x30, 0x45,
    0x31, 0x46, 0xffff, 0x1e, 0x32, 0x47, 0xffff, 0x1f, 0x21,
    0x33, 0xffff, 0x20, 0x34, 0x48, 0xffff, 0x22, 0xffff, 0x34,
    0x35, 0x49, 0x36, 0x4a, 0xffff, 0x23, 0x3d, 0x4b, 0x3e, 0x4c,
    0x3f, 0x4d, 0xffff, 0x24, 0xffff };

uint16_t lrPGo[22] = {
    0, 1, 3, 7, 9, 0xd, 0xf, 0x11, 0x13, 0x15, 0x1d, 0x1f, 0x21,
    0x23, 0x25, 0x2b, 0x2f, 0x33, 0x37, 0x39, 0x3f, 0xffff };

// hash table
uint16_t hashTab[128];
#define HASHMASK    0x7f
uint8_t hashCode;

// lexical analyzer global variables
uint8_t tokBuf[30];
uint16_t tokVal;
uint16_t tokType;

#define TOKBUFTOP   (sizeof(tokBuf) - 1)
#define TOKTOP      tokBuf[0]           // correct indexing

uint8_t tokCont;
uint8_t tokError;
uint8_t ic = { ' ' };                // input character
uint8_t nc = { ' ' };                // next character
uint16_t ncP;                    // poiinter to nc in macro table space or input
uint16_t lrLVal;
uint16_t line = { 1 };
#define SPECIALC    1
#define EOFILE      0
uint8_t outside = { 1 };             // 0 when scanning inside macro calls
uint8_t count;                     // no. unmaged mbegin's inside string

// macro text stack
#define MTSSIZE 0x8000          // size of mts, note size <= 0x8000 as index > 0x8000 used to flag new entry

uint8_t mts[MTSSIZE];              // replaces mtsb address, mts based mtsb(1) byte

uint16_t tTop = { 0x32 };          // just past reserved word list

// macro descriptor stack
#define MDSSIZE 180
uint16_t mds[MDSSIZE];
uint16_t dTop = { 0xffff };        // top of MDS
uint16_t rTop;                   // pointer to topmost reserverd macro in MDS

// macro types
#define MNUM    1                   // numeric
#define MMAC    2                   // textual

    // status stack
#define STSSIZE     50
uint16_t qtSave[STSSIZE];        // ttop save value
uint16_t qdSave[STSSIZE];        // dtop save value
uint8_t qcSave[STSSIZE];           // nc save value
uint16_t qnSave[STSSIZE];        // ncp save value
uint8_t qTop = { 0xff };             // status stack top pointer

// variables associated with the left hand side of production
uint16_t hh;
// varaibles associated with handles
uint16_t h1, h2, h3, h4, h5, h6;
#define l1  (h1 & 0xff)
#define l2  (h2 & 0xff)
#define l3  (h3 & 0xff)
#define l4  (h4 & 0xff)
#define l5  (h5 & 0xff)
#define l6  (h6 & 0xff)

// global variables for semantic actions
uint8_t hSave;                     // hascode save value
uint16_t dSave;                  // save value for dTop
uint16_t tSave;                  // save value for tTop
uint16_t ttSave;                 // yet another save value for tTop
uint8_t nf;                        // no. formal parameters
uint8_t na;                        // no. actual parameters
uint8_t h;                         // temporaries
uint16_t i;
uint16_t j;


#ifdef _TRACE
#include <conio.h>
uint8_t tracing;
uint8_t z;
#define TRACE(n)    Trac(n)
#else
#define TRACE(n)
#endif


// literals for relations
enum { EQ = 1, LT, GT, NE, LE, GE };

bool Neg(uint16_t a) {          // true if a negative
    return a > 0x7fff;
}

int WriteError() {              // helper function for C-port
    fprintf(stderr, "Error writing to %s\n", outFile);
    fprintf(stderr, "Abend m81\n");
    exit(1);
    return 0;		// never gets here but allows simple fprint test
}

void Putc(uint8_t ch) {         // putc with check
    if ((putc(ch, outFp)) == EOF)
        WriteError();
}

uint8_t Getc() {                // getc with check for EOF and line count
    int c;
    if ((c = getc(inFp)) == EOF)
        return CPMEOF;
    if (c == '\n')
        line++;
    return c;
}

void Exit() {                   // all done
    printf("\nEnd M81\n");
    exit(0);
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
    if (*s)
        inFile = strdup(src);   // already has extension
    else
        inFile = NewExt(src, s - src, ".m80");  // add .m80
    outFile = NewExt(src, s - src, ".l80");     // create the .l80 file

    if ((inFp = fopen(inFile, "rt")) == NULL) {
        fprintf(stderr, "Cannot open %s\n", inFile);
        Exit();
    }
    if ((outFp = fopen(outFile, "wt")) == NULL) {
        fprintf(stderr, "Cannot create %s\n", outFile);
        Exit();
    }
    line = 1;
}

void Terminate()                // finalise output file
{
    Putc(CPMEOF);
    if (fclose(outFp) != 0)
        fprintf(stderr, "Cannot close %s\n", outFile);
    fclose(inFp);
    printf("Errors: %d\n", errorCount); // log error count
    Exit();
}


void Error(uint8_t n) {         // log error with line number
    errorCount++;
    if (fprintf(outFp, "\n*** Line %d: error %02X\n", line, n) < 0)
        WriteError();
    if ((n & 0xf0) == 0xf0)     // done if fatal error
        Terminate();
}

bool Blank(uint8_t c) {         // true if c is whitespace char
    return c == ' ' || c == '\r' || c == '\n' || c == '\t';
}

bool Numeric(uint8_t c) {       // true if c is a digit
    return '0' <= c && c <= '9';
}

bool Alphabetic(uint8_t c) {    // true if c is a latter or $
    return 'A' <= c && c <= 'Z' || 'a' <= c && c <= 'z' || c == '$';
}

bool AlphaNumeric(uint8_t c) {  // true is c is alphanumeric
    return Alphabetic(c) || Numeric(c);
}

bool Hex(uint8_t c) {           // true if c is a hex character
    return Numeric(c) || 'A' <= c && c <= 'F';
}

uint8_t HexVal(uint8_t c) {     // return hex value of hex character c
    if (Numeric(c))
        return c - '0';
    return c - 'A' + 10;
}

void GetIC() {
    /*
        get one character from source program into ic;
        look ahead at next character (nc);
        end of file condicitons are denoted (at the exit of getic) by:
        ic=0, nc=0: eof;
        ic<>0, nc=0: almost eof;
        ic<>0, nc<>0: not eof
    */
    ic = nc;
    // now get the new nc
    if (nc == 0)
        return;
    if (ncP == 0) {         // get it form source file
        if ((nc = Getc()) == CPMEOF)
            nc = 0;
    } else {
        if (++ncP >= MTSSIZE)
            Error(ERR_STRTOOLONG);
        else
            nc = mts[ncP];
    }
}

void PutIC() {              /* put input character ic into the tokbuf */
    tokBuf[++tokBuf[0]] = ic;
}

void PgIC() {
    PutIC();
    GetIC();
}

void HashF(uint8_t c) {     /* update global hashcode with the input character c */
    hashCode = (hashCode + c) & HASHMASK;
}

bool Loading() {            /* return true if it is ok to keep on loading tokbuf */
    return tokBuf[0] < TOKBUFTOP && tokCont;
}


void GetId()    // load an identifier into the tokBuf
{
    tokType = IDENTIFIER;
    while (Loading()) {
        HashF(ic);
        PgIC();
        tokCont = AlphaNumeric(ic);
    }
}

void GetStr() {                     /* load a string into tokbuf */
    tokType = STRING;
    while (Loading()) {
        if (ic != '\'') {
            if (ic == '[')
                count++;
            if (ic == ']')
                count--;
            PgIC();
            if (ic == 0) {          // unexpected EOF
                tokError = ERR_EOF;
                tokCont = 0;
                return;
            }
        } else                      // found a quote
            if (count == 0) {       //  not inside a macrocall
                GetIC();            // skip hte quote
                if (ic != '\'')
                    tokCont = 0;    // end of string
                else
                    PgIC();         // quoted string
            } else                  // inside macrocall
                PgIC();             // continue
    }
}

bool NhNc() {                       /* true if nc is neither hex nor 'h' */
    return (!Hex(nc)) && nc != 'H';
}

void GetNum() {                     /* load a number into tokbuf, put its value into tokval */
    unsigned long num = 0;
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
        if (base > 1)	            /* gobble the char */
            GetIC();
        else if (base == 1)
            base = 10;
        tokCont = base == 0;        // continue until base is known
    }
    for (int i = 1; i <= TOKTOP; i++) {     // compute the value
        if ((d = HexVal(tokBuf[i])) >= base)
            tokError = ERR_BADNUM;
        num = num * base + d;
        if (num >= 0x10000)         // overflow check
            tokError = ERR_TOOBIG;
    }
    tokVal = (uint16_t)num;
}

void GetSpecial() {
    tokType = 1;
    tokVal = 0;
    if (ic == '\\' && nc == '\\')       // XOR
        tokVal = 260;
    else if (ic == '<' && nc == '>')    // NE
        tokVal = 261;
    else if (ic == '<' && nc == '=')    // LE
        tokVal = 262;
    else if (ic == ':' && nc == '=')    // ASSIGN
        tokVal = 0x5f;
    else if (ic == '>' && nc == '=')    // GE
        tokVal = 263;
    if (tokVal != 0)
        PgIC();                         // skip one extra character
    else
        tokVal = ic;                    // single special character
    if (ic == ']') {                    // do not read next char outside macrocall
        PutIC();
        outside = 1;
    } else
        PgIC();                         // read the next character
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
    uint8_t something;
    tokError = tokBuf[0] = 0;
    if (tokCont == 0) {
        tokVal = 0;
        something = hashCode = 0;
        count = 0;
        tokCont = 1;
        while (something == 0) {
            while (Blank(ic))                       // skip blanks
                GetIC();
            if (ic == '/' && nc == '*') {           // comment
                GetIC();
                GetIC();
                while (ic != '*' || nc != '/') {    // end comment
                    if (ic == 0) {
                        tokType = 0;
                        return;
                    }
                    GetIC();
                }
                GetIC();
                GetIC();
            } else
                something = 1;
        }
        if (ic == 0)
            tokType = EOFILE;
        else if (Alphabetic(ic))
            GetId();
        else if (Numeric(ic))
            GetNum();
        else if (ic == '\'') {
            GetIC();
            GetStr();
        } else
            GetSpecial();

    } else if (tokType == IDENTIFIER)
        GetId();
    else if (tokType == STRING)
        GetStr();
    else if (tokType == NUMBER)
        GetNum();
    else
        GetSpecial();
}

#ifdef _TRACE
void Trac(uint8_t i) {          // tracing procedure
    uint16_t k;
     if (tracing != 'Y')
        return;
    putchar(i);
    while ((z = getche()) != ' ') {
        putchar('\n');
        if (z == 'C')
            printf("%c%c,%d", ic, nc, ncP);
        if (z == 'N')
            printf(" %d,%d,%d,%d", h1, h2, h3, h4);
        if (z == 'H')
            for (i = 0; i < sizeof(hashTab) / sizeof(hashTab[0]); i++)
                if (hashTab[i])
                    printf(" %d,%d", i, hashTab[i]);

        if (z == 'D')
            for (i = 0; i <= dTop; i++)
                printf(" %d,%d", i, mds[i]);
        if (z == 'Q')
            for (i = 0; i <= qTop; i++)
                printf(" %d,%d %d %c %d", i, qtSave[i], qdSave[i], qcSave[i], qnSave[i]);
        if (z == 'P')
            for (i = 0; i <= pTop; i++)
                printf(" %d,%d", pStack[i], vStack[i]);
        if (z == 'T')
            for (k = 0; k <= tTop; k++) {
                z = mts[k];
                printf(AlphaNumeric(z) ? " %c" : " %02X", z);
            }
    }
}

#endif

bool Compar(uint8_t *s1, uint8_t *s2) {     // string compare 1 if same
    return strcmp(s1, s2) == 0;
}

void PushD(uint16_t i)          // push i into the macro descriptor stack
{
    if (++dTop >= MDSSIZE)
        Error(ERR_TOOMANYMACROS);
    else
        mds[dTop] = i;
}



uint8_t NType(uint16_t i)      /* type of macro i*/
{
    return mds[i] & 0xff;
}

uint8_t NFp(uint16_t i)        // number of formal parameters of macro i
{
    return mds[i] >> 8;
}

uint16_t TName(uint16_t i)      // indexc in mds of name cell of textual macro i
{
    return i - NFp(i) - 3;
}

uint16_t IFp(uint8_t n, uint16_t i)     // index in mts of nth formal param of macro i
{
    return mds[TName(i) + n];
}


uint16_t MLink(uint16_t i)          // link field of macro i
{
    return mds[i - 1];
}

bool MNumeric(uint16_t i)          // true if macro i is of type numeric
{
    return NType(i) == MNUM;
}

bool Reserved(uint16_t i)        // true if  macro i is a reserved word
{
    return i <= rTop;
}


uint16_t JName(uint16_t i)      // index of cell in mds containing pointer to name of macro i
{
    if (MNumeric(i))
        return i - 3;
    else if (Reserved(i))
        return i - 2;
    return TName(i);
}

uint16_t IName(uint16_t i)      // index of name in macro i in mts
{
    TRACE('M');
    return mds[JName(i)];
}

void PushSt() {                 /* push status quo (nc, ncp, tsave, dsave) into the status stack */
    TRACE('+');
    if (++qTop >= STSSIZE)
        Error(ERR_TOOMUCHNESTING);
    qtSave[qTop] = tSave;
    qdSave[qTop] = dSave;
    qcSave[qTop] = nc;
    qnSave[qTop] = ncP;
    outside = 1;                // begin to scan a macrobody in 'outside' mode
}

void PopSt() {
    /* exit from scanning a macrobody: pop previous status from
       the status stack and set things for the scanner */
    TRACE('-');
    if (qTop == 255)	/* empty scak, the macrobody just scanned was the source  program itself */
        Terminate();
    /* else: restore previous status */
    /* restore the hashtable */
    while (dTop > qdSave[qTop]) {   /* set hashtable entry pointing to next macro */
        hashTab[mts[IName(dTop) - 1]] = MLink(dTop);
        dTop = JName(dTop) - 1;     /* index of next macro to pop */
    }
    tTop = qtSave[qTop];
    ncP = qnSave[qTop];
    nc = qcSave[qTop];
    qTop = qTop - 1;
    outside = 1;                    // get ready for other macrocalls
}

uint16_t MIdent(uint16_t j)         // idenfity macro whoes name is at mts[j]
{
    uint8_t h;
    uint16_t i, k;

    h = mts[j - 1];
    k = hashTab[h];                 // hashcode

    while (k != 0) {                // search
        i = IName(k);               // index of macroname in mts
        if (Compar(j + mts, i + mts))
            return k;               // found
        k = MLink(k);
    }
    return 0;
}

uint16_t SaveTok(uint16_t i) {
    /* move current token to mts, starting at mts(i) */
    /* return pointer to next character in mts */
    uint8_t j;
    if (tokBuf[0] + i > MTSSIZE)
        Error(ERR_STRTOOLONG);
    for (j = 1; j <= tokBuf[0];)
        mts[i++] = tokBuf[j++];

    mts[i] = 0;                     // end marker
    return i;
}

uint16_t LRLex() {  /* lexical analyzer */
    /* return the internal number of a reserved word, or:
       indentifier: lrlval = index of macro in mds, or
            -pointer to name in mts, if id is not a macroname
       string:     lrlval = pointer to string in mts
       number:     lrlval = value of hte number
       0:     eofile */
    uint16_t j;
    while (outside) {
        GetIC();
        if (ic == '[') {            // begin a macrocall
            outside = 0;            // enter macrocall
            GetIC();                // skip the [
            return '[';
        }
        if (ic == 0)                // end of macrobody
            PopSt();                // pop status, go on scanning
        else
            Putc(ic);               // write ic on output file
    }
    GetToken();
    while (tokError != 0) {
        Error(tokError);
        GetToken();
    }
    lrLVal = tokVal;
    if (tokType == SPECIALC)        /* special */
        return tokVal;
    if (tokType == NUMBER)          /* number*/
        return NUMBER;
    if (tokType == EOFILE)          /* eof */
        return EOFILE;
    ttSave = tTop;                  // save value of tTop
    lrLVal = tTop + 2;
    j = SaveTok(lrLVal);            // mvove token to mts
    while (tokCont != 0) {          // get remainder of token
        GetToken();
        j = SaveTok(j);
    }
    mts[lrLVal - 1] = hashCode;     // store hashcode
    tTop = j;                       // push the name into mts
    if (tokType == STRING)
        return STRING;
    j = MIdent(lrLVal);
    // is identifier - check whether reserved word
    if (j == 0) {                   // not there
        lrLVal = -lrLVal;           // -pointer to id in mts
        return IDENTIFIER;
    }
    tTop = lrLVal - 2;	            /* discard the name */
    if (Reserved(j))                // reserved word ?
        return mds[j];              // return word no.
    lrLVal = j;                     // index of the macro in mds
    return IDENTIFIER;
}

void LookAt(uint16_t i) {           /* tell scanner to scan text beginning at mts(i) */
    ncP = i;
    nc = mts[i];
}


void PushH2() {                     /* create entry in mds for macroname at handle(2) */
    TRACE('X');
    if (Neg(h2))
        h2 = -h2;                   // new identifier
    else                            // identifier is a macroname
        h2 = IName(h2);             // pointer to name in mts
    TRACE('Y');
    PushD(h2);
}

void InitDecl() {
    /* semantic actions for the productions
       <INT.DECL> ::= INT <IDENTIFIER>
       <INT.DECL> ::= <INT.DECL> <INDENTIFIER> */
    uint8_t h;
    // create numeric macro
    PushH2();                       // push ponter to macroname into mds
    PushD(0);                       // initialize its value to 0
    h = mts[h2 - 1];                // get identifier's hashcode
    PushD(hashTab[h]);              // set link
    PushD(1);                       // 0 formal params, type = numeric macro
    hashTab[h] = dTop;              // pointer to the new macro
}

void Xmit1() {
    hh = h1;
}

void LRAction(uint8_t n) {    /* execute semantic action associated with ppoduction n */
    if (n < 10)               /* no action to be performed */
        return;
    switch (n) {
        /*** <PROGRAM> ::= <STMT> */
        /*** <PROGRAM> ::= <PROGRAM> <STMT> */
        /*** <STMT> ::= <INT.DECL> */
        /*** <STMT> ::= <ASSIGN.STMT> */
        /*** <STMT> ::= <EVAL.STMT> */
        /*** <STMT> ::= <MACRO.DECL> */
        /*** <STMT> ::= <MACRO.CALL> */
        /*** <STMT> ::= <IF.STMT> */
        /*** <STMT> ::= <ERROR> */

    case 10:	InitDecl();     /*** <INT.DECL> ::= INT <IDENTIFIER> */
        break;
    case 11:	InitDecl();     /*** <INT.DECL> ::= <INT>DECL> <IDENTIFIER> */
        break;
    case 12:                    /*** <ASSIGN.STMT> ::= <IDENTIFIER> = <EXPR> */
        if (Neg(h1))            // undefined macro
            Error(ERR_UNDEF);
        else if (MNumeric(h1))  // numeric macro
            mds[h1 - 2] = h3;   // execute assignment
        else
            Error(ERR_NONNUMASSIGN);    // cannot assign
        hh = h3;                //  propagate value of <expr>
        break;
    case 13:                    /*** <EVAL.STMT> ::= <FORMAT> <EXPR> */
        if ((h1 & 0xff) == 10)
            fprintf(outFp, "%d", (short)h2) >= 0 || WriteError();
        else if ((h1 & 0xff) == 8)
            fprintf(outFp, "%oQ", h2) >= 0 || WriteError();
        else if ((h1 & 0xff) == 22)
            Putc((uint8_t)h2);
        else
            fprintf(outFp, "0%04XH", h2) >= 0 || WriteError();
        break;
    case 14: hh = 10; break;    /*** <FORMAT> ::= DEC */
    case 15: hh = 8; break;     /*** <FORMAT> ::= OCT */
    case 16: hh = 16; break;    /*** <FORMAT> ::= HEX*/
    case 17: hh = 22; break;    /*** <FORMAT> ::= CHAR */
    case 18:                    /*** <MACRO.DECL> ::= <MD.HEAD> <STRING> */
                                // create macrobody = <string>; install new macro
        PushD(h2);              // pointer to macrobody
        PushD(hashTab[hSave]);  // set link field
        PushD((nf << 8) | 2);   // no. formals, type
        hashTab[hSave] = dTop;  // pointer to macro descriptor
        break;
    case 19:                    /*** <MD.HEAD> ::= MACRO <IDENTIFIER> */
                                // create macro name
        PushH2();               // hr=pointer to identifier in mts
        hSave = mts[h2 - 1];    // save its hashcode
        nf = 0;                 // no. formal parameters
        break;
    case 20:                    /*** <MD.HEAD> ::= <MD.HEAD> <IDENTIFIER> */
        PushH2();               // h2 = pointer to identifier in mts
        nf = nf + 1;            // another formal parameter
        break;
    case 21:                    /*** <MACRO.CALL> ::= <MD.HEAD> */
                               // temporary macros have already been created
                               // status has been saved in dsave, tsave
        if (!Neg(h1)) {        // no errors
            // value of numeric macros, pointer to body of textual macros
            i = mds[h1 - 2];
            if (MNumeric(h1))   // numeric value
                fprintf(outFp, "%d", (short)i) >= 0 || WriteError();
            else {
                PushSt();       // texual macro, save status quo
                LookAt(i);      // scan macrobody
            }
        }
        break;
    case 22:                    /*** <MACRO.HEAD> ::= <IDENTIFIER> */
        if (Neg(h1))            // undefined macro
            Error(ERR_UNDEF);
        dSave = dTop;           // save status quo
        tSave = ttSave;
        na = 0;                 // no. actual parameters
        Xmit1();
        break;
    case 23:                    /*** <MACRO.HEAD> ::= <MACRO.HEAD> <STRING> */
        // create temporary macro, with
        // macroname = formal param, macrobody = actrual param
        if (!Neg(h1)) {         // no errors
            hh = 0;
            na = na + 1;
            if (MNumeric(h1))   // numedic macro cannot have parameters
                Error(ERR_MACROHASPARAMS);
            else if (NFp(h1) < na)  // too many parameters
                Error(ERR_TOOMANYPARAMS);
            else
                Xmit1();
            if (!Neg(hh)) {
                j = IFp(na, h1);    // j = pointer to corresponding formal parameter in mts
                PushD(j);           // formal param becomes macroname
                PushD(h2);          // macrobody = string
                h = mts[j - 1];     // hashcode of new macroname
                PushD(hashTab[h]);  // set link
                PushD(2);           // formal params, type = text
                hashTab[h] = dTop;  // pointer to new macro
            }
        }
        break;
    case 24:                    /*** <IF.STMT> ::= <IF> <EXPR> THEN <STRING> */
        if (h2) {               // <expr> is true
            PushSt();           // save status quo
            LookAt(h4);         // tell scanner ot scan string
        }
        break;
    case 25:                    /*** <IF.STMT> ::= <IF> <EXPR> THEN <STRING> ELSE <STRING> */
        PushSt();               // save status quo
        if (h2)                 // <expr> is true
            LookAt(h4);         // scan 1st string
        else
            LookAt(h6);         // scan 2nd string
        break;
    case 26:                    /*** <IF> ::= IF */
        dSave = dTop;
        tSave = tTop;
        break;
    case 27: Xmit1(); break;    /*** <EXPR> ::= <LOG.FACTOR> */

    case 28: hh = h1 | h3; break;   /*** <EXPR> ::= <EXPR> OR <LOG.FACTOR> */
    case 29: hh = h1 ^ h3; break;   /*** <EXPR> ::= <EXPR> XOR <LOG.FACTOR> */
    case 30: Xmit1(); break;        /*** <LOG.FACTOR> ::= <LOG.SEC> */
    case 31: hh = h1 & h3; break;   /*** <LOG.FACTOR> ::= <LOG.FACTOR> AND <LOG.SEC> */
    case 32: Xmit1(); break;        /*** <LOG.SEC> ::= <LOG.PRIM> */
    case 33: hh = !h2; break;       /*** <LOG.SEC> ::= NOT <LOG.PRIM> */
    case 34: Xmit1(); break;        /*** <LOG.PRIM> ::= NOT <ARIT.EXPR> */
    case 35:                        /*** <LOG.PRIM> ::= <ARIT.EXPR> <REL> <ARIT.EXPR> */
    {
        bool ll;
        if ((h2 & 0xff) == 1)
            ll = h1 = h3;
        else if ((h2 & 0xff) == 2)
            ll = Neg(h1 - h3);
        else if ((h2 & 0xff) == 3)
            ll = Neg(h3 - h1);
        else if ((h2 & 0xff) == 4)
            ll = h1 != h3;
        else if ((h2 & 0xff) == 5)
            ll = (h1 == h3) || Neg(h1 - h3);
        else
            ll = (h1 == h3) || Neg(h3 - h1);
        hh = ll;
    }
    break;
    case 36: hh = 1; break;         /*** <REL> ::= = */
    case 37: hh = 2; break;         /*** <REL> ::= < */
    case 38: hh = 3; break;         /*** <REL> ::= > */
    case 39: hh = 4; break;         /*** <REL> ::= <> */
    case 40: hh = 5; break;         /*** <REL> ::= <= */
    case 41: hh = 6; break;         /*** <REL> ::= >= */
    case 42: Xmit1(); break;        /*** <ARIT.EXPR> ::= <TERM> */
    case 43: hh = h1 + h3; break;   /*** <ARIT.EXPR> ::= <ARIT.EXPR> + <TERM> */
    case 44: hh = h1 - h3; break;   /*** <ARIT.EXPR> ::= <ARIT.EXPR> - <TERM> */
    case 45: Xmit1(); break;        /*** <TERM> ::= <PRIM> */
    case 46: hh = h1 * h3; break;   /*** <TERM> ::= <TERM> * <PRIM> */
    case 47: hh = h1 / h3; break;   /*** <TERM> ::= <TERM> / <PRIM> */
    case 48: hh = h1 % h3; break;   /*** <TERM> ::= <TERM> MOD <PRIM> */
    case 49:                        /*** <PRIM> ::= <IDENTIFIER> */
        if (Neg(h1))                // undefined macro
            Error(ERR_UNDEF);
        else if (MNumeric(h1))      // numeric macro
            hh = mds[h1 - 2];
        else
            Error(ERR_NONNUMERICEXPR);
        break;
    case 50: Xmit1(); break;        /*** <PRIM> ::= <NUMBER> */
    case 51: hh = h2; break;        /*** <PRIM> ::= <ASSIGN.STMT> */
    case 52: hh = h2; break;        /*** <PRIM> ::= ( <EXPR> ) */
    case 53: hh = -h2; break;       /*** <PRIM> ::=  - <NUMBER> */
    }
} /* LRAction */

uint16_t Table(uint16_t s, uint16_t i) {    /* return ith entry in the action table of state s */
    return lrAct[lrPAct[s + 1] + i];
}

void Pop(uint8_t n) {                       /* pop n topmost states of the parse stack */
    if ((pTop = pTop - n) < 0)
        Error(ERR_M81ERROR);
}


void Push(uint16_t s, uint16_t v) {         /* push state s and value v into the parse stacks */
    if ((pTop = pTop + 1) >= 30)
        Error(0xf4);
    pStack[pTop] = s;
    vStack[pTop] = v;
}


uint16_t GotoF(uint16_t state, uint16_t nonTerm) {  /* return next state after a reduction */
    i = lrPGo[nonTerm];                     // pointer to goto table in LRGo
    while (1) {                             // table search
        if (lrGo[i] == 0xffff || lrGo[i] == state)
            return lrGo[i + 1];
        i = i + 2;
    }
}


void Reduce() {                             // execute a reduction
    uint8_t pn;     // production number
    uint8_t nrs;    // no. elements right hand side
    uint16_t ls;    // id no. of left hand side
    uint16_t *h;    // handle base (replaces declare hb address, h based hb(7) address
    pn = tab & 0xfff;
    /* clear recovery flag unless <STMD> :: = <ERROR> */
    if (pn != 9)
        recovering = 0;
    nrs = lrR2[pn];
    ls = lrR1[pn];
    h = &vStack[pTop - nrs];
    h1 = h[1];
    h2 = h[2];
    h3 = h[3];
    h4 = h[4];
    h5 = h[5];
    h6 = h[6];
    TRACE('R');
    LRAction(pn);       /* excute semantic action */
    /* update the parse stack */
    Pop(nrs);           /* pop right hand side */
    Push(GotoF(pStack[pTop], ls), hh);  // next state
    TRACE('A');
} /* Reduce() */


void Shift() {          // excute a shift action
    TRACE('S');
    Push(tab & 0xfff, lrLVal);  // push next state
    inputSy = LRLex();          // get another input symbol
} /* Shift() */

void Accept() {         // break parser loop
    parsing = 0;
} /* Accept() */

void PError() {         // parsing error handler
    TRACE('E');
    if (recovering) {   // error message for this error already given
        if (pTop <= 0)  // search for a state with shift on <ERROR>
            Error(ERR_FATALSYNTAX);
        Pop(1);
        return;
    }
    Error(ERR_SYNTAX);
    inputSy = YERROR;   // <ERROR>     
    recovering = 1;
} /* PError() */

void Parse() {                          /* parse source program */
    inputSy = LRLex();                  // get first input symbol
    while (parsing) {
        it = 0;                         // index within action table of current state
        searching = 1;
        while (searching) {             // identify applicable action
            searching = 0;
            tab = Table(pStack[pTop], it);  // curren table entry
            action = tab & ACTMASK;     // decode the action
            if (action == REDMASK)
                Reduce();
            else if (action == ERRORMASK)
                PError();
            else if ((inputSy | SYMBMASK) == tab) { // entry found for this input
                it = it + 1;            // look for corresponding action
                tab = Table(pStack[pTop], it);
                action = tab & ACTMASK;
                if (action == REDMASK)
                    Reduce();
                else if (action == SHIFTMASK)
                    Shift();
                else
                    Accept();
            } else {                    // no mach
                it = it + 2;            // keep on searching
                searching = 1;
            }

        }
    }
} /* Parse() */

uint8_t reservedWords[49] = {
    8, 'I', 'N', 'T', 0,
    9, 'D', 'E', 'C', 0,
    10, 'O', 'C', 'T', 0,
    11, 'H', 'E', 'X', 0,
    12, 'C', 'H', 'A', 'R', 0,
    13, 'M', 'A', 'C', 'R', 'O', 0,
    14, 'T', 'H', 'E', 'N', 0,
    15, 'E', 'L', 'S', 'E', 0,
    16, 'I', 'F', 0 };

void InitTab() {
    memcpy(mts, reservedWords, sizeof(reservedWords));  // copy over the reserved words
    // hash table doesn't need reset since C initialises to 0
    // initilaise the mts
    j = 1;                              // index of rirst reserved word in mts
    while (j < tTop) {
        hashCode = 0;                   // compute hash code for word
        i = j;
        while (mts[i] != 0)
            HashF(mts[i++]);
        PushD(j);                       // pointer to macro name
        PushD(hashTab[hashCode]);       // link
        PushD(mts[j - 1] + 0x100);      // reserved word no.
        mts[j - 1] = hashCode;          // store hashcode in mts
        hashTab[hashCode] = dTop;       // pointer to the macro created
        j = i + 2;                      // next
    }
    rTop = dTop;                        // save index of topmost reserved macro
} /* InitTab */

void main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: ml80 [-v] | [-V] | file\n");
        Exit();
    }
    if (stricmp(argv[1], "-v") == 0) {
        showVersion(stdout, argv[1][1] == 'V');
        exit(0);
    }
    InitTab();
    InitFiles(argv[1]);
    Parse();
}
