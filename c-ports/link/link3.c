#include "link.h"

static byte msgmultdef[] = " - MULTIPLELY DEFINED, DUPLICATE IN ";
static byte msgmore1main[] = "MORE THAN 1 MAIN MODULE, CONFLICT IN ";
static byte msgmodnotinlib[] = "MODULE not IN LIBRARY, LOOKING FOR ";
static byte msgbadcom[] = "/ - UNEQUAL COMMON LENGTH, CONFLICT IN ";
static byte msglinkmap[] = "\r\nLINK MAP OF MODULE ";
static byte msgwrittento[] = "\r\nWRITTEN to FILE ";
static byte msgmodtype[] = "\r\nMODULE IS A MAIN MODULE";
static byte msgnotmain[] = "\r\nMODULE IS not A MAIN MODULE";
static byte msgcomSegInfo[] = "\r\n\nSEGMENT INFORMATION:"
			        "\r\nSTART   STOP LENGTH REL NAME\r\n";
byte msgmodincluded[] = "\r\n\nINPUT MODULES INCLUDED:\r\n";
byte msgunresolved[] = "\r\nUNRESOLVED EXTERNAL NAMES:\r\n";
byte spc14[] = "              ";
byte aRange[] = "XXXXH  XXXXH  ";
byte aSize[] = "XXXXH  X  ";
byte msgGap[] = "  *GAP*";
byte msgOverlap[] = "  *OVERLAP*";
byte aInpageSegment[] = "  *INPAGE SEGMENT > 256 BYTES*";
byte msgPublics[] = " (PUBLICS)";
byte slash2[2] = "//";
byte aAbsolute[] =  "ABSOLUTE" "CODE    " "DATA    " "STACK   " "MEMORY  ";
byte aTypes[] = " AIPB";

byte segUsed[6];
byte segToUse = SEG_BLANK - 1;
bool noCommonSeen = true;
bool initTraiIdVn = true;
comseginfo_t *comSegInfoP = 0;
segFrag_t *segFrags[6];
segFrag_t *segFragP;
segFrag_t *curSegFragP, *prevSegFragP;
symbol_t *tailSegOrderLink;
symbol_t *nxtSymbolP;
module_t *hmoduleP;
byte publicsMode;
byte moreRecords;
byte haveModuleHdr;
byte inSegCombine;
byte curSeg;
byte dummyrec;
word inSegLen;
word segI;
word symcnt;
word newUnresolved;
word externCnt;
word inSegOffset;

void PrimeRecord()
{
	/* save the record type and con inRecord to believe it is a real record */
	dummyrec = inRecordP->rectyp;
	inRecordP = (record_t *)&dummyrec;
	/* set live bufP to where we have procssed to - keeps in buffer */
	bufP = inP;
} /* PrimeRecord() */

void SkipRecord()
{
	while (ebufP - bufP < recLen) {		/* Read() in off disk if (needed */
		recLen = recLen - (word)(ebufP - bufP);
		bufP = ebufP;
		ChkRead(1);
	}
	bufP = bufP + recLen;				/* update bufP to start of next record */
} /* SkipRecord() */

byte ReadName()
{
	ChkRead(1);		/* make sure 1 char there */
	inP = bufP;			/* set to start of name */
	if (recLen < 2)		/* only CRC left ? */
	{
		bufP = bufP + recLen;	/* advance and flag no more */
		return (byte)(recLen = 0);
	}
	ChkRead(*inP + 1);		/* check all of the name is there */
	inP = bufP;			/* in case it has moved */
	if (recLen < *inP + 1)	/* overran */
		IllFmt();
	bufP = bufP + *inP + 1;	/* past name */
	recLen = recLen - (*inP + 1);	/* account for name */
					/* inP points to the name */
	return true;			/* Read() a name */
} /* ReadName() */

/* Read() in 4 byte block num and byte num inP points to the data, bufP beyond it */
void ReadBlkByt()
{
	if (recLen < 4)
		IllFmt();
	ChkRead(4);
	bufP = (inP = bufP) + 4;
	recLen = recLen - 4;
} /* ReadBlkByt() */

/* get next record, test type, && Error() if (! as expected */
void ExpectType(byte type)
{
	GetRecord();
	recNum = 0;
	if (type != inRecordP->rectyp)
		BadRecordSeq();
} /* ExpectType() */

