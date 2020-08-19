#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

void showVersion(FILE *fp, bool full);


#define MODHDR  2
#define MODEND  4
#define CONTENT 6
#define RELOC   0x22

#define MAXHEX  16

uint8_t getByte(FILE *fp) {
    int c = getc(fp);
    if (c == EOF) {
        fprintf(stderr, "Unexpected EOF in obj file\n");
        exit(1);
    }
    return (uint8_t)c;
}

uint16_t getAddress(FILE *fp) {
    uint16_t val = getByte(fp);
    return val + getByte(fp) * 256;
}

// return the trailing filename part of the passed in path
const char *basename(const char *path) {
    const char *t;
    while (t = strpbrk(path, ":\\/"))       // allow windows & unix separators - will fail for unix if : in filename!!
        path = t + 1;
    return path;
}

int main(int argc, char **argv) {

    if (argc == 2 && stricmp(argv[1], "-v") == 0) {
        showVersion(stdout, argv[1][1] == 'V');
        exit(0);
    }

    if (!(argc == 3 || (argc == 4 && stricmp(argv[2], "TO") == 0)) ) {
        fprintf(stderr, "usage: %s -v | -V | objfile [to] hexfile\n", basename(argv[0]));
        exit(1);
    }

    FILE *fpin, *fpout;

    if ((fpin = fopen(argv[1], "rb")) == NULL) {
        fprintf(stderr, "Cannot open obj file %s\n", argv[1]);
        exit(1);
    }
    if ((fpout = fopen(argv[argc - 1], "wt")) == NULL) {
        fclose(fpin);
        fprintf(stderr, "Cannot create hex file %s\n", argv[2]);
        exit(1);
    }

    /*
       READ OBJECT RECORDS, WRITE HEXADECIBAL RECORDS.
    */
    bool error = false;
    for (; !error;) {
        uint16_t addr;
        uint8_t type = getByte(fpin);

        if (type < MODHDR || type >= RELOC || (type & 1)) {
            fprintf(stderr, "Unsupported type %02x\n", type);
            exit(1);
        }
        uint16_t reclen = getAddress(fpin);
        if (type == MODEND) {
            getByte(fpin);
            getByte(fpin);
            addr = getAddress(fpin);
            error = fprintf(fpout, ":00%04X01%02X\n", addr, 0xff & -((addr >> 8) + addr + 1)) < 0;
            break;
        }
        if (type != CONTENT)
            error = fseek(fpin, reclen, SEEK_CUR) != 0;
        else {
            getByte(fpin);
            addr = getAddress(fpin);
            reclen -= 4;
            while (reclen) {
                uint8_t hexlen = reclen > MAXHEX ? MAXHEX : reclen;
                reclen -= hexlen;
                uint8_t crc = hexlen + (addr >> 8) + addr;
                error = fprintf(fpout, ":%02X%04X00", hexlen, addr) < 0;

                for (int i = 0; !error && i < hexlen; i++) {
                    uint8_t c = getByte(fpin);
                    crc += c;
                    error = fprintf(fpout, "%02X", c) < 0;
                }
                if (!error)
                    error = fprintf(fpout, "%02X\n", (-crc) & 0xff) < 0;
                addr += hexlen;
            }
            getByte(fpin);  // junk crc
        }
    }
    if (error)
        fputs("Error writing hex file\n", stderr);
    fclose(fpin);
    fclose(fpout);
    return 0;
}