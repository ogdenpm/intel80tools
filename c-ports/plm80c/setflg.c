#include "plm.h"

void SetFlag(pointer base, byte flag)
{
	byte off, bit;

	off = tblOffsets[flag];
	bit = tblBitFlags[flag];
	base[off] |= bit;
}