/* print segment base, Size() and alignment */
void WriteBaseSizeAlign(word baddr, word bsize, byte align)
{

	WriteCRLF();
	if ((align = align + 1) <= 1)	/* ANONE || AUNKNOWN */
	{	/* insert start and stop in hex */
		BinAsc(baddr, 16, '0', aRange, 4);	/* 'XXXXH  XXXXH  ' */
		BinAsc(baddr + bsize - 1, 16, '0', aRange + 7, 4);
		WriteBytes(aRange, 14);	/* print start and stop */
	}
	else	/* replace start / stop with spaces */
		WriteBytes(spc14, sizeof(spc14) - 1);	// -1  to ignore \0
	/* format the Size() in hex */
	BinAsc(bsize, 16, ' ', aSize, 4);
	/* insert the align type ' AIPB' */
	aSize[7] = aTypes[align];
	WriteBytes(aSize, 10);	/* print the Size() */
} /* WriteBaseSizeAlign() */

/* wrtie module statistics */
void WriteStats()
{
//	byte i;

	if (! mapWanted)	/* user doesn't want */
		return;
	WriteBytes(msglinkmap, sizeof(msglinkmap) - 1);	/* '\r\nLINK MAP OF MODULE ' */
	WriteBytes(&outModuleName[1], outModuleName[0]);		/* module name */
	WriteBytes(msgwrittento, sizeof(msgwrittento) - 1);	/* '\r\nWRITTEN to FILE ' */
	WriteBytes(&toFileName[1], toFileName[0]);		/* file name */
	if (modEndModTyp == MT_NOTMAIN)
		WriteBytes(msgnotmain, sizeof(msgnotmain) - 1);	/* '\r\nMODULE IS not A MAIN MODULE' */
	else
		WriteBytes(msgmodtype, sizeof(msgmodtype) - 1);	/* '\r\nMODULE IS A MAIN MODULE' */
	WriteBytes(msgcomSegInfo, sizeof(msgcomSegInfo) - 1);		/* '\r\n\nSEGMENT INFORMATION\r\n' */
									/* 'START   STOP length() REL NAME\r\n */
	for (curSeg = SEG_CODE; curSeg <= SEG_MEMORY; curSeg++) {	/* regular segments */
		if (segLen[curSeg] > 0)	/* segment is used */
		{
			WriteBaseSizeAlign(0, segLen[curSeg], alignType[curSeg]);
			WriteBytes(&aAbsolute[curSeg * 8], 8);	/* print 8 character seg name */
			if (alignType[curSeg] == AINPAGE && segLen[curSeg] > 256)
				WriteBytes(aInpageSegment, sizeof(aInpageSegment) - 1);	/* '  *INPAGE SEGMENT > 256 BYTES*' */
			segFragP = segFrags[curSeg];
			while (segFragP > 0) {				/* print the details for any gap fragments */
				WriteBaseSizeAlign(segFragP->bot, segFragP->top - segFragP->bot + 1, ANONE); 
				WriteBytes(&aAbsolute[curSeg * 8], 8);
				WriteBytes(msgGap, sizeof(msgGap) - 1);	/* note the gap */
				segFragP = segFragP->link;		/* loop till done */
			}
		}
	}
	comdefInfoP = headSegOrderLink;				/* common block segments */
	while (comdefInfoP > 0) {
		WriteBaseSizeAlign(0, comdefInfoP->len, comdefInfoP->flags);		/* print Size() info */
		WriteBytes(slash2, 1);						/* add the name at the end */
		WriteBytes(&comdefInfoP->name[1], comdefInfoP->name[0]);
		WriteBytes(slash2, 1);
		if (comdefInfoP->flags == AINPAGE && comdefInfoP->len > 256)		/* warn of problems */
			WriteBytes(aInpageSegment, sizeof(aInpageSegment) - 1);	/* inpage segment > 256 bytes */
		comdefInfoP = comdefInfoP->nxtSymbol->hashLink;
	}
	if (segLen[0] > 0)								/* handle blank common */
	{
		WriteBaseSizeAlign(0, segLen[0], alignType[0]);
		WriteBytes(slash2, 2);						/* // */
		if (alignType[0] == 1 && segLen[0] > 256)
			WriteBytes(aInpageSegment, sizeof(aInpageSegment)-1);	/* inpage segment > 256 bytes */
	}
	segI = 0;			/* segI used High() water mark for over lap detection - start at address 0 */
	segFragP = segFrags[0];	/* frags for SEG_ABS are real data blocks */
	while (segFragP > 0) {		/* go throught them */
		WriteBaseSizeAlign(segFragP->bot, segFragP->top - segFragP->bot + 1, 0);	/* print the sizes and segment */
		WriteBytes(aAbsolute, 8);
		if (segFragP->bot < segI)				/* check for overlap */
			WriteBytes(msgOverlap, sizeof(msgOverlap) - 1);
		if (segFragP->top >= segI)				/* update the High() water mark for next block */
			segI = segFragP->top + 1;
		segFragP = segFragP->link;		/* to next fragment */
	}	/* do while */
	WriteBytes(msgmodincluded, sizeof(msgmodincluded) - 1);	/* '\r\n\nINPUT MODULES INCLUDED:\r\n' */ 
	curObjFile = objFileHead;					/* go over each file processed */
	while (curObjFile > 0) {
		curModule = curObjFile->modList;				/* and each module processed */
		while (curModule > 0) {
			if (! curObjFile->publicsMode || curModule->symlist > 0)	/* ignore if publics only && nothing loaded */
			{
				WriteBytes(" ", 1);			/* ' filename[modulename]' */
				WriteBytes(&curObjFile->name[1], curObjFile->name[0]);
				WriteBytes("(", 1);
				WriteBytes(&curModule->name[1], curModule->name[0]);
				WriteBytes(")", 1);
				if (curObjFile->publicsMode)			/* note publics only */
					WriteBytes(msgPublics, sizeof(msgPublics) - 1);
				WriteCRLF();
			}
			curModule = curModule->link;
		}
		curObjFile = curObjFile->link;
	}
} /* WriteStats() */

