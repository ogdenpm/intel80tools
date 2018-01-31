#include "plm.h"

bool trunc;

void GNxtCh()
{
    inChrP++;
    while (*inChrP == '\n')
		GetLin();
    lastCh = *inChrP;
}


// get next char into *inChrP
// char is 0x81 on EOF, cr is discarded

byte InGetC()
{
    byte c;
    do {
		if (offCurCh == offLastCh) {
			offCurCh = 0;
			ReadF(&srcFil, inbuf, 512, &offLastCh);
			if (offLastCh == 0) {
				return *inChrP = ISISEOF;	// EOF
			} else
				offLastCh--;	// base from 0
		} else
			offCurCh++;
    }
    while ((c = inbuf[offCurCh] & 0x7f) == '\r');
    return *inChrP = c;
}


void RSrcLn()
{
    byte i = 127;		// max chars;
	
    for (;;) {
		if (*inChrP == '\n' || *inChrP == ISISEOF)
			return;
		if (--i == 0)
			break;
		inChrP++;
		InGetC();
    }
    trunc = true;
    while (InGetC() != '\n' && *inChrP != ISISEOF)	// junk rest of long line
		;
}
