#include "loc.h"

byte nxtSegOrder;

word ParseLPNumRP()
{
	word num;

	ExpectLP();
	if (*cmdP < '0' || '9' < *cmdP)	/* must have digit */
		CmdErr(ERR203);	/* invalid syntax */
	num = ParseNumber(&cmdP);
	if (PastFileName(cmdP) != cmdP)	/* make sure ! a filename || bad number */
		CmdErr(ERR203);	/* invalid syntax */
	ExpectRP();
	return num;
} /* ParseLPNumRP */


byte GetCommonSegId()
{
	byte name[32];

	if (*cmdP == '/')
		return SBLANK;	/* blank common */
	GetPstrName(name);
	name[name[0] + 1] = ' ';	/* space at end */
	ObjSeek(0, 0);
	GetRecord();		/* skip modhdr record */

	while (1) {
		GetRecord();
		if (inRecordP->rectyp != R_COMDEF)
			ErrChkReport(ERR237, &name[1], true);	/* common not found */
		while (inP < erecP) {
			if (Strequ(name, ((comnam_t *)inP)->name, name[0]+1))
				return ((comnam_t *)inP)->segId;
			inP = inP + 2 + ((comnam_t *)inP)->name[0];
		}
	}
} /* GetCommonSegId */


void InsSegIdOrder(byte seg)
{
	byte i;
	nxtSegOrder = nxtSegOrder + 1;
	if (segOrder[nxtSegOrder] == seg)	/* alReady in order */
		return;
	for (i = 0; i <= nxtSegOrder; i++) {
		if (segOrder[i] == seg)	/* alReady Allocated() ? */
			CmdErr(ERR203);	/* invalid syntax */
	}
	for (i = nxtSegOrder + 1; i <= SBLANK - 1; i++) {	/* find the current location of seg */
		if (segOrder[i] == seg)
			break;
	}

	while (i > nxtSegOrder) {
		segOrder[i] = segOrder[i - 1];	/* make room for this seg */
		i = i - 1;
	}
	segOrder[nxtSegOrder] = seg;		/* insert it */
} /* InsSegIdOrder */


void ReadCmdLine()
{

	Read(1, cmdP, 128, &actRead, &statusIO);
	ErrChkReport(statusIO, cin, true);
	cmdP[actRead] = '\r';
	StrUpr(cmdP);
} /* ReadCmdLine */


