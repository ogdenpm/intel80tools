#include "loc.h"

byte aUnsatisfiedExt[] = "UNSATISFIED EXTERNAL(XXXXX) ";

byte aReferenceToUns[] = "REFERENCE TO UNSATISFIED EXTERNAL(XXXXX) AT XXXXH\r\n";
byte aSymbolTableOfM[] = "\r\nSYMBOL TABLE OF MODULE ";
byte aReadFromFile[] = "\r\nREAD FROM FILE ";
byte aWrittenToFile[] = "\r\nWRITTEN TO FILE ";
byte aValueType[] = "VALUE TYPE ";
byte aSymbol[] = "\x6" "SYMBOL";
byte aPub[] = "XXXXH PUB  ";
byte aSym[] = "XXXXH SYM  ";
byte alin[] = "XXXXH LIN  ";
byte aMod[] = "      MOD  ";
byte x5[] = "\x5" "XXXXX";
byte spc32[] = "                                ";

byte loHiBoth;
byte outSegType;
static byte unused[2];
address curColumn;	// used both as pointer and word !!!
word workingSegBase;
word unsatisfiedCnt = {0};
byte curcol = {0};
pointer curListField = {0};
byte nameLen = {0};
record_t *lsoutP;


void SeekOutFile(word mode, wpointer pblk, wpointer pbyt)
{
	Seek(outputfd, mode, pblk, pbyt, &statusIO);
	ErrChkReport(statusIO, &outFileName[1], true);
} /* SeekOutFile */


void FlushOut()
{
	Write(outputfd, outRecordP, (word)(outP - outRecordP), &statusIO);
	ErrChkReport(statusIO, &outFileName[1], true);
	outP = outRecordP;
} /* FlushOut */

void WriteBytes(pointer bufP, word cnt)
{
	word bcnt;

	bcnt = (word)(eoutP - outP);
	while (cnt > bcnt) {
		memmove(outP, bufP, bcnt);
		cnt = cnt - bcnt;
		bufP = bufP + bcnt;
		outP = outP + bcnt;
		FlushOut();
		bcnt = npbuf;
	}
	memmove(outP, bufP, cnt);
	outP = outP + cnt;
} /* WriteBytes */


void InitRecOut(byte rectyp)
{
	if (eoutP - outP < 1028 )
		FlushOut();
	lsoutP = (record_t *)outP;
	lsoutP->rectyp = rectyp;
	outP = lsoutP->record;	/* skip rectyp and reclen */
} /* InitRecOut */


void EndRecord()
{
	byte crc;
	pointer pch;

	/* check record not too long */
	if ((lsoutP->reclen = (word)(outP - &lsoutP->rectyp) - 2) > 1025 )
		ErrChkReport(ERR211, &outFileName[1], true);	/* RECORD TOO LONG */
	/* for what is there generate the CRC */
	crc = 0;
	for (pch = (pointer)lsoutP; pch <= outP - 1; pch++) {
		crc = crc + *pch;
	}
	*outP = -crc;
	outP = outP + 1;
} /* EndRecord */


void EmitModDat(dataFrag_t *curDataFragP)
{
	word len, bcnt;
	pointer pmem;
	word curPC, j;
	byte crc;

	InitRecOut(R_MODDAT);
	len = curDataFragP->eaddr - curDataFragP->saddr + 1;
	lsoutP->reclen = len + 4;
	((moddat_t *)outP)->segId = SABS;		/* abs seg */
	curPC = (((moddat_t *)outP)->offset = curDataFragP->saddr);		/* load address */
					/* initialise the CRC */
	crc = High(lsoutP->reclen) + 6 + Low(lsoutP->reclen) + High(((moddat_t *)outP)->offset) + Low(((moddat_t *)outP)->offset);
	outP = outP + 3;

	while (len > 0) {
		bcnt = 256;	/* Write() 256, || less if (no page boundary || less bytes available */
		if (Low(curPC) != 0 )
			bcnt = 256 - Low(curPC);
		if (bcnt > len )
			bcnt = len;
		pmem =  AddrInCache(curPC);	/* get a pointer to where the data is now */
		for (j = 0; j <= bcnt - 1; j++) {		/* include the data in the CRC check */
			crc = crc + pmem[j];
		}
		WriteBytes(pmem, bcnt);	/* add the bytes to the output record */
		if (bcnt == 256 )		/* if was a whole page then free it */
		{
			pageTab1P[pageTab2P[High(curPC)].pageIdx].state = 0xff;
			pageTab2P[High(curPC)].pageIdx = 0xff;	/* mark page as free */
		}
		curPC = curPC + bcnt;		/* account for written data */
		len = len - bcnt;
	}	/* DO WHILE */

	crc = -crc;			/* Close() off the record */
	WriteBytes(&crc, 1);
} /* EmitModDat */


