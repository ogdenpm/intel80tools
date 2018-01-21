#include "link.h"

static byte ToUpper(byte ch)
{
		if (ch < 'a' )
			return ch;
		if (ch > 'z' )
			return ch;
		return ch & 0xDF;
	} /* ToUpper() */

void CrStrUpper(pointer pch)
{
	while (*pch != '\r') {
		*pch = ToUpper(*pch);
		pch++;
	}

} /* CrStrUpper() */

