#include "link.h"

pointer ScanBlank(pointer pch)
{
	while (1) {
		pch = Delimit(pch);
		if (*pch == '*' || *pch == '?' )
			pch = pch + 1;
		else
			return pch;
	}
} /* ScanBlank() */
