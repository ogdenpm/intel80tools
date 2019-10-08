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

int main(int argc, char **argv)
{
	FILE *fp;

	if (argc != 2) {
		fprintf(stderr, "usage: %s file\n", argv[0]);
		exit(1);
	}
	if ((fp = fopen(argv[1], "rb")) == NULL) {
		fprintf(stderr, "can't open %s\n", argv[1]);
		exit(2);
	}
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
