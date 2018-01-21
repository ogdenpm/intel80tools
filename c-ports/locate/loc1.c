/*
	vim:ts=4:shiftwidth=4:expandtab:
*/
#include "loc.h"

static byte copyright[] = "(C) 1976, 1977, 1979 INTEL CORP";
byte version[] = "V3.0";
static byte nullRec[] = {0,0,0};
static word_t pageOutOffset = { .w = 0 };
static word_t cachePageOffset = { .w = 0 };

word readfd, outputfd, printfd, tmpfd, statusIO, actRead;
byte inFileName[16], outFileName[16];
byte printFileName[16], tmpFileName[16];
byte columns;
seen_t seen;	/* START, STACK, RESTART0, MAP, PUBLICS, SYMBOLS, LINES, PURGE, NAME */
byte moduleName[32];
byte isMain;
byte modhdrX1;
byte modhdrX2;
word startAddr;
byte segOrder[255];
word segBases[256];	/* abs, code, data */
word segSizes[256];
byte segFlags[256];
record_t *inRecordP;
pointer erecP;
pointer inP;
word recNum;
word recLen;
word npbuf;
pointer sibufP;
pointer iBufP;
pointer eiBufP;
pointer outRecordP;
pointer outP;
pointer eoutP;
pointer spbufP;
pointer pbufP;
pointer epbufP;
pointer topHeap;
pointer topDataFrags;
segFrag_t *curSegFragP;
segFrag_t *nextSegFragP;
pointer botHeap;
dataFrag_t *curDataFragP;
dataFrag_t *nextDataFragP;
dataFrag_t inFragment;
bool usePrintBuf;
bool outRealFile;
pointer baseMemImage;
byte havePagingFile;
byte pageCacheSize;
byte roundRobinIndex;
byte nxtPageNo;
byte pageIndexTmpFil;
page1_t *pageTab1P;
page2_t *pageTab2P;

byte crlf[2] = { '\r', '\n' };
byte aRecordType[] = " RECORD TYPE XXH, RECORD NUMBER *****\r\n";
word inBlk = 0, inByt = 0;
pointer inbP;
byte inCRC;

void ConStrOut(pointer buf, word cnt)
{
	Write(0, buf, cnt, &statusIO);
}


void FlushPrintBuf()
{
	if (usePrintBuf)	/* only process if buffered print */
	{
		Write(printfd, spbufP, (word)(pbufP - spbufP), &statusIO);
		ErrChkReport(statusIO, &printFileName[1], true);
	}
	pbufP = spbufP;	/* rest buffer pointer */
}


void FatalErr(byte errCode)
{
	FlushPrintBuf();
	ConStrOut(" ", 1);
	ConStrOut(&inFileName[1], inFileName[0]);
	ConStrOut(",", 1);
	Errmsg(errCode);
	BinAsc(inRecordP->rectyp, 16, '0', &aRecordType[13], 2);
	if (recNum > 0)
		BinAsc(recNum, 10, ' ', &aRecordType[32], 5);
	ConStrOut(aRecordType, 39); 
	Exit();
}


void IllegalRecord()
{
	FatalErr(ERR218);	/* Illegal() Record() Format() */
}


void IllegalReloc()
{
	FatalErr(ERR212);	/* Illegal() Relo() Record() */
}


void BadRecordSeq()
{
	FatalErr(ERR224);	/* Bad() Record() Sequence() */
}

void PStrcpy(pointer psrc, pointer pdst)
{	/* copy pascal style string */
	memmove(pdst, psrc, psrc[0] + 1);
}


void PrintString(pointer bufp, word cnt)
{
	word bcnt;

	if (usePrintBuf)	/* using buffer ? */
	{
		bcnt = (word)(epbufP - pbufP); 	/* space in buffer */
		while (cnt > bcnt) {		/* ! enough room for all chars */
			memmove(pbufP, bufp, bcnt);	/* copy what we can */
			cnt = cnt - bcnt;	/* adjust what is left */
			bufp = bufp + bcnt;	/* advance position in the src string */
			pbufP = pbufP + bcnt;	/* and the print buffer position */
			FlushPrintBuf();	/* Write() the buffer */
			bcnt = npbuf;		/* reset to the buffer size */
		}
		memmove(pbufP, bufp, cnt);	/* copy the remaining chars */
		pbufP = pbufP + cnt;		/* advance the print buf ptr */
	}
	else
	{	/* just use the operating system functions */
		Write(printfd, bufp, cnt, &statusIO);
		ErrChkReport(statusIO, &printFileName[1], TRUE);
	}
}