void ChainUnresolved()
{	/* creates a chain of unresolved externals */
	symbol_t *p;
       	word toChain;

	if (unresolved == 0)		/* nothing to Write() */
		return;
	WriteAndEcho(msgunresolved, sizeof(msgunresolved) - 1);	/* '\r\nUNRESOLVED external NAMES:\r\n' */
	toChain = unresolved;
	for (segI = 0; segI <= 127; segI++) {		/* traverse all of the HashF() chains */
		symbolP = hashTab[segI];
		while (symbolP) {	/* traverse the single HashF() chain */
			if (symbolP->flags == F_EXTERN)		/* we have an extern */
			{
				nxtSymbolP = (symbol_t *)&headUnresolved;	/* Lookup() where to insert this symbol on the list */
				p = headUnresolved;
				while (p > 0) {			/* scan whole list if (necessary */
					if (p->offsetOrSym > symbolP->offsetOrSym)
						break;	/* passed insert point using sym num */
					nxtSymbolP = (symbol_t *)&p->nxtSymbol;
					p = p->nxtSymbol;
				}
				symbolP->nxtSymbol = nxtSymbolP->hashLink;	/* add to the unresolved chain */
				nxtSymbolP->hashLink = symbolP;
				if ((toChain = toChain - 1) == 0)		/* done */
					return;
			}
			symbolP = symbolP->hashLink;
		} /* do while */
	}	/* for */
} /* ChainUnresolved() */

/* inserts a block of data into segment list at proper address */
void CreateFragment(byte seg, word bot, word top)
{
	if (! mapWanted)		/* we are not creating a map so ignore */
		return;
	segFragP = (segFrag_t *)GetHigh(sizeof(segFrag_t));	/* allocate and initialise the fragment */
	segFragP->bot = bot;
	segFragP->top = top;
	prevSegFragP = (segFrag_t *)&segFrags[seg];		/* separate lists for each segment */
	curSegFragP = segFrags[seg];
	/* look for insert point or reach the end */
	while (curSegFragP > 0) {
		if (curSegFragP->bot > bot)
			goto insert;
		prevSegFragP = curSegFragP;	/* next */
		curSegFragP = curSegFragP->link;
	}
insert:
	segFragP->link = prevSegFragP->link;		/* insert this fragment */
	prevSegFragP->link = segFragP;
} /* CreateFragment() */

void P1CommonSegments()
{
	if (curSeg == SEG_BLANK)	/* blank common */
	{
		if (segUsed[0])	/* record seen in slot 0 */
			IllFmt();
		segUsed[0] = true;
		if (inSegLen > segLen[0])	/* record max Size() */
			segLen[0] = inSegLen;
		if (alignType[0] == AUNKNOWN)
			alignType[0] = inSegCombine;
		else if (alignType[0] != ABYTE || inSegCombine != ABYTE)
			alignType[0] = APAGE;	/* if (! both byte align make page align */
	}
	else
	{
		if (noCommonSeen)	/* we have commons so far so allocate mapping table */
		{
			if (comSegInfoP == 0)	/* no table allocated so allocate one */
				comSegInfoP = (comseginfo_t *)GetHigh(sizeof(comseginfo_t));
			for (segI = 0; segI <= SEG_BLANK; segI++) {	/* initialise */
				comSegInfoP[segI].combine = AUNKNOWN;
			}
			noCommonSeen = 0;
		}
		if (comSegInfoP[curSeg].combine != AUNKNOWN)	/* duplicate */
			IllFmt();
		comSegInfoP[curSeg].combine = inSegCombine;	/* save combine and Size() */
		comSegInfoP[curSeg].lenOrLinkedSeg = inSegLen;
	}
} /* P1CommonSegments() */

