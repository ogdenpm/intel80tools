#include "plm.h"

static 	byte errHeader[] = "\r\n\nPL/M-80 I/O ERROR --\r\n  FILE: ";
static byte aName[] = "\r\n  NAME: ";
static byte aError[] = "\r\n  ERROR: ";
static byte aTerminate[] = "\r\nCOMPILATION TERMINATED\r\n\n";
static byte errStrTable[] = {
	"\x4" "ILLEGAL FILENAME SPECIFICATION\0"
	"\x5" "ILLEGAL OR UNRECOGNIZED DEVICE SPECIFICATION IN FILENAME\0"
	"\xC" "ATTEMPT TO OPEN AN ALREADY OPEN FILE\0"
	"\xD" "NO SUCH FILE\0"
	"\xE" "FILE IS WRITE PROTECTED\0"
	"\x13" "FILE IS NOT ON A DISKETTE\0"
	"\x16" "DEVICE TYPE NOT COMPATIBLE WITH INTENDED FILE USE\0"
	"\x17" "FILENAME REQUIRED ON DISKETTE FILE\0"
	"\x1C" "NULL FILE EXTENSION\0"
	"\xFE" "ATTEMPT TO READ PAST EOF\0"};

static word off, slen;  // global for nested function


static void xFindErrStr(word errNum)
{
    word i, j;

    i = 0;
    while (errStrTable[i] != 0) {	
        j = i;
        while (errStrTable[i] != 0) {
            i = i + 1;
        }
        if (errStrTable[j] == errNum )
        {
            off = j + 1;
            slen = i - off;
            return;
        }
        i = i + 1;
    }
    slen = 0;
} /* FindErrStr() */


void FatlIO(file_t *fileP, word errNum)
{
	byte buf[5];
    byte len;

	PrintStr(errHeader, Length(errHeader));
	PrintStr(fileP->sNam, 6);
	PrintStr(aName, Length(aName));	
	PrintStr(fileP->fNam, 15);
	PrintStr(aError, Length(aError));
	len = Num2Asc(errNum, 0, 10, buf);
	PrintStr(buf, len);
	xFindErrStr(errNum);
	if (slen != 0 )
	{
		PrintStr("--", 2);
		PrintStr(&errStrTable[off], (byte)slen);
	}
	PrintStr(aTerminate, Length(aTerminate));
	//if (debugFlag )
	//	REBOOTVECTOR();
	//else
		Exit();
} /* FatlIO() */