void PrintCrLf()
{
	PrintString(crlf, 2);
}


void ConAndPrint(pointer buf, word cnt)
{
	PrintString(buf, cnt);
	if (printfd > 0)	/* make sure always seen on console */
		ConStrOut(buf, cnt);
}


void ChkRead(word cnt)
{
	word bcnt;

	if ((bcnt = (word)(eiBufP - iBufP)) < cnt)	/* data alReady in buffer */
	{
		memmove(sibufP, iBufP, bcnt);	/* move down bytes in buffer */
		Read(readfd, sibufP + bcnt, npbuf - bcnt, &actRead, &statusIO);
		ErrChkReport(statusIO, &inFileName[1], true);
		inBlk = inBlk + (inByt + (word)(iBufP - sibufP)) / 128;
		inByt = (inByt + iBufP - sibufP) % 128;
		if ((bcnt = bcnt + actRead) < cnt)
			ErrChkReport(ERR204, &inFileName[1], TRUE);  /* Premature() EOF */
		eiBufP = (iBufP = sibufP) + bcnt;
	}
}

void GetRecord()
{

	word bcnt;

	if ((bcnt = (word)(eiBufP - iBufP)) >= 4)
	{
		inRecordP = (record_t *)iBufP;
		erecP = (pointer)inRecordP + inRecordP->reclen + 2;
	}
	else
	{
		inRecordP = (record_t *)nullRec;	/* scratch area */
		erecP = NULL;						/* isis sets to top of memory */
	}
	if (erecP == NULL || erecP >= eiBufP)	/* test modified to handle chnage above*/
	{
		if (inRecordP->reclen <= 1025)
		{
			ChkRead(bcnt + 1);
			inRecordP = (record_t *)iBufP;
			if ((erecP = ((pointer)inRecordP) + inRecordP->reclen + 2) >= eiBufP)
				ErrChkReport(ERR204, &inFileName[1], true);	/* premature EOF */
		}
	}
	recLen = inRecordP->reclen;
	inP = inRecordP->record;
	iBufP = erecP + 1;	/* update for next record */
	recNum = recNum + 1;
	if (inRecordP->rectyp > R_COMDEF || (inRecordP->rectyp & 1))	/* > 0x2e || odd */
		IllegalReloc();
	if (inRecordP->rectyp == R_MODDAT)
		return;
	if (recLen > 1025)
		FatalErr(ERR211);	/* Record() too long */
	inCRC = 0;
	for (inbP = (pointer)inRecordP; inbP <= erecP; inbP++) {
		inCRC = inCRC + *inbP;
	}
	if (inCRC != 0)
		FatalErr(ERR208);	/* checksum Error() */
}


void ObjSeek(word blk, word byt)
{
	recNum = 0;		/* reset record number */
	if (inBlk <= blk && blk <= inBlk + (inByt + (eiBufP - sibufP)) / 128)
		if ((iBufP = sibufP + (blk - inBlk) * 128 + (byt - inByt)) >= sibufP
		    && iBufP < eiBufP)
			return;
	Seek(readfd, SEEKABS, &blk, &byt, &statusIO);
	ErrChkReport(statusIO, &inFileName[1], true);
	iBufP = eiBufP;
	ChkRead(1);	/* Read() the buffer */
	inBlk = blk;
	inByt = byt;
}



void SeekPagingFile(byte para)
{
	word blk;
	word zeroByt = 0;

	blk = para * 2;
	Seek(tmpfd, SEEKABS, &blk, &zeroByt, &statusIO);
	ErrChkReport(statusIO, &tmpFileName[1], true);
}


void PageOut(byte page, pointer bufp)
{
	page2_t *pt;

	pt = &pageTab2P[page];
	if (pt->fileIdx == 0xff)	/* not used */
	{
		pt->fileIdx = pageIndexTmpFil;	/* record location in tmp file */
		pageIndexTmpFil = pageIndexTmpFil + 1;
	}
	SeekPagingFile(pt->fileIdx);
	Write(tmpfd, bufp, 256, &statusIO);	/* Write() the page out */
	ErrChkReport(statusIO, &tmpFileName[1], true);
	pt->pageIdx = 0xfe;	/* flag as paged out */
}


