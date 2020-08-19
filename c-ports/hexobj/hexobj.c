#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

void showVersion(FILE *fp, bool full);
// intel OMF record types
#define MODHDR  2
#define MODEND  4
#define CONTENT 6
#define MODEOF  0xe
#define MODLOC  0x12

#define MAXNAME     31
#define MAXCMDLINE  128
#define MAXNUMSTR   17
#define BUFFERSIZE  0xc000

char *inFile;
char *outFile;
int startValue = -1;
uint8_t hexcrc;

/*                                 */
/*  content record definition      */
/*                                 */
struct {
    uint8_t type;
    uint8_t length[2];
    uint8_t segId;
    uint8_t addr[2];
    uint8_t dat[BUFFERSIZE + 1];    // allow for crc
} content = { CONTENT };


// return the trailing filename part of the passed in path
const char *basename(const char *path) {
    const char *t;
    while (t = strpbrk(path, ":\\/"))       // allow windows & unix separators - will fail for unix if : in filename!!
        path = t + 1;
    return path;
}


void OutRecord(uint8_t *p, FILE *fpout) {
    uint16_t addr = p[1] + p[2] * 256;
    uint8_t crc = 0;

    for (int i = 0; i <= addr + 1; i++)
        crc += p[i];
    p[addr + 2] = -crc;
    if (fwrite(p, 1, addr + 3, fpout) != addr + 3) {
        fprintf(stderr, "%s: write error\n", outFile);
        exit(1);
    }
}

uint8_t nibble(uint8_t c) {
    return  isdigit(c) ? c - '0' : isxdigit(c) ? c - 'A' + 10 : 255;
}

int ScanInteger(char **pp) {
    int val = 0;
    char *s;
    char *p = *pp;
    uint8_t radix = 10;

    while (isblank(*p))
        p++;

    for (s = p; isxdigit(*s); s++)
        ;
    *pp = s;
    if (*s == 'H')
        radix = 16;
    else if (*s == 'O' || *s == 'Q')
        radix = 8;
    else if (s != p && (s[-1] == 'B' || s[-1] == 'D') && *--s == 'B')
        radix = 2;

    if (p == s)
        return -1;
    for (; p < s; p++) {
        uint8_t digit = nibble(*p);
        if (digit > radix)
            return -1;
        val = val * radix + digit;
        if (val > 0xffff)
            return -1;
    }
    return val;
}

uint8_t getChar(FILE *fp) {
    int c = getc(fp);
    if (c == EOF) {
        fprintf(stderr, "%s: unexpected EOF\n", inFile);
        exit(1);
    }
    return toupper(c & 0x7f);
}

uint8_t getHex(FILE *fp) {
    return nibble(getChar(fp));
}

uint8_t getByte(FILE *fp) {
    uint8_t c = getHex(fp) << 4;
    hexcrc += (c += getHex(fp));
    return c;
}

uint16_t getWord(FILE *fp) {
    uint16_t v = getByte(fp) * 256;
    return v + getByte(fp);
}


void putWord(uint8_t *p, uint16_t val) {
    *p++ = val % 256;
    *p = val / 256;
}

char *deblank(char *s) {
    while (*s == ' ')
        s++;
    return s;
}

bool parseStdOpt(int argc, char **argv) {
    int addr;
    startValue = -1;
    if (argc < 3 || argc > 4)
        return false;
    inFile = argv[1];
    outFile = argv[2];
    if (argc == 3)
        return true;
    char *s = argv[3];
    if ((addr = ScanInteger(&s)) < 0)
        return false;
    startValue = addr;
    return true;
}


bool parseIntelOpt(int argc, char **argv) {
    char cmdOpts[MAXCMDLINE + 1];
    startValue = -1;

    if (argc < 4 || stricmp(argv[2], "to") != 0)
        return false;
    inFile = argv[1];
    outFile = argv[3];
    if (argc == 4)
        return true;
    // join all the remaining opts into a single string
    for (int i = 4; i < argc; i++) {
        if (strlen(cmdOpts) + strlen(argv[i] + 1) < MAXCMDLINE)
            strcat(cmdOpts, argv[i]);
        strcat(cmdOpts, " ");
    }

    char *s = deblank(cmdOpts);
    if (*s == '$')
        s = deblank(s + 1);
    if (strnicmp(s, "start", 5) != 0) {
        int addr;
        s = deblank(s + 5);
        if (*s != '(')
            return false;
        s = deblank(s + 1);
        if ((addr = ScanInteger(&s)) < 0)
            return false;
        s = deblank(s);
        if (*s != ')')
            return false;
        startValue = addr;
        s = deblank(s + 1);
    }
    return *s == 0;
}


void writeModHdr(FILE *fp, char *path) {
    /*
     * output module header record, taking name from input file
     * this version allows upto MAXNAME characters, original limited at 6
     * non alpha numeric are deleted and all alpha is converted to uppercase
     */
    struct {
        uint8_t type;
        uint8_t length[2];
        uint8_t nameLen;
        uint8_t name[MAXNAME + 3];  // allow for trnId, trnVn and checksum
    } modhdr = { MODHDR };

    int i = 0;
    for (const char *name = basename(path); *name && *name != '.'; name++)
        if (isalnum(*name) && i < MAXNAME)
            modhdr.name[i++] = toupper(*name);
    modhdr.nameLen = i;
    putWord(modhdr.length, i + 4);
    modhdr.name[i++] = 0;   // TRN ID
    modhdr.name[i++] = 0;   // TRN VN
    OutRecord((uint8_t *)&modhdr, fp);
}



