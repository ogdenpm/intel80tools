#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

void readRec(FILE *fp, int id);
void skipRec(FILE *fp, int len);
void read6(FILE *fp, int len, int id);

unsigned int mem[0x10000];
unsigned int low = 0x10000;
unsigned int high = 0;

void loadfile(char *s, int id);
void dumpdiff();
void showVersion(FILE *fp, bool full);

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "-v") == 0) {
        showVersion(stdout, true);
        exit(0);
    }
    if (argc != 3) {
        showVersion(stderr, false);
        fprintf(stderr, "\nusage: %s -v | file1 file2\n", argv[0]);
        exit(0);
    }
    loadfile(argv[1], 1);
    loadfile(argv[2], 2);
    dumpdiff();
}

void loadfile(char *file, int id)
{

    FILE *fp;
    if ((fp = fopen(file, "rb")) == NULL) {
        fprintf(stderr, "can't open input file %s\n", file);
        exit(0);
    }
    do {
        readRec(fp, id);
    } while (! feof(fp));
    fclose(fp);
}
    
    
void dumpdiff()
{
    unsigned int i;
    for (i = low; i < high; i++) {
        switch (mem[i] >> 16) {
        case 1:
            printf("%04X: %02X --\n", i, mem[i] & 0xff);
            break;
        case 2:
            printf("%04X: -- %02X\n", i, (mem[i] >> 8) & 0xff);
            break;
        case 3:
            if ((mem[i] & 0xff) != ((mem[i] >> 8) & 0xff))
                printf("%04X: %02X %02X\n", i, mem[i] & 0xff, (mem[i] >> 8) & 0xff);
        }

    }
}



void readRec(FILE *fp, int id)
{
    int type;
    int len;


    type = getc(fp);
    if (type == EOF) return;
    len = getc(fp);
    len += getc(fp) * 256;

    if (type == 6)
        read6(fp, len - 1, id);
    else
        skipRec(fp, len - 1);
    (void)getc(fp);	// crc
}

void skipRec(FILE *fp, int len)
{
    while (len-- > 0)
        (void)getc(fp);
}



void read6(FILE *fp, int len, int id)
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
        while (len-- > 0)
            if (id == 1)
                mem[addr++] = getc(fp) | 0x10000;
            else {
                mem[addr] &= 0x100ff;
                mem[addr++] |= (getc(fp) << 8) | 0x20000;
            }
        if (addr > high) high = addr;
    }
}