void ProcArgsInit()
{
	pointer argsP;
	address p;

	/* work out a good io buf size */
	/* checks are for >= 32k of space, >= 16k of space */
	if ((pageCacheSize = High((word)(MemCk() - baseMemImage)) - 1) >= 128)
		npbuf = 4096;
	else if (pageCacheSize >= 64)
		npbuf = 2048;
	else
		npbuf = 1056;

	eiBufP = iBufP = MemCk();	/* in buffer at top of memory */
	sibufP = eoutP = eiBufP - npbuf;	/* output buffer is below this */
						/* and for now now print buffer */
	outRecordP = outP = spbufP = pbufP = epbufP = eoutP - npbuf;
	scmdP = MEMORY;		/* command line buffer */
	/* if (>= 32k) also use a print buffer */
	if ((usePrintBuf = pageCacheSize >= 64))
		scmdP = outRecordP;	/* seems odd to share output buf */
	*scmdP = '-';
	Rescan(1, &statusIO);	/* rewind to get command line args */
	cmdP = scmdP + 1;
	ReadCmdLine();		/* Read() the command line args */
	cmdP = SkipSpc(cmdP);		/* skip leading space, DEBUG if (present && loc command */
	if (Strequ(cmdP, mdebug, 6))
		cmdP = SkipSpc(cmdP + 6);
	cmdP = argsP = SkipSpc(PastFileName(cmdP));	/* argsP marks start of real args */
	while (*cmdP != '\r') {		/* Read() until end of args */
		if (*cmdP == '&')	/* handle continuation line */
		{
			p.bp = cmdP;	/* mark the & */
			cmdP = SkipSpc(cmdP + 1);	/* check we didn't have anything after */
			if (*cmdP != '\r')
				CmdErr(ERR203);	/* invalid syntax */
			cmdP = p.bp;	/* reset to & */
			ConStrOut(mstar2, 2);	/* emit ** to user */
			cmdP[1] = '\r';	/* put the cr lf ** after the & */
			cmdP[2] = '\n';
			cmdP[3] = '*';
			cmdP[4] = '*';
			cmdP = cmdP + 5;	/* position to Read() the next line */
			ReadCmdLine();	/* Read() another line */
		}
		else	/* skip char */
			cmdP = cmdP + 1;
	}	/* DO WHILE */

	/* display the sign on message skipping the form feed */
	ConStrOut(signonMsg + 1, 23);
	ConStrOut(version, 4);
	ConStrOut(crlf, 2);
	/* if (we have run into the in buffer) we have a problem */
	if (cmdP > sibufP)
		CmdErr(ERR210);	/* insufficient memory */
	/* skip the leading space of the command args */
	SkipNonArgChars(argsP);
	/* get the file we are locating */
	GetFile();
	/* can only locate a disk file */
	if (spathInfo.deviceType != 3)
		ErrNotADisk();
	MakeFullName(&spathInfo, &inFileName[1]);
	/* convert to a omf style string by putting the length in at the front */
	inFileName[0] = (byte)(PastFileName(&inFileName[1]) - &inFileName[1]);
	/* check for TO (followed by space or & */
	if (Strequ(cmdP, mto, 3) || Strequ(cmdP, mtoand, 3))
	{
		SkipNonArgChars(cmdP + 2);
		GetFile();	/* get the output file */
	}
	else
	{	/* handle the case of TO missing unless the input did not have an extension */
		if (spathInfo.ext[0] == 0)
			CmdErr(ERR233);	/* TO expected */
		spathInfo.ext[0] = 0;	/* assume TO would be input file name without extension */
		spathInfo.ext[1] = 0;
		spathInfo.ext[2] = 0;
	}
	/* get the output file name in standard format */
	MakeFullName(&spathInfo, &outFileName[1]);
	outFileName[0] = (byte)(PastFileName(&outFileName[1]) - &outFileName[1]);
	/* only accept a disk file or the bit bucket */
	if (spathInfo.deviceType != 3 && spathInfo.deviceId != 22)	/* 22 -> :BB: */
		ErrNotADisk();
	/* copy the disk name over and create the tmp file name */
	tmpFileInfo[0] = spathInfo.deviceId;
	MakeFullName((spath_t *)tmpFileInfo, &tmpFileName[1]);
	tmpFileName[0] = (byte)(PastFileName(&tmpFileName[1]) - &tmpFileName[1]);
	/* record if (we are creating a real file */
	outRealFile = spathInfo.deviceId != 22;
	/* Open() the file to locate */
	Open(&readfd, &inFileName[1], 1, 0, &statusIO);
	ErrChkReport(statusIO, &inFileName[1], true);
	recNum = 0;
	/* check we have a relocation file */
	GetRecord();
	if (inRecordP->rectyp != R_MODHDR)
		ErrChkReport(ERR239, &inFileName[1], true);	/* no module header record */
	/* assume listing to :CO: */
	PStrcpy(cout, printFileName);
	/* set other defaults and record that we haven't seen the commands */
	columns = 1;
	seen.start = 0;
	seen.stackSize = 0;
	seen.restart0 = 0;
	seen.map = 0;
	seen.publics = 0;
	seen.symbols = 0;
	seen.lines = 0;
	seen.purge = 0;
	seen.name = 0;
	isMain = 0;		/* assume not main module */
	modhdrX1 = 0;
	modhdrX2 = 0;
	startAddr = 0;
	/* initialise the segment order */
	InitSegOrder();
	/* and set all as not yet seen with 0 size and location */
	for (p.w = 0; p.w <= 255; p.w++) {
		segFlags[p.w] = AUNKNOWN;
		segBases[p.w] = segSizes[p.w] = 0;
	}
	/* process the rest of the command args */
	while (*cmdP != '\r') {
		ProcessControls();
	}
	/* no paging file yet and no Allocated() pages */
	havePagingFile = 0;
	roundRobinIndex = 0;
	pageIndexTmpFil = 0;
	/* recalculate pageCache size */
	pageCacheSize = (word)High(outRecordP - (pointer)(pageTab1P = (page1_t *)(pageTab2P + 1))) - 1;
	/* set a new baseMemImage after Allocting() pageTab1 slots - one for each page */
	baseMemImage =  ((pointer)pageTab1P) + (pageCacheSize + 1) * 2 ;
	/* initialise the paging table control information */
	for (p.w = 0; p.w <= 255; p.w++) {
		/* we have only Allocated() space for up to pageCacheSize entries in pageTab1 */
		if (p.w <= pageCacheSize)
			pageTab1P[p.w].state = 0xff; /* not used */
		pageTab2P[p.w].pageIdx = 0xff;	/* not used */
		pageTab2P[p.w].fileIdx = 0xff;	/* not used */
	}
	/* Allocate() the print buffer below the output buffer if (we have one */
	if (usePrintBuf)
		spbufP = pbufP = epbufP - npbuf;
	/* set up the heap locations below the print buffer */
	topHeap = topDataFrags = botHeap = spbufP;
	/* recalculate the pageCacheSize now available */
	pageCacheSize = (word)High(outRecordP - (pointer)(pageTab1P = ((page1_t *)(pageTab2P + 1)))) - 1;
	/* create the output file*/
	Delete(&outFileName[1], &statusIO);
	Open(&outputfd, &outFileName[1], 3, 0, &statusIO);
	ErrChkReport(statusIO, &outFileName[1], true);
	/* and the print file (or console) */
	Open(&printfd, &printFileName[1], 2, 0, &statusIO);
	ErrChkReport(statusIO, &printFileName[1], true);
	/* if (! console) emit the signon && command line to the print file */
	if (printfd > 0)
	{
		PrintString(signonMsg, 24);
		PrintString(version, 4);
		PrintString(aInvokedBy, 14);
		PrintString(scmdP, (word)(cmdP - scmdP) + 2);
	}
} /* ProcArgsInit */

