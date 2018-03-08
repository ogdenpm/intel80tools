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

void Wr2cLst(word arg1w)
{
    pointer bp;

    bp = (pointer)&arg1w;
    WrcLst(bp[1]);
    WrcLst(bp[0]);
}

void WrnStrLst(pointer str, word cnt)
{

    while (cnt-- != 0)
        WrcLst(*str++);
}
