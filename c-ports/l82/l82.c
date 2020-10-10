
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
void showVersion(FILE *fp, bool full);


#ifdef _DBGRAM              // define to display grammar productions on reduce
void printg(uint16_t n);
#endif

#define RELTSIZE	30
#define STACKSIZE	30
#define SYMTSIZE	254
#define MAXBLOCKDEPTH	10
/* MNEMONICS FOR 'TYPE' */
#define KLABEL 1
#define KBYTE  2
#define PROC   3
#define EXT    4
#define GLOB   5
#define STRING 7
#define UNDECL 6



#define SRELOC	0
#define CA	1
#define IDA	2
#define WA	3
#define UNDEF	4
#define UNUSED	5


/* PARSER ACTIONS OPCODES */
#define XREDUCE 1
#define XSHIFT  2
#define XACCEPT 3
#define XLINE   4


#define TREG	0
#define TLABEL	1
#define EXT	4
#define GLOB	5
#define TSTRING	0x11
#define TNUMBER	0x13
#define TADDRVAR	0x15
#define TADDRSTR	0x17
#define TMXX	0x21
#define TVAR	0x23
#define TIN	0x25
#define TOUT	0x27
#define TMSTRING	0x29
#define TMNUMBER	0x2b
#define TMDOTADDRVAR	0x2d
#define TMDOTADDRSTR	0x2f
#define TMHL	6
#define TA	7
#define TBC	0xf
#define TDE	0x1f
#define THL	0x2f
#define TSP	0x3f
#define TSTACK	0x2e
#define TPSW	0x3e
#define TCY	0x1e


FILE *fpPol;
FILE *fpCode;
FILE *fpData;
FILE *fpReloc;
char *sFile, *cFile, *dFile, *rFile;
char *fname, *pFile;
uint8_t *symList;
int errcnt;

uint16_t psh[STACKSIZE];         // handle stack
uint8_t psx[STACKSIZE];            // y stack
uint16_t psy[STACKSIZE];         // auxaux stack
uint8_t pTop;                      // top of parse stacks

/* values associated with the left hand side of productions */
uint16_t hh;
uint16_t yy;
uint8_t ll;
uint8_t xx;

/* values associated with handles */
uint16_t *h;                     // replaces h based baseh (1) uint16_t
uint8_t *x;                        // replaces x based basex (1) uint8_t
uint16_t *y;                     // replaces y based basey (1) uint16_t
uint16_t h1, h2, h3, h4, h5;
uint8_t x1, x2, x3, x4;
uint16_t y1, y2, y3;
#define l1  (h1 & 0xff)
#define l2  (h2 & 0xff)
#define l3  (h3 & 0xff)
#define l4  (h4 & 0xff)
#define l5  (h5 & 0xff)

/* symbol list */



uint16_t rloc[RELTSIZE];                /* pointer to the address to reloc */
uint8_t rsb[RELTSIZE];                  /* segment (4 bits), base (4 bits) */
uint16_t rdispl[RELTSIZE];              /* displacement */
uint16_t rext[RELTSIZE];                /* ptr to external name, 0 if not ext */
uint8_t rtn = { 0 };                    /* no. occupied entries in reltab */
uint8_t rtt = { RELTSIZE };             /* current top of reltab */
uint16_t caNext;
uint16_t idaNext;
uint16_t waNext;
uint16_t waSize;
uint16_t iida;
uint8_t stb[SYMTSIZE];
uint16_t sDispl[SYMTSIZE];
uint8_t sLink[SYMTSIZE];
uint16_t sAtr[SYMTSIZE];
uint16_t sName[SYMTSIZE];
uint8_t symTop;
uint8_t blStack[MAXBLOCKDEPTH];
uint16_t waStack[MAXBLOCKDEPTH];
uint8_t blStkTop;
uint8_t compiling = { 1 };
uint16_t line = { 1 };
uint8_t i;
uint8_t c;
uint8_t temp;
uint8_t temp1;
uint8_t temp2;
uint16_t temp3;
uint16_t temp4;

/* SOME OPCODES */
#define JMP   0xC3
#define RET   0xC9
#define PCHL  0xE9
#define PUSHH 0xE5
#define POPH  0xE1
#define PUSHD 0xD5
#define POPD  0xD1
#define SHLD  0x22
#define XCHG  0xEB
#define LXIH  0x21
#define INXH  0x23
#define DADD  0x19
#define MOVEM 0x5E
#define MOVDM 0x56



uint8_t RBase(uint8_t j) {      /* return base field of entry j in reltab */
    return rsb[j] & 0xf;
}


#ifdef _TRACE
#include <conio.h>
#include <ctype.h>
uint8_t trace = ' ';

void Trac(uint8_t t) {
    uint8_t i;

    if (trace == ' ')
        return;
    putchar(t);
    while ((t = getche()) != ' ') {
        switch (toupper(t)) {
        case 'S':
            for (i = 0; i <= symTop; i++)
                printf(":%02X:%02X:%04X:%02X:%04X:%04X", i, stb[i], sDispl[i], sLink[i], sAtr[i], sName[i]);
            break;
        case 'R':
            for (i = 0; i < sizeof(rsb); i++)
                if (RBase(i) != UNUSED)
                    printf(":%02X:%04X:%02X:%04X:%04X", i, rloc[i], rsb[i], rdispl[i], rext[i]);
            break;
        case 'H':
            for (i = 0; i <= pTop; i++)
                printf(":%02X:%04X", i, psh[i]);
            break;
        case 'A':
            printf(":%04X:%02X:%04X:%02X", hh, ll, yy, xx);
        }
    }
}

#endif




__declspec(noreturn) void Exit(int ecode) {
    printf("\nEnd L82\n");
    exit(ecode);
}

void CloseAll() {               /* close all files and return to os */
    if (fclose(fpCode) != 0)
        errcnt++, fprintf(stderr, "cannot close %s\n", cFile);
    if (fclose(fpData) != 0)
        errcnt++, fprintf(stderr, "cannot close %s\n", dFile);
    if (fclose(fpReloc) != 0)
        errcnt++, fprintf(stderr, "cannot close %s\n", rFile);
    Exit(errcnt != 0);
}


void KError(uint8_t n) {
    errcnt++;
    fprintf(stderr, "Line %04x: Error %02X\n", line, n);
    if ((n & 0xf0) == 0xf0)     // fatal error
        CloseAll();
    xx = 0xee;                  // set handle type to error
}

uint16_t DoubleT(uint8_t l, uint8_t h) {  /* return address formed by bytes l, h */
    return h * 256 + l;
}

uint8_t Get1() {
    /* read next character from input buffer */
    return (c = getc(fpPol)) == EOF ? 0 : c;
}

uint16_t Get2() {               /* read j 16 bit value from the input buffer */
    int c;
    c = Get1();
    return DoubleT(c, Get1());
}

void Wr(uint8_t s, uint8_t c) {     /* write byte c on file s(ca,ida,rt) */
    if (s == IDA) {
#ifdef _TRACE
        if (trace = 'D') {
            printf("%04X:%02X", idaNext, c);
            Trac('.');
        }
#endif
        idaNext++;
    } else if (s == CA) {
#ifdef _TRACE
        if (trace = 'D') {
            printf("%04X:%02X", caNext, c);
            Trac('.');
        }
#endif
        caNext++;
    }

    if (putc(c, (s == IDA) ? fpData : (s == CA) ? fpCode : fpReloc) != c) {
        fprintf(stderr, "Write error: %s\n", (s == IDA) ? dFile : (s == CA) ? cFile : rFile);
        errcnt++;
        CloseAll();
    }
}

void WrR1(uint8_t c) {          /* write c onto the rt file */
    Wr(SRELOC, c);
}


void WrR2(uint16_t a) {         /* write j onto the rt file */
    WrR1(a & 0xff);
    WrR1(a >> 8);
}

void DumpRt(uint8_t j) {        /* write one reltab entry on the rt file */
    WrR1('R');
    WrR2(rloc[j]);
    WrR2(rdispl[j]);
    WrR2(rext[j]);
    WrR1(rsb[j]);
}

void SLLink(uint16_t l, uint8_t lk) {   /* set link of symbol l in symList to lk */
    symList[l - 1] = lk;
}

uint8_t LLink(uint16_t l) {    /* return link field of entry l in symList */
    return symList[l - 1];
}


