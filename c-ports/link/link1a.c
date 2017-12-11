#include "link.h"

#pragma warning(disable:4715)	// next two functions don't return on error. Suppress warning not all paths return value

pointer GetHigh(word count)
{
	if (topHeap - membot >= count)
		return (membot = membot + count) - count;
	FileError(ERR210, &toFileName[1], TRUE);	/* Insufficient() memory */
} /* GetHigh() */

pointer GetLow(word count)
{
	if (topHeap - membot >= count)
		return (topHeap = topHeap - count);
	FileError(ERR210, &toFileName[1], TRUE);	/* Insufficient() memory */
} /* GetLow() */

#pragma warning(default:4715)

void ChkRead(word cnt)
{	/* make sure next cnt bytes are in the Input() buffer */
	word bcnt;
	
	/* see if (enough bytes in the i/o buf. if ! shuffle down && reload more */
	if ((bcnt = (word)(ebufP - bufP)) < cnt)
	{
		memmove(sbufP, bufP, bcnt);
		Read(inFile, sbufP + bcnt, npbuf - bcnt, &actRead, &statusIO);
		FileError(statusIO, &inFileName[1], TRUE);
		/* calculate new inBlk and inByt */
		inBlk = inBlk + (word)(inByt + bufP - sbufP) / 128;
		inByt = (inByt + bufP - sbufP) % 128;
		if ((bcnt = bcnt + actRead) < cnt)
			FileError(ERR204, &inFileName[1], TRUE);    /* Premature() eof */
		/* mark the new end */
		ebufP = (bufP = sbufP) + bcnt;
	}
} /* ChkRead() */

void GetRecord()
{
	word bcnt;

	if ((bcnt = (word)(ebufP - bufP)) >= 4)
	{
		inRecordP = (record_t *)bufP;
		erecP = ((pointer)inRecordP) + inRecordP->reclen + 2;	/* type + data + crc */
	}
	else
	{
		inRecordP = (record_t *)DUMMYREC;
		erecP = NULL;			// isis sets to top of memory, doesn't work here
	}
	if (erecP == NULL || erecP >= ebufP)
		if (inRecordP->reclen <= 1025)	/* should be able to get all of record in buffer */
		{
			ChkRead(bcnt + 1);
			inRecordP = (record_t *)bufP;
			if ((erecP = ((pointer)inRecordP) + inRecordP->reclen + 2) >= ebufP) /* redundant - done in ChkRead() */
				FileError(ERR204, &inFileName[1], TRUE);	/* premature eof */
		}
	recLen = inRecordP->reclen;
	inP =  inRecordP->record;
	bufP = erecP + 1;
	recNum = recNum + 1;
	if (inRecordP->rectyp > R_COMDEF || (inRecordP->rectyp & 1))	/* > 0x2E || odd */
		IllegalRelo();
	if (inRecordP->rectyp == R_MODDAT)			/* content handled specially */
		return;
	if (inRecordP->rectyp >= R_LIBLOC  && inRecordP->rectyp <= R_LIBDIC)
		return;						/* library records handled specially */
	if (recLen > 1025)
		FatalErr(ERR211);	/* record too long */
	inCRC = 0;					/* test checksum */
	for (inbP = (pointer)inRecordP; inbP <= erecP; inbP++) {
		inCRC = inCRC + *inbP;
	}
	if (inCRC != 0)
		FatalErr(ERR208);	/* checksum Error() */
} /* GetRecord() */

void Position(word blk, word byt)
{	/* Seek() in Input() buffer */
	/* check if (already in memory, if so update bufP only */
	if (inBlk <= blk && blk <= (inByt + (ebufP - sbufP))  / 128 + inBlk)
	{
		if ((bufP = sbufP + (blk - inBlk) * 128 + (byt - inByt))
		    >= sbufP && bufP < ebufP)
			return;
	}
	Seek(inFile, 2, &blk, &byt, &statusIO);		/* Seek() on disk */
	FileError(statusIO, &inFileName[1], true);
	recNum = 0;						/* reset vars and Read() at least 1 byte */
	bufP = ebufP;
	ChkRead(1);
	inBlk = blk;
	inByt = byt;
} /* Position() */

void OpenObjFile()
{
	Pstrcpy(curObjFile->name, &inFileName[0]);		/* copy the user supplied file name */
	inFileName[inFileName[0]+1] = ' ';			/* terminate with a space */
	Open(&inFile, &inFileName[1], 1, 0, &statusIO);	/* Open() the file */
	FileError(statusIO, &inFileName[1], TRUE);
	recNum = 0;
	curModule = 0;					/* reset vars and Read() at least 1 byte */
	bufP = ebufP;
	ChkRead(1);
	inBlk = inByt = 0;
} /* OpenObjFile() */

void CloseObjFile()
{					/* Close() file and link to next one */
	Close(inFile, &statusIO);
	FileError(statusIO, &inFileName[1], TRUE);
	curObjFile = curObjFile->link;
} /* CloseObjFile() */