void ForceSOL()
{
	if (curcol != 0 )
		PrintCrLf();
	curcol = 0;
} /* ForceSOL */



void PrintColumn(pointer field, pointer pstr)
{

	/* change of field type ) force new line */
	if (field != curListField )
		ForceSOL();
	curListField = field;
	/* reached max of columns ) force new line */
	if (curcol == columns )
		ForceSOL();
	/* space to next column: 32 chars - length of previous name */
	if (curcol != 0 )
		PrintString(spc32, 32 - nameLen);
	curcol = curcol + 1;
	nameLen = pstr[0];
	/* print out field lead */
	PrintString(field, 11);
	/* and the name */
	PrintString(&pstr[1], nameLen);
} /* PrintColumn */


byte SetWorkingSeg(byte seg)
{	/* always returns 0 */
	workingSegBase = segBases[seg];
	return SABS;
} /* SetWorkingSeg */


void ProcModend()
{
	pointer rst0P;

	if (((modend_t *)inP)->modType == 1)	/* is main module */
	{
		isMain = 1;	/* flag as main module */
		outSegType = SetWorkingSeg(((modend_t *)inP)->segId);
		if (! seen.start )	/* use the main module's address */
			startAddr = workingSegBase + ((modend_t *)inP)->offset;
		if (seen.restart0 )	/* restart0 the put jump to startAddr */
		{
			rst0P = AddrInCache(0);	/* location of address 0 in cache */
			*rst0P = 0xc3;			/* insert the jmp */
			*(wpointer)(rst0P + 1) = startAddr;
			AddSegFrag(AABS, SABS, 0, 3);/* add to the frags */
			AddDataFrag(0, 2);
		}
	}
	else
		startAddr = 0;	/* 0 start address if (! a main module */
	FlushOut();			/* flush any data in the buffers */
	curDataFragP = (dataFrag_t *)topDataFrags;
	while ((pointer)curDataFragP != botHeap) {	/* emit all of the data frags */
		curDataFragP--;
		EmitModDat(curDataFragP);
	}

	InitRecOut(R_MODEND);	/* generate the rectyp 4 record */
	((modend_t *)outP)->modType = isMain;
	((modend_t *)outP)->segId = 0;
	((modend_t *)outP)->offset = startAddr;
	outP = outP + 4;
	WriteBytes(inP + 4, recLen - 4 - 1);	/* copy rectyp 4 tail over */
	EndRecord();					/* insert length and CRC */
	InitRecOut(R_MODEOF);				/* EOF record */
	EndRecord();
	FlushOut();
} /* ProcModend */


