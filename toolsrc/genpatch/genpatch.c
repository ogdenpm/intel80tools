#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#define MAXMEM 0x11000
#define MAXCOL 60
#define MINRUN 3

uint8_t mem[MAXMEM];
uint8_t use[MAXMEM];

unsigned low = 0x10000;
unsigned high = 0;

unsigned start = 0x10000;

enum { UNUSED = 0, OBJ, SKIP, CHANGE, DELETE, APPEND };

void showVersion(FILE *fp, bool full);

int readRec(FILE *fp);
bool read4(FILE *fp, int len);
bool read6(FILE *fp, int len);


unsigned getword(FILE *fp) {
    int c = getc(fp);
    return c + getc(fp) * 256;
}


bool loadBin(char *file, bool isIntel, bool defaultZero) {
    FILE *fp;
    unsigned addr;
    unsigned len;
    bool result = true;
    int c;

    if ((fp = fopen(file, "rb")) == NULL) {
        fprintf(stderr, "Can't open bin file %s\n", file);
        return false;
    }
    if (!isIntel) {
        if (low < 0x400 && low != 0x100)
            fprintf(stderr, "load address != 0x100, possibly a problem for CP/M .COM file\n");
        addr = low;
        while ((c = getc(fp)) != EOF && addr < MAXMEM) {
            if ((use[addr] == UNUSED && (c != 0 || !defaultZero)) || use[addr] == OBJ && mem[addr] != c) {
                use[addr] = addr < high ? CHANGE : APPEND;
                mem[addr] = c;
            } else
                use[addr] = addr < high ? SKIP : APPEND;
            addr++;
        }
        if (addr >= MAXMEM)
            fprintf(stderr, "Too many patch bytes\n");
        if (addr > high)
            high = addr;
        else
            while (addr < high)
                use[addr++] = DELETE;
                
    } else {
        c = 0;
        while (c != EOF && (len = getword(fp)) != 0 && len < 0x10000) {
            addr = getword(fp);
            if (addr + len > high)
                high = addr;
            if (addr < low)
                low = addr;
            while (len-- > 0 && (c = getc(fp)) != EOF && addr < MAXMEM) {
                if (use[addr] = UNUSED || use[addr] == OBJ && mem[addr] != c) {
                    use[addr] = CHANGE;
                    mem[addr] = c;
                } else
                    use[addr] = SKIP;
                addr++;
            }
        }
        addr = getword(fp);
        if (addr >= 0x10000 || addr != start)
            fprintf(stderr, "Warning start address mismatch OBJ: %04X, BIN: %04X\n", start, addr);
        while ((c = getc(fp)) != EOF && high < MAXMEM) {
            mem[high] = c;
            use[high++] = APPEND;
        }
        if (c != EOF)
            fprintf(stderr, "Too many patch bytes\n");
    }
    fclose(fp);
    return true;
}




bool loadObj(char *file) {

    FILE *fp;
    int result;

    if ((fp = fopen(file, "rb")) == NULL) {
        fprintf(stderr, "can't open obj file %s\n", file);
        return false;
    }

    while ((result = readRec(fp)) > 0)
        ;
    fclose(fp);
    if (result == EOF) {
        fprintf(stderr, "Corrupt obj file %s\n", file);
        return false;
    }
    return true;
}

int readRec(FILE *fp) {
    int type;
    int len;
    bool ok = true;

    type = getc(fp);
    if (type == EOF)
        return EOF;
    if (type == 0xe)
        return 0;
    len = getword(fp);

    if (type == 6)
        ok = read6(fp, len - 1);
    else if (type == 4)
        ok = read4(fp, len - 1);
    else
        fseek(fp, len - 1, SEEK_CUR);

    return getc(fp) != EOF && ok ? 1 : EOF;
}



bool read6(FILE *fp, int len) {
    unsigned short addr;
    int c;

    if (len < 3) {
        fprintf(stderr, ">>>corrupt type 6 field\n");
        return false;
    }

    (void)getc(fp);	// Seg
    addr = getword(fp);
    len -= 3;
    if (addr < low)
        low = addr;
    c = 0;
    while (len-- > 0 && (c = getc(fp)) != EOF) {
        mem[addr] = c;
        use[addr++] = OBJ;
    }
    if (addr > high)
        high = addr;
    return c != EOF;

}

