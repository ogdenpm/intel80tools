#include "loc.h"

byte ToUpper(byte ch)
{
	if (ch < 'a' )
		return ch;
	if (ch > 'z' )
		return ch;
	return ch & 0xdf;
} /* ToUpper */
