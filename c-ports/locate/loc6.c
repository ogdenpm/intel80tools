#include "loc.h"


spath_t spathInfo;
byte signonMsg[] = "\fISIS-II OBJECT LOCATER ";
byte aInvokedBy[] = " INVOKED BY:\r\n";
byte aCommandTailErr[] = "COMMAND TAIL ERROR NEAR #:";
byte tmpFileInfo[] = "\0LOCATETMP";
pointer scmdP;
pointer cmdP;

byte controls[] = 
		"\x0\x0\xfd"		/* 0xfd = 253 = -3 */
		"\x1\x1\x4" "CODE"
		"\x1\x2\x4" "DATA"
		"\x1\x3\x5" "STACK"
		"\x1\x4\x6" "MEMORY"
		"\x2\x0\x5" "START"
		"\x3\x1\x9" "STACKSIZE"
		"\x4\x2\x8" "RESTART0"
		"\x4\x3\x3" "MAP"
		"\x4\x4\x7" "PUBLICS"
		"\x4\x5\x7" "SYMBOLS"
		"\x4\x6\x5" "LINES"
		"\x4\x7\x5" "PURGE"
		"\x5\x8\x4" "NAME"
		"\x6\x0\x5" "PRINT"
		"\x7\x0\x5" "ORDER"
		"\x8\x0\x7" "COLUMNS";

byte cin[] = ":CI: ";
byte cout[] = "\x5" ":CO: ";
byte mdebug[] = "DEBUG ";
byte mstar2[] = "**";
byte mto[] = "TO ";
byte mtoand[] = "TO&";


void CmdErr(word err)
{
	if (PastFileName(cmdP) == cmdP)
	{
		if (*cmdP != '\r')
			cmdP = cmdP + 1;
	}
	else
		cmdP = PastFileName(cmdP);

	*cmdP = '#';	/* put a marker in */
	ConStrOut(aCommandTailErr, 26);
	Errmsg(err);
	ConStrOut(scmdP, (word)(cmdP - scmdP) + 1);
	ConStrOut(crlf, 2);
	Exit();
} /* CmdErr */


void SkipNonArgChars(pointer pch)
{
	cmdP = SkipSpc(pch);
	/* skip any continuation bits and leading space */
	while (*cmdP == '&') {
		cmdP = SkipSpc(cmdP + 5);
	}
} /* SkipNonArgChars */


void ExpectChar(byte ch, byte err)
{
	SkipNonArgChars(cmdP);
	if (ch == *cmdP)
		SkipNonArgChars(cmdP + 1);
	else
		CmdErr(err);
} /* ExpectChar */


void ExpectLP()
{
	ExpectChar('(', ERR227);	/* left parenthesis expected */
} /* ExpectLP */


void ExpectRP()
{
	ExpectChar(')', ERR228);	/* right parenthesis expected */
} /* ExpectRP */


void ExpectSlash()
{
	ExpectChar('/', ERR203);	/* invalid syntax */
} /* ExpectSlash */


void GetFile()
{
	Spath(cmdP, &spathInfo, &statusIO);	/* try to parse file */
	if (statusIO > 0)
		CmdErr(statusIO);
	SkipNonArgChars(PastFileName(cmdP));
} /* GetFile */


void InitSegOrder()
{
	byte i;
	segOrder[0] = SABS;
	segOrder[1] = SCODE;
	segOrder[2] = SSTACK;
	/* next come the common segs in order */
	for (i = 3; i <= 252; i++) {
		segOrder[i] = i + 3;
	}
	/* DATA is after these and MEMORY is last */
	segOrder[253] = SDATA;
	segOrder[254] = SMEMORY;
} /* InitSegOrder */


void ErrNotADisk()
{
	MakeFullName(&spathInfo, &inFileName[1]);
	ErrChkReport(ERR17, &inFileName[1], true);	/* not a disk file */
} /* ErrNotADisk */


void GetPstrName(pointer pstr)
{

	if (*cmdP < '?' || 'Z' < *cmdP)
		CmdErr(ERR225);	/* invalid Name() */
	pstr[0] = 0;			/* set length */
	while ('0' <= *cmdP && *cmdP <= '9' || '?' <= *cmdP && *cmdP <= 'Z') {
		pstr[0] = pstr[0] + 1;
		if (pstr[0] > 31)
			CmdErr(ERR226);	/* name too long */
		pstr[pstr[0]] = *cmdP;
		cmdP = cmdP + 1;
	}
} /* GetppstrName */


