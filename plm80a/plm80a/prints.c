#include "plm.h"

void PrintStr(pointer str, byte len)
{
	word status;

	Write(0, str, len, &status);
} /* PrintStr() */

