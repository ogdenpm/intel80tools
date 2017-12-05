#include "lib.h"

static byte hexdigits[] = "0123456789ABCDEF";

void Binasc(word num, byte base, byte padch, pointer chaP, byte width)
{
	byte i;

	for (i = 1; i <= width; i++) {
		chaP[width - i] = hexdigits[num % base];
		num /= base;
	}
	i = 0;
	while (chaP[i] == '0' && i < width - 1) {
		chaP[i] = padch;
		i = i + 1;
	}
} /* Binasc */


