#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdint.h>


/*
*   obj2bin started off as a simple tool to convert Intel OMF object files to binary files e.g. CP/M .COM files
*   In addition, as .COM files often contained additional data to pad to a sector boundary and did not support
*   uninitalised data, the tool allows a patch file to be applied
*   The tool now also supports a -i option to allow Intel .BIN files to be created
* 
*   THe patch file has two modes, PATCH and APPEND.
*   Initially the mdoe is PATCH and if the key word APPEND (case insensitive) is seen at the star of a line
*   the mode swithes to APPEND
*   In patch mode each line starts with a hex address to start the patch, anything other than a hex value is ignore
*   In append mode the next available address is implied.
*   All other data one the line is assumed to be a set of space separated values to use in one of several formats
* 
*   value ['x' repeatCnt]
*       where value can be either
*           hexvalue 
*           'string'                        note string supports \n \r \t \' and \\ escapes
*           -                               set to uninitialised (really only useful for Intel .BIN files)
*           =                               leave unchanged i.e. skip the bytes
* 
*   If the first item on a line doesn't match a hex address in PATCH mode or a value in APPEND mode the 
*   line is silently skipped
*   In APPEND mode - and = are treated as 0
* 
*  whilst anthing that doesn't look like a value termiante the line a value followed by a bad repeat cound
*  notes an error.
*  The above noted a ; is treated as the end of line
* 
*   Note APPEND mode is neded for Intel .BIN files to place data after the start address record
*   A normal patch would incorrectly include the extra data before this start address
* 
*  Note the older start-end option has been removed as this would clash with the - option. The x repeatCnt
*  provides an alternative approach
*/



void showVersion(FILE *fp, bool full);
__declspec(noreturn) void usage(char *fmt, ...);
char *invokedBy;

void readRec(FILE *fp);
void skipRec(FILE *fp, int len);
unsigned start;

void read6(FILE *fp, int len);
void read4(FILE *fp, int len);

#define MAXMEM 0x11000       /* appended data is placed after the program code,  allow 4k for worst case of code ending at 0xffff */

uint8_t mem[MAXMEM];
uint8_t use[MAXMEM];

/* the use types for data */
enum { UNINITIALISED = 0, DATA, PADDING};

/* types for values */
enum { EOL, HEXVAL, STRING, SKIP, DEINITIALISE, APPEND, INVALID, ERROR};
typedef struct {
    uint8_t type;
    union {
        unsigned hval;
        char *str;
    };
    int repeatCnt;
} value_t;



unsigned int low = MAXMEM;
unsigned int high = 0;

void loadfile(char *s);
int dumpfile(char *s, bool intelBin);
void patchfile(char *s);


__declspec(noreturn) void usage(char *fmt, ...) {

    if (fmt) {
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
    }
    fprintf(stderr, "\nUsage: %s -v | -V |  [-i] infile [patchfile] outfile\n"
                      "Where -v/-V provide version information\n"
                      "and   -i    produces Intel bin files\n", invokedBy);

    exit(1);
}




int main(int argc, char **argv)
{
    bool intelBin = false;
    invokedBy = argv[0];

    if (argc == 2 && _stricmp(argv[1], "-v") == 0) {
        showVersion(stdout, argv[1][1] == 'V');
        exit(0);
    }
    if (argc >= 2 && _stricmp(argv[1], "-i") == 0) {
        intelBin = true;
        argc--;
        argv++;
    }
    if (argc < 3 || argc > 4)
        usage(NULL);

    loadfile(argv[1]);
    if (argc == 4)
        patchfile(argv[2]);
    return dumpfile(argv[argc - 1], intelBin);
}

void loadfile(char *file)
{

    FILE *fp;
    if ((fp = fopen(file, "rb")) == NULL)
        usage("can't open input file %s\n", file);

    do {
        readRec(fp);
    } while (! feof(fp));
    fclose(fp);
}


bool fputblock(FILE *fp, unsigned low, unsigned high, bool intelBin) {
    unsigned len = high - low;
    if (intelBin) {
        fputc(len % 256, fp);
        fputc(len / 256, fp);
        fputc(low % 256, fp);
        fputc(low / 256, fp);
    }
    return fwrite(&mem[low], 1, high - low, fp) == high - low;
}


