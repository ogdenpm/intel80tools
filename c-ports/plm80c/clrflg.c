#include "plm.h"

void ClrFlag(pointer base, byte flag)
{
	byte off, mask;

	off = tblOffsets[flag];
	mask = ~tblBitFlags[flag];
	base[off] &= mask;
}
