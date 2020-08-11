#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <io.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include "omf.h"
void showVersion(FILE *fp, bool full);

#pragma pack(1)
typedef unsigned char byte;
typedef unsigned short address;
FILE *logfp = NULL;
char *invoke;

enum {
    Ok = 1, NormEof = 0, PremEof = -1, BadCrc = -2, BadType = -3, BadLen = -4, Spurious = -5
};


int omfVer = OMF85;

void usage(char const *s) {
        if (s && *s)
            fputs(s, stderr);
        fprintf(stderr, "usage: %s -v | -V | binfile objfile [patchfile]\n", invoke);
        exit(1);
}

void dumprec(FILE *fpout, bool isGood);

long fpos = 0;

uint8_t rec[MAXREC];
unsigned int reclen;
int rectype;
unsigned int recptr;
unsigned savedRecptr;

void Log(char const *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    vfprintf(stderr, fmt, args);
    if (logfp && (logfp != stdout || !_isatty(_fileno(logfp))))
        vfprintf(logfp, fmt, args);
    va_end(args);
}

int getrec(FILE *fp) {
    static bool endSeen = false;

    long start = ftell(fp);

    if ((rectype = getc(fp)) == EOF)
        return NormEof;
    if (rectype % 2 || !((2 <= rectype && rectype <= 0x2e) || (0x6e <= rectype && rectype <= 0xce))) {
        fseek(fp, start, SEEK_SET);
        if (endSeen)
            return(Spurious);
        return BadType;
    }
    reclen = getc(fp);
    reclen += getc(fp) * 256;

    if (reclen > MAXREC) {
        fseek(fp, start, SEEK_SET);
        if (endSeen)
            return Spurious;
        return BadLen;
    }
    if (fread(rec, 1, reclen, fp) != reclen) {
        fseek(fp, start, SEEK_SET);
        if (endSeen)
            return Spurious;
        return PremEof;
    }
    endSeen = rectype == 0xe || rectype == 0x8a || rectype == 0xaa;
    byte crc = rectype + reclen % 256 + reclen / 256;
    for (unsigned int i = 0; i < reclen; i++)
        crc += rec[i];

    reclen--;
    recptr = 0;

    return crc == 0 ? Ok : BadCrc;
}


bool getRecNumber(uint8_t size, uint32_t *pval) {
    if (recptr + size > reclen)
        return false;
    uint32_t val = 0;
    for (int i = 0; i < size; i++) {
        val += rec[recptr++] << (8 * i);
    }
    if (pval)
        *pval = val;
    return true;
}

bool getRecIndex(uint32_t *pval) {
    if (recptr >= reclen || ((rec[recptr] & 0x80) && recptr + 1 >= reclen))
        return false;

    uint32_t val = rec[recptr++];
    if (val & 0x80)
        val = ((val & 0x7f) << 8) + rec[recptr++];
    if (pval)
        *pval = val;
    return true;
}

bool getRecName(uint32_t *plen, uint8_t *str) {
    if (recptr >= reclen || (recptr + rec[recptr]) >= reclen)
        return false;

    uint8_t len = rec[recptr++];
    for (uint32_t i = 0; i < len; i++, recptr++)
        if (str)
            *str++ = rec[recptr];
    if (str)
        *str = 0;
    if (plen)
        *plen = len;
    return true;
}

bool getRecRest(uint32_t *plen, uint8_t *str) {     // cannot be used for more than 255 remaining chars
    if (recptr >= reclen || reclen - recptr > 255)
        return false;
    if (plen)
        *plen = reclen - recptr;
    for (; recptr < reclen; recptr++)
        if (str)
            *str++ = rec[recptr];
    if (str)
        *str = 0;
    return true;
}


char const *getRecToEnd(uint32_t *len) {
    *len = reclen - recptr;
    return rec + recptr;
}

uint32_t getRecPos() {
    return recptr;
}

void setRecPos(uint32_t pos) {
    recptr = pos;
}

bool isRecEnd() {
    return recptr >= reclen;
}



int main(int argc, char **argv) {
    FILE *fp;
    FILE *fpout;
    bool done = false;
    int recCnt = 0;;
    int len;
    unsigned addr = 0;

    invoke = argv[0];
    if (argc == 2 && _stricmp(argv[1], "-v") == 0) {
        showVersion(stdout, argv[1][1] == 'V');
        exit(0);
    }
    if (argc < 2 || (fp = fopen(argv[1], "rb")) == NULL)
        usage("can't open input file\n");

    if (argc != 3 || (fpout = fopen(argv[2], "w")) == NULL)
        fpout = stdout;
    logfp = fpout;

    while (!done) {
        switch (getrec(fp)) {
        case NormEof:
            done = true;
            break;
        case Ok:
            dumprec(fpout, true);
            recCnt++;
            break;
        case BadCrc:
            if (recCnt++) {
                Log("Corrupt record type %d, length %d\n", rectype, reclen);
                dumprec(fpout, false);
            } else {
                Log("Not OMF file\n");
                done = true;
            }
            break;
        case Spurious:
            if (recCnt == 0) {
                Log("Not OMF file\n");
            } else {
                Log("Spurious data at end of file\n");
                do {
                    len = fread(rec, 1, sizeof(rec), fp);
                    if (len) {
                        reclen = len;
                        recptr = 0;
                        printData(fpout, addr);
                        addr += len;
                    }
                } while (len == sizeof(rec));
            }
            done = true;
            break;
        case BadType:
            if (recCnt == 0)
                Log("Not OMF file\n");
            else
                Log("Invalid Record type %02X\n", rectype);
            done = true;
            break;
        case PremEof:
            if (recCnt == 0)
                Log("Not OMF file\n");
            else
                Log("Fatal: Premature EOF - record type %d\n");
            done = true;
            break;

        case BadLen:
            if (recCnt == 0)
                Log("Not OMF file\n");
            else
                Log("Fatal: Record length > %d\n", MAXREC);
            done = true;
            break;
        }
    }
    fclose(fpout);
    return 0;
}


void dumprec(FILE *fpout, bool isGood) {
 //   fprintf(fpout, "%02X:%02X ", fpos / 128, fpos % 128);
 //   VColumn = 6;


    parseRule(fpout, rectype);
    fpos += 4 + reclen;
    putc('\n', fpout);
}