int dumpfile(char *file, bool intelBin)
{
    FILE *fp;
    bool isOk = true;
    unsigned addr;

    if ((fp = fopen(file, "wb")) == NULL)
        usage("can't create output file %s\n", file);

    if (!intelBin)
        isOk = fputblock(fp, low, high, false);
    else {
        unsigned len = 0;
        for (addr = low; addr < high && isOk && use[addr] != PADDING; addr++) {
            if (use[addr] != UNINITIALISED) {
                if (len++ == 0)
                    low = addr;
            } else if (len) {
                isOk = fputblock(fp, low, low + len, true);     // write the block initialised data
                len = 0;
            }
        }
        if (isOk && len)
            isOk = fputblock(fp, low, low + len, true);         // write the last block
        if (isOk) {
            fputc(0, fp);
            fputc(0, fp);
            fputc(start % 256, fp);
            fputc(start / 256, fp);
        }

        if (isOk && addr < high)
            isOk = fwrite(&mem[addr], 1, high - addr, fp) == high - addr;
    }
    fclose(fp);
    if (!isOk)
        fprintf(stderr, "write failure on %s\n", file);
    return isOk ? 0 : 2;
}


    

void readRec(FILE *fp)
{
    int type;
    int len;


    type = getc(fp);
    if (type == EOF) return;
    len = getc(fp);
    len += getc(fp) * 256;

    if (type == 6)
        read6(fp, len - 1);
    else if (type == 4)
        read4(fp, len - 1);
    else
        skipRec(fp, len - 1);
    (void)getc(fp);	// crc
}

void skipRec(FILE *fp, int len)
{
    while (len-- > 0)
        (void)getc(fp);
}



void read6(FILE *fp, int len)
{
    unsigned short addr;
    if (len < 3) {
        fprintf(stderr, ">>>corrupt type 6 field\n");
        skipRec(fp, len);
    } else {
        (void)getc(fp);	// Seg
        addr = getc(fp);
        addr += getc(fp) * 256;
        len -= 3;
        if (addr < low) low = addr;
        while (len-- > 0) {
            mem[addr] = getc(fp);
            use[addr++] = DATA;
        }
        if (addr > high)
            high = addr;
    }
}

void read4(FILE *fp, int len)
{
    unsigned modType;
    unsigned segId;
    unsigned offset;

    modType = getc(fp);
    segId = getc(fp);
    offset = getc(fp);
    offset += getc(fp) * 256;
    len -= 4;
    printf("Image:\t%04XH-%04XH\nType:\t%d\nStart:\t%02XH:%04XH\n", low, high, modType, segId, offset);
    if (len > 0) {
        printf("contains %d bytes of optional info\n", len);
        skipRec(fp, len);
    }
    if (segId == 0)
        start = offset;
}

char* skipSpc(char* s)
{
    while (*s == ' ' || *s == '\t')
        s++;
    return s;
}


// scan the string starting at s for a value and optional repeat count
// sets the parsed information in *val (a value_t)
// returns a pointer to the next character to parse

char *getValue(char *s, value_t *val)
{
    val->repeatCnt = -1;

    s = skipSpc(s);
    if (_strnicmp(s, "append", 6) == 0) {
        val->type = APPEND;
        return s + 6;
    }
    if (!isxdigit(*s)) {
        if (*s == '\'') {
            val->str = s + 1;
            val->type = STRING;
            while (*++s && *s != '\'') {
                if (*s == '\\' && s[1])
                    s++;
            }
            if (!*s) {
                val->type = ERROR;
                return s;
            }

        } else if (*s == '-')
            val->type = DEINITIALISE;
        else if (*s == '=')
            val->type = SKIP;
        else {
            val->type = !*s || *s == '\n' || *s == ';' ? EOL : INVALID;
            return *s ? s + 1 : s;
        }
        s++;
    } else {
        unsigned hval = 0;
        while (isxdigit(*s)) {
            hval = hval * 16 + (isdigit(*s) ? *s - '0' : toupper(*s) - 'A' + 10);
            s++;
        }
        val->type = HEXVAL;
        val->hval = hval;
    }
    s = skipSpc(s);
    if (tolower(*s) == 'x') {   // we have a repeat count
        s = skipSpc(s + 1);
        if (!isxdigit(*s)) {

            val->type = ERROR;  // oops not a valid repeat cht
        } else {
            unsigned hval = 0;

            while (isxdigit(*s)) {
                hval = hval * 16 + (isdigit(*s) ? *s - '0' : toupper(*s) - 'A' + 10);
                s++;
            }
            val->repeatCnt = hval;
        }
    }
    return s;
}

