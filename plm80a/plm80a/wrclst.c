#include "plm.h"

void WrcLst(byte ch)
{
    lBufP[lChCnt] = ch;
    if (lChCnt == lBufSz) {
        if (! lfOpen) {
            OpenF(&lstFil, 2);
            lfOpen = true;
        }
        WriteF(&lstFil, lBufP, lBufSz + 1);
        lChCnt = 0;
    } else
        lChCnt++;
}
