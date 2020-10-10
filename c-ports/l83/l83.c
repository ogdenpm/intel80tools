#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
void showVersion(FILE *fp, bool full);

#define SCODE	1
#define SDATA	2
#define SLOCAL	3


char *rootDir;

FILE *fpCom;
FILE *fpPrn;
FILE *fpRel;
char *rtFile;
char *comFile;
char *prnFile;

/* mnemonics for segments */
#define CA  1       /* CODE AREA */
#define IDA 2       /* INITIAL DATA AREA */
#define WA  3       /* WORK AREA */

// module map
#define MSIZE	20		// max no. modules l83 can handle
uint8_t *mName[MSIZE]; 	// pointer to module name
uint16_t mCA[MSIZE];	// uint16_t of ca
uint16_t nCA[MSIZE];	// length of ca
uint16_t mIDA[MSIZE];	// uint16_t of ida
uint16_t nIDA[MSIZE];	// length of ida
uint16_t mWA[MSIZE];	// uint16_t of wa
uint8_t *mSymb[MSIZE];	// base uint16_t of module's symb list
uint8_t mHash[MSIZE];	// hashcode of module's name
uint8_t mTop = 0;		// index of last entry in the mm


/* logical record from the relocation table path */
uint8_t rec;        /* record type */
uint16_t rLoc;      /* location of address to be relocated */
uint16_t rDispl;    /* displacement */
uint16_t rExt;      /* pointer to external names */
uint8_t rSeg;       /* segment of address to be relocated */
uint8_t rBase;      /* base */



/* (address (base, displ) is to be inserted at (seg, loc)) */
/* rec = 'r' indicates 'relocation' record */
/* rec = 's' indicates 'symbol ref' record */
/* rec = '.' indicates last record (counters) */

/* field redefinitions for 's' records */
#define sDispl	rLoc
#define sLine	rDispl
#define sName   rExt
#define sType   rSeg
#define sBase   rBase

// field definitions for '.' records
#define caSize  rLoc
#define idaSize	rDispl
#define waSize	rExt


uint8_t *symList;           /* symbol list (for each module) replaces symbl addres, symList based symbl byte */
uint8_t *seg;               /* segment being updated in memory, replaces segb address, seg  based segb byte */
uint16_t loadAdr = 0x100;  /* LOAD ADDRESS FOR OBJ MODULE */
uint16_t la;                /* LOAD ADDRESS FOR EACH SEGMENT */
uint16_t memPtr;            /* POINTER TO NEXT BYTE IN MEMORY */
bool lstReq;                /* true TO PRINT SYMBOL LIST */
uint8_t m;                  /* index of module in the module map */
uint16_t mp;                /* auxiliary memory pointer */
#define HASHMASK    128
uint8_t hashCode;
uint16_t temp;

char *symAddress;


/* MNEMONICS FOR 'TYPE' */
#define KLABEL 1
#define KBYTE  2
#define PROC   3
#define EXT    4
#define GLOB   5
#define STRING 7
#define UNDECL 6

#ifdef _TRACE
#include <conio.h>
#include <ctype.h>
bool trace;
uint16_t tract;
#endif


char *MkPath(char *file, char *ext);

uint16_t DoubleT(uint8_t l, uint8_t h) {
    return (h << 8) + l;

}

void Store(uint16_t n, uint8_t *a) {
    a[0] = (uint8_t)n;
    a[1] = n / 256;
}


void Exit(int rcode) {
    printf("\nEnd L83\n");
    exit(rcode);
}


__declspec(noreturn) void Error(uint8_t n) {
    switch (n) {
    case 0:
        fprintf(stderr, "Too many modules\n");
        break;
    case 1:
        fprintf(stderr, "Can't create executable file %s\n", comFile);
        break;
    case 2:
        fprintf(stderr, "Error closing %s\n", comFile);
        break;
    case 3:
        fprintf(stderr, "Write error to executable %s\n", comFile);
        break;
    case 4:
        fprintf(stderr, "Read error on %s\n", rtFile);
        break;
    case 5:
        fprintf(stderr, "can't open relocation file %s\n", rtFile);
        break;
    case 6:
        fprintf(stderr, "Out of memory\n");
        break;
    case 7:
        fprintf(stderr, "Bad reloc file %s\n", rtFile);
        break;

    }
    Exit(1);
}


#ifdef _TRACE
void Trac(uint8_t b) {
    uint8_t i;
    uint16_t j;

    if (!trace)
        return;

    putchar(b);
    while ((b = toupper(getche())) != ' ') {
        if (b == 'M')
            for (i = 0; i <= mTop; i++)
                printf(":%02X:%p:%04X:%04X:%04X:%p:%02X", i, mName[i], nCA[i], mIDA[i], mWA[i], mSymb[i], mHash[i]);
        else if (b == 'R')
            printf(":%02X:%04X:%04X:%04X:%02X:%02X", rec, rLoc, rDispl, rExt, rSeg, rBase);
        else if (b == 'S')
            for (j = 0; j < tract; j++)
                printf(":%02X", seg[j]);
        putchar(' ');
    }

}
#endif