void P1StdSegments()
{
	word prevLen, segLoadBase;

	if (segUsed[curSeg])		/* duplicate seg Size() info */
		IllFmt();
	segUsed[curSeg] = 0xFF;		/* note seen */
	if (curSeg == SEG_ABS || curSeg > SEG_MEMORY)
		IllFmt();
	if (inSegLen == 0)		/* nothing to do */
		return;
	if (curSeg == SEG_CODE || curSeg == SEG_DATA)
	{
		if (alignType[curSeg] == AUNKNOWN)	/* first seg */
		{
			alignType[curSeg] = inSegCombine;
			segLen[curSeg] = inSegLen;
			segLoadBase = 0;
		}
		else
		{
			prevLen = segLen[curSeg];
			switch(inSegCombine) {
			case 1:		/* IN PAGE */
				if (Low(prevLen) + inSegLen <= 0x100)		/* if fits in current page */
					segLoadBase = prevLen + inSegLen;	/* calculate the base */
				else						/* else start new page */
					segLoadBase = ((prevLen + 0xFF) & 0xFF00) + inSegLen;

				if (alignType[curSeg] != AINPAGE || segLoadBase > 0x100)	/* check if we should use page align */
					alignType[curSeg] = APAGE;
				break;
			case 2: 	/* PAGE */
				alignType[curSeg] = APAGE;			/* calculate new page */
				segLoadBase = ((prevLen + 0xFF) & 0xFF00) + inSegLen;
				break;
			case 3:	/* byte */
				if (alignType[curSeg] == AINPAGE)		/* if were in page we now have to assume page align */
					alignType[curSeg] = APAGE;
				segLoadBase = prevLen + inSegLen;		/* Lookup() out Load() address */
				break;
			}
			segLen[curSeg] = segLoadBase;					/* update the overall seg len */
			if ((segLoadBase = segLoadBase - inSegLen) > prevLen)	/* backup to start of this Load() address */
				CreateFragment(curSeg, prevLen, segLoadBase - 1);	/* not contiguous so create fragment */
			if (segLen[curSeg] < segLoadBase)				/* oops we went over 64k */
				FatalErr(ERR221);	/* segment too large */
		}
		if (curSeg == SEG_CODE)		/* update the code / data base address */
			curModule->scode = segLoadBase;
		else
			curModule->sdata = segLoadBase;
	}
	else
	{	/* SEG_STACK or SEG_MEMORY or SEG_RESERVE */
		if (alignType[curSeg] == AUNKNOWN)		/* set initial combine */
			alignType[curSeg] = inSegCombine;
		else if (alignType[curSeg] != ABYTE || inSegCombine != ABYTE)	/* APAGE if ! both ABYTE */
			alignType[curSeg] = APAGE;
		segLen[curSeg] = segLen[curSeg] + inSegLen;	/* length() is additive */
	}
} /* P1StdSegments() */

/*
	select requested seg, setting inSegOffset to point to base
	returns returns seg - note for common len is replaced by final linked seg
	after the comdef records have been processed
*/

byte SelectInSeg(byte seg)
{
	inSegOffset = 0;
	if (seg == SEG_CODE)
		inSegOffset = curModule->scode;
	else if (seg == SEG_DATA)
		inSegOffset = curModule->sdata;
	else if (seg >= SEG_NAMCOM && seg != SEG_BLANK)
	{
		if (noCommonSeen)	/* selecting common when none exists !! */
			IllFmt();
		if (comSegInfoP[seg].combine == ANONE)	/* named common has been seen so ok */
			return (byte)comSegInfoP[seg].lenOrLinkedSeg;
		if (comSegInfoP[seg].combine != AUNKNOWN)
			BadRecordSeq();
		IllFmt();
	}
	return seg;
} /* SelectInSeg() */

void P1ModHdr()
{
	if (haveModuleHdr)			/* catch multiple header errors */
		BadRecordSeq();
	haveModuleHdr = true;
	nxtSymbolP = (symbol_t *)&curModule->symlist;
	if (publicsMode)			/* ! loading data */
		return;
	inP = inP + inP[0] + 1;	/* past module name */
	if (initTraiIdVn)			/* copy x1x2 data */
	{
		initTraiIdVn = 0;
		outTranId = inP[0]; // tranId
		outTranVn = inP[1]; // tranVn
	}
	if (inP[0] != outTranId)	/* propgate none 0 only if same */
		outTranId = 0;
	if (inP[1] != outTranVn)
		outTranVn = 0;
	inP = inP + 2;			/* past the x1 x2 */
	curModule->scode = segLen[SEG_CODE];	/* code and data offsets of this module */
	curModule->sdata = segLen[SEG_DATA];
	noCommonSeen = true;			/* first */
	for (segI = SEG_ABS; segI <= SEG_RESERVE; segI++) {
		segUsed[segI] = 0;
	}
	while (inP < erecP) {		/* while more segments */
		if ((inSegCombine = ((segdef_t *)inP)->combine) - 1 > 2)	/* only AINPAGE - ABYTE valid */
			IllFmt();
		inSegLen = ((segdef_t *)inP)->len;
		if ((curSeg = ((segdef_t *)inP)->segId) >= SEG_NAMCOM)
			P1CommonSegments();
		else
			P1StdSegments();
		inP = inP + sizeof(segdef_t);	/* advance to next def */
	}
} /* P1ModHdr() */

