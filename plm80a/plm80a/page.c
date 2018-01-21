#include "plm.h"

void NewPgl()
{
	byte i, j, k, m;
    byte pnum[3];

	if (! PAGING)
		return;
	WrcLst(0xc);
	linLft = PAGELEN;
	pageNo = pageNo + 1;
	i = Num2Asc(pageNo, 3, 10, pnum);
	j = PWIDTH - 41;
	if (j < TITLELEN)
		m = j;
	else
		m = TITLELEN;
	WrnStrLst(plm80Compiler, 20);
	WrnStrLst(TITLE, m);
	k = j - m + 2;
	while (k != 0) {
		WrcLst(' ');
		k = k - 1;
	}
	WrnStrLst(DATE, 9);
	WrnStrLst("  PAGE ", 7);
	WrnStrLst(pnum, 3);
	WrnStrLst("\r\n\n\n", 4);
	b3CFF = 0;
}


void NlLead()
{
	Wr2cLst(0xd0a);
	col = 0;
	linLft = linLft - 1;
	if (linLft == 0)
		NewPgl();
	while (col < b3CFB) {
		WrcLst(' ');
		col = col + 1;
	}
	if (b3CFC != 0) {
		WrcLst(b3CFC);
		col = col + 1;
	}
	while (col < b3CFD) {
		WrcLst(' ');
		col = col + 1;
	}
}
