#include "plm.h"

void FindScopedInfo(word scope)
{
    word p, q;
    offset_t next;
    byte infoType;

    curInfoP = SymbolP(curSymbolP)->infoP;
    p = 0;
    while (curInfoP != 0) {
        if (scope == GetScope()) {
            infoType = GetType();
            if (infoType == LIT_T || infoType == MACRO_T)
                ;
            else if (TestInfoFlag(F_MEMBER))
                goto nxt;
            if (p != 0)	/* not at start of Chain() */
            {
                next = GetLinkOffset();	/* Move() to head of Chain() */
                q = curInfoP;		/* save current */
                curInfoP = p;		/* pick up previous */
                SetLinkOffset(next);	/* set its link */
                curInfoP = q;		/* restore current */
                SetLinkOffset(SymbolP(curSymbolP)->infoP);	/* set its link to current head */
                SymbolP(curSymbolP)->infoP = curInfoP;	/* set head to found info */
            }
            return;
        }
    nxt:	p = curInfoP;
        curInfoP = GetLinkOffset();
    }
}
