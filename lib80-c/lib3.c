#include "lib.h"

pointer SkipSpc(pointer chP)
{

	while (*chP == ' ')
		chP = chP + 1;
	return chP;
} /* SkipSpc */

pointer PastFileName(pointer chP)
{

	while (*chP == ':' || *chP == '.' || (*chP >= '0' && *chP <= '9') || (*chP >= 'A' && *chP <= 'Z'))
		chP = chP + 1;
	return chP;
} /* PastFileName */

