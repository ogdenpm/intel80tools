#include "plm.h"
void CpyTill(pointer srcP, pointer dstP, word cnt, byte endch)
{
	while (cnt != 0) {
		if (*srcP == endch )
			return;
		*dstP++ = *srcP++;
		cnt = cnt - 1;
	}
} /* CpyTill() */
