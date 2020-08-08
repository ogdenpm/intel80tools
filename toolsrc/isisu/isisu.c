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

void usage(char *fmt, ...) {


	showVersion(stderr, false);
	if (fmt) {
		va_list args;
		va_start(args, fmt);
		putc('\n', stderr);
		vfprintf(stderr, fmt, args);
		va_end(args);
	}
	fprintf(stderr, "\nUsage: %s -v | file1 file2\n", invokedBy);

	exit(1);
}




int main(int argc, char **argv)
{
	FILE *fp;
	invokedBy = argv[0];

	if (argc != 2)
		usage(NULL);
	if (strcmp(argv[1], "-v") == 0) {
		showVersion(stdout, true);
		exit(0);
	}
	if ((fp = fopen(argv[1], "rb")) == NULL)
		usage("can't open %s\n", argv[1]);

	while (1) {
		int len = getword(fp);
		int start = getword(fp);
		if (start < 0 || len <= 0) {
			if (len == 0)
				printf("load point: %04X\n", start);
			break;
		}
		printf("%04X - %04X\n", start, start + len - 1);
		while (len-- > 0)
			(void)getc(fp);
	}
	fclose(fp);
	return 0;
}
