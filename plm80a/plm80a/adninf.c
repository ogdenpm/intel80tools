#include "plm.h"

void AdvNxtInfo()
{
	while (1) {
		curInfoP = curInfoP + GetLen();
		if (curInfoP >= topInfo)
		{
			curInfoP = 0;
			return;
		}
		else if (GetType() != 9 )
			return;
	}
}