void writeContent(FILE *fpin, FILE *fpout) {
    struct {
        uint8_t type;
        uint8_t length[2];
        uint8_t segid;
        uint8_t offset[2];
        uint8_t namelen;
        uint8_t name[MAXNAME + 2];    // allow for 0 and crc
    } modloc = { MODLOC };

    uint8_t rlen = 1;
    uint8_t c;
    unsigned i;
    uint16_t recordPtr = 0;
    uint16_t contentPos = 0;
    uint16_t recordAddress;
    /*
     * lines processed as follows
     * 1)  public names with the format number name address
     * 2)  standard intel nibble - these start with a :
     */

    while (rlen) {
        while ((c = getChar(fpin)) != ':') {
            if (isdigit(c)) {
                while (getChar(fpin) != ' ')         // skip number & following space
                    ;
                while ((c = getChar(fpin)) == ' ')
                    ;
                for (i = 0; c != ' '; c = getChar(fpin))
                    if (isalnum(c) && i < MAXNAME)
                        modloc.name[i++] = c;

                modloc.name[i] = 0;
                modloc.namelen = i;
                putWord(modloc.length, i + 6);

                while (!isdigit(c = getChar(fpin)))
                    ;
                char temp[MAXNUMSTR + 1];
                for (i = 0; c != ' ' && c != ' ' && c != '\n'; c = getChar(fpin))
                    if (i < MAXNUMSTR)
                        temp[i++] = c;
                temp[i] = 0;
                char *s = temp;
                putWord(modloc.offset, ScanInteger(&s));
                OutRecord((uint8_t *)&modloc, fpout);
            }
        }
        hexcrc = 0;
        rlen = getByte(fpin);
        if (rlen) {
            recordAddress = getWord(fpin);
            if (recordPtr != recordAddress || contentPos >= BUFFERSIZE) {
                if (contentPos) {
                    putWord(content.length, contentPos + 4);
                    OutRecord((uint8_t *)&content, fpout);
                }
                contentPos = 0;
                recordPtr = recordAddress;
                putWord(content.addr, recordAddress);
            }
            getByte(fpin);
            for (i = 0; i < rlen; i++)
                content.dat[contentPos++] = getByte(fpin);
            recordPtr += rlen;
            getByte(fpin);   /* compute checksum */
            if (hexcrc) {
                fprintf(stderr, "%s: Checksum error\n", inFile);
                exit(1);
            }
        }
    }
    if (contentPos) {
        putWord(content.length, contentPos + 4);
        OutRecord((uint8_t *)&content, fpout);
    }
}

void writeModEnd(FILE *fpin, FILE *fpout) {
    struct {
        uint8_t type;
        uint8_t length[2];
        uint8_t modtype;
        uint8_t segId;
        uint8_t offset[2];
        uint8_t crc;
    } modend = { MODEND, 5, 0, 1 };

    // note rlen byte already read
    uint16_t recordAddress = getWord(fpin);
    putWord(modend.offset, startValue >= 0 ? startValue : recordAddress);
    OutRecord((uint8_t *)&modend, fpout);
    getByte(fpin);
    getByte(fpin);
    if (startValue < 0 && hexcrc)
        fprintf(stderr, "%s: Warning checksum error in start address\n", inFile);
}

void writeModEof(FILE *fpout) {
    struct {
        uint8_t type;
        uint8_t length[2];
        uint8_t crc;
    } modeof = { MODEOF, 1, 0 };
    OutRecord((uint8_t *)&modeof, fpout);
}



int main(int argc, char **argv) {
    FILE *fpin, *fpout;
    int number = 0;

    if (argc == 2 && stricmp(argv[1], "-v") == 0) {
        showVersion(stdout, argv[1][1] == 'V');
        exit(0);
    }
    // support both a standard command line and the original Intel one
    if (!parseStdOpt(argc, argv) && !parseIntelOpt(argc, argv)) {
        fprintf(stderr, "Invalid command line\n"
            "Usage: %s hexfile objfile [startaddr]\n"
            "Or:    %s hexfile TO objfile [$] [START ( startaddr ) ]\n", inFile, inFile);
        exit(1);
    }
    if ((fpin = fopen(inFile, "rt")) == 0) {
        fprintf(stderr, "%s: cannot open hex file\n", inFile);
        exit(1);
    }
    if ((fpout = fopen(outFile, "wb")) == 0) {
        fprintf(stderr, "%s: cannot create obj file\n", outFile);
        exit(1);
    }

    writeModHdr(fpout, inFile);
    writeContent(fpin, fpout);
    writeModEnd(fpin, fpout);
    writeModEof(fpout);
    fclose(fpin);
    if (fclose(fpout) < 0) {
        fprintf(stderr, "%s: Error closeing file\n", outFile);
        exit(1);
    }
}
