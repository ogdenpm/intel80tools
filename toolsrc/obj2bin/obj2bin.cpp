#include <stdio.h>
#include <stdlib.h>


void readRec(FILE *fp);
void skipRec(FILE *fp, int len);
void read6(FILE *fp, int len);

unsigned char mem[0x10000];
unsigned int low = 0x10000;
unsigned int high = 0;

void loadfile(char *s);
void dumpfile(char *s);

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "usage: %s infile outfile\n", argv[0]);
        exit(0);
    }
    loadfile(argv[1]);
    dumpfile(argv[2]);

}

void loadfile(char *file)
{
    FILE *fp;
    if ((fp = fopen(file, "rb")) == NULL) {
        fprintf(stderr, "can't open input file %s\n", file);
        exit(1);
    }
    do {
        readRec(fp);
    } while (! feof(fp));
    fclose(fp);
}

void dumpfile(char *file)
{
    FILE *fp;
    if ((fp = fopen(file, "wb")) == NULL) {
        fprintf(stderr, "can't create output file %s\n", file);
        exit(1);
    }
    if (fwrite(&mem[low], 1, high - low, fp) != high - low) {
        fprintf(stderr, "write failure on %s\n", file);
        fclose(fp);
        exit(2);
    }
    fclose(fp);
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
    else
        skipRec(fp, len - 1);
    getc(fp);   // crc
}

void skipRec(FILE *fp, int len)
{
    while (len-- > 0)
        getc(fp);
}



void read6(FILE *fp, int len)
{
    unsigned int addr;
    if (len < 3) {
        fprintf(stderr, ">>>corrupt type 6 field\n");
        skipRec(fp, len);
    } else {
        getc(fp);   // Seg
        addr = getc(fp);
        addr += getc(fp) * 256;
        len -= 3;
        if (addr < low) low = addr;
        while (len-- > 0)
            mem[addr++] = getc(fp);
        if (addr > high)
            high = addr;
    }
}



