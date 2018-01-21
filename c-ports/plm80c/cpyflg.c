#include "plm.h"

void CpyFlags(pointer base)
{
	byte i;

	for (i = 0; i <= 2; i++) {
		InfoP(curInfoP)->flag[i] = base[i];
	}
}

