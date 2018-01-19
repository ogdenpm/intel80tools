#include "plm.h"

static byte hexdigits[] = "0123456789ABCDEF";
static byte numsuffix[] = "BXXXXXQXDXXXXXH";

byte Num2Asc(word num, byte width, byte radix, pointer bufP)
{
	byte buf[18];
	byte padch, fmt, j, lwidth;
	byte tmp, firstCh, i;

	padch = ' ';
	if (width > 0x7F )
	{
		padch = '0';
		width = -width;
	}
	if (width == 0 )
		lwidth = 16;
	else if (width > 16 )
		lwidth = 16;
	else
		lwidth = width;
	fmt = 0;
	if (radix > 0x7F )
	{
		fmt = 0xFF;
		radix = -radix;
	}
	if (radix > 16 )
		radix = 16;
	tmp = 16;
	firstCh = tmp - lwidth + 1;
	memset(&buf[firstCh], padch, lwidth);
	i = lwidth;
	j = 16;
	while (i != 0) {
		buf[j] = hexdigits[num % radix];
		if ((num /= radix) == 0 )
			i = 0;
		else
		{
			i = i - 1;
			j = j - 1;
		}
	}
	if (width == 0 )
	{
		if (fmt )
			if (buf[j] > '9' )
			{
				j = j - 1;
				buf[j] = '0';
			}
		lwidth = 17 - (firstCh = j);
	}
	if (fmt )
	{
		lwidth = lwidth + 1;
		buf[17] = numsuffix[radix - 2];
	}
	memmove(bufP, &buf[firstCh], lwidth);

	return lwidth;
} /* Num2Asc() */