void AnotherPage(byte page)
{
	byte i;

	if (usePrintBuf)	/* use print buffer space */
	{
		FlushPrintBuf();	/* flush the buffer */
		usePrintBuf = FALSE;	/* no longer available */
		/* move the heap up */
		memmove(botHeap + npbuf, botHeap, topHeap - botHeap);
		/* adjust the topheap and the start/cur print buf addresses */
		spbufP = pbufP = topHeap = epbufP;
		/* adjust the descriptor pointers */
		topDataFrags = topDataFrags + npbuf;
		curSegFragP = curSegFragP + npbuf;
		curDataFragP = curDataFragP + npbuf;
		/* page Allocated() is first after the current max pageCacheSize */
		nxtPageNo = pageCacheSize + 1;
		/* increase the pageCacheSize available */
		pageCacheSize = High((word)((botHeap += npbuf) - baseMemImage)) - 1;
		return;
	}
	if (! havePagingFile)	/* have to page, create file if ! done */
	{
		havePagingFile = true;
		Delete(&tmpFileName[1], &statusIO);
		Open(&tmpfd, &tmpFileName[1], 3, false, &statusIO);
		ErrChkReport(statusIO, &tmpFileName[1], true);
	}

	pageTab1P[page].state = 0xff;	/* mark this page as now free */
	i = pageTab1P[page].mpage;	/* get the current target page stored here */
	pageOutOffset.hb = page;
	PageOut(i, baseMemImage + pageOutOffset.w);
}


pointer AllocNewPage(byte page)
{
	for (nxtPageNo = 0; nxtPageNo <= pageCacheSize; nxtPageNo++) {		/* see if (free page */
		if (pageTab1P[nxtPageNo].state)	/* one is free */
			goto found;
	}
	if (roundRobinIndex > pageCacheSize)		/* manage roundRobin for paging to disk */
		roundRobinIndex = 0;
	nxtPageNo = roundRobinIndex;
	AnotherPage(roundRobinIndex);		/* get a page */
	if (nxtPageNo == roundRobinIndex)	/* if the roundRobin page then we didn't get from printbuf reuse */
		roundRobinIndex = roundRobinIndex + 1;	/* so manage the roundRobin counter */
found:
	pageTab1P[nxtPageNo].state = 0;	/* no longer free */
	pageTab1P[nxtPageNo].mpage = page;	/* record the memory page located here */
	pageTab2P[page].pageIdx = nxtPageNo;	/* save the mapping */
	cachePageOffset.hb = nxtPageNo;
	return baseMemImage + cachePageOffset.w;		/* return the pointer in memory */
}


/* convert a target address into its current location in cache */
pointer AddrInCache(word addr)
{
	address baddr;
	page2_t *pt;

	pt = &pageTab2P[High(addr)];
	if (pt->pageIdx == 0xFF)	/* needs Allocating() */
		return AllocNewPage(High(addr)) + Low(addr);
	if (pt->pageIdx == 0xFE)	/* on disk */
	{
		baddr.bp = AllocNewPage(High(addr));		/* Alloc page in cache */
		SeekPagingFile(pt->fileIdx);		/* Seek() to its place in the paging file */
		Read(tmpfd, baddr.bp, 256, &actRead, &statusIO);	/* Read() in */
		ErrChkReport(statusIO, &tmpFileName[1], true);
		if (actRead != 256)			/* if problems & ~ :BB: out file then Error() */
			if (outRealFile)
				ErrChkReport(ERR204, &tmpFileName[1], true);    /* premature EOF */
		return baddr.bp + Low(addr);		/* return the mapped address */
	}
	baddr.hb = pt->pageIdx;		/* page offset in cache */
	baddr.lb = Low(addr);		/* offset in page */
	return baseMemImage + baddr.w;	/* mapped address */
}


