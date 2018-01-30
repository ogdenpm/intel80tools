#include "plm.h"

void SetDate(pointer str, byte len)
{
	if (len > 9 )
		len = 9;
	memset(DATE, ' ', 9);
    memmove(DATE, str, len);
} /* SetDate() */


void SetPageLen(word len)
{
	PAGELEN = (byte)len;
} /* SetPageLen() */


//void SetMarkerInfo(byte b1, byte b2, byte b3)
//{
//	wrapMarkerCol = b1;
//	wrapMarker = b2;
//	wrapTextCol = b3;
//}

void SetPageNo(word v)
{
	pageNo = v - 1;
}


void SetMarginAndTabW(byte b1, byte b2)
{
	margin = b1 - 1;
	tWidth = b2;
}


void SetTitle(pointer str, byte len)
{
	if (len > 60 )
		len = 60;
    memmove(TITLE, str, len);
	TITLELEN = len;
} /* SetTitle() */


void SetPageWidth(word width)
{
	PWIDTH = (byte)width;
} /* SetPageWidth() */


