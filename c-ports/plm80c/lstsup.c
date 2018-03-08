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
	linLft--;
	col = 0;
}


void TabLst(byte tabTo)
{
	if (tabTo > 127) {
		tabTo = -tabTo;
		if (col >= tabTo)
			NewLineLst();
		tabTo -= col + 1;
	}
	while (tabTo-- != 0)
		PutLst(' ');

}


void NewPageNextChLst()
{
	linLft = 0;
}

void SetMarkerInfo(byte markerCol, byte marker, byte textCol)
{
	wrapMarkerCol = markerCol;
	wrapMarker = marker;
	wrapTextCol = textCol;
}


void SetStartAndTabW(byte startCol, byte width)
{
	margin = startCol - 1;
	tWidth = width;
}


void SetSkipLst(byte cnt)
{
	skipCnt = cnt;
}


void Xputstr2cLst(pointer str, byte endch)
{
	while (*str != endch)
		PutLst(*str++);
}

void XwrnstrLst(pointer str, byte cnt)
{
	while (cnt-- != 0)
		PutLst(*str++);
}


void XnumLst(word num, byte width, byte radix)
{
	byte i, buf[7];

	i = Num2Asc(num, width, radix, buf);
	XwrnstrLst(buf, i);
}
