#include "plm.h"

byte TestInfoFlag(byte flag)
{
	return TestFlag(InfoP(curInfoP)->flag, flag);
}