uint8_t Get1() {
    int c;
    if ((c = getc(fpRel)) == EOF) {
        if (feof(fpRel))
            return 0;
        Error(4);
    }
    return c;
}

uint16_t Get2() {
    uint8_t c;
    c = Get1();
    return DoubleT(c, Get1());
}


void Dump(uint16_t n) {
    /* dump n bytes of sebment (ca, ida) currently in memory into the
       object module output path */
    if (fwrite(seg, 1, n, fpCom) != n)
        Error(3);
} /* Dump() */

void ReadRR() {                 // read logical record from rt path
    rec = Get1();
    rLoc = Get2();
    rDispl = Get2();
    rExt = Get2();
    if (rec != '.') {           // not trailer record
        uint8_t c = Get1();
        rSeg = c >> 4;          // higher nibble
        rBase = c & 0xf;        // lower nibble
    }
#ifdef _TRACE
    Trac('T');
#endif
} /* ReadRR() */


void OpenRT() {
    free(rtFile);
    char *rtFile = MkPath(mName[m], ".80r");
    if ((fpRel = fopen(rtFile, "rb")) == NULL)
        Error(5);
    symList = mSymb[m];
}



uint16_t GetDirLen(char *path) {
    char *s = path;
    char *t;
    if (t = strrchr(s, '\\'))       // find last \ directory separator if one
        s = t + 1;
    if (t = strrchr(s, '/'))        // find last / directory separator just in case
        s = t + 1;
    if (*s && s[1] == ':')          // for windows check for device
        s += 2;
    return (uint16_t)(s - path);
}

char *getFile(char *file) // assumes dir already stripped !!!
{
    char *s = strdup(file);
    char *t = strrchr(s, '.');
    if (t)
        *t = 0;
    return s;
}

char *MkPath(char *file, char *ext) {
    char *fn = malloc(strlen(rootDir) + strlen(file) + strlen(ext) + 1);
    if (fn == NULL)
        Error(6);
    strcpy(fn, rootDir);
    char *s = strchr(fn, '\0');     // locate file name as this will  be lowercased
    strcat(s, file);
    strcat(s, ext);
    strlwr(s);
    return fn;
}

uint8_t *ReadF(char *fn) {
    FILE *fp;
    long size;
    uint8_t *fdata;


    if ((fp = fopen(fn, "rb")) == NULL) {
        fprintf(stderr, "Cannot open file %s\n", fn);
        Exit(1);
    }
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    if ((fdata = malloc(size)) == NULL) {
        fclose(fp);
        fprintf(stderr, "Out of memory reading %s\n", fn);
        Exit(1);
    }
    if (fread(fdata, 1, size, fp) != size) {
        fprintf(stderr, "Error reading in file %s\n", fn);
        fclose(fp);
        Exit(1);
    }
    fclose(fp);
    return fdata;
}




bool Compar(uint8_t *a1, uint8_t *a2) { /* true if strings at a1,a2 are equal */
    return strcmp(a1, a2) == 0;
} /* Compar() */

uint8_t HashF(uint8_t *a) {             /* return hascode of name at address symAddress */
    uint8_t h = 0;
    while (*a)
        h += *a++;
    return h & 0x7f;
}

bool New(uint8_t *a) {                  /* true if name at symAddress is not in the module map */

    hashCode = HashF(a);                /* compute hashcode of the name */
    for (uint8_t k = 0; k <= mTop; k++) {
        if (mHash[k] == hashCode) {     // may be equal
            if (Compar(a, mName[k]))
                return false;           // equal
        }
    }
    return true;
}

void Concat(uint16_t *a) {
    /* compute load addresses of segments in the module map,
       so that all segments are concatenated in memory */

    for (uint8_t j = 0; j <= mTop; j++) {
        temp = a[j] + la;           // save address of next segment
        a[j] = la;                  // set size of this segment
        la = temp;                  // set address for next segment
    }
}

uint16_t Entry(uint8_t n) {

    if (rBase == CA)
        return mCA[n];
    if (rBase == IDA)
        return mIDA[n];
    if (rBase == WA)
        return mWA[n];
    Error(7);                       // corrupt path
}

void Resolve() {
    /* use the information in the current relocation record
       to resolve one address in the segment (ca, ida) currently
       in memory, which belongs to module m */
    uint8_t k;

    if (rExt) {                     // an external reference,
        k = symList[rExt - 1];      /* pointer to external module in mm */
        temp = Entry(k);
        k = seg[rLoc - 1];          /* look at previous instruction */
        /* check for call or cnz, cz... */
        if (rBase == CA && (k == 0xcd || (k & 0xc7) == 0xc4))
            temp += 3;              // adjust entry to procedure
    } else
        temp = Entry(m);            // not an external reference
    Store(temp + rDispl, &seg[rLoc]);
} /* Resolve() */

