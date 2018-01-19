#include "plm.h"

void Wr2cLst(word arg1w)
{
    pointer bp;

	bp = (pointer)&arg1w;
	WrcLst(bp[1]);
	WrcLst(bp[0]);
}
