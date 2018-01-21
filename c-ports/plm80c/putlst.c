#include "plm.h"

void PutLst(byte ch)
{
    byte i;
    if (col == 0) {
        if (linLft == 0)
            NewPgl();
        else if (linLft <= b3CFF)
            NewPgl();
        else
            while (b3CFF != 0) {
                WrcLst('\n');
                linLft--;
                b3CFF--;
            }
    } else if (col >= PWIDTH)
        NlLead();


    if (ch == '\t') {
        if (col >= margin)
            return;
        i = tWidth - (col - margin) % tWidth;
        if (col + i >= PWIDTH) {
            NlLead();
            return;
        }
        while (i-- != 0) {
            WrcLst(' ');
            col++;
        }
    } else {
        WrcLst(ch);
        if (ch == '\r')
            col = 0;
        else
            col++;
    }
}
