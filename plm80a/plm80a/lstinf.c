#include "plm.h"

void LstModuleInfo()
{
	word p, q, r;
	p = 0;
	for (q = 1; q <= procCnt; q++) {
		curInfoP = botInfo + procInfo[q];
		r = GetBaseVal();
		if (r > p )
			p = r;
	}
	SetSkipLst(3);
	Xputstr2cLst("MODULE INFORMATION:", 0);
	NewLineLst();
	SetSkipLst(1);
	TabLst(5);
	Xputstr2cLst("CODE AREA Size()     = ", 0);
	XnumLst(csegSize, 0xFC, 0xF0);
	TabLst(2);
	XnumLst(csegSize, 5, 0xF6);
	NewLineLst();
	TabLst(5);
	Xputstr2cLst("VARIABLE AREA Size() = ", 0);
	XnumLst(dsegSize, 0xFC, 0xF0);
	TabLst(2);
	XnumLst(dsegSize, 5, 0xF6);
	NewLineLst();
	TabLst(5);
	Xputstr2cLst("MAXIMUM STACK Size() = ", 0);
	XnumLst(p, 0xFC, 0xF0);
	TabLst(2);
	XnumLst(p, 5, 0xF6);
	NewLineLst();
	TabLst(5);
	XnumLst(linesRead, 0, 0xA);
	Xputstr2cLst(" LINES READ", 0);
	NewLineLst();
	TabLst(5);
	XnumLst(programErrCnt, 0, 0xA);
	Xputstr2cLst(" PROGRAM ERROR", 0);
	if (programErrCnt != 1)
		Xputstr2cLst("S", 0);

	NewLineLst();
	SetSkipLst(1);
	Xputstr2cLst("END OF PL/M-80 COMPILATION", 0);
	NewLineLst();
	FlushLstBuf();
	CloseF(&lstFil);
	lfOpen = false;
}