void SRBase(uint8_t j, uint8_t b) {  /* set base field of entry j in reltab */
    rsb[j] = (rsb[j] & 0xf0) | b;
}

void SRSeg(uint8_t a, uint8_t seg) {    /* set segment of entry j in reltab */
    rsb[a] = (rsb[a] & 0xf) | (seg << 4);
}

void SRAddr(uint8_t j, uint8_t b, uint16_t d) {  /* set address fields of entry j in reltab to (b, d) */
    SRBase(j, b);
    rdispl[j] = d;
}


void BRAddr(uint8_t j, uint8_t b, uint16_t d) {
    /* backstuff b,d on chain of entries in reltab,
       linked by the ext field, starting at j */
    uint8_t next;

    while (j > 0) {
        next = (uint8_t)rext[j];
        SRAddr(j, b, d);
        rext[j] = 0;
        j = next;
    }
}

uint8_t SType(uint8_t i) {      /* return type of symbol i in symtab */
    return stb[i] >> 4;
}

uint8_t SBase(uint8_t i) {      /* return base of symbol i in symtab */
    return stb[i] & 0xf;
}

void SSType(uint8_t i, uint8_t t) {     /* set type of symbol i in symtab to t */
    stb[i] = (stb[i] & 0xf) | (t << 4);
}


void SSBase(uint8_t i, uint8_t b) {     /* set base of symbol i in symtab to b */
    stb[i] = (stb[i] & 0xf0) | b;
}


void SSAddr(uint8_t i, uint8_t b, uint16_t d) { /* set address of symbol i in symtab to (b,d) */
    if (SBase(i) == UNDEF)           /* backsuff (b,d) on chain of references */
        BRAddr((uint8_t)sDispl[i], b, d);
    SSBase(i, b);
    sDispl[i] = d;
}

void BSType(uint8_t i, uint8_t t) {     /* backstuff type t on chain of identifiers at symtab(i) */
    while (i > 0) {
        SSType(i, t);               // set type
        if (t == EXT)
            SSBase(i, CA);
        if (t == GLOB)
            SSBase(i, IDA);
        i = (uint8_t)sAtr[i];      // next
    }
}

void BSAdatr(uint8_t i, uint8_t b, uint16_t d, uint16_t a) {
    /* backstuff base b, displacement d, attribute a on a chain
       of identifiers in symtab, linked by the atr field */
    uint8_t j;

    while (i > 0) {
        d = d - a;                  // displacement is modified by a
        SSAddr(i, b, d);
        j = (uint8_t)sAtr[i];       // save attribute=link
        sAtr[i] = a;                // neew attribute
        i = j;                      // next
    }
}

#pragma warning(disable: 4715)
uint8_t IncRelt(uint8_t b) {
    /* create an entry in reltab for an address to be
       relocated in segment b (ca or ida);
       return pointer to the entry created;
       dump resolved entries onto disk while looking for the vacancy;
       if table full with unresolved references then abend */
    rtn = 0;                        // mumber of occupied entries in reltab
    while (rtn < RELTSIZE) {        // look for an empty entry
        // rtt: top of reltab
        if (rtt >= RELTSIZE)
            rtt = 1;                // wrap around
        if (RBase(rtt) == UNUSED) { // found empty entry
            rloc[rtt] = b == CA ? caNext : idaNext;    // set pointer to the address to relocate
            SRSeg(rtt, b);
            return rtt;
        }
        if (RBase(rtt) != UNDEF) {  // an address alreayd resolved
            DumpRt(rtt);            // write entry onto rt file
            SRBase(rtt, UNUSED);    // set entry free
        } else {                    // occupied
            rtn++;
            rtt++;
        }
    }
    // if get here : relocation table overflow: quit
    KError(0xf4);	/* too many unresolved cases */
}
#pragma warning(default : 4715)

uint16_t First2(uint16_t l) {       /* return first 2 charaters of string at symList(l) */
    uint8_t hi, lo;

    hi = symList[l];
    lo = symList[l + 1];
    if (hi == 0)            // empty string
        return 0;
    if (lo == 0)            // 1-character string
        return hi;
    return DoubleT(lo, hi);
}

bool Undeclared(uint16_t l) {    /* return true if symbol at symList(l) not yet declared */
    return LLink(l) == 0;
}

bool NewSymb(uint16_t l) {     /* return true if symbol at symList(l) is not in the current block */
    return LLink(l) <= blStack[blStkTop];
}


uint8_t CreateSym(uint8_t t, uint8_t b, uint16_t d, uint16_t a, uint8_t k) {
    /* create an entry in symtab referring to symbol at handle(k);
       return pointer to the entry created */
    uint16_t hk;

  
    if (++symTop >= SYMTSIZE)     // symbol table overflow
        KError(0xf2);	        /* too many symbols */
    // else set the symbol
    SSType(symTop, t);          // type
    SSBase(symTop, b);          // base
    sDispl[symTop] = d;         // displ
    sAtr[symTop] = a;           // attribute
    hk = h[k];                  // pointer to symbol name in symList
    sLink[symTop] = LLink(hk);  // previous entry
    sName[symTop] = hk;         // pointer to name
    SLLink(hk, symTop);
    return symTop;
}


void EmitStr(uint8_t k, uint8_t s) {    /* emit in segment s (ca, ida) the string at handle(k) */
    uint16_t n;
    uint8_t c;

    n = s == CA ? caNext : idaNext;
    c = CreateSym(STRING, s, n, 0, k);  // create an entry in symtab
    n = h[k];                           // pointer to symList
    while (c = symList[n]) {
        Wr(s, c);
        n++;
    }
}

void PushBl() {
    /* ENTER A NEW BLOCK */
    /* PUSH SYMTOP, WANEXT INTO THE BLOCK LEVEL STACK */
    if (++blStkTop >= MAXBLOCKDEPTH)
        KError(0xf3);	/* too many nested blocks */
    blStack[blStkTop] = symTop;
    waStack[blStkTop] = waNext;
}

void PopBl(uint8_t shrwa) {     /* exit from a block: pop symtop, wanext from the block stack */
    uint16_t i;

    while (blStack[blStkTop] != symTop) {
        /* write a 'symbol' record onto the rt file */
        i = sName[symTop];      // pointer to name in symList
        WrR1('S');
        WrR2(sDispl[symTop]);
        WrR2(line);
        WrR2(i);
        WrR1(stb[symTop]);
        SLLink(i, sLink[symTop]);
        symTop--;
    }
    if (shrwa)
        waNext = waStack[blStkTop];
    blStkTop = blStkTop - 1;
}

bool AReg(uint8_t k) {          /* true if handle(k) is register a */
    return (h[k] & 0xff) == TA;
}

bool BCReg(uint8_t k) {         /* true if handle(k) is register bc */
    return (h[k] & 0xff) == TBC;
}

bool DEReg(uint8_t k) {         /* true if handle(k) is register de */
    return (h[k] & 0xff) == TDE;
}

bool HLReg(uint8_t k) {         /* true if handle(k) is register hl */
    return (h[k] & 0xff) == THL;
}

bool SPReg(uint8_t k) {         /* true if handle(k) is register sp */
    return (h[k] & 0xff) == TSP;
}


bool CYReg(uint8_t k) {         /* true if handle(k) is 'CARRY' */
    return (h[k] & 0xff) == TCY;
}


bool StackR(uint8_t k) {         /* true if handle(k) is 'STACK' */
    return (h[k] & 0xff) == TSTACK;
}

bool BDHSP(uint8_t k) {         /* check for BC, DE, HL or SP */
    return (h[k] & 0xf) == 0xf;
}

bool BDH(uint8_t k) {         /* is BC, DE or HL */
    if (SPReg(k))
        return false;
    return BDHSP(k);
}

bool BDHPsw(uint8_t k) {    /* true PWS, BC, DE or HL */
    return BDH(k) || (h[k] & 0xff) == 0x3e;
}

bool AM(uint8_t k) {        /* is A,B,C...,M */
    return (h[k] & 0xf8) == 0;
}

uint8_t Mvi() {             /* return mvi opcode referring to register at handle(1) */
    return ((h1 & 0xff) << 3) | 6;
}

uint8_t Lxi() {                /* return lxi opcode referring to register at handle(1) */
    return h1 & 0xf1;
}

void IncWA(uint16_t n) {    /* increament wa by n */
    waNext += n;
    if (waSize < waNext)
        waSize = waNext;
}

void ZeroXX() {
    xx = 0;
}

