#include "plm.h"

void WrnStrLst(pointer str, word cnt)
{

	while (cnt != 0) {
		WrcLst(*str++);
		cnt = cnt - 1;
	}
}