void P1ModEnd()
{
	moreRecords = 0;
	if (publicsMode)
		return;
	if (((modend_t *)inP)->modType == MT_MAIN)
		if (modEndModTyp != MT_NOTMAIN)	/* duplicate main modules !! */
			WAEFnAndMod(msgmore1main, sizeof(msgmore1main) - 1);
		else
		{
			modEndModTyp = MT_MAIN;	/* record main and save entry point */
			modEndSegId = SelectInSeg(((modend_t *)inP)->segId);
			modEndOffset = inSegOffset + ((modend_t *)inP)->offset;
		}
	if (! noCommonSeen)	/* check common alignments */
		for (segI = 0; segI <= 255; segI++) {
			if (comSegInfoP[segI].combine + 1 > 1)	/* only AUNKNOWN & ANONE valid */
				BadRecordSeq();
		}
} /* P1ModEnd() */

void Pass1CONTENT()
{	
	PrimeRecord();
	ChkRead(3);	/* make sure seg and address Read() */
	inP = bufP;
	if (! publicsMode)	/* skip if just processing publics */
	{
		if ((curSeg = ((moddat_t *)inP)->segId) == SEG_ABS)	/* absolute record */
			CreateFragment(SEG_ABS, ((moddat_t *)inP)->offset, ((moddat_t *)inP)->offset + recLen - 5);
		else						/* relocatable record */
		{
			if (recLen > 1025)		/* only abs > 1025 */
				FatalErr(ERR211);	/* record too long */
			if (curSeg == SEG_STACK)
				FatalErr(ERR238);	/* illegal stack content record */
			if (curSeg < SEG_NAMCOM)
			{
				if (! segUsed[curSeg])	/* seg was ! defined in modhdr */
					IllFmt();
			}
			else
			{
				if (curSeg == 0xFF)	/* blank common */
				{
					if (! segUsed[0])	/* set was ! defined in modhdr */
						IllFmt();
				}
				else
					curSeg = SelectInSeg(curSeg);
			}
		}
	}
	SkipRecord();	/* don't need the data on the first pass */
} /* Pass1CONTENT() */

void Pass1COMDEF()
{
	if (publicsMode)	/* ! needed */
		return;
	if (noCommonSeen)	/* can't have common def if no common segments */
		BadRecordSeq();
	while (inP < erecP) {
		if ((curSeg = ((comnam_t *)inP)->segId) < SEG_NAMCOM || curSeg == SEG_BLANK)	/* ! a named common */
			IllFmt();
		if (comSegInfoP[curSeg].combine + 1 < 2)	/* AUNKNOWN && ANONE invalid */
			IllFmt();
		if (Lookup(((comnam_t *)inP)->name, &comdefInfoP, F_ALNMASK))	/* already exist ? */
		{
			/* if (! both ABYTE) make APAGE */
			if (comdefInfoP->flags != ABYTE || comSegInfoP[curSeg].combine != ABYTE)
				comdefInfoP->flags = APAGE;
			if (comdefInfoP->len != comSegInfoP[curSeg].lenOrLinkedSeg)	/* if not same size */
			{
				if (comSegInfoP[curSeg].lenOrLinkedSeg > comdefInfoP->len)	/* set as max of sizes */
					comdefInfoP->len = comSegInfoP[curSeg].lenOrLinkedSeg;
				WriteAndEcho(slash2, 1);			/* warning sizes are different */
				WriteAndEcho(&comdefInfoP->name[1], comdefInfoP->name[0]);
				WAEFnAndMod(msgbadcom, sizeof(msgbadcom) - 1);
			}
		}
		else	/* new entry required */
		{
			comdefInfoP->hashLink = (symbol_t *)GetLow(sizeof(symbol_t) + ((comnam_t *)inP)->name[0]);
			comdefInfoP = comdefInfoP->hashLink;	/* link in and mark new end of chain */
			comdefInfoP->hashLink = 0;
			comdefInfoP->flags = comSegInfoP[curSeg].combine;	/* save the combine value */
			if (segToUse < SEG_NAMCOM)	/* check we haven't created too many segs in the linked file */
				FatalErr(ERR236);	/* too many common segments */
			comdefInfoP->linkedSeg = segToUse;	/* record the linked seg for this segment */
			segToUse = segToUse - 1;
			Pstrcpy(((comnam_t *)inP)->name, comdefInfoP->name);	/* copy the name */
			comdefInfoP->len = comSegInfoP[curSeg].lenOrLinkedSeg;		/* and Size() */
			comdefInfoP->nxtSymbol->hashLink = tailSegOrderLink->hashLink;		/* chain into seg order */
			tailSegOrderLink->hashLink = comdefInfoP;
			tailSegOrderLink = comdefInfoP->nxtSymbol;
		} /* else */
		comSegInfoP[curSeg].combine = ANONE;		/* flag as done */
		comSegInfoP[curSeg].lenOrLinkedSeg = comdefInfoP->linkedSeg;	/* replace len with the linkedSeg */
		inP = inP + 2 + ((comnam_t *)inP)->name[0];			/* next name */
	}	/* do while */
} /* Pass1COMDEF() */