void XmitX2() {
    xx = x2;
}

void XmitH1() {
    hh = h1;
}

void Xmit1() {                  /* pass along values of first element of the handle */
    XmitH1();
    xx = x1;
    yy = y1;
}

void Xmit2() {
    hh = h2;
    XmitX2();
    yy = y2;
}

void SetReg(uint8_t b) {        /* assign internal register numbers to handle(1) */
    ll = b;
    ZeroXX();
}

void CantDo() {           /* issue error message for invalid register operations */
    KError(7);	/* not j machine temp */
}

void Emit1(uint8_t op) {        /* emit a 1-byte instruction */
    Wr(CA, op);
}

void Emit2(uint8_t op1, uint8_t op2) {  /* emit 2 1-byte instructions */
    Emit1(op1);
    Emit1(op2);
}

void Emit4(uint8_t op1, uint8_t op2, uint8_t op3, uint8_t op4) {    /* EMIT 4 1-BYTE INSTRUCTIONS */

    Emit2(op1, op2);
    Emit2(op3, op4);
}

void Emit2N(uint16_t n, uint8_t b) { /* emit 2 bytes representing n in area b (ca, ida) */

    Wr(b, n & 0xff);
    Wr(b, n >> 8);
}

void EmitN(uint16_t n, uint8_t b) { /* emit 1 or 2 bytes representing n in area b (ca, ida) */
    if ((n & 0xff00) == 0)
        Wr(b, (uint8_t)n);
    else
        Emit2N(n, b);
}

void Emit3D(uint8_t op, uint16_t d16) { /* emit a 3-byte non-relocatable instruction */
    Emit1(op);
    Emit2N(d16, CA);	/* code segment */
}

uint8_t EmitL(uint8_t op, uint8_t l) {
    /* emit op 3-byte instruction, providing for relocation;
       the address will be backstuffed later, so create an
       entry in reltab, link it to the chain pointed to by l,
      and return pointer to the entry */
    uint8_t j;
    Emit1(op);
    j = IncRelt(CA);        /* pointer to next entry in reltab */
    SRBase(j, UNDEF);
    rdispl[j] = 0;
    rext[j] = l;            /* link using the ext field in reltab */
    Emit2(0, 0);            /* slot for the address */
    return j;
}

void EmitA(uint8_t b, uint16_t d, uint16_t e) {
    uint8_t j;
    j = IncRelt(CA);
    SRBase(j, b);
    rdispl[j] = d;
    rext[j] = e;
    Emit2(0, 0);
}

void Emit3A(uint8_t op, uint8_t b, uint16_t d, uint16_t e) {    /* emit a 3-byte relocatable instruction with address (b,d,e) */
    Emit1(op);
    EmitA(b, d, e);
}


void Emit2IA(uint16_t l, uint16_t d, uint8_t b) {
    /* emit in area b (ca, ida) a relocatable address equal to
       the address of symbol l in symList plus displacement d */
    uint8_t i, j, k;

    if (Undeclared(l))      // cannot reference
        KError(4);	        /* ref to undeclared identifier */
    else {
        j = IncRelt(b);     // pointer to an empty entry in reltab
        i = LLink(l);       // pointer into symtab
        k = SBase(i);       // get base of symbol i
        SRBase(j, k);       // set based of relocatable item
        if (k == UNDEF) {   // address not yet defined
            // link the entry in reltab to a chain of unresolved references ot the identifier
            rext[j] = sDispl[i];
            sDispl[i] = j;
            rdispl[j] = d;
        } else {            // address already known
            rdispl[j] = sDispl[i] + d;  // displacement
            rext[j] = (k = SType(i)) == GLOB || k == EXT ? l : 0;
        }
        Emit2N(0, b);       // slot for the address
    }
}

void Emit3IA(uint8_t op, uint16_t l, uint16_t d) {
    /* emit a 3-byte instruction with relocatable address
       equal to address of symbol l in symList plus displ d */
    Emit1(op);
    Emit2IA(l, d, CA);
}

void Emit2SA(uint8_t k, uint8_t b) {
    /* emit in area b (ca, ida) a relocatable address equal to
       the address of the string at handle(k) */
    uint8_t i, j;

    if (Undeclared(h[k]))       // string not yet entered
        EmitStr(k, IDA);        // emit it in ida
    // string already emitted
    j = IncRelt(b);             // pointer to an entry in reltab
    i = LLink(h[k]);            // pointer to string entry in symtab
    SRBase(j, SBase(i));        // set base in reltab
    rdispl[j] = sDispl[i];      // set displacement
    rext[j] = 0;                // not external
    Emit2N(0, b);               // slot for the address
}

void Emit3SA(uint8_t op, uint8_t k) {
    /* emit a 3-byte instruction with relocatable address
       equal to the address of the string at handle(k) */
    Emit1(op);
    Emit2SA(k, CA);
}

void Emit3C(uint8_t op, uint8_t k) {
    /* emit a 3-byte instruction with address equal to the
       <CONSTANT> at handle(k) */
    uint16_t hk;
    uint8_t xk;

    hk = h[k];
    xk = x[k];
    if (xk == TSTRING)              // constant is a string
        Emit3D(op, First2(hk));
    if (xk == TNUMBER)              // constant is a number
        Emit3D(op, hk);
    if (xk == TADDRVAR)             // constant is address of identifier
        Emit3IA(op, hk, y[k]);
    if (xk == TADDRSTR)             // constant is address of string
        Emit3SA(op, k);
}

bool LdaHL(uint8_t op, uint8_t k) {
    /*  emit instruction op (lda,lhld,sta,shld) for the constructs:
        <REG> = <VAR>
        <VAR> = <REG>
        k: pointer to <var> in the handle
        return true if instruction is emitted (<VAR> not in error) */
    uint8_t xk;
    uint16_t hk;

    xk = x[k];
    hk = h[k];
    if (xk == TVAR || xk == TMDOTADDRVAR)   /* <VAR> is <IDENTIFIER(<NUMBER>) */
                                         /* or <VAR> is M(.<IDENTIFIER>(<NUMBER>) */
        Emit3IA(op, hk, y[k]);
    else if (xk == TMSTRING)                /* <VAR> is M(<STRING>) */
        Emit3D(op, First2(hk));
    else if (xk == TMNUMBER)                /* <VAR> is M(<NUMBER>) */
        Emit3D(op, hk);
    else if (xk == TMDOTADDRSTR)            /* <VAR> is M(.<STRING>) */
        Emit3SA(op, k);
    else
        return false;                      /* did not emit instruction */
    return true;
}


void LoadPrim(uint8_t j) {
    uint8_t xj;
    uint16_t hj;

    xj = x[j];
    hj = h[j];
    c = xj & 0xf0;                          /* get the type oF <PRIM> */
    if (c == 0) {                           /* <PRIM> is a register */
        if (l1 == (hj & 0xff))              /* same as <REG> */
            return;
        // different registers
        if (AM(1) && AM(j))	                /* emit a move instruction */
            Emit1((h1 << 3) | 0x40 | hj);
        else if (StackR(1) && BDHPsw(j))	/* <REG> is the stack */
            Emit1((hj & 0xf0) | 0xc5);      /* emit a push */
        else if (StackR(j) && BDHPsw(1))	/* <PRIM> is the stack */
            Emit1((h1 & 0xf0) | 0xc1);      /* emit a pop*/
        else if (SPReg(1) && HLReg(j))	    /* sp = hl */
            Emit1(0xf9);                    // emit sphl */
        else if (BDH(1) && BDH(j)) {        /* bc=de, bc=hl etc. */
            c = (h1 ^ 0x4f) | ((hj & 0xf0) >> 3);
            Emit2(c, c + 9);                /* b=d, c=e, b=h, c=l etc. */
        } else                              /* error */
            CantDo();
        return;
    }
    if (c == 0x10) {                        // <PRIM> is a constant
        if (xj == TNUMBER) {                // <PRIM is a number
            if (AM(1))                      // emit a move immediate
                Emit2(Mvi(), (uint8_t)hj);
            else if (BDHSP(1))              // emit a load extended immediate
                Emit3D(Lxi(), hj);
            else if (CYReg(1)) {            // <REG> is CY
                if (hj == 1)                // CY = 1
                    Emit1(0x37);		    /* stc */
                else if (hj == 0)           // CY = 0
                    Emit2(0x37, 0x3f);	    /* stc ! cmc */
                else
                    CantDo();
            } else                          // error bad register
                CantDo();
            return;
        }
        if (xj == TSTRING) {                // <PRIM> is a string
            if (AM(1))                      // emtio a move immediate
                Emit2(Mvi(), symList[hj]);	// 1st char
            else if (BDHSP(1))              // emit a load extended immediate
                Emit3D(Lxi(), First2(hj));
            else
                CantDo();
            return;
        }
        if (BDHSP(1)) {
            if (xj == TADDRVAR) {           // <PRIM> is .<IDENTIFIER>(<NUMBER>)
                Emit3IA(Lxi(), hj, y[j]);
            } else if (xj == TADDRSTR)      // <PRIM> is .<STRING>
                Emit3SA(Lxi(), j);
            // else(error): do nothing - error msg already issued
        } else                              // error (bad register
            CantDo();
        return;
    }
    if (c == 0x20) {                        // <PRIM> is a constant
        if (AReg(1)) {                      // <REG> is register A
            if (xj == TMXX)                 // <PRIM> is M(BC) or M(DE)
                Emit1(hj & 0xfa);	        /* ldax b|d */
            else if (xj == TIN)             // <PRIM> is IN(<NUMBER>)
                Emit2(0xdb, (uint8_t)hj);	        /* in xx */
            else if (LdaHL(0x3a, j))	    /* type to emit lda instruction */
                return;                     // emited all ok
            else                            // error
                CantDo();
            return;
        }
        if (HLReg(1)) {                     // <REG> is HL
            if (LdaHL(0x2a, j))		        /* try to emit a lhld instruction */
                return;                     // ok
        }
        // error
    }   //error
    CantDo();
}

