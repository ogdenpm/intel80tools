#include "plm.h"

void ClrFlags(pointer base)
{
	byte i;

	for (i = 0; i <= 2; i++) {
		base[i] = 0;
	}
}

