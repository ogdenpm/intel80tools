#include "plm.h"

void FindMemberInfo()
{
	word tmp;

	tmp = curInfoP;
	curInfoP = SymbolP(curSymbolP)->infoP;
	while (curInfoP != 0) {
		if (TestInfoFlag(F_MEMBER))
			if (tmp == GetParentOffset())
				return;
		curInfoP = GetLinkOffset();
	}
}