void BinOpSec(uint8_t j, uint8_t k) {
    /* code generation for the construct:
       <REG> = ... <BINARY.OP> <SEC>
       j,k: pointers to <BINARY.OP>, <SEC> in the handle */
    uint8_t xj, xk;
    uint16_t hk;

    xj = x[j];
    xk = x[k];
    hk = h[k];
    c = xk & 0xf0;                          /* get the type of <SEC> */
    if (c == 0) {                           /* <SEC> is a register */
        if (AReg(1)) {                      /* <REG> is reg a */
            if (AM(k)) {	                /* generate 'accumulator' instruction */
                Emit1(xj | (hk & 0xff));
                return;
            }
            CantDo();
            return;
        }
        if (HLReg(1)) {                     /* <REG> is hl */
            if (BDHSP(k) && xj == 0x80)	{   /* <SEC> is bc,de,hl,sp; <BINARY.OP> is + */
                 Emit1(hk & 0xf9);          /* emit dad b, dad d, dad h, dad sp */
                return;
            }
            CantDo();
            return;
        }
        CantDo();                           // not a or hl
        return;
    }
    if (c == 0x10) {                        // <SEC> is a constant
        if (AM(1) || BDHSP(1)) {            /* A,B,...M,BC,DE,HL,SP */
            if (xk == TNUMBER && hk == 1) { /* +/- 1 */
                if (xj == 0x80)	{	        /* <REG> + 1 */
                    if (AM(1))	            /* A,B,C...M */
                        Emit1(((h1 & 0xff) << 3) | 4);  // inr ops
                    else
                        Emit1(h1 & 0xf3);   // inx ops
                    return;
                }
                if (xj == 0x90)	{	        /* <REG - 1 */
                    if (AM(1))	            // A,B,C...M
                        Emit1(((h1 & 0xff) << 3) | 5);  // dcr  ops
                    else
                        Emit1(h1 & 0xfb);   // dcx ops
                    return;
                }
            }
            if (!AReg(1)) {                 // <REG> is not A
                CantDo();
                return;
            }
            // else: <REG> is reg A
            if (xk == TSTRING)              // string
                c = symList[hk];            // get its first character
            if (xk == TNUMBER)              // number
                c = (uint8_t)hk;            // get its value
            if (xk > TNUMBER)               // address: not implemented
                KError(8);	                /* feature not implemented */
            else
                Emit2(xj | 0x46, c);        // emit acc immediate group
            return;
        }
        // else <REG> is psw,stack, cy: error
        CantDo();
    }
    // else: <SEC> is error: do nothing (error already processed)
}


void Idn(uint8_t k, uint16_t n) {
    /* symantic actions for the productions:
       <VAR> ::= <IDENTIFIER> ( <NUMBER> )
       <CONSTANT> ::= . <IDENTIFIER> ( <NUMBER> )
       K: INDEX OF <IDENTIFIER> IN THE HANDLE
       N: VALUE OF <NUMBER> */

    uint16_t hk;

    hk = h[k];
    if (Undeclared(hk))         // error: cannot be referenced
        KError(4);	            /* refernence to undeclared identifier */
    yy = n;	                    /* transmit <NUMBER> */
    hh = hk;                    // transmit pointer to identifier
}

void Chc(uint8_t b) {           // B is base (CA or IDA)
    /* semantic actions for the productions:
       <DATA.LIST> ::= <DATA.HEAD> <CONSTANT> )
       <DATA.HEAD> ::= <DATA.HEAD> <CONSTANT> ,
       <INITIAL.LIST> ::= <INITIAL.HEAD> <CONSTANT> )
       <INITIAL.HEAD> ::= <INITIAL.HEAD> <CONSTANT> ,  */
    if (x2 == TSTRING)          // constant is a string
        EmitStr(2, b);          // emit the string in ca or ida
    else if (x2 == TNUMBER)     // constant is a number
        EmitN(h2, b);           // emit the number in ca or ida
    else if (x2 == TADDRVAR) {  // constant is address of identifier
        Emit2IA(h2, y2, b);     // emit the address in ca or ida
    } else if (x2 == TADDRSTR)  // constant is address of string
        Emit2SA(2, b);          // emit the address in ca or ida
    // else: error - do nothing - error message already issued
    Xmit1();
}


void Fph() {
    /* semantic actions for the productions:
       <FORMAL.PARAM.LIST> ::= <FP.HEAD> <IDENTIFIER> )
       <FP.HEAD>           ::= <FP.HEAD> <IDENTIFIER> , */
    if (NewSymb(h2)) {          // first appearence of identifier in current block
        c = CreateSym(KBYTE, WA, waNext, 2, 2);
        IncWA(2);               // assign 2 byte s in wa
        xx = x1 + 1;            // no. formal parameters
    } else {                    // identifier already defiend in current block
        KError(2);
        Xmit1();                // ignore the identifier
    }
}

void Prh(uint16_t a) {
    /* semantic actions for the productions:
       <PROC.HEAD> ::= <PROC.NAME> ;
       <PROC.HEAD> ::= <PROC.NAME> <FORMAL.PARAM.LIST> ; */
    SSType(l1, PROC);
    sAtr[l1] = a;               // address of formal parameters
    Xmit1();
}

void Sdb(uint16_t j) {
    /* semantic actions for the productions:
       <STORAGE.DECLARATION> ::= <IDENT.SPECIFICATION> BYTE
       <STORAGE.DECLARATION> ::= <BOUND.HEAD> <NUMBER> BYTE */
    Xmit1();
    BSType(l1, KBYTE);          // backstuff type
    yy = j;                     // length requested per identifier
}

void Idl() {
    /* semantic actions for the productions:
       <IDENT.SPECIFICATION> ::= <IDENT.LIST> <IDENTIFIER> )
       <IDENT.LIST>          ::= <IDENT.LIST> <IDENTIFIER) , */
    if (NewSymb(h2)) {          /* first appearance of identifier in current block */
        ll = CreateSym(UNDECL, UNDEF, 0, h1, 2);
        xx = x1 + 1;            // no. identifiers
    } else {                    /* identifier already defined in current block */
        KError(2);
        Xmit1();                /* ignore redeclaraiton */
    }
}

bool CantCall(uint16_t j, uint8_t n) {   /* j pointer to identifier in symList, n no. parameters */
    /* return true if cannot call identifier with n parameters */
    if (Undeclared(j))
        return 1;
    if ((c = SType(LLink(j))) == KLABEL && n != 0)
        return true;
    return c == KBYTE || c == GLOB;
}


