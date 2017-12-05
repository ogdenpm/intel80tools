#include "loc.h"

pointer PastFileName(pointer pch)
{

	while (*pch == ':' || *pch == '.'
	   || ('0' <= *pch && *pch <= '9')
	   || ('A' <= *pch && *pch <= 'Z')) {
		pch = pch + 1;
	}
	return pch;
} /* PastFileName */


