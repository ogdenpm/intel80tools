#include "plm.h"

void Fatal(pointer str, byte len)
{
	PrintStr("\r\n\nPL/M-80 FATAL ERROR --\r\n\n", 28);
	PrintStr(str, len);
	PrintStr("\r\n\nCOMPILATION TERMINATED\r\n\n", 28);
	//if (debugFlag)
	//	REBOOTVECTOR();
	//else
		Exit();
} /* Fatal() */