void Aph() {
    /* SEMANTIC ACTIOSN FOR THE PRODUCTIONS:
       <ACTUAL.PARAM.LIST> ::= <AP.HEAD> <CONSTANT> )
       <AP.HEAD> ::= <AP.HEAD> <CONSTANT> , */
    Emit3C(LXIH, 2);            /* load the constant into hl */
    if (SType(l1) == EXT)       /* external procedure */
        temp3 = sName[h1 & 0xff];
    else                        /* temp3 will be moved to the ext field in reltab */
        temp3 = 0;
    Emit3A(0x22, WA, sAtr[l1] + (x1 << 1), temp3);    /* store parameter */
    x1++;                       /* no. actual paramters */
    Xmit1();
}

void Reduce(uint16_t pn) {
#ifdef _DBGRAM
    printg(pn);
#endif
#ifdef _TRACE
    if (trace == 'P') {
        printf("P%04X:", pn);
        Trac(';');
    }
#endif
    if (pn < 30)                // no temp to be performed
        return;
    switch (pn) {
    /*** <PROGRAM> ::= <STMT.LIST> ; EOF */
    /*** <STMT.LIST> ::= <STMT> */
    /*** <STMT.LIST) ::= <STMT.LIST) ; <STMT> */
    /*** <STMT) ::= <BASIC.STMT) */
    /*** <STMT> ::= <IF.STMT> */
    /*** <STMT) ::= <ERROR) */
    /*** <BASIC.STMT) ::= <DECL.STMT> */
    /*** <BASIC.STMT) ::= <GROUP> */
    /*** <BASIC.STMT> ::= <PROC.DEFINITION> */
    /*** <BASIC.STMT> ::= <RETURN.STMT> */
    /*** <BASIC.STMT) ::= <CALL.STMT> */
    /*** <BASIC.STMT> ::= <GOTO.STMT> */
    /*** <BASIC.STMT) :: = <REPEAT.STMT> */
    /*** <BASIC.STMT> :: = <CONTROL.STMT> */
    /*** <BASIC.STMT) ::= <COMPARE.STMT> */
    /*** <BASIC.STMT> ::= <EXCHANGE.STMT> */
    /*** <BASIC.STMT> ::= <ASSIGN.STMT> */
    /*** <BASIC.STMT> ::= <LABEL.DEFINITION) <BASIC.STMT> */
    /*** <IF.STMT> ::= <LABEL.DEFINITION> <IF.STMT> */
    /*** <DECL.STMT) ::= DECLARE <DECL.ELEMENT> */
    /*** <DECL.STMT> ::= <DECL.STMT> , <DECL.ELEMENT) */
    /*** <INITIAL.HEAD> ::= INITIAL ( */
    /*** <ENDING> ::= END */
    /*** <ENDING> ::= END <IDENTIFIER> */
    /*** <ENDING> ::= <LABEL.DEFINITION) <ENDING> */
    /*** <INITIALIZATION> :: = <EMPTY> */
    /*** <INITIALIZATION> ::= <ASSIGN.STMT> */
    /*** <ASSIGN.STMT> ::= <VAR.ASSIGN> */
    /*** <ASSIGN.STMT> ::= <REG.ASSIGN> */ 
    case 30:                     /*** <LABEL.DEFINITION> ::= <IDENTIFIER> */
                                 /* return: ll = pointer to identifier entry in symtab
                                            xx = 1 to indicate identifier label */
        xx = 1;
        if (NewSymb(h1)) {       /* first appearance of identifier in current block */
            ll = CreateSym(KLABEL, CA, caNext, 0, 1);
        } else {                /* identifier already defined in current block */
            h1 = LLink(h1);     /* pointer int symtab */
            if (SType((uint8_t)h1) != KLABEL)
                KError(1);	    /* identifier redclared in same block */
            else if (SBase((uint8_t)h1) != UNDEF)
                KError(2);      /* conflicting definition */
            else {              /* define the label */
                SSAddr(l1, 1, caNext);
                Xmit1();
            }
        }
        break;
    case 31:					/*** <LABEL.DEFINITION> ::= <NUMBER> */
        ZeroXX();               /* to indicate numeric label */
        if (h1 < caNext)        /* code overlap */
            KError(3);
        while (h1 > caNext) {
            Wr(CA, 0);           /* fill with 0's */
        }
        break;
    case 32:                    /*** <IF.STMT> ::= <IF.CLAUSE> <STMT> */
                                /* h1 points to f(false) chain in reltab */
        BRAddr(l1, CA, caNext); /* backsuff f address */
        break;
    case 33:                    /*** <IF.STMT> ::= <IF.CLAUSE> <TRUE.PART> <STMT> */
                                /* h2 pointes to e(exit) entry in reltab */
        BRAddr(l1, CA, rloc[l2] + 2);   /* BACKSTUFF F */
        BRAddr(l2, CA, caNext);         /* BACKSTUFF E */
        break;
    case 34:                    /*** <IF.CLAUSE> ::= IF <COMPOUND.CONDITION> THEN */
        ll = l2; break;         /* return pointer to f chain in reltab */
    case 35:                    /*** <TRUE.PART> ::= <BASIC.STMT> ELSE */
                                /* return pointer to e entry in reltab */
        ll = EmitL(0xc3, 0); 	/* emit jmp e (exit) */
        break;
    case 36:                    /*** <COMPOUND.CONDITION> ::= <AND.HEAD> <SIMPLE.CONDIITON> */
                                /* return pointer to f chain in reltab */
        ll = EmitL(x2 ^ 0xa, l1);   /* emit jncond f */
        break;
    case 37:                    /***<COMPOUND.CONDITION> :: = <OR.HEAD> <SIMPLE.CONDIITON> * /
        ll = EmitL(x2 ^ 0xa, 0);    /* emit jncond f */
        BRAddr(l1, CA, caNext);     /* backstuff t */
        break;
    case 38:                    /*** <COMPOUND.CONDITION> ::= <SIMPLE.CONDIITON> */
        ll = EmitL(x1 ^ 0xa, 0);    /* emit jncond f */
        break;
    case 39:                    /*** <AND.HEAD> ::= <SIMPLE.CONDIITON> AND */
        ll = EmitL(x1 ^ 0xa, 0);	/* emit jncond f */
        break;
    case 40:                    /*** <AND.HEAD> ::= <AND.HEAD> <SIMPLE.CONDIITON> AND */
        ll = EmitL(x2 ^ 0xa, l1);	/* emit jncond f */
        break;
    case 41:                    /*** <OR.HEAD> ::= <SIMPLE.CONDIITON> OR */
        ll = EmitL(x1 | 2, 0);	/* emit jcond t */
        break;
    case 42:                    /*** <OR.HEAD> ::= <OR.HEAD> <SIMPLE.CONDIITON> OR */
        ll = EmitL(x2 | 2, l1);	/* emit jcond t */
        break;
    case 43: xx = x4; break;     /*** <SIMPLE.CONDITION> ::= ( <STMT.LIST> ) <CONDIITON> */
                                /* return a numeric code representing the condition */

    case 44: Xmit1(); break;    /*** <SIMPLE.CONDITION> ::= <CONDITION> */ 
    case 45: xx = 0xc0; break;  /*** <CONDITION> ::= NOT ZERO */
    case 46: xx = 0xc8; break;  /*** <CONDITION> ::= ZERO */
    case 47: xx = 0xd0;	break;  /*** <CONDITION> ::= NOT CY */
    case 48: xx = 0xd8;	break;  /*** <CONDITION> ::= CY */
    case 49: xx = 0xe0;	break;  /*** <CONDITION> ::= PY ODD */
    case 50: xx = 0xe8;	break;  /*** <CONDITION> ::= PY EVEN */
    case 51: xx = 0xf0;	break;  /*** <CONDITION> ::= PLUS */
    case 52: xx = 0xf8;	break;  /*** <CONDITION> ::= MINUS */
    case 53:                    /*** <DECL.ELEMENT> ::= <STORAGE.DECLARATION> */
        /* <STORAGE.DECLARATION> returns:
           h1: pointer to a chain of identifiers in symtab,
           x1: no. identifiers declared,
           y1: length (bytes) requested per identifier;
           it also sets the global varaiable
           iida: idanext before <DECL.ELEMENT> was parsed */
        temp3 = idaNext - iida; /* total length initialized */
        temp4 = 0;
        for (i = 1; i <= x1; i++)
            temp4 += y1;        /* total length requested */

        if (temp4 < temp3) {    /* initial data too long */
            KError(0);
            temp4 = temp3;      /* right justify inside data block */
        }
        if (temp3 == 0) {       /* no data initialized */
            IncWA(temp4);       /* allocate in wa */
            BSAdatr(l1, WA, waNext, y1);
        } else {                /* initialization requested (data is in ida) */
            iida += temp4;
            while (idaNext < iida)
                Wr(IDA, ' ');  /* pad with blanks */
            BSAdatr(l1, IDA, idaNext, y1);  /* allocate in ida */
        }
        break;
    case 54:                    /*** <DECL.ELEMENT> ::= <IDENT.SPECIFICATION> <TYPE> */
        BSType(l1, x2);         /* back stuff type x2 into identifiers in chain ha */
        break;
    case 55:                    /*** <DECL.ELEMENT> ::= <IDENTIFIER> <DATA.LIST> */
                                /* <DATA.LIST> returns a pointer to 'skip' entry in reltab */
        if (NewSymb(h1)) {      /* first appearance of identifier in current block */
                                /* set entry for identifier in symbab */
            temp4 = rloc[l2] + 2;   /* address of 1st byte of data */
            temp3 = caNext - temp4; /* total length initialized */
            temp = CreateSym(KBYTE, CA, temp4, temp3, 1);
            BRAddr(l2, 1, caNext);
        } else
            KError(1);          /* error: identifier already declared in this block */
        break;
    case 56:                    /*** <DATA.LIST> ::= <DATA.HEAD> <CONSTANT> ) */
                                /* return pointer to 'skip' entry in reltab */
        Chc(CA); break;         /* emit the constant into ca */
    case 57:                    /*** <DATA.HEAD> ::= DATA ( */
                                /* return pointer to 'skip' entry in reltab */
        ll = EmitL(JMP, 0);	    /* emit jmp skip */
        break;
    case 58:                    /*** <DATA.HEAD> :: = <DATA.HEAD> <CONSTANT>, */
        Chc(CA); break;         /* emit the constant into ca */
    case 59:                    /*** <STORAGE.DECLARATION> ::= <IDENT.SPECIFICATION> BYTE */
        Sdb(1); break;          /* return pointer to indentifier chain in symtab */
    case 60: Sdb(h2); break;    /*** <STORAGE.DECLARATION> ::= <BOUND.HEAD> <NUMBER> ) BYTE */
    case 61:                    /*** <STORAGE.DECLARATION> ::= <STORAGE.DECLARATION> <INITIAL.LIST> */
        Xmit1(); break;         /* <INITIAL.LIST> already stored in ida */
    case 62: xx = KLABEL; break;/*** <TYPE> :: = LABEL */
    case 63: xx = EXT; break;   /*** <TYPE> :: = EXTERNAL */
    case 64: xx = GLOB; break;  /*** <TYPE> :: = COMMON */
    case 65:                    /*** <IDENT.SPECIFICATION> ::= <IDENTIFIER> */
                                /* return pointer to identifier chain in symtab */
        if (NewSymb(h1)) {      /* first apparence of identifier in current block */
            ll = CreateSym(UNDECL, UNDEF, 0, 0, 1);
            xx = 1;             /* no. elements in <ident.specification> */   
            iida = idaNext;     /* saved current value of idanext */
        } else
            KError(1);          /* identifier already defined in current block */
        break;
    case 66: Idl();	break;      /*** <IDENT.SPECIFICATION> ::= <IDENT.LIST> <IDENTIFIER> ) */
    case 67:                    /*** <IDENT.LIST> ::= ( */
                                /* return pointer to identifier chain in symtab */
        ll = 0;                 // pointer
        ZeroXX();               /* xx: no. identifiers in <IDENT.LIST> */
        iida = idaNext;         /* save current value of idanext */
        break;
    case 68: Idl(); break;      /*** <IDENT.LIST> ::= <IDENT.LIST> <IDENTIFIER> , */
    case 69:                    /*** <BOUND.HEAD> ::= <IDENT.SPECIFICATION> */
        Xmit1(); break;         /* return pointer to identifier chain in symtab */
    case 70:                    /*** <INITIAL.LIST> ::= <INITIAL.HEAD> <CONSTANT> ) */
        Chc(IDA); break;        /* emit the constant into ida */
    case 71:                    /*** <INITIAL.HEAD> ::= <INITIAL.HEAD> <CONSTANT> , */
        Chc(IDA); break;        /* emit the constant into ida */
    case 72:                    /*** <GROUP> ::= <GROUP.HEAD> ; <ENDING> */
        if (x1 & 1) {           /* this is a case group */
            temp1 = l1;         /* pointer to the chain */
            temp2 = 0;          /* pointer to an empty chain */
            /* y1: no. stmts in the case group */
            while (rext[temp1] < 0x8000) {     // positive
                /* invert the chain */
                temp = (uint8_t)rext[temp1];    // next
                rext[temp1] = temp2;
                temp2 = temp1;
                temp1 = temp;
            }
            /* temp1 now points to the jpvec entry in reltab */
            /* temp2 points to the 'exit' chain in reltab */
            temp4 = -rext[temp1];       //  address of lo
            SRAddr(temp1, 1, caNext);   /* backsuff jpvec */
            rext[temp1] = 0;            /* not external */
            temp3 = caNext + (y1 << 1); /* address of exit */
            while (temp2) {             /* generate jump vector, backsuff exit */
                EmitA(CA, temp4, 0);    /* emit n-th label (ln) */
                temp1 = (uint8_t)rext[temp2];
                rext[temp2] = 0;        /* not external */
                SRAddr(temp2, CA, temp3);/* backsuff exit */
                temp4 = rloc[temp2] + 2;/* compute next ln */
                temp2 = temp1;          /* next entry */
            }
        } else if (x1 == 2) {           /* this is an iterative group */
            /* h1 points to f chain in reltab */
            Emit3A(JMP, CA, y1, 0);     /* emit jmp loop */
            BRAddr(l1, CA, caNext);     /* backstuff f */
        }
        PopBl(1);                       /* exit block, shrink wa */
        break;
    case 73:                    /*** <GROUP.HEAD> ::= <DO> */
        ZeroXX(); break;        /* return xx indicating type of group */
    case 74:				    /*** <GROUP.HEAD> ::= <DO> <ITERATIVE.CLAUSE */
        Xmit2();
        xx = 2;
        break;
    case 75:					/*** <GROUP.HEAD> :: = <DO> <CASE.SELECTOR> */
        Xmit2();
        xx = 3;
        break;
    case 76:                    /*** <GROUP.HEAD> ::= <GROUP.HEAD> ; <STMT> */
        Xmit1();
        if (x1 & 1) {           // case group
            hh = EmitL(JMP, l1);/* emit jmp exit */
            yy = y1 + 1;        /* no. statements in the case group */
        }
        break;
    case 77:                    /*** <DO> ::= DO */
        PushBl(); break;        /* enter a new block */
    case 78:                    /*** <ITERATIVE.CLAUSE> ::= <INITIALIZATION> <BY> <ASSIGN.STMT>
                                                            <WHILE> <COMPOUND.CONDITION> */
                                /* return hh=pointer to f chain in reltab, yy=loop address */
        ll = l5;                /* pointer */
        BRAddr(l2, CA, h4);     /* backstuff skip */
        yy = rloc[l2] + 2;      /* address of loop */
        break;
    case 79:                    /*** <ITERATIVE.CLAUSE> ::= <INITIALIZATION> <WHILE><COMPOUND.CONDITION> */
        yy = h2;                /* address of loop */
        ll = l3;                /* pointer to f chain in reltab */
        break;
    case 80:                    /*** <BY> ::= BY */
                                /* return pointer to skip entry in reltab */
        ll = EmitL(JMP, 0);	    // emit jmp skip
        break;
    case 81:                    /*** <WHILE> ::= WHILE */
        hh = caNext; break;     /* return address of next instruction (compound.condiiton) */
    case 82:					/*** <CASE.SELECTOR> ::= CASE <REG> */
                                /* return hh = pointer to a chain in reltab.
                                          yy = no. statements in the case group */
        if (HLReg(2)) {		    // <REG> is hl
            Emit2(PUSHD, XCHG);
            ll = EmitL(LXIH, 0);        /* emit lxih jpvec */
            rext[ll] = -(caNext + 8);   /* save - address of lo */
            Emit4(DADD, DADD, MOVEM, INXH);
            Emit4(MOVDM, XCHG, POPD, PCHL);
            yy = 0;                     /* no. statements */
        } else
            KError(8);          /* not implemented */
        break;
    case 83:                    /*** <PROC DEFINITION> ::= <PROC.HEAD> <STMT.LIST> ; <ENDING> */
        Emit1(RET);		        /* supply a return */
        BRAddr((uint8_t)y1, CA, caNext); /* backstuff skip */
        PopBl(0);               /* exit block, do not shrink wa */
        break;
    case 84:                    /*** <PROC.HEAD> ::= <PROC.NAME> ; */
                                /* return pointer to skip entry in reltab */
        Prh(0xffff);	        /* no formal params: set atr to -1 */
        break;
    case 85:                    /*** <PROC.HEAD> ::= <PROC.NAME> <FORMAL.PARAM.LIST */
        Prh(waNext - (x2 << 1));    /* address of formal parms */
        break;
    case 86:                    /*** <PROC.NAME> :: <LABEL.DEFINITION> PROCEDURE */
                                /* return: hh = pointer to procedure entry in symtab
                                           yy = pointer to 'skip' entry in reltab */
        Xmit1();
        if (x1 & 1) {           /* identifier label */
            yy = EmitL(JMP, 0); /* emit code to skip the procedure */
                                /* return pointer to skip entry in reltab */
            sDispl[l1] = caNext;  /* procedure entry is after the jmp instruction */  
            PushBl();           /* enter a new block */
        } else
            KError(3);          /* not a valid procedure */
        break;
    case 87:                    /*** <FORMAL.PARAM.LIST> ::= <FP.HEAD> <IDENTIFIER> ) */
        Fph(); break;           /* return number of formal parameters */
    case 88:                    /*** <FP.HEAD> ::= ( */
        ZeroXX(); break;        /* return no. of formal parameters */
    case 89: Fph(); break;      /*** <FP.HEAD> ::= <FP.HEAD> <IDENTIFIER> , */
    case 90: Emit1(RET); break;	/*** <RETURN.STMT> ::= RETURN */
    case 91:                    /*** <RETURN.STMT> ::= IF <SIMPLE.CONDITION> RETURN */
        Emit1(x2); break;       /* emit rnz, rz, rnc ... */
    case 92:                    /*** <CALL.STMT> ::= <CALL> <IDENTIFIER> */
        if (CantCall(h2, 0))
            KError(6);
        else
            Emit3IA(x1 | 4, h2, 0);
        break;
    case 93:                    /*** <CALL.STMT> ::= <CALL> <ACTUAL.PARAM.LIST> */
        Emit1(POPH);
        Emit3IA(x1 | 4, sName[l2], 0);
        break;
    case 94:					/*** <CALL.STMT> ::= <CALL> <NUMBER> */
        if (x1 == 0xcd && (h2 & 0xffc7) == 0)   /* unconditional call 0, 8, 16...56 */
            Emit1(h2 | 0xc7);   // emit rst instruction
        else                    // emit call
            Emit3D(x1 | 4, h2); // /* call, cnz, cz .... */
        break;
    case 95: Aph(); break;      /*** <ACTUAL.PARAM.LIST> ::= <AP.HEAD> <CONSTANT> ) */
    case 96:					/*** <AP.HEAD> ::= <IDENTIFIER> ( */
        if (CantCall(h1, 1))
            KError(6);
        else {
            ll = LLink(h1);     /* pointer to identifier in symtab */
            ZeroXX();           /* no. actual parameters */
            Emit1(PUSHH);	    /* save hl */
        }
        break;
    case 97: Aph(); break;      /*** <AP.HEAD> ::= <AP.HEAD> <CONSTANT> , */
    case 98: xx = 0xcd; break;  /*** <CALL> ::= CALL */
    case 99: XmitX2(); break;   /*** <CALL> ::= IF <SIMPLE.CONDITION> CALL */
    case 100:					/*** <GOTO.STMT> ::= <GOTO> <IDENTIFIER> */
        if (CantCall(h2, 0))
            KError(11);
        else
            Emit3IA(x1 | 2, h2, 0);  /* emit jmp, jnz, jz... */
        break;
    case 101:                   /*** <GOTO.STMT> ::= <GOTO> <NUMBER> */
        Emit3D(x1 | 2, h2);     /* jmp, jnz, jz */
        break;
    case 102: Emit1(PCHL); break;   /*** <GOTO.STMT> ::= <GOTO> M ( HL ) */
    case 103: xx = JMP; break;      /*** <GOTO> ::= GOTO */
    case 104: XmitX2(); break;      /*** <GOTO> ::= IF <SIMPLE.CONDITION> GOTO */
    case 105:                       /*** <REPEAT.STMT) ::= <REPEAT> <STMT.LIST> ; UNTIL <COMPOUND.CONDIITON */
        BRAddr(l5, CA, h1);         /* backstuff look address */
        break;
    case 106:                       /*** <REPEAT> ::= REPEAT */
        hh = caNext; break;			/* save address of next instruction */
    case 107: Emit1(0x76); break;   /*** <CONTROL.STMT> :: = HALT */
    case 108: Emit1(0);	break;  	/*** <CONTROL.STMT> :: = NOP */
    case 109: Emit1(0xf3); break;   /*** <CONTROL.STMT> :: = DISABLE */
    case 110: Emit1(0xfb); break;   /*** <CONTROL.STMT> :: = ENABLE */
    case 111:                       /*** <COMPARE.STMT> ::= <REG> :: <SEC> */
        if (AReg(1)) {              /* <REG> is reg a */
            if (x3 == TREG && AM(3))/* <SEC> is a , b ...m */
                Emit1(l3 | 0xb8);	/* emit cmp */
            else if (x3 == TNUMBER)
                Emit2(0xfe, l3);	/* emit cpi */
            else
                CantDo();
        } else
            CantDo();
        break;
    case 112:                       /*** <EXCHANGE.STMT> ::= <REG> == <REG.EXPR> */
        if (HLReg(1)) {             // <REG> is hl
            if (DEReg(3))
                Emit1(XCHG);
            else if (StackR(3))
                Emit1(0xe3);		/* xthl */
            else
                CantDo();
        } else
            CantDo();
        break;
    case 113:                       /*** <VAR.ASSIGN> ::= <VAR> = <REG.EXPR> */
        if (AReg(3)) {              /* <REG.EXPR> refers to reg a */
            if (x1 == TMXX)         /* <VAR> IS M(BC) OR M(DE) */
                Emit1(l1 & 0xf2);	/* stax b|d */
            else if (x1 == TOUT)    /* <VAR> is OUT(<NUMBER>) */
                Emit2(0xd3, l1);	/* emit 'out' instruction */
            else if (LdaHL(0x32, 1))/* emitted sta */
                ;
            else
                CantDo();
        } else if (HLReg(3)) {      /* <REG.EXPR> refers to hl */
            if (LdaHL(0x22, 1))		/* shld */
                ;
            else
                CantDo();
        } else
            CantDo();
        break;
    case 114: Xmit1(); break;       /*** <REG.EXPR> ::= <REG> */
    case 115: Xmit2(); break;       /*** <REG.EXPR> ::= ( <REG.ASSIGN> ) */
    case 116:                       /*** <REG.ASSIGN> ::= <REG> = <PRIM> <BINARY.OP> <SEC> */
        LoadPrim(3);                /* load <PRIM> into <REG> */
        BinOpSec(4, 5);             /* execute <BINARY.OP> on <SEC> */
        Xmit1();
        break;
    case 117:                       /*** <REG.ASSIGN> ::= <REG> = <UNARY.OP> <PRIM> */
        LoadPrim(4);                /* load <PRIM> into <REG> */
        /* now execute <UNARY.OP> */
        if (AReg(1))                /* <REG> is a */
            Emit1(x3);              /* emit rlc, rrc, ral, rar, daa, cma */
        else if (CYReg(1) && x3 == 0x2f)    /* <REG> IS CARRY */
            Emit1(0x3f);	        /* emit cmc */
        else
            CantDo();
        Xmit1();
        break;
    case 118:                       /*** <REG.ASSIGN> ::= <REG> = <PRIM> */
        LoadPrim(3);                /* load <PRIM> into reg */
        Xmit1();
        break;
    case 119:                       /*** <REG.ASSIGN> ::= <REG.ASSIGN> , <BINARY.OP> <SEC> */
        BinOpSec(3, 4);             /* execute <BINARY.OP> on <SEC> */
        Xmit1();
        break;
    case 120:                       /*** <PRIM> ::= <VAR> */
    case 121:                       /*** <PRIM> ::= <SEC> */
    case 122:                       /*** <SEC> ::= <REG.EXPR> */
    case 123:                       /*** <SEC> ::= <CONSTANT> */
        Xmit1();
        break;
    case 124:					    /*** <CONSTANT> ::= <STRING> */
        xx = TSTRING;
        XmitH1();                   /* pointer to string in symList */
        break;
    case 125:					    /*** <CONSTANT> ::= <NUMBER> */
        xx = TNUMBER;
        XmitH1();                   /* value of number */
        break;
    case 126:					    /*** <CONSTANT> ::= - <NUMBER> */
        xx = TNUMBER;
        hh = -h2;
        break;
    case 127:					    /*** <CONSTANT> ::= . <IDENTIFIER> */
        xx = TADDRVAR;
        Idn(2, 0);
        break;
    case 128:					    /*** <CONSTANT> ::= . <IDENTIFIER> ( <NUMBER> ) */
        xx = TADDRVAR;
        Idn(2, h4);
        break;
    case 129:					    /*** <CONSTANT> ::= . <STRING> */
        xx = TADDRSTR;
        hh = h2;                    /* pointer to string in symList */
        break;
    case 130:                       /*** <REG> ::= <A.L> */
        SetReg((uint8_t)h1);	break;          /* a=7, b=0, c=1, d=2, e=3, h=4, l=5 */
    case 131: SetReg(TBC); break;	/*** <REG> ::= BC */
    case 132: SetReg(TDE); break;	/*** <REG> ::= DE */
    case 133: SetReg(THL); break;	/*** <REG> ::= HL */
    case 134: SetReg(TSP); break;	/*** <REG> ::= SP */
    case 135: SetReg(TSTACK); break;/*** <REG> ::= STACK */
    case 136: SetReg(TPSW); break;	/*** <REG> ::= PSW */
    case 137: SetReg(TMHL);	break;	/*** <REG> ::= M ( HL ) */
    case 138: SetReg(TCY); break;	/*** <REG> ::= CY */
    case 139: xx = 0x80; break;     /*** <BINARY.OP> ::= + */
    case 140: xx = 0x90; break;		/*** <BINARY.OP> ::= - */
    case 141: xx = 0xa0; break;		/*** <BINARY.OP> ::= AND */
    case 142: xx = 0xb0; break;		/*** <BINARY.OP> ::= OR */
    case 143: xx = 0xa8; break;		/*** <BINARY.OP> ::= OR */
    case 144: xx = 0x88; break;		/*** <BINARY.OP> ::= ++ */
    case 145: xx = 0x98; break;		/*** <BINARY.OP> ::= -- */
    case 146: xx = 0x17; break;		/*** <BINARY.OP> ::= < (ral) */
    case 147: xx = 0x1f; break;		/*** <BINARY.OP> ::= > (rar) */
    case 148: xx = 0x2f; break;		/*** <BINARY.OP> ::= NOT (cma)*/
    case 149: xx = 0x27; break;		/*** <BINARY.OP> ::= DEC (daa)*/
    case 150: xx = 0xf;	 break;		/*** <BINARY.OP> ::= >> (rrc) */
    case 151: xx = 7;	 break;		/*** <BINARY.OP> ::= << (rlc) */
    case 152:					    /*** <VAR> ::= M ( BC ) */
        xx = TMXX;
        ll = 0xf;
        break;
    case 153:					    /*** <VAR> ::= M ( DE ) */
        xx = TMXX;
        ll = 0x1f;
        break;
    case 154:					    /*** <VAR> ::= <IDENTIFIER> */
        xx = TVAR;
        Idn(1, 0);
        break;
    case 155:					    /*** <VAR> ::= <IDENTIFIER> ( NUMBER ) */
        xx = TVAR;
        Idn(1, h3);
        break;
    case 156:					    /*** <VAR> ::= IN ( NUMBER ) */
        xx = TIN;
        hh = h3;
        break;
    case 157:					    /*** <VAR> ::= OUT ( NUMBER ) */
        xx = TOUT;
        hh = h3;
        break;
    case 158:		                /*** <VAR> ::= M ( <CONSTANT> ) */
        xx = (x3 & 0xf) | 0x28;
        /* STRING	: CHANGES FROM 11H TO 29H
           NUMBER	: CHANGES FROM 13H TO 2BH
           .IDENTIFIER	: CHANGES FROM 15H TO 2DH
           .STRING	: CHANGES FROM 17H TO 2FH */
        hh = h3;
        yy = y3;
    }
} /* Reduce() */

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