unsigned fill(unsigned addr, value_t *val, bool appending) {
    char *s;

    if (addr < low)
        low = addr;
    if (val->repeatCnt < 0)
        val->repeatCnt = 1;
    for (int i = 0; i < val->repeatCnt; i++) {
        if (addr >= MAXMEM) {
            fprintf(stderr, "appending past end of memory\n");
            break;
        }
        switch (val->type) {
        case HEXVAL:
            mem[addr] = val->hval;
            use[addr++] = appending ? PADDING : DATA;
            break;
        case SKIP:
            if (use[addr] == UNINITIALISED)
                fprintf(stderr, "Skipping uninitialised data at %04X\n", addr);
            addr++;
            break;
        case UNINITIALISED:
            use[addr++] = UNINITIALISED;
            break;
        case STRING:
            for (s = val->str; *s && *s != '\''; s++) {
                if (addr >= MAXMEM) {
                    fprintf(stderr, "appending past end of memory\n");
                    break;
                }
                if (*s == '\\') {
                    switch (*++s) {
                    case 'r': mem[addr] = '\r'; break;
                    case 'n': mem[addr] = '\n'; break;
                    case 't': mem[addr] = '\t'; break;
                    case '\\': mem[addr] = '\\'; break;
                    case '\'': mem[addr] = '\''; break;
                    default:
                        fprintf(stderr, "warning invalid escape char \\%c\n", *s);
                        mem[addr] = *s;
                    }
                } else {
                    if (*s < ' ' || *s > '~')
                        fprintf(stderr, "warning invalid char %02X\n", *s);
                    mem[addr] = *s;
                }
                use[addr++] = appending ? PADDING : DATA;
            }
        }
    }
    if (addr > high)
        high = addr;
    return addr;
}

void patchfile(char *fname) {
    char line[256];
    value_t val;
    char *s;
    bool append = false;
    unsigned addr;

    FILE *fp = fopen(fname, "rt");
    if (fp == NULL) {
        fprintf(stderr, "can't load patchfile (ignoring)\n");
        return;
    }

    while (fgets(line, 256, fp)) {
        s = getValue(line, &val);
        if (val.type == EOL || val.type == INVALID)
            continue;
        if (val.type == ERROR || (!append && (val.type == SKIP || val.type == DEINITIALISE || val.type == STRING))) {
            fprintf(stderr, "invalid patch line: %s", line);
            continue;
        }
        if (val.type == APPEND) {
            if (!append)     // only set address on first append detected
                addr = high;
            append = true;
            s = getValue(s, &val);
        } else if (!append) {       // get the start address
            if (val.hval >= 0x10000) {
                fprintf(stderr, "Address (%04X) too big: %s", val.hval, line);
                continue;
            } else if (val.repeatCnt >= 0) {
                fprintf(stderr, "Address cannot have repeat count: %s", line);
                continue;
            }
            addr = val.hval;
            s = getValue(s, &val);
        }
        // at this point addr has the insert point and val has the first value to put in
        bool ok = true;
        while (ok && val.type != EOL && val.type != INVALID && val.type != ERROR) {
            if (val.type == HEXVAL && val.hval >= 0x100) {
                fprintf(stderr, "bad hex value: %s", line);
                ok = false;
            } else {
                addr = fill(addr, &val, append);
                s = getValue(s, &val);
            }

        } 
        if (val.type == ERROR)
            fprintf(stderr, "bad value: %s", line);
    }

    fclose(fp);
}