void Alloc(word cnt)
{
	/* Allocate() but check whether we have crossed a page boundary */
	if (High(botHeap - baseMemImage) != High((botHeap -= cnt) - baseMemImage))
	{
		if (! pageTab1P[pageCacheSize].state)	/* if current cache all used, try forcing using print buf */
			AnotherPage(pageCacheSize);	/* or paging the last page to disk */

		if ((pageCacheSize = High((word)(botHeap - baseMemImage)) - 1) == 0xff)	/* check we haven't eliminated all cache */
			ErrChkReport(ERR210, &inFileName[1], true);	/* Insufficient() memory */
	}
}


void AddSegFrag(byte flags, byte seg, word start, word len)
{
	segFrag_t *curSegFragP, *nextSegFragP;

	curSegFragP = (segFrag_t *)topHeap;

	while ((pointer)curSegFragP != topDataFrags) {
		curSegFragP--; // modified for C pointer arithmetic
		if (curSegFragP->start > start)
			if (start + len == curSegFragP->start && curSegFragP->seg == seg) /* does the new frag join the curfrag */
			{	/* merge the blocks */
				curSegFragP->start = start;
				curSegFragP->len = curSegFragP->len + len;
				return;
			}
			else
				goto done;
		else if (curSegFragP->start + curSegFragP->len == start)	/* does the curFrag join the new frag */
			if (curSegFragP->seg == seg)			/* segs must be same */
			{
				curSegFragP->len = curSegFragP->len + len;	/* merge the lengths */
				if ((pointer)curSegFragP != topDataFrags)	/* if not at the end of list see if this joins the next one */
				{
					nextSegFragP = curSegFragP - 1; // C pointer arith
					if (nextSegFragP->start == curSegFragP->start + curSegFragP->len && nextSegFragP->seg == curSegFragP->seg)
					{
						curSegFragP->len = curSegFragP->len + nextSegFragP->len;	/* join and Delete() extra frag */
						memmove(botHeap + sizeof(segFrag_t), botHeap, ((pointer)nextSegFragP) - botHeap);
						topDataFrags = topDataFrags + sizeof(segFrag_t);
						botHeap = botHeap + sizeof(segFrag_t);
					}
				}
				return;
			}
	}
	curSegFragP--;	/* will insert at end of list */
done:
	Alloc(sizeof(segFrag_t));	/* reserve space for the segFrag */
	topDataFrags = topDataFrags - sizeof(segFrag_t);
	memmove(botHeap, botHeap + sizeof(segFrag_t), ((pointer)curSegFragP) - botHeap);	/* create the space in the list */
	curSegFragP->flags = flags;	/* set all of the values */
	curSegFragP->seg = seg;
	curSegFragP->start = start;
	curSegFragP->len = len;
}


void AddDataFrag(word saddr, word eaddr)
{
	dataFrag_t *curDataFragP, *nextDataFragP;

	curDataFragP = (dataFrag_t *)topDataFrags;
	while ((pointer)curDataFragP != botHeap) {
		curDataFragP = curDataFragP--;
		if (curDataFragP->saddr > saddr)	/* the curfrag starts above this one */
		{
			if (eaddr >= curDataFragP->saddr - 1)	/* does the new frag join || overlap the curent one */
			{
				curDataFragP->saddr = saddr;		/* update the start */
				if (eaddr > curDataFragP->eaddr)	/* it overlaps */
				{
					curDataFragP->eaddr = eaddr;	/* extend the existing */
					nextDataFragP = curDataFragP - 1;	/* check if (more are overlapped */
					while ((pointer)curDataFragP != botHeap) {
						if (eaddr + 1 < nextDataFragP->saddr)	/* no there is a gap all done */
							return;
						if (eaddr + 1 == nextDataFragP->saddr)	/* joins so update the end address */
							curDataFragP->eaddr = nextDataFragP->eaddr;
						/* remove the other descriptor if (either joined || was overlapped */
						memmove(botHeap + sizeof(dataFrag_t), botHeap, ((pointer)nextDataFragP) - botHeap);
						botHeap = botHeap + sizeof(dataFrag_t);
					}
				}
				return;
			}
			else	/* we have a new fragment */
			{
				Alloc(4);	/* Alloc() more space and insert descriptor above this one */
				memmove(botHeap, botHeap + sizeof(dataFrag_t), ((pointer)curDataFragP) - botHeap);
				curDataFragP->saddr = saddr;
				curDataFragP->eaddr = eaddr;
				return;
			}
		}
		else	/* check if (candidate frag to join / overlap */
		{
			if (curDataFragP->saddr == saddr || saddr - 1 <= curDataFragP->eaddr)
			{
				if (curDataFragP->eaddr >= eaddr)	/* alReady overlapped */
					return;
				curDataFragP->eaddr = eaddr;		/* set new end address */
				nextDataFragP = curDataFragP - 1;	/* Delete() other join/overlapping frags */
				while ((pointer)curDataFragP != botHeap) {
					if (nextDataFragP->saddr - 1 > eaddr)
						return;
					if (nextDataFragP->saddr - 1 == eaddr)
						curDataFragP->eaddr = nextDataFragP->eaddr;
					memmove(botHeap + sizeof(dataFrag_t), botHeap, ((pointer)nextDataFragP) - botHeap);
					botHeap = botHeap + sizeof(dataFrag_t);
				}
				return;
			}
		}	/* if (else */
	}	/* DO WHILE */

	Alloc(sizeof(dataFrag_t));
	curDataFragP = (dataFrag_t *)botHeap;	/* Allocated() at new botHeap */
	curDataFragP->saddr = saddr;	/* record the start and end address */
	curDataFragP->eaddr = eaddr;
}