void MarkPublic()
{
	symbolP->flags = F_PUBLIC;	/* now public */
	symbolP->linkedSeg = curSeg;	/* location known */
	symbolP->offsetOrSym = inSegOffset + ((def_t *)inP)->offset;
	segmap[curSeg] = 0xFF;		/* flag as used seg */
	symbolP->nxtSymbol = nxtSymbolP->hashLink;	/* add to the publics chain */
	nxtSymbolP->hashLink = symbolP;
	nxtSymbolP = (symbol_t *)&symbolP->nxtSymbol;
} /* MarkPublic() */


void Pass1PUBNAMES()
{
	/* inP -> segId */
	if ((curSeg = *inP) != SEG_ABS && publicsMode)	/* publics only requires absolute targets */
		return;
	curSeg = SelectInSeg(curSeg);	/* get the linked seg */
	inP = inP + 1;
	while (inP < erecP) {		/* while more public definitions */
		if (Lookup(((def_t *)inP)->name, &symbolP, F_SCOPEMASK))	/* Lookup() extern || public */
		{
			if (symbolP->flags == F_PUBLIC)			/* Error() if ! same location */
			{
				if (curSeg != 0 || symbolP->linkedSeg != curSeg || symbolP->offsetOrSym != ((def_t *)inP)->offset)
				{
					WriteAndEcho(&symbolP->name[1], symbolP->name[0]);
					WAEFnAndMod(msgmultdef, sizeof(msgmultdef) - 1);
				}
			}
			else
			{	/* was extern but we now have an address */
				unresolved = unresolved - 1;
				MarkPublic();
			}
		}
		else
		{	/* create a new entry */
			symbolP->hashLink = (symbol_t *)GetLow(sizeof(symbol_t) + ((def_t *)inP)->name[0]);
			symbolP = symbolP->hashLink;	/* add to HashF() chain */
			symbolP->hashLink = 0;
			Pstrcpy(((def_t *)inP)->name, symbolP->name);	/* add the name */
			MarkPublic();				/* make it a public */
		}
		inP = inP + 4 + ((def_t *)inP)->name[0];			/* to next pubdef */
	}	/* do while */
} /* Pass1PUBNAMES() */

void Pass1EXTNAMES()
{
	if (publicsMode)		/* skip if ! wanted */
		return;
	while (erecP > inP) {		/* while more external definitions */
		externCnt = externCnt + 1;	/* bump the number of externs */
		if (! Lookup(inP, &symbolP, F_SCOPEMASK))	/* ! currently extern || public */
		{
			symbolP->hashLink = (symbol_t *)GetLow(sizeof(symbol_t) + inP[0]);	/* create an entry */
			symbolP = symbolP->hashLink;		/* link in */
			symbolP->hashLink = 0;
			symbolP->flags = F_EXTERN;		/* extern and record symcnt number */
			symbolP->offsetOrSym = (symcnt = symcnt + 1);
			Pstrcpy(inP, symbolP->name);	/* copy name */
			unresolved = unresolved + 1;		/* one more to resolve */
			newUnresolved = newUnresolved + 1;
		}
		inP = inP + 2 + inP[0]; /* 2 for len byte and extra 0 at end */
	}
} /* Pass1EXTNAMES() */