bool read4(FILE *fp, int len) {
    unsigned modType;
    unsigned segId;
    unsigned offset;

    modType = getc(fp);
    segId = getc(fp);
    offset = getc(fp);
    offset += getc(fp) * 256;
    len -= 4;
    if (len > 0)
        fseek(fp, len, SEEK_CUR);
    if (segId != 0) {
        fprintf(stderr, "ERROR: Object file has relocatable offset\n");
        return false;
    }
    start = offset;
    return true;
}

int typeRunLen(unsigned addr) {
    unsigned i;
    for (i = addr + 1; i < high && use[addr] == use[i]; i++)
        ;
    return i -addr;
}

int valRunLen(unsigned addr) {
    unsigned i;
    for (i = addr + 1; i < high && use[addr] == use[i] && mem[addr] == mem[i] ; i++)
        ;
    return i -addr;

}

void genPatch(char *file, bool showSting) {
    FILE *fp;
    unsigned addr;
    unsigned runlen;
    unsigned col = 0;
    int prevUse = -1;
    int thisUse;

    if ((fp = fopen(file, "wt")) == NULL) {
        fprintf(stderr, "can't create patch file %s\n", file);
        return;
    }

    addr = low;
    while (addr < high) {
        switch (thisUse = use[addr]) {
        case SKIP:
        case UNUSED:
            if (col && prevUse != SKIP && prevUse != UNUSED) {
                putc('\n', fp);
                col = 0;
            }
            runlen = typeRunLen(addr);
            break;
        case CHANGE:
        case DELETE:
            if (col && prevUse != CHANGE && prevUse != DELETE) {
                putc('\n', fp);
                col = 0;
            }

            if (col == 0)
                col += fprintf(fp, "%04X", addr);
            runlen = thisUse == CHANGE ? valRunLen(addr) : typeRunLen(addr);
            if (thisUse == CHANGE) {
                if (runlen >= MINRUN)
                    col += fprintf(fp, " %02X x %02X", mem[addr], runlen);
                else
                    col += fprintf(fp, " %02X", mem[addr]);
            } else if (runlen >= MINRUN)
                col += fprintf(fp, " - x %02X", runlen);
            else
               col += fprintf(fp, " -");

            break;
        case APPEND:
            if (prevUse != APPEND) {
                if (col)
                    fputc('\n', fp);
                fprintf(fp, "APPEND\n");
                col = 0;
            }
            if (col) {
                fputc(' ', fp);
                col++;
            }

            runlen = valRunLen(addr);
            if (runlen >= MINRUN)
                col += fprintf(fp, "%02X x %02X", mem[addr], runlen);
            else
                col += fprintf(fp, "%02X", mem[addr]);

            break;
        }
        if (col > MAXCOL) {
            fputc('\n', fp);
            col = 0;
        }
        prevUse = thisUse;
        addr += runlen < MINRUN ? 1 : runlen;
    }
    if (col)
        fputc('\n', fp);
    fclose(fp);
}


int main(int argc, char **argv) {
    char *invoke = argv[0];
    bool intelBin = false;
    bool showString = false;
    bool defaultZero = true;

    if (argc == 2 && stricmp(argv[1], "-v") == 0) {
        showVersion(stdout, argv[1][1] == 'V');
        exit(0);
    }

    while (argc > 1 && argv[1][0] == '-') {
        if (strcmp(argv[1], "-i") == 0)
            intelBin = true;
        else if (strcmp(argv[1], "-s]") == 0)
            showString = true;
        else if (strcmp(argv[1], "-z") == 0)
            defaultZero = false;
        else
            fprintf(stderr, "Unknown option %s\n", argv[1]);
        argc--;
        argv++;
    }
    if (argc != 4) {
        fprintf(stderr, "usage: %s [-i] [-s] [-z] objFile binfile patchfile\n", invoke);
        exit(1);
    }
    if (loadObj(argv[1]) && loadBin(argv[2], intelBin, defaultZero))
        genPatch(argv[3], showString);
}