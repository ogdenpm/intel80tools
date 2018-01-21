#include "plm.h"

static byte infoLengths[] = {10, 12, 18, 18, 18, 22, 11, 10, 8, 9};


offset_t AllocInfo(word infosize)
{
    offset_t p, q;

	Alloc(infosize, infosize);
	p = topInfo + 1;
	if (botSymbol < (q = topInfo + infosize))
		FatalError(ERR83);
	memset(ByteP(p), 0, infosize);
	topInfo = q;
	return p;
} /* AllocInfo() */


void CreateInfo(word val, byte type)
{
	byte len;

	len = infoLengths[type];
	curInfoP = AllocInfo(len);
	if (curSymbolP != 0 )
	{
		SetLinkOffset(SymbolP(curSymbolP)->infoP);
		SymbolP(curSymbolP)->infoP = curInfoP;
	}
	SetType(type);
	SetLen(len);
	SetScope(val);
	SetSymbol(curSymbolP);
} /* CreateInfo() */