/* process pass 1 records */
void P1Records(byte newModule)
{
	if (newModule)	/* create entry for new module */
	{
		hmoduleP->link = (curModule = (module_t *)GetLow(sizeof(module_t) + inP[0]));
		curModule->link = 0;
		curModule->symlist = 0;
		Pstrcpy(inP, curModule->name);
		hmoduleP = curModule;
	}
	externCnt = 0;
	recNum = 1;
	haveModuleHdr = 0;
	moreRecords = true;
	while (moreRecords) {
		switch (inRecordP->rectyp) {
		case 0x00: IllegalRelo(); break;	/* 0 */
		case 0x02: P1ModHdr(); break;		/* R_MODHDR */
		case 0x04: P1ModEnd(); break;		/* R_MODEND */
		case 0x06: Pass1CONTENT(); break;	/* R_CONTENT */
		case 0x08: break;			/* R_LINENO */
		case 0x0A: IllegalRelo(); break;	/* 0A */
		case 0x0C: IllegalRelo(); break;	/* 0C */
		case 0x0E: FileError(ERR204, &inFileName[1], true); break;	/* Premature() R_EOF */
		case 0x10: break;			/* R_ANCESTOR */
		case 0x12: break;			/* R_LOCALS */
		case 0x14: IllegalRelo(); break;	/* 14 */
		case 0x16: Pass1PUBNAMES(); break;	/* R_PUBLICS */
		case 0x18: Pass1EXTNAMES(); break;	/* R_EXTNAMES */
		case 0x1A: IllegalRelo(); break;	/* 1A */
		case 0x1C: IllegalRelo(); break;	/* 1C */
		case 0x1E: IllegalRelo(); break;	/* 1E */
		case 0x20: break;			/* R_EXTREF */
		case 0x22: break;			/* R_RELOC */
		case 0x24: break;			/* R_INTERSEG */
		case 0x26: BadRecordSeq(); break;	/* R_LIBLOCS */
		case 0x28: BadRecordSeq(); break;	/* R_LIBNAMES */
		case 0x2A: BadRecordSeq(); break;	/* R_LIBDICT */
		case 0x2C: BadRecordSeq(); break;	/* R_LIBHDR */
		case 0x2E: Pass1COMDEF(); break;	/* R_COMDEF */
		}
		GetRecord();
	}
	if (externCnt > maxExternCnt)	/* get max of externs */
		maxExternCnt = externCnt;
	/* make sure next record is valid or eof */
	if (inRecordP->rectyp != R_MODEOF && inRecordP->rectyp != R_MODHDR && inRecordP->rectyp != R_LIBNAM)
		FatalErr(ERR220);	/* no eof */
} /* P1Records() */

void P1LibScan()
{
//	byte pad[3];
	word blk, byt, blk2A, byt2A, modIdx, toResolve;
    word i, libModulesToLoad;
    loc_t *modlocsP = 0;

	curObjFile->hasModules = true;
	if (unresolved > 0)
	{
		hmoduleP = (module_t *)&curObjFile->modList;
		blk = ((loc_t *)(inP + 2))->blk;		/* library header block  just after the count word*/
		byt = ((loc_t *)(inP + 2))->byt;		/* library header byte */
		Position(blk, byt);	/* Seek() to library module names record */
		ExpectType(R_LIBNAM);	/* get rec and validate type */ 
		PrimeRecord();			/* save pointers */
		blk = (byt + recLen + 3) / 128 + blk; /* location of the libloc record */
		byt = (byt + recLen + 3) % 128;
		SkipRecord();
		ExpectType(R_LIBLOC);	/* should have the locations */
		blk2A = blk + (byt + recLen + 3) / 128;	/* location of the libdic record */
		byt2A = (byt + recLen + 3) % 128;
		newUnresolved = unresolved;	/* first pass attempts to resolve all unresolved */
		while (newUnresolved > 0) {	/* loop incase scan adds more externs that the lib can resolve */
			libModulesToLoad = 0;
			modlocsP = (loc_t *)GetHigh(0);		/* base memory of the new module locations info */
			Position(blk2A, byt2A);	/* go to the libdic */
			ExpectType(R_LIBDIC);
			PrimeRecord();
			modIdx = 1;
			toResolve = newUnresolved;	/* 1st pass scans for all unresolved */
							/* later passes may finish quicker as only new unresolved */
							/* can be matched */
			newUnresolved = 0;
			/* scan the dictionary across all names or until no unresolved */
			while (ReadName() && toResolve > 0) {
				if (inP[0] == 0)	/* new module - 0 separates */
					modIdx = modIdx + 1;
				else if (Lookup(inP, &symbolP, F_EXTERN))	/* matched an unresolved external */
				{
					/* i = */ GetHigh(4);		/* adds to modlocsP vector - return address is junked */
					modlocsP[libModulesToLoad].blk = modIdx;	/* record the module id */
					libModulesToLoad = libModulesToLoad + 1;
					toResolve = toResolve - 1;
					while (ReadName() && inP[0] != 0) 	/* skip to next module's names */
						;
					modIdx = modIdx + 1;
				}
			} /* do while */
			if (libModulesToLoad > 0)	/* matched some modules */
			{
				Position(blk, byt);	/* get the libloc data */
				ExpectType(R_LIBLOC);
				PrimeRecord();
				modIdx = 0;
				for (i = 0; i <= libModulesToLoad - 1; i++) {			/* for each module */
					while (modlocsP[i].blk > modIdx) {	/* step its location information */
						modIdx = modIdx + 1;
						ReadBlkByt();
					}
					modlocsP[i].blk = ((loc_t *)inP)->blk;	/* save the location information */
					modlocsP[i].byt = ((loc_t *)inP)->byt;
				}
				for (i = 0; i <= libModulesToLoad - 1; i++) {			/* process each module needed */
					Position(modlocsP[i].blk, modlocsP[i].byt);		/* Seek() module location in library */
					ExpectType(R_MODHDR);
					P1Records(true);		/* creates a new module entry */
					curModule->blk = modlocsP[i].blk;		/* record the module location */
					curModule->byt = modlocsP[i].byt;		/* in the module data for phase 2 */
				}
			}
		}	/* do while */
	}
} /* P1LibScan() */

