#include "link.h"

pointer Deblank(pointer pch)
{

	while (*pch == ' ') {
		pch = pch + 1;
	}
	return pch;
} /* Deblank() */

