#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

int getword(FILE *fp)
{
	int cl = getc(fp);
	int ch = getc(fp);
	if (cl == EOF || ch == EOF)
		return -1;
	return (ch << 8) + cl;
}

void putword(int val, FILE *fp)
{
	putc(val & 0xff, fp);
	putc((val >> 8) & 0xff, fp);
}


int main(int argc, char **argv)
{
	FILE *fpIn;
	FILE *fpOut;

	if (argc != 2) {
		fprintf(stderr, "usage: %s isis-located-file\n", argv[0]);
		exit(1);
	}
	if ((fpIn = fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", argv[1]);
		exit(2);
	}
	if ((fpOut = fopen("isis.bin", "wb")) == NULL) {
		fprintf(stderr, "can't create isis.bin\n");
		exit(3);
	}

	while (! feof(fpIn)) {
		int type = getc(fpIn);
		int len = getword(fpIn);
		if (type == 0xe || ferror(fpIn))
			break;
		else if (type != 6)
			fseek(fpIn, len, SEEK_CUR);
		else {
			putword(len - 4, fpOut);	// segId, offset and CRC are not in the isis.bin length field
			(void)getc(fpIn);			// junk the segId
			putword(getword(fpIn), fpOut);	// offset to load to
			len -= 4;	// correct for bytes to copy
			while (len-- > 0) 
				putc(getc(fpIn), fpOut);
			(void)getc(fpIn);	// junk the CRC
		}
	}
	putword(0, fpOut);
	putword(0, fpOut);
	fclose(fpIn);
	fclose(fpOut);
	return 0;
}