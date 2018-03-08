#include "plm.h"

static byte tmp[] = "     1";
static word lastNo = 1;

void LstLineNo()
{
    pointer q;
    bool i;

	if (stmtCnt > lastNo + 20 )
	{
		lastNo = Num2Asc(stmtCnt, 6, 10, tmp);
		lastNo = stmtCnt;
	}
	else
	while (lastNo < stmtCnt) {
		q = &tmp[5];
		i = true;
		while (i) {
			if (*q == '9' )
				*q = '0';
			else
			{
				if (*q == ' ' )
					*q = '1';
				else
					++*q;
				i = false;
			}
			q = q - 1;
		}
		lastNo = lastNo + 1;
	}
	XwrnstrLst(&tmp[2], 4);
}
