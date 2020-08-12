#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>

void showVersion(FILE *fp, bool full);
char *invokedBy;

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

void usage(char *fmt, ...) {
	if (fmt) {
		va_list args;
		va_start(args, fmt);
		vfprintf(stderr, fmt, args);
		va_end(args);
	}
	fprintf(stderr, "\nUsage: %s -v | -V | isis-located-file\n", invokedBy);

	exit(1);
}

int main(int argc, char **argv)
{
	FILE *fpIn;
	FILE *fpOut;
	invokedBy = argv[0];

	if (argc != 2)
		usage(NULL);

	if (_stricmp(argv[1], "-v") == 0) {
		showVersion(stdout, argv[1][1] == 'V');
		exit(0);
	}
	if ((fpIn = fopen(argv[1], "rb")) == NULL)
		usage("can't open %s\n", argv[1]);

	if ((fpOut = fopen("isis.bin", "wb")) == NULL)
		usage("can't create isis.bin\n");


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