void LoadSymbols(char *fn) {
    FILE *fp;
    long size;

    if ((fp = fopen(fn, "rb")) == NULL) {
        fprintf(stderr, "Cannot open symbol file %s\n", fn);
        Exit(1);
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if ((symList = malloc(size)) == NULL) {
        fprintf(stderr, "Out of memory!!!\n");
        fclose(fp);
        Exit(1);
    }
    if (fread(symList, 1, size, fp) != size) {
        fprintf(stderr, "Error reading in symbol file %s\n", fn);
        fclose(fp);
        Exit(1);
    }
    fclose(fp);
}

#define action  temp

void main(int argc, char **argv) {
    uint8_t i;

    if (argc != 2) {
        fprintf(stderr, "Usage: l82 [-v] | [-V] | file\n");
        Exit(1);
    }
    
    if (stricmp(argv[1], "-v") == 0) {
        showVersion(stdout, argv[1][1] == 'V');
        exit(0);
    }

#ifdef _TRACE
    printf("TRACE(C,D,P,N):");
    trace = getche();
#endif

    char *s = GetExt(argv[1]);

    sFile = NewExt(argv[1], s - argv[1], ".80s");
    pFile = NewExt(argv[1], s - argv[1], ".80p");
    dFile = NewExt(argv[1], s - argv[1], ".80d");
    cFile = NewExt(argv[1], s - argv[1], ".80c");
    rFile = NewExt(argv[1], s - argv[1], ".80r");

    LoadSymbols(sFile);

    if ((fpPol = fopen(pFile, "rb")) == NULL) {
        fprintf(stderr, "can't open %s\n", pFile);
        Exit(1);
    }
    if ((fpCode = fopen(cFile, "wb")) == NULL) {
        fprintf(stderr, "can't create %s\n", cFile);
        Exit(1);
    }
    if ((fpData = fopen(dFile, "wb")) == NULL) {
        fprintf(stderr, "can't create %s\n", dFile);
        Exit(1);
    }
    if ((fpReloc = fopen(rFile, "wb")) == NULL) {
        fprintf(stderr, "can't create %s\n", rFile);
        Exit(1);
    }
    // initialise the relocation table
    for (i = 0; i <= RELTSIZE; i++) {
        SRBase(i, UNUSED);
    }
    // enter the initial block and start compiling
    PushBl();
    while (1) {
        action = Get1();
        if (action == XREDUCE) {
            uint8_t i;      // index of handle(0)

            i = pTop - Get1(); // Get1: no. elements in handle
            h = &psh[i];
            x = &psx[i];
            y = &psy[i];
            // set non-indexed variables
            h1 = h[1];
            h2 = h[2];
            h3 = h[3];
            h4 = h[4];
            h5 = h[5];
            x1 = x[1];
            x2 = x[2];
            x3 = x[3];
            x4 = x[4];
            y1 = y[1];
            y2 = y[2];
            y3 = y[3];
            hh = 0;
            ll = 0;
            Reduce(Get1() & 0xff);
            pTop = i + 1;	   // pop tokens
            psh[pTop] = hh | ll;
            psx[pTop] = xx;
            psy[pTop] = yy;
#ifdef _TRACE
            if (trace == 'P')
                Trac('Q');
#endif
        } else if (temp == XSHIFT) {	  // push token value
            if (++pTop >= STACKSIZE)
                KError(0xf1);
            psh[pTop] = Get2();
#ifdef _TRACE
            if (trace == 'P')
                Trac('X');
#endif
        } else if (temp == XLINE)	// set line number
            line = Get2();
        else if (temp == XACCEPT)	// all done
            break;
        else
            KError(0xf5);
    }
    for (i = 0; i < sizeof(rsb); i++) {
        if (RBase(i) == UNDEF)
            KError(0xe);
        if (RBase(i) != UNUSED)
            DumpRt(i);
    }
    PopBl(0);
    WrR1('.');
    WrR2(caNext);
    WrR2(idaNext);
    WrR2(waSize);
    CloseAll();
}