void ProcModdat()
{
	struct { word blk; word byt; } mark, endrec;
	byte fixExtUsed;
	word fixRaddr;
//	word pad;
	word locRecLen, recSegBaseAddr;
	pointer fixLoc, 	/* fixup location of byte */
	       	fixLoc2;	/* fixup location of 2nd byte if cross page boundary */
	word_t bothByt; /* used to handle cross page fixup */
	static byte r_moddat = R_MODDAT;	// local used implement .(R_MODDAT)

	inRecordP = (record_t *)&r_moddat;		/* initialise to appear as a MODDAT record */
	iBufP = inP;
	ChkRead(3);		/* make sure the record and length are in the buffer */
	inP = iBufP;
	if (((moddat_t *)inP)->segId == SSTACK )
		FatalErr(ERR238);	/* ILLEGAL STACK CONTENT RECORD */

	outSegType = SetWorkingSeg(((moddat_t *)inP)->segId);	/* set to ABS and get working base address */
	recSegBaseAddr = workingSegBase;
	/* set the bounds for this moddat record checking for wrap round */
	inFragment.saddr = recSegBaseAddr + ((moddat_t *)inP)->offset;
	if ((inFragment.eaddr = inFragment.saddr + recLen - 5) < inFragment.saddr )
		IllegalRecord();	/* oops over 64k */
	if (((moddat_t *)inP)->segId == SABS )	/* add this as a new frag */
		AddSegFrag(0, 0, inFragment.saddr, recLen - 4);
	else if ((segFlags[((moddat_t *)inP)->segId] & AMASK) == AUNKNOWN )
		IllegalRecord();	/* align is invalid */
	if ((locRecLen = recLen) > 1025 )	/* long records only ok if ABS */
	{
		if (((moddat_t *)inP)->segId != 0 )
			FatalErr(ERR211);	/* RECORD TOO LONG */
		AddDataFrag(inFragment.saddr, inFragment.eaddr);
	}
	LoadModdat(((moddat_t *)inP)->segId);	/* suck the data in */
	GetRecord();
	if (locRecLen > 1025 )	/* can only happen for ABS blocks so no fixup */
		return;

	fixExtUsed = 0;

	while (inRecordP->rectyp == R_FIXEXT || inRecordP->rectyp == R_FIXLOC || inRecordP->rectyp == R_FIXSEG) {
		if (inRecordP->rectyp == R_FIXEXT )
		{
			if (! fixExtUsed )
			{
				FlushOut();	/* make sure written to disk and record current location */
				SeekOutFile(SEEKTELL, &mark.blk, &mark.byt);
				endrec.blk = 0;
				endrec.byt = locRecLen + 3;
				SeekOutFile(SEEKFWD, &endrec.blk, &endrec.byt);	/* past where data will go */
			}
			fixExtUsed = true;	/* note we have fixext record[s] */
			InitRecOut(R_FIXEXT);	/* initialise the fixext record */
			if ((loHiBoth = *inP /* inLoHiBoth */) - 1 > FBOTH - 1 )	/* check fixup type valid */
				IllegalRecord();

			*outP /* outLoHiBoth */ = loHiBoth;	/* copy to the output */
			outP = outP + 1;
			inP = inP + 1;

			while (inP < erecP) {
				/* set the new offset but do bounds check also */
				FixupBoundsChk(((extfix_t *)outP)->offset = recSegBaseAddr + ((extfix_t *)inP)->offset);
				if (loHiBoth == FBOTH )
					FixupBoundsChk(((extfix_t *)outP)->offset + 1);	/* check 2nd byte in range */
				if ((((extfix_t *)outP)->namIdx = ((extfix_t *)inP)->namIdx) >= unsatisfiedCnt )
					BadRecordSeq();	/* name index out of range */
				ForceSOL();			/* make sure on new line to record the problem */
				BinAsc(((extfix_t *)outP)->namIdx, 10, ' ', &aReferenceToUns[34], 5);
				BinAsc(((extfix_t *)outP)->offset, 16, '0', &aReferenceToUns[44], 4);
				/* print 'REFERENCE TO UNSATISFIED EXTERNAL(' */
				ConAndPrint(aReferenceToUns, 34);
				/* print the 'XXXXX) AT XXXXH\r\n' skipping leading spaces */
				ConAndPrint((curColumn.bp = SkipSpc(&aReferenceToUns[34])),
						17 - (word)(curColumn.bp - &aReferenceToUns[34]));
				outP = outP + 4;	/* advance out and in position */
				inP = inP + 4;
			}

			EndRecord();	/* finish off the record */
		}
		else
		{
			workingSegBase = recSegBaseAddr;			/* restore fixup seg base to the moddat's own */
			if (inRecordP->rectyp == R_FIXSEG )				/* if different seg the set it up */
			{
				outSegType = SetWorkingSeg(*inP /* inSegId */);
				inP = inP + 1;				/* rest treat as rectyp FIXLOC */
			}
			if ((loHiBoth = *inP /* inLoHiBoth */) - 1 > FBOTH - 1 )	/* is fixup valid */
				IllegalRecord();
			inP = inP + 1;

			while (inP < erecP) {					/* process the record */
				/* set the fixup real address and check in range, both bytes for BOTH fixup type */
				FixupBoundsChk(fixRaddr = recSegBaseAddr + *(wpointer)inP /* inFixOffset */);
				if (loHiBoth == FBOTH )
					FixupBoundsChk(fixRaddr + 1);
				/* find the fixup location in memory */
				fixLoc = AddrInCache(fixRaddr);
				switch (loHiBoth - 1) {
				case 0:	*fixLoc = *fixLoc + Low(workingSegBase); break;	/* LO add in low of target base address */
				case 1: *fixLoc = *fixLoc + High(workingSegBase);	break;  /* HI add in high of target base address */
				case 2:						/* BOTH */
					if (Low(fixRaddr) != 0xff)	/* doesn't span pages */
						*(wpointer)fixLoc += workingSegBase;	/* simple add in to existing word value */
					else
					{
						bothByt.lb = *fixLoc;			/* get the low byte */
						fixLoc2 = AddrInCache(fixRaddr + 1);	/* find address of high byte */
						bothByt.hb = *fixLoc2;
						bothByt.w = bothByt.w + workingSegBase;	/* add in the target base address */
						*fixLoc = bothByt.lb;			/* and put back in place */
						*fixLoc2 = bothByt.hb;
					}
				}
				inP = inP + 2;
			}
		   }
		GetRecord();
	}
	if (fixExtUsed)
	{
		FlushOut();	/* flush the extdef data */
		SeekOutFile(SEEKABS, &mark.blk, &mark.byt);	/* backup to the data record */
		EmitModDat(&inFragment);			/* emit the data record fragment as it needs fixup */
		FlushOut();
		SeekOutFile(SEEKEND, &endrec.blk, &endrec.byt);	/* back to end of file */
	}
	else
		AddDataFrag(inFragment.saddr, inFragment.eaddr);	/* record the fragment */

} /* ProcModdat */