byte aInpageSegment2[] = "INPAGE SEGMENT > 256 byteS COERCED TO PAGE BOUNDRY\r\n";
byte segId;
byte pad7935[3];

word AlignAddress(byte align, word size, word laddr)
{
	if (size == 0)	/* no size to no alignment needed */
		return laddr;

	if (align == ABYTE)
		return laddr;	/* no alignment needed */
	if (align == AINPAGE)
		if (size <= 256)	/* check if fits in page */
		{
			if (High(laddr) == High(laddr + size - 1))
				return laddr;
		}
		else    /* inpage seg > 256 coerced to page boundary */
			ConAndPrint(aInpageSegment2, 52);	
	return (laddr + 0xff) & 0xff00;	/* get a whole page */
} /* AlignAddress */


void ProcModhdr()
{
	word loadAddress, segSize, segOrderId;
	byte atTopOfMem, loadHasSize;

	if (! seen.name)	/* copy over the module name if ! user overridden */
		PStrcpy(inP, moduleName);
	inP = inP + inP[0] + 1;	/* past name */
	modhdrX1 = inP[0];		/* copy the reserved bytes */
	modhdrX2 = inP[1];
	inP = inP + 2;
	loadHasSize = 0;
	while (inP < erecP) {		/* process all of the seg info */
		if ((segFlags[segId = ((segdef_t *)inP)->segId] & AMASK) != AUNKNOWN)
			IllegalRecord();	/* oops seen twice */
		if (segId != SSTACK)		/* by default copy the length */
			segSizes[segId] = ((segdef_t *)inP)->len;
		else if (! seen.stackSize)	/* for stack copy if ! user overridden */
			segSizes[SSTACK] = ((segdef_t *)inP)->len;
		if (segSizes[segId] > 0)	/* check we have some data */
			loadHasSize = true;
		if (((segdef_t *)inP)->combine - 1 > ABYTE - 1)	/* check valid alignment */
			IllegalRecord();
		segFlags[segId] = segFlags[segId] + ((segdef_t *)inP)->combine;	/* set the combine info */
		inP += sizeof(segdef_t);	/* skip to next seg info */
	}
	if ((segFlags[SSTACK] & AMASK) == AUNKNOWN)		/* if STACK ! specified put default */
		segFlags[SSTACK] = segFlags[SSTACK] + ABYTE;	/* set to byte align */
	if ((segFlags[SMEMORY] & AMASK) == AUNKNOWN)	/* if MEMORY ! specified put default */
		segFlags[SMEMORY] = segFlags[SMEMORY] + ABYTE;	/* set to byte align */
	if (! seen.stackSize && loadHasSize)		/* it STACK ! specified by user && we have data */
		segSizes[SSTACK] = segSizes[SSTACK] + 12;	/* reserve an additional 12 bytes */
	loadAddress = 0x3680;				/* ISIS load address */
	atTopOfMem = 0;

	for (segOrderId = 1; segOrderId <= 254; segOrderId++) {	/* process the segs taking account of user specified order */
		segId = segOrder[segOrderId];
		segSize = segSizes[segId];	/* pick up seg to process */
		if ((segFlags[segId] & FHASADDR) == 0)	/* no address specified */
		{
			if (atTopOfMem != 0)	/* check for going over 64k */
			{
				if (segSize > 0)	/* program exceeds 64k */
					ErrChkReport(ERR240, &inFileName[1], true);
				segFlags[segId] |= FWRAP0;
			}
		}
		else
		{	/* user specified address */
			if (segSize > 0)	/* reset topOfMemflag */
				atTopOfMem = 0;
			loadAddress = segBases[segId];	/* set the loadAddress to user specified */
		}
		/* update this segments base address allowing for alignment */
		segBases[segId] = AlignAddress(segFlags[segId] & AMASK, segSize, loadAddress);
		if (segId == SMEMORY)
			if (segSize == 0 && loadHasSize)	/* if ! specified && program has size */
								/* calcualte a MEMORY size */
				if (atTopOfMem == 0 && MEMCK /*MemCk()*/ > segBases[SMEMORY])
					segSizes[SMEMORY] = segSize = MEMCK /*MemCk()*/ - segBases[SMEMORY];
		/* advance loadAddress and check for memory wrap around */
		if ((loadAddress = segBases[segId] + segSize) < segBases[segId])
			if (loadAddress == 0)		/* at 64k ok else Error() */
				atTopOfMem = FWRAP0;
			else
				ErrChkReport(ERR240, &inFileName[1], true);  /* exceeds 64k */
	} 
	segBases[SSTACK] = segBases[SSTACK] + segSizes[SSTACK];	/* stack goes down so update to top */
	GetRecord();	/* preload next record */
} /* ProcModhdr */