void LoadModdat(byte segId)
{
	word dataLen, bytes2Read, curLoadAddr;
	byte curLoadPage;

	dataLen = recLen - 4;
	inCRC = High(recLen) + 6 + Low(recLen) + inP[0] + inP[1] + inP[2];
	iBufP = iBufP + 3;
	if (Low(curLoadAddr = inFragment.saddr) != 0)
	{
		if (High(curLoadAddr + (bytes2Read = dataLen) - 1) != High(curLoadAddr))
			bytes2Read = 256 - Low(curLoadAddr);	/* don't go over page boundary */
		ChkRead(bytes2Read);			/* make sure data is in the buffer */
		for (inbP = iBufP; inbP <= iBufP + bytes2Read - 1; inbP++) {	/* add the data to the crc */
			inCRC = inCRC + *inbP;
		}
		memmove(AddrInCache(curLoadAddr), iBufP, bytes2Read);	/* copy to the pagecache location */
		dataLen = dataLen - bytes2Read;			/* adjust bytes Read() */
		curLoadAddr = curLoadAddr + bytes2Read;
		iBufP = iBufP + bytes2Read;
	}
	while (dataLen >= 256) {				/* Read() in whole pages */
		ChkRead(256);				/* make sure data in buffer */
		for (inbP = iBufP; inbP <= iBufP + 255; inbP++) {		/* add to crc */
			inCRC = inCRC + *inbP;
		}
		if (segId == SABS && havePagingFile)		/* if abs && paging Write() it out to paging file */
								/* there is no fixup involved */
		{
			/* if (a page cache entry exists) this would be trashed anyway so free up */
			if (pageTab2P[curLoadPage = High(curLoadAddr)].pageIdx <= 0xfd)
				pageTab1P[pageTab2P[curLoadPage].pageIdx].state = 0xff;
			PageOut(curLoadPage, iBufP);
		}
		else						/* copy to the page cache */
			memmove(AddrInCache(curLoadAddr), iBufP, 256);

		dataLen = dataLen - 256;			/* adjust and continue */
		curLoadAddr = curLoadAddr + 256;
		iBufP = iBufP + 256;
	}
	if (dataLen > 0)					/* load an remaining into the page cache */
	{
		ChkRead(dataLen);
		for (inbP = iBufP; inbP <= iBufP + dataLen - 1; inbP++) {
			inCRC = inCRC + *inbP;
		}
		memmove(AddrInCache(curLoadAddr), iBufP, dataLen);
		iBufP = iBufP + dataLen;
	}
	ChkRead(1);
	inbP = iBufP++;
	if ((byte)(inCRC + *inbP) != 0)					/* sanity check */
		FatalErr(ERR208);	/* Checksum() Error() */
}

void Start()
{
//	baseMemImage = pageTab2P = controls;	/* once we have processed args overWrite area */
	pageTab2P = (page2_t *)(baseMemImage = MEMORY);	// isis version reuses memory
	
	ProcArgsInit();
	LocateFile();
	FlushPrintBuf();
	Close(printfd, &statusIO);
	Exit();

}
