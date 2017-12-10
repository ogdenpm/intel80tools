#include "link.h"

pointer Delimit(pointer pch)
{

	while (*pch == ':' || *pch == '.' || (*pch >= '0' && *pch <= '9') || (*pch >= 'A' && *pch <= 'Z')) {
		pch = pch + 1;
	}
	return pch;
} /* Delimit() */

