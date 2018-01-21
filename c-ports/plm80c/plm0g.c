#include "plm.h"

offset_t CreateLit(pointer pstr)
{
    word litLen;
    offset_t litSymbol;

	litLen = pstr[0] + 1;
	litSymbol = AllocSymbol(litLen + 3);
    memmove(ByteP(litSymbol + 1), pstr, litLen);
    memmove(ByteP(litSymbol + litLen + 1), " \n", 2);
	ByteP(litSymbol)[0] = 255;		/* put max Size() \n will terminate */
	return litSymbol;
}