void ProcComdef()
{
	while (inP < erecP) {
		if ((segId = ((comnam_t *)inP)->segId) < SNAMED || segId == SBLANK)
			IllegalRecord();
		/* check if (combine value not appropriate for common or if this one alReady seen */
		if ((segFlags[segId] & AMASK) == AUNKNOWN || (segFlags[segId] & FSEGSEEN) != 0)
			IllegalRecord();
		segFlags[segId] |= FSEGSEEN;	/* flag as seen */
		inP = inP + 2 + ((comnam_t *)inP)->name[0];		/* past byte, len, string */
	}
	GetRecord();
} /* ProcComdef */


void ProcHdrAndComDef()
{
	ObjSeek(0, 0);	/* rewind */
	GetRecord();		/* get modhdr */
	ProcModhdr();	/* load the modhdr and set the segment bases */

	while (inRecordP->rectyp == R_COMDEF) {	/* process any comdefs */
		ProcComdef();
	}
	for (segId = SNAMED; segId <= SBLANK - 1; segId++) {	/* check commons in modhdr have comdef entries */
		if (((segFlags[segId] & AMASK) != AUNKNOWN) && ((segFlags[segId] & FSEGSEEN) == 0))
			BadRecordSeq();
	}
} /* ProcHdrAndComDef */

