#include "loc.h"

static byte errTab[] = 
	  "\x2" "ILLEGAL AFTN ARGUMENT\0"
	   "\x3" "TOO MANY OPEN FILES\0"
	   "\x4" "INCORRECTLY SPECIFIED FILE\0"
	   "\x5" "UNRECOGNIZED DEVICE NAME\0"
	   "\x9" "DISK DIRECTORY FULL\0"
	 "\xC" "FILE IS ALREADY OPEN\0"
	 "\xD" "NO SUCH FILE\0"
	 "\xE" "WRITE PROTECTED\0"
	 "\x11" "NOT A DISK FILE\0"
	 "\x13" "ATTEMPTED SEEK ON NON-DISK FILE\0"
	 "\x14" "ATTEMPTED BACK SEEK TOO FAR\0"
	 "\x15" "CAN'T RESCAN\0"
	 "\x16" "ILLEGAL ACCESS MODE TO OPEN\0"
	 "\x17" "MISSING FILENAME\0"
	 "\x1B" "ILLEGAL SEEK COMMAND\0"
	 "\x1C" "MISSING EXTENSION\0"
	 "\x1F" "CAN'T SEEK ON WRITE ONLY FILE\0"
	 "\x20" "CAN'T DELETE OPEN FILE\0"
	 "\x23" "SEEK PAST EOF\0"
	"\xCB" "INVALID SYNTAX\0"
	"\xCC" "PREMATURE EOF\0"
	"\xD0" "CHECKSUM ERROR\0"
	"\xD2" "INSUFFICIENT MEMORY\0"
	"\xD3" "RECORD TOO LONG\0"
	"\xD4" "ILLEGAL RELO RECORD\0"
	"\xD5" "FIXUP BOUNDS ERROR\0"
	"\xDA" "ILLEGAL RECORD FORMAT\0"
	"\xE0" "BAD RECORD SEQUENCE\0"
	"\xE1" "INVALID NAME\0"
	"\xE2" "NAME TOO LONG\0"
	"\xE3" "LEFT PARENTHESIS EXPECTED\0"
	"\xE4" "RIGHT PARENTHESIS EXPECTED\0"
	"\xE5" "UNRECOGNIZED CONTROL\0"
	"\xE9" "'TO' EXPECTED\0"
	"\xED" "COMMON NOT FOUND\0"
	"\xEE" "ILLEGAL STACK CONTENT RECORD\0"
	"\xEF" "NO MODULE HEADER RECORD\0"
	"\xF0" "PROGRAM EXCEEDS 64K";

void Errmsg(word errCode)
{
	word i;
	word status;
	
	if (Low(errCode) != 0 )
	{
		i = 0;
		while (i < sizeof(errTab)) {
			if (Low(errCode) == errTab[i])
			{
				Write(0, " ", 1, &status);
				while (errTab[i = i + 1] != 0) {
					Write(0, &errTab[i], 1, &status);
				}
				Write(0, "\r\n", 2, &status);
				return;
			}
			else
			{
				while (errTab[i = i + 1] != 0) 
					;
				i = i + 1;
			}
		}
		Error(errCode);	/* ISIS Error() code */
	}
}
