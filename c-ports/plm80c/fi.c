#include "plm.h"

void FindInfo()
{
    word i;

    if (SymbolP(curSymbolP)->infoP == 0) {
        curInfoP = 0;
        return;
    }
    i = blockDepth;
    while (i != 0) {
        FindScopedInfo(procChains[i]);
        if (curInfoP != 0)
            return;
        i = i - 1;
    }
}
