#include "link.h"
void FileError(word errCode, pointer file, bool errExit)
{
	word status;

	if (errCode != 0 )
	{
		file = Deblank(file);
		Write(0," ", 1, &status);
		Write(0, file , (word)(ScanBlank(file) - file), &status);
		Write(0, ",", 1, &status);
		ReportError(errCode);
		if (errExit )
			Exit();
	}
} /* FileError() */