void P1LibUserModules()
{
	word modIdx, i;
    byte unmatched;


	Position(((loc_t *)(inP + 1))->blk, ((loc_t *)(inP + 1))->byt);	/* Seek() to the libnam section - libhdr is current rec */
	ExpectType(R_LIBNAM);
	PrimeRecord();		/* record its a LIBNAM record and the start of the content */
	modIdx = 0;
	unmatched = true;
	while (ReadName() && unmatched) {	/* scan the library looking for specified modules */
		modIdx = modIdx + 1;	/* index for later into libloc data */
		unmatched = 0;		/* set flag for Exit() if (module ! matched */
		curModule = curObjFile->modList;	/* go over the supplied list of modules */
		while (curModule > 0) {		
			if (curModule->scode == 0)	/* ! matched yet */
			{
				if (Strequ(inP, curModule->name, curModule->name[0] + 1))
					curModule->scode = modIdx;	/* record matched module */
				else
					unmatched = true;	/* at least one module not matched */
			}
			curModule = curModule->link;	/* next one */
		}
	}
	SkipRecord();		/* skip any remaining names */
	ExpectType(R_LIBLOC);	/* expect the libloc record */
	PrimeRecord();			/* tag where we are */
	for (i = 1; i <= modIdx; i++) {		/* for all locations */
		ReadBlkByt();	/* get the location info */
		curModule = curObjFile->modList;	/* scan the module list to see if (we need this one */
		while (curModule > 0) {
			if (i == curModule->scode)
			{
				curModule->blk = ((loc_t *)inP)->blk;	/* put in the location info */
				curModule->byt = ((loc_t *)inP)->byt;
			}
			curModule = curModule->link;	/* keep going */
		}
	}
	hmoduleP = (module_t *)&curObjFile->modList;		/* pointer to remove missing modules from the chain */
	curModule = curObjFile->modList;
	while (curModule > 0) {
		if (curModule->scode == 0)
		{
			WAEFnAndMod(msgmodnotinlib, sizeof(msgmodnotinlib) - 1);	/* Module() not in library, looking for */
			hmoduleP->link = curModule->link;	/* remove this module from the list */
		}
		else
		{
			Position(curModule->blk, curModule->byt);	/* process the module */
			ExpectType(R_MODHDR);
			P1Records(false);		/* module already known so don't create entry for it */
			hmoduleP = curModule;			/* this module remains on the list */
		}
		curModule = curModule->link;		/* keep going */
	}
} /* P1LibUserModules() */

void Phase1()
{
//	membot = controls;			/* use memory no longer needed */
	for (segI = SEG_ABS; segI <= SEG_RESERVE; segI++) {
		segFrags[segI] = 0;
	}
	symcnt = 0;				/* no symbols */
	tailSegOrderLink = (symbol_t *)&headSegOrderLink;	/* no common segments */
	curObjFile = objFileHead;
	while (curObjFile > 0) {		/* process each item in the Input() list */
		OpenObjFile();		/* Open() the file */
		publicsMode = curObjFile->publicsMode;
		GetRecord();			/* Load() the first record */
		if (inRecordP->rectyp == R_LIBHDR)	/* library? */	
		{
			if (curObjFile->hasModules)	/* user specified modules */
				P1LibUserModules();
			else
				P1LibScan();		/* library scan */
		}
		else
		{
			if (inRecordP->rectyp == R_MODHDR)	/* simple object file */
			{
				if (curObjFile->hasModules)	/* oops user thought it was a library */
					FatalErr(ERR235);	/* not a library */
				hmoduleP = (module_t *)&curObjFile->modList;
				while (inRecordP->rectyp == R_MODHDR) {	/* process each module in file */
					P1Records(true);	/* this is a new module */
				}
				if (inRecordP->rectyp != R_MODEOF)
					FatalErr(ERR220);	/* no eof */
			}
			else
				FatalErr(ERR239);	/* no module header record */
		}
		CloseObjFile();
	}
	WriteStats();
	ChainUnresolved();
} /* Phase1() */

