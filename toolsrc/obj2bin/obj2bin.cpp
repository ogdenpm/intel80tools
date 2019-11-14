#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


void readRec(FILE *fp);
void skipRec(FILE *fp, int len);
void read6(FILE *fp, int len);
void read4(FILE *fp, int len);

unsigned char mem[0x10000];
unsigned int low = 0x10000;
unsigned int high = 0;

void loadfile(char *s);
void dumpfile(char *s);
void patchfile(char *s);

int main(int argc, char **argv)
{

    if (argc < 3 || argc > 4) {
        fprintf(stderr, "usage: %s outfile infile [patchfile]\n", argv[0]);
        exit(0);
    }
    loadfile(argv[2]);
    if (argc == 4)
        patchfile(argv[3]);
    dumpfile(argv[1]);
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
		while (len-- > 0)
			mem[addr++] = getc(fp);
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

}

char *gethex(char *s, unsigned *val)
{
    *val = 0;
    while (*s == ' ' || *s == '\t')
        s++;
    if (!isxdigit(*s))
        return 0;
    while (isxdigit(*s)) {
        *val = *val * 16 + (isdigit(*s) ? *s - '0' : toupper(*s) - 'A' + 10);
        s++;
    }
    return s;
}



void patchfile(char *fname) {
    char line[256];
    char *s;
    unsigned addr, patch;

    FILE *fp = fopen(fname, "rt");
    if (fp == NULL) {
        fprintf(stderr, "can't load patchfile (ignoring)\n");
        return;
    }

    while (fgets(line, 256, fp)) {
        for (s = line; *s && (*s == ' ' || *s == '\t' || *s == '\n'); s++)
            ;
        if (!*s || *s == '#')
            continue;

        if ((s = gethex(s, &addr)) == NULL)
            fprintf(stderr, "bad address: %s", line);
        else {
            if (addr < low) {
                fprintf(stderr, "Warning: new start address %04X\n", addr);
                low = addr;
            }
            while ((s = gethex(s, &patch))) {
                if (patch < 0 || patch >= 0x100) {
                    fprintf(stderr, "bad patch (%04X) in line %s", patch, line);
                    break;
                } else
                    mem[addr++] = patch;
            }
            if (addr > high)
                high = addr;
        }
    }
    fclose(fp);
}