#include "plm.h"


void ClrInfoFlag(byte flag)
{
	ClrFlag(InfoP(curInfoP)->flag, flag);
}


