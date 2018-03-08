#include "plm.h"

void AdvNxtInfo()
{
    while (1) {
        curInfoP += GetLen();
        if (curInfoP >= topInfo) {
            curInfoP = 0;
            return;
        } else if (GetType() != CONDVAR_T)
            return;
    }
}


