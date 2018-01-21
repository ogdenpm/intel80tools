#include "plm.h"
/* common source for lstsp[456].plm */

void FlushLstBuf()
{
	if (lChCnt != 0) {
		if (! lfOpen) {
			OpenF(&lstFil, 2);
			lfOpen = true;
		}
		WriteF(&lstFil, lBufP, lChCnt);
		lChCnt = 0;
	}
}

void NewLineLst()
{
	if (col == 0)
		if (linLft == 0)
			NewPgl();
	Wr2cLst(0x0d0a);
	linLft = linLft - 1;
	col = 0;
}


void TabLst(byte arg1b)
{
	if (arg1b > 0x7f) {
		arg1b = -arg1b;
		if (col >= arg1b)
			NewLineLst();
		arg1b = arg1b - col - 1;
	}
	while (arg1b != 0) {
		PutLst(' ');
		arg1b = arg1b - 1;
	}
}


void NewPageNextChLst()
{
	linLft = 0;
}

void SetMarkerInfo(byte arg1b, byte arg2b, byte arg3b)
{
	b3CFB = arg1b;
	b3CFC = arg2b;
	b3CFD = arg3b;
}


void SetStartAndTabW(byte arg1b, byte arg2b)
{
	margin = arg1b - 1;
	tWidth = arg2b;
}


void SetSkipLst(byte arg1b)
{
	b3CFF = arg1b;
}


void Xputstr2cLst(pointer str, byte endch)
{
	while (*str != endch)
		PutLst(*str++);
}

void XwrnstrLst(pointer str, byte cnt)
{
	while (cnt != 0) {
		PutLst(*str++);
		cnt = cnt - 1;
	}
}


void XnumLst(word num, byte width, byte radix)
{
	byte i, buf[7];

	i = Num2Asc(num, width, radix, buf);
	XwrnstrLst(buf, i);
}
