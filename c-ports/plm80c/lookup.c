#include "plm.h"

byte Hash(pointer pstr)
{
    pointer p = pstr;
    byte len = *p;
    byte hash = 0;

    while (len != 0) {
        hash = Rol(hash, 1) + *p++;
        len = len - 1;
    }
    return hash & 0x3F;
} /* Hash() */



void Lookup(pointer pstr)
{
	offset_t p, q, r;
    word hval;
	byte cmp;

	hval = Hash(pstr);
	curSymbolP = WordP(hashChainsP)[hval];
	p = 0;
	while (curSymbolP != 0) {
		if (SymbolP(curSymbolP)->name[0] == pstr[0]) {
			cmp = Strncmp(&SymbolP(curSymbolP)->name[1], pstr + 1, pstr[0]);
			if (cmp == 0) {
				if (p != 0 ) {
					q = SymbolP(curSymbolP)->link;
					r = curSymbolP;
					curSymbolP = p;
					SymbolP(curSymbolP)->link = q;
					curSymbolP = r;
					SymbolP(curSymbolP)->link = WordP(hashChainsP)[hval];
					WordP(hashChainsP)[hval] = curSymbolP;
				}
				return;
			}
		}
		p = curSymbolP;
		curSymbolP = SymbolP(curSymbolP)->link;
	}
	Alloc(0, pstr[0] + 1);
	curSymbolP = AllocSymbol(sizeof(sym_t) + pstr[0]);
	memmove(SymbolP(curSymbolP)->name, pstr, pstr[0] + 1);
	SymbolP(curSymbolP)->infoP = 0;
	SymbolP(curSymbolP)->link = WordP(hashChainsP)[hval];
	WordP(hashChainsP)[hval] = curSymbolP;
} /* Lookup() */