void ProcessControls()
{
	pointer p, q;
	word_t cindex;
	byte cid, caux, clen, cSegId;

	/* find the command length */
	clen = (byte)(PastFileName(cmdP) - cmdP);
	cindex.lb = 3;
	cid = 0;
	if (*cmdP == '/')	/* common seg address */
	{
		clen = 1;	/* treat specially */
		cid = 9;
	}
	else if (clen > 0)	/* look up the control */
		while (cindex.lb < sizeof(controls) - 1) {
			if (controls[cindex.lb+2] == clen && Strequ(cmdP, &controls[cindex.lb+3], clen))
			{
				cid = controls[cindex.lb];	
				caux = controls[cindex.lb+1];
				cindex.lb = sizeof(controls) - 1;
			}
			else
				cindex.lb = cindex.lb + controls[cindex.lb + 2] + 3;
		}
	if (cid == 0)		/* check we have a valid command */
		CmdErr(ERR229);	/* unrecognised control */
	SkipNonArgChars(cmdP + clen);
	switch (cid - 1) {
	    case 0:		/* CODE, DATA, STACK, MEMORY */
			segFlags[caux] = FHASADDR;	/* note that the address is specified */
			segBases[caux] = ParseLPNumRP();	/* and its value */
	    	break;
	    case 1:		/* START */
	    	seen.start = true;		/* got a start address */
			startAddr = ParseLPNumRP();	/* and its value */
	    	break;
	    case 2:		/* STACKSIZE */
	        seen.stackSize = true;		/* got a stack size */
			segSizes[SSTACK] = ParseLPNumRP();/* and its value */
	    	break;
	    case 3:
			seen.all[caux] = true; 		/* simple command seen - RESTART0, MAP, PUBLICS, SYMBOLS, LINES, PURGE */
			break;
		case 4:		/* NAME */
	    	seen.name = true;		/* got a module name */
			ExpectLP();
			GetPstrName(moduleName);	/* getand its value */
			ExpectRP();
	    	break;
	    case 5:		/* PRINT */		/* get the print file */
			ExpectLP();
			GetFile();
			MakeFullName(&spathInfo, &printFileName[1]);
			printFileName[0] = (byte)(PastFileName(&printFileName[1]) - &printFileName[1]);
			ExpectRP();
	    	break;
	    case 6:		/* ORDER */		/* process the order list */
	    	InitSegOrder();
			nxtSegOrder = 0;
			ExpectLP();
			while (*cmdP != ')') {
				cindex.lb = 3;			/* skip seg 0 */
				while (controls[cindex.lb] == 1) {	/* CODE DATA STACK MEMORY */
					if (Strequ(cmdP, &controls[cindex.lb + 3], controls[cindex.lb + 2]))
					{
						InsSegIdOrder(controls[cindex.lb + 1]);
						cmdP = cmdP + controls[cindex.lb + 2];
						cindex.lb = 0;
					}
					/* note if (cindex.lb == 0) this keeps where we are & will terminate loop */
					cindex.lb = cindex.lb + controls[cindex.lb + 2] + 3;
				}
				if (cindex.lb != 0)	/* check we haven't alReady processed */
				{			/* if (! scan for /common/ names */
					ExpectSlash();
					cSegId = GetCommonSegId();
					ExpectSlash();
					InsSegIdOrder(cSegId);
				}
				SkipNonArgChars(cmdP);
				/* look for list of names */
				if (*cmdP == ',')
					ExpectChar(',', ERR203); /* invalid syntax */
			}
			ExpectRP();
	    	break;
	    case 7:		/* COLUMNS */		/* get the number of columns */
	    		p = cmdP;	/* mark where we are */
			ExpectLP();	/* look for the ( */
			q = cmdP;	/* mark start of number */
			cmdP = p;	/* reset to start of (n) */
			columns = (byte)ParseLPNumRP();
			if (columns < 1 || columns > 3)	/* must be 1-3 */
			{
				cmdP = q;	/* for Error() reporting */
				CmdErr(ERR203);	/* invalid syntax */
			}
	    	break;
	    case 8:		/* / -> common */	/* find the common name or blank */
	    	cSegId = GetCommonSegId();	/* look up seg */
			ExpectSlash();		/* end with a slash */
			segFlags[cSegId] = FHASADDR;	/* note we have an address */
			segBases[cSegId] = ParseLPNumRP(); /* and its value */
	    	break;
	}
} /* ProcessControls */