void LstRef() {
    /* print information about symbol whose relocation record
       is currently in memory, and which belongs to module m */
    if (sType == EXT || sType == GLOB)
        return;
    fprintf(fpPrn, "L:%04X A:%04X T:", sLine, Entry(m) + sDispl);
    if (sType == KLABEL)
        putc('L', fpPrn);	/* label */
    else if (sType == KBYTE)
        putc('B', fpPrn);	/* uint8_t */
    else if (sType == PROC)
        putc('P', fpPrn);	/* procedure */
    else if (sType == STRING)
        putc('S', fpPrn);
    fprintf(fpPrn, " %s\n", &symList[sName]);
}

void main(int argc, char **argv) {
    int i;
    uint16_t dirLen;

    if (argc == 2 && stricmp(argv[1], "-v") == 0) {
        showVersion(stdout, argv[1][1] == 'V');
        exit(0);
    }

    for (i = 1; i < argc - 1; i++) {
        if (strcmp(argv[i], "-s") == 0)
            lstReq = true;
#ifdef _TRACE
        else if (strcmp(argv[i], "-t") == 0)
            trace = true;
#endif
        else if (strcmp(argv[i], "-l") == 0 && i + 1 < argc) {
            int addr;
            char junk;
            if (scanf(argv[++i], "%x%c", &addr, &junk) != 1 || addr > 0xffff) {
                fprintf(stderr, "Error parsing hex address %s\n", argv[i]);
                Exit(1);
            } else
                loadAdr = addr;
        } else {
#ifdef _TRACE
            fprintf(stderr, "Usage: l83 [-v|V] | [-s] [-l addr] [-t] file\n");
#else
            fprintf(stderr, "Usage: l83 [-s] [-l addr] file\n");

#endif

            Exit(1);
        }
    }
    dirLen = GetDirLen(argv[i]);
    rootDir = malloc(dirLen + 1);
    strncpy(rootDir, argv[1], dirLen);
    rootDir[dirLen] = 0;
    mName[0] = strupr(strdup(argv[i] + dirLen));

    la = loadAdr;
    mHash[0] = HashF(mName[0]);
    for (m = 0; m <= mTop; m++) {
        mSymb[m] = ReadF(MkPath(mName[m], ".80s")); // load symbol list into memory
        OpenRT();               // open rt file of module m
        ReadRR();               // read record from rt file
        while (rec != '.') {    // not end
            if (rec == 'R' && rExt != 0) {  // relocation record & ext reference
                symAddress = &symList[rExt];// address of the symbol
                if (New(symAddress)) {      // not yet in module map
                    if (++mTop >= MSIZE)
                        Error(0);
                    symList[rExt - 1] = mTop;   // set pointer to mm
                    mName[mTop] = symAddress;   // set pointer from mm to symbol
                    mHash[mTop] = hashCode;     // save syymbol hashcode in mm
                }   // installed
            } // created
            ReadRR();                           // read next record from rt file
        }
        fclose(fpRel);
        mCA[m] = nCA[m] = caSize;
        mIDA[m] = nIDA[m] = idaSize;
        mWA[m] = waSize;
    }
    Concat(mCA);        // work out total size of each area
    Concat(mIDA);
    Concat(mWA);
#ifdef _TRACE
    Trac('P');
#endif
    // construct the object module
    // create the .com file
    comFile = MkPath(mName[0], ".com");
    if ((fpCom = fopen(comFile, "wb")) == NULL)
        Error(1);

    if (lstReq) {
        prnFile = MkPath(mName[0], ".prn");
        if ((fpPrn = fopen(prnFile, "wt")) == NULL) {
            fprintf(stderr, "Error creating symbols listing file %s.prn. Skipping\n", mName[0]);
            lstReq = false;
        }
    }

    for (m = 0; m <= mTop; m++) {               // process all of the code modules
        seg = ReadF(MkPath(mName[m], ".80c"));
#ifdef _TRACE
        tract = nCA[m];
        Trac('A');
#endif
        OpenRT();
        ReadRR();
        while (rec != '.') {                    // process all applicable relocation records
            if (rec == 'R' && rSeg == CA)
                Resolve();
            ReadRR();
#ifdef _TRACE
            Trac('C');
#endif
        }
        Dump(nCA[m]);                           // dump the code segment
        fclose(fpRel);
        free(seg);                              // give back the seg memory
    }
    for (m = 0; m <= mTop; m++) {               // do same for data records
        seg = ReadF(MkPath(mName[m], ".80d"));
#ifdef _TRACE
        tract = nIDA[m];
        Trac('B');
#endif
        OpenRT();
        ReadRR();
        while (rec != 0x2e) {
            if (rec == 'R' && rSeg == IDA)
                Resolve();
            if (rec == 'S' && lstReq)           // save symbol info if requested
                LstRef();
            ReadRR();
        }
        Dump(nIDA[m]);                          // save teh updated data area
        fclose(fpRel);
        free(seg);                              // and free memory
    }
    if (fpPrn && fclose(fpPrn) != 0)
        fprintf(stderr, "Error closing symbols listing %s\n", prnFile);
    if (fclose(fpCom) != 0)
        Error(2);

    printf("\nLoad address - %04X\n", loadAdr);
    printf("Address last = %04X\n", la - 1);
    printf("Modules linked:\n");

    for (i = 0; i <= mTop; i++)
        printf("%s\n", mName[i]);
    Exit(0);
}
