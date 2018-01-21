#include "plm.h"

bool TestFlag(pointer base, byte flag)
{
	byte off, bit;

	off = tblOffsets[flag];
	bit = tblBitFlags[flag];
	if ((base[off] & bit) != 0 )
		return true;
	else
		return false;
} /* TestFlag() */

