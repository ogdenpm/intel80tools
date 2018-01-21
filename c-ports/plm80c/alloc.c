#include "plm.h"

void Alloc(word size1, word size2)
{
	blk1Used = blk1Used + size1;
	blk2Used = blk2Used + size2;
	if (blk1Used >= blkSize1 || blk2Used >= blkSize2)
		FatalError(ERR83);
} /* Alloc() */
