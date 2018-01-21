#include "loc.h"

byte aMemoryMapOfMod[] = "\r\nMEMORY MAP OF MODULE ";
byte aStartStopLengt[] = "\r\n\nSTART   STOP LENGTH REL NAME\r\n\n";
byte aModuleStartAdd[] = "MODULE START address XXXXH";
byte aModuleIsNotAMa[] = "MODULE IS NOT A MAIN MODULE";
byte aStartControlIg[] = ", START CONTROL IGNORED";
byte aRestartControl[] = ", RESTART CONTROL IGNORED";
byte aAddresses[] = "XXXXH  XXXXH  XXXXH  X  ";
byte aMemOverlap[] = "  [MEMORY OVERLAP FROM XXXXH THROUGH XXXXH]";
byte a0LengthSegment[] = "  [0 LENGTH SEGMENT WRAPPED AROUND TO 0000H]";
byte segNames[] = "ABSOLUTE', 'CODE    ', 'DATA    ', 'STACK   ', 'MEMORY  ";
byte alignNames[] = "AIPB";

pointer GetCommonName(byte segid)
{
	static byte nullName[1] = { 0 };

	if (segid == SBLANK )
		return nullName;	/* blank name */
	ObjSeek(0,0);	/* Seek() start of file */
	GetRecord();		/* junk the modhdr record */
	while (1) {
		GetRecord();	/* scan comdef records */
		if (inRecordP->rectyp != R_COMDEF )
			BadRecordSeq();
		while (inP < erecP) {	/* look for the seg */
			if (segid == ((comnam_t *)inP)->segId )
				return ((comnam_t *)inP)->name;	/* return pointer to name */
			inP = inP + sizeof(comnam_t) + ((comnam_t *)inP)->name[0];	/* skip to next seg/name */
		}
	}
} /* GetCommonName */

void PrintMemoryMap()
{
	word minStart, q;
	byte i, wrapped, addCRLF;

	if (seen.map)		/* print map header info if asked for */
	{
		PrintListingHeader(aMemoryMapOfMod, sizeof(aMemoryMapOfMod)); /* '\r\nMEMORY MAP OF MODULE ' */
		if (isMain )
		{
			BinAsc(startAddr, 16, '0', &aModuleStartAdd[21], 4);  /* insert start address in string */
			PrintString(aModuleStartAdd, sizeof(aModuleStartAdd)-1); /* 'MODULE START address XXXXH' */
		}
		else
		{
			PrintString(aModuleIsNotAMa, sizeof(aModuleIsNotAMa)-1); /* 'MODULE IS NOT A MAIN MODULE' */
			if (seen.start)
				PrintString(aStartControlIg, sizeof(aStartControlIg)-1); /* ', START CONTROL IGNORED' */
			if (seen.restart0)
				PrintString(aRestartControl, sizeof(aRestartControl)-1); /* ', RESTART CONTROL IGNORED' */
		}
		PrintString(aStartStopLengt, 34);	/* '\r\nSTART   STOP length REL NAME\r\n\n' */
	}

	segBases[SSTACK] = segBases[SSTACK] - segSizes[SSTACK];
	for (q = 1; q <= 254; q++) {
		if (segSizes[i = segOrder[q]] > SABS  || (segFlags[i] & 0x20) != 0 )
			AddSegFrag(segFlags[i], i, segBases[i], segSizes[i]);
	}

	minStart = wrapped = 0;
	/* emit information on all of the blocks */
	curSegFragP = (segFrag_t *)topHeap;
	while ((pointer)curSegFragP != topDataFrags) {
		curSegFragP--;			/* point to start of block info C pointer arith */
		BinAsc(curSegFragP->start, 16, '0', aAddresses, 4);	/* start address */
		aAddresses[7] = '*';		/* assume no length so show **** for stop address */
		aAddresses[8] = '*';
		aAddresses[9] = '*';
		aAddresses[10] = '*';
		if (curSegFragP->len > 0 )		/* we have length so fill stop address */
			BinAsc(curSegFragP->start + curSegFragP->len - 1, 16, '0', &aAddresses[7], 4);
		BinAsc(curSegFragP->len, 16, ' ', &aAddresses[14], 4);	/* length */
		/* copy the start and stop into the memory overlap message */
		memmove(&aMemOverlap[23], aAddresses, 4);
		memmove(&aMemOverlap[37], &aAddresses[7], 4);
		if (curSegFragP->start + curSegFragP->len > minStart )	/* none | ~ total overlap */
		{
			aMemOverlap[37] = '*';		/* clear out the through address */
			aMemOverlap[38] = '*';
			aMemOverlap[39] = '*';
			aMemOverlap[40] = '*';
			/* if (we are overlap put in the end of the last block */
			if (curSegFragP->len > 0 && (minStart > 0  || wrapped != 0) )
				BinAsc(minStart - 1, 16, '0', &aMemOverlap[37], 4);
		}
		addCRLF = 0;
		if (seen.map)	/* if MAP is being produced */
		{
			aAddresses[21] = alignNames[curSegFragP->flags & AMASK];	/* add the align type */
			PrintString(aAddresses, sizeof(aAddresses)-1);	/* and print the address info */
			if (curSegFragP->seg >= SNAMED )				/* seg is named common */
			{
				PrintString("/", 1);			/* print the / */
				inP = GetCommonName(curSegFragP->seg);		/* look up the common name in the input file */
				PrintString(&inP[1], inP[0]);	/* print it and the closing /, name is in pstr format */
				PrintString("/", 1);
			}
			else							/* standard so used the canned names */
				PrintString(&segNames[curSegFragP->seg << 3], 8);	/* shl 3 is *8 */
		}
		if (curSegFragP->start < minStart || wrapped != 0 )			/* emit any overlap notification */
		{
			addCRLF = true;
			/* '  (MEMORY OVERLAP FROM XXXXH THROUGH XXXXH)'  values filled in above */
			ConAndPrint(aMemOverlap, sizeof(aMemOverlap)-1);
		}
		if ((curSegFragP->flags & FWRAP0) != 0 )				/* && if wrapped to 0 */
		{
			addCRLF = true;
			/* '  (0 length SEGMENT WRAPPED AROUND TO 0000H)' */
			ConAndPrint(a0LengthSegment, sizeof(a0LengthSegment)-1);
		}
		if (addCRLF )							/* finish of any started line */
			ConAndPrint(crlf, 2);
		else if (seen.map)
			PrintCrLf();
		if (curSegFragP->start + curSegFragP->len == 0 && curSegFragP->len > 0 )		/* we wrapped to 0 */
		{
			minStart = 0;
			wrapped = FWRAP0;
		}
		else if (wrapped == 0 )
			if (curSegFragP->start + curSegFragP->len > minStart )		/* update the minStart */
				minStart = curSegFragP->start + curSegFragP->len;
	}
} /* PrintMemoryMap */


pointer SkipSpc(pointer pch)
{
	while (*pch == ' ') {
		pch = pch + 1;
	}
	return pch;
} /* SkipSpc */

