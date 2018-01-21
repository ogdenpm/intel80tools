#include "loc.h"

void ErrChkReport(word errCode, pointer file, bool errExit)
{
	word status;
	
	if (errCode != 0 )
	{
		file = SkipSpc(file);
		Write(0, " ", 1, &status);
		Write(0, file, (word)(PastAFN(file) - file), &status);	/* Write() file name */
		Write(0, ",", 1, &status);
		Errmsg(errCode);
		if (errExit )
			Exit();
	}
} /* ErrChkReport */
