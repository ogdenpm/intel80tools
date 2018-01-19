#include "plm.h"


static byte endMsg[] = "PL/M-80 COMPILATION COMPLETE.  ";
static byte errMsg[] = "XXXXX PROGRAM ERROR";

void EndCompile()
{
	byte junk;
	junk = Num2Asc(programErrCnt, 5, 10, errMsg);
	PrintStr(endMsg, sizeof(endMsg) - 1);
	PrintStr(errMsg, sizeof(errMsg) - 1);
	if (programErrCnt != 1 )
		PrintStr("S", 1);
	PrintStr("\r\n\r\n", 4);
}	

