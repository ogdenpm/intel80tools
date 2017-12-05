/*
    vim:ts=4:shiftwidth=4:expandtab:
*/
#include "lib.h"
#include <setjmp.h>



word actual;
word status;
static word outConn;
static pointer libOutFile;
word inConn;
pointer argFilename;
static arg_t *argHead;
static arg_t *argChain;
static module_t *moduleHead;
static module_t *moduleP;

symbol_t *hashTable[128];
static symbol_t *hashPtr;	/* probably Hash item */
//static word junk1 = {0};
//static byte junk2[4];
static word libModuleCnt;
static byte addCnt;
static byte moduleName[33];
static word zeroBlk = {0};
static word zeroByte = {0};
//static address junk3 = {10};
static word one = {1};
static byte copyBuf[1028];
static word recBlk;
static word recByt;
static byte outCRC;
static word outRecLen;

static byte copyright[] = "(C) 1976, 1977 INTEL CORP";
static byte initLibContent[] = {LIBHDR, 7, 0, 0, 0, 0, 0, 0xA, 0, 0xC3,
                                NAMES, 1, 0, 0xD7,
                                LIBLOC, 1, 0, 0xD9,
                                LIBDICT, 1, 0, 0xD5,
                                ENDFILE, 1, 0, 0xF1};

//label reset;
jmp_buf reset;		// C equivalent

static void WriteBuf(word count, pointer buf)
{
	word i;
	WriteFile(outConn, buf, count, &status);
	count = count - 1;	/* adjust for 0 based offset */
	for (i = 0; i <= count; i++) {
		outCRC = outCRC + buf[i];
	}
	outRecLen = outRecLen + 1 + count;
} /* WriteBuf */

static void OutRecInit(word recLen)
{
	outCRC = 0;
	/* record where start of record is */
	SeekFile(outConn, 0, &recBlk, &recByt, &status);
	/* Write() type and length */
	WriteBuf(1, &curRec.type);
	WriteBuf(2, (pointer)&recLen);
	outRecLen = 1;
}


static void WriteName(pointer name)
{

	WriteBuf(name[0] + 1, name);
}

static void WriteCRC()
{
	outCRC = -outCRC;
	WriteFile(outConn, &outCRC, 1, &status);
}


static void FixupLen()
{
	word savblk, savbyt;

	SeekFile(outConn, 0, &savblk, &savbyt, &status);	/* where we are */
	SeekFile(outConn, 2, &recBlk, &recByt, &status);	/* start of record */
	SeekFile(outConn, 3, &zeroByte, &one, &status);	/* offset of length */
	WriteBuf(2, (pointer)&outRecLen);				/* the 2 byte length */
	SeekFile(outConn, 2, &savblk, &savbyt, &status);	/* back to where we were */
}

static byte libTmp[] = ":  :LIB.TMP ";

static void CreateTmpFile(pointer path)
{

	libTmp[1] = 'F';	/* assume temp is on F0() */
	libTmp[2] = '0';
	path = SkipSpc(path);
	if (path[0] == ':')
	{
		libTmp[1] = path[1];
		libTmp[2] = path[2];
	}
	libOutFile = libTmp;
	Delete(libTmp, &status);
	OpenFile(&outConn, libTmp, 3, 0, &status);
}



static void ReplaceLib(pointer pathP)
{
	CloseFile(outConn, &status);
	CloseFile(inConn, &status);
	DeleteFile(pathP, &status);
	RenameFile(libOutFile, pathP, &status);
}


static void ModNotFound()
{
	pointer s;

	s = argChain->name;
	Log("  ", 2);	/* "  " */
	Log(s, (word)(PastFileName(SkipSpc(s)) - s));	/* emit the file name */
	Log("(", 1);	/* ( */
	Log(moduleP->name + 1, moduleP->name[0]);	/* and the module name */
	LogCRLF("): NOT FOUND", 12); /* ): NOT FOUND */
}


bool LookupModule()
{
	byte i;
    bool found;

	found = false;
	moduleP = (module_t *)&argChain->modules;	/* chase down the list of modules requested */
	while (moduleP->link != 0) {
 
		moduleP = moduleP->link;
		found = true;		/* assume found */
	
		/* compare names */	
		for (i = 0; i <= moduleName[0]; i++) {
			if (moduleName[i] != moduleP->name[i])  // include length byte
				found = false;
		}
		if (found)
			return found;
	}
	return found;
}




static void ChainModules()
{
	moduleHead = 0;
	moduleP = (module_t *)&moduleHead;
	argChain = (arg_t *)&argHead;

	while (argChain->link != 0) {
		argChain = argChain->link;
		moduleP->link = argChain->modules;
		/* Seek() to the end of the module list */
		while (moduleP->link != 0) {
			moduleP = moduleP->link;
		}
	}
}


static void GetFileAndModuleNames()
{
	byte i;

	GetToken();
	/* add another arg entry to the chain */
	argChain->link = (arg_t *)AllocUp(sizeof(arg_t));
	argChain = argChain->link;
	argChain->link = 0;
	moduleP = (module_t *)&argChain->modules;
	argChain->name = tokenP;
	if (lookAheadP[0] == '(' )	/* add the list of modules if present */
	{
		inModuleList = 0xFF;
		GetToken();

		while (tokenP[0] == '(' || tokenP[0] == ',') {
			inModuleList = 0;	/* already seen the file name, so we should get ( , or ) */
			GetToken();
			inModuleList = 0xFF;	/* assume we are going to get another module */
			moduleP->link = (module_t *)AllocUp(tokLen + sizeof(module_t));
			moduleP = moduleP->link;
			moduleP->link = 0;
			moduleP->found = 0;
			moduleP->name[0] = tokLen;
			/* add the module name */
			for (i = 0; i <= tokLen - 1; i++) {
				moduleP->name[i+1] = tokenP[i]; 	
			}
			GetToken();
		}
		if (tokenP[0] != ')')
			LibError(ERR228);	/* RIGHT PARENTHESIS EXPECTED */
	}
	moduleP->link = 0;	
	inModuleList = 0;
}


static pointer GetXField()
{
	static byte b;
	GetRecordBytes(1, &b);
	return &b;
}	


static word XFieldSize(pointer arg1)
{
	return 1;
}


static byte sub_3C80(pointer s, pointer t)
{

	return *s == *t;
}



static void sub_3C98(pointer s, pointer t)
{
	*t = *s;
}


static void sub_3CAD(pointer arg1w)
{
	WriteBuf(1, arg1w);
}

static void CopyRestRec()
{
	word cnt;
	while (curRec.bytesLeft > 1) {
		if (curRec.bytesLeft > 1028)
			cnt = 1028;
		else
			cnt = curRec.bytesLeft - 1;
		GetRecordBytes(cnt, copyBuf);
		WriteBuf(cnt, copyBuf);
	}
	ReadChkCrc();
	WriteCRC();
}


static void AddModule(byte addName)
{
	pointer xField;
	word offset;
	byte tmpByte;
	byte objNameLen;
//	byte junk[4];	/* safety net for GetRecordBytes */

	addCnt = addCnt + 1;
	OutRecInit(curRec.len);
	GetRecordBytes(1, &objNameLen);
	if (addName)
	{
		/* get to the end of the current list of modules */
		while (moduleP->link != 0) {
			moduleP = moduleP->link;
		}

		moduleP->link = (module_t *)AllocUp(objNameLen + sizeof(module_t));
		moduleP = moduleP->link;
		moduleP->link = 0;
		moduleP->found = true;
	}

	/* record location */
	moduleP->blk = recBlk;
	moduleP->byt = recByt;
	/* set the name */
	moduleP->name[0] = objNameLen;
	GetRecordBytes(objNameLen, moduleP->name + 1);
	WriteBuf(objNameLen + 1, moduleP->name);
	/* copy the header to the new file */
	CopyRestRec();
	PrepRecord();

	while (curRec.type != OBJEND) {	/* loop over all of this module */
		OutRecInit(curRec.len);
		if (curRec.type == PUBLICS)
		{
			GetRecordBytes(1, &tmpByte);	/* copy segid */
			WriteBuf(1, &tmpByte);

			while (curRec.bytesLeft > 1) {
				GetRecordBytes(2, (pointer)&offset); /* symbol entry offset */
				GetName(moduleName);
				xField = GetXField();
				/* check for duplicate public symbols */
				if (LookupSymbol(moduleName, &hashPtr))
					Fatal(ERR230);	/* DUPLICATE SYMBOL IN INPUT */

				/* create the symbol entry */
				hashPtr->link =  (symbol_t *)AllocUp(moduleName[0] + sizeof(symbol_t));	/* sizeof symbol entry */
				hashPtr = hashPtr->link;
				hashPtr->link = 0;
				hashPtr->curCmd = argFilename;
				hashPtr->moduleCnt = libModuleCnt;
				for (objNameLen = 0; objNameLen <= moduleName[0]; objNameLen++) {
					hashPtr->symbol[objNameLen] = moduleName[objNameLen];	
				}

				/* Write() the offset, name and xField */
				WriteBuf(2, (pointer)&offset);
				WriteBuf(moduleName[0]+1, moduleName);
				WriteBuf(XFieldSize(xField), xField);
			}
		}
		/* copy the rest of the record and prep for the next loop */
		CopyRestRec();
		PrepRecord();
	}	/* of while */
	
	/* copy the objend record */
	OutRecInit(curRec.len);
	CopyRestRec();
	/* keep tally for the new library */
	libModuleCnt = libModuleCnt + 1;
}


void FinaliseLib(pointer buf)
{
	/* Write() the end of file marker */
	curRec.type = ENDFILE;
	OutRecInit(1);
	WriteCRC();
	/* rewind the output file */
	SeekFile(outConn, 2, &zeroByte, &zeroByte, &status);
	LibSeek(2, &zeroBlk);	/* rewind the input */
	PrepRecord();		/* pick up the original lib header */
	OutRecInit(curRec.len);	/* Write() the start out */
	WriteBuf(2, (pointer)&libModuleCnt); /* Write() the module count */
	WriteBuf(4, buf);		/* Write() the module offset */
	WriteCRC();			/* and CRC */
}

/* utility procedure to copy data from in to out */
static void CopyFileBytes(word cnt)
{
    ReadFile(inConn, copyBuf, cnt, &actual, &status);
    if (actual < cnt)
        Fatal(ERR204);	/* PREMATURE EOF */

    WriteFile(outConn, copyBuf, cnt, &status);
}


static void AddCmd()
{
	word blkIdx;
	word byteIdx;
    libhdr_t addLibHdr;
	libhdr_t oldLibHdr;
	word inLibConn;
//    address junk1;
	word j;
    pointer libFileName;
//    address junk2;
    byte chunk;
    byte found;


	byte i;
	word nameIdx;
	word addBlk;
	word addByt;
//    byte junk3[4];
    byte locFields[4];	/* used to copy the location data over */
    static word group;
    byte k;
	word n;



	GetFileAndModuleNames();

	/* loop if (more */
	while (lookAheadP[0] == ',') {
		GetToken();
		GetFileAndModuleNames();
	}

	/* need a TO */
	if (! MatchLookAhead("TO",2) )	/* "TO" */
		LibError(ERR233);	/* 'TO' EXPECTED */

	GetToken();	/* waste the TO */
	libFileName = lookAheadP;	/* pick up filename to create */
	GetToken();
	if (lookAheadP[0] != CR )	/* check this is the end */
		LibError(ERR203);	/* INVALID SYNTAX */

	CreateTmpFile(libFileName);
	/* Open() the existing file */
	OpenFile(&inLibConn, libFileName, readOnly, 0, &status);
	inConn = inLibConn;
	argFilename = libFileName;
	curRec.bytesLeft = 0;
	AcceptRecord(LIBHDR);
	GetRecordBytes(6, (pointer)&oldLibHdr);
	ReadChkCrc();
	libModuleCnt = oldLibHdr.modCnt;
	blkIdx = byteIdx = 0;
	LibSeek(2, &zeroBlk);	/* rewind */
	/*
	   copy all the existing module over i.e. until the NAMES record
	   the algorithm copies up to 1024 bytes at a time shrinking to smaller
	   blocks if (there are less bytes
	*/
	chunk = 8;
	while (chunk > 0) {		/* continue while there are blocks */
		if (oldLibHdr.idxBlk - blkIdx >= chunk)
		{
			CopyFileBytes(chunk * 128);
			blkIdx = blkIdx + chunk;
		}
		else
			chunk >>= 1;	/* shrink the chunk size */
	}

	CopyFileBytes(oldLibHdr.idxByte); /* copy remaining bytes */

	AcceptRecord(NAMES);
	addCnt = 0;

	/* scan all of the files to add */
	argChain = (arg_t *)&argHead;
	while (argChain->link != 0) {
		argChain = argChain->link;
		argFilename = argChain->name;
		OpenLib();
		PrepRecord();
		if (curRec.type == LIBHDR )	/* library to add? */
		{
			GetRecordBytes(6, (pointer)&addLibHdr);	/* load the 'add file' module header */
			ReadChkCrc();	/* check ok */
			if (argChain->modules == 0 ) /* adding all of the library */
			{
				moduleP = (module_t *)&argChain->modules;
				PrepRecord();
				while (curRec.type == OBJHDR) {
					AddModule(0xFF);
					PrepRecord();
				}
			}
			else
			{
				nameIdx = 0;
				/* find the names record */
				LibSeek(2, &addLibHdr.idxBlk);
				AcceptRecord(NAMES);

				while (curRec.bytesLeft > 1) {	/* process the names record */
					GetName(moduleName);
					
					found = 0;
					moduleP = (module_t *)&argChain->modules;
					while (moduleP->link != 0 && !found) {
						found = 0xFF;
						moduleP = moduleP->link;
						for (i = 0; i <= moduleName[0]; i++) {
							if (moduleName[i] != moduleP->name[i])
								found = 0;
						}
						moduleP->found = moduleP->found || found;
					}

					if (found )	/* record the index into the locations */
						moduleP->blk = nameIdx;
					nameIdx = nameIdx + 1;
				}

				AcceptRecord(LIBLOC);
				SaveCurPos();	/* position of the locations block/byte number */
				moduleP = (module_t *)&argChain->modules;

				while (moduleP->link != 0) {
					moduleP = moduleP->link;
					if (! moduleP->found)
						ModNotFound();
					else
					{
						/* convert the name index into a block / byte offset */
						addByt = moduleP->blk * 4;
						addBlk = (moduleP->blk >> 14) << 9;
						RestoreCurPos();	/* get to start of locations */
						LibSeek(3, &addBlk);	/* index into them */
						GetRecordBytes(4, (pointer)&moduleP->blk);	/* set the real block/byte */
					}
				}

				/* scan over the modules */
				moduleP = (module_t *)&argChain->modules;
				while (moduleP->link != 0) {
					moduleP = moduleP->link;
					if (moduleP->found)
					{
						/* locate the OBJHDR */
						LibSeek(2, &moduleP->blk);
						AcceptRecord(OBJHDR);
						AddModule(0);
					}
				}
			}
		}
		else if (curRec.type == OBJHDR )	/* standard object file to add */
		{
			if (argChain->modules != 0 )	/* user thought it was a library */
				Fatal(ERR235);	/* NOT LIBRARY */

			moduleP = (module_t *)&argChain->modules;

			while (curRec.type == OBJHDR) {
				AddModule(0xFF);
				PrepRecord();
			}
			if (curRec.type != ENDFILE)
				Fatal(ERR224);	/* BAD RECORD SEQUENCE */
		}
		else
			Fatal(ERR224);	/* BAD RECORD SEQUENCE */

		CloseFile(inConn, &status);
	}

	ChainModules();
	inConn = inLibConn;
	argFilename = libFileName;
	curRec.bytesLeft = 0;
	/* locate the old library NAMES record */
	LibSeek(2, &oldLibHdr.idxBlk);
	AcceptRecord(NAMES);
	OutRecInit(0);

	/* Read() in the names */
	while (curRec.bytesLeft > 1) {
		GetName(moduleName);
		WriteBuf(moduleName[0] + 1, moduleName);
		moduleP = (module_t *)&moduleHead;
		/* check there is not a duplicate module */
		while (moduleP->link != 0) {
			moduleP = moduleP->link;
			for (j = 0; j <= moduleName[0]; j++) {
				if (moduleName[j] != moduleP->name[j])
					goto outerloop;
			}
			Log(&moduleName[1], moduleName[0]);
			LogCRLF(" - ATTEMPT TO ADD DUPLICATE MODULE NAME", 0x22);
			longjmp(reset, 1);
		outerloop:
			;
		}
	}

	/* scan the modules end emit all of the module names needed */

	moduleP = (module_t *)&moduleHead;

	while (moduleP->link != 0) {
		moduleP = moduleP->link;
		if (moduleP->found)
			WriteBuf(moduleP->name[0] + 1, moduleP->name);
	}

	FixupLen();	/* fix the names record length & Write() CRC */
	WriteCRC();
	/* record in the header where the NAMES record is located */
	oldLibHdr.idxBlk = recBlk;
	oldLibHdr.idxByte = recByt;
	/* prep the lib locations record */
	AcceptRecord(LIBLOC);
	OutRecInit(libModuleCnt * 4 + 1);

	/* copy the original library locations information over */
	while (curRec.bytesLeft > 1) {
		GetRecordBytes(4, locFields);
		WriteBuf(4, locFields);
	}

	/* now add the new locations */
	moduleP = (module_t *)&moduleHead;

	while (moduleP->link != 0) {
		moduleP = moduleP->link;
		if (moduleP->found)
			WriteBuf(4, (pointer)&moduleP->blk);

	}

	WriteCRC();

	AcceptRecord(LIBDICT);
	OutRecInit(0);

	/* copy the existing dictionary checking that the new publics added don't
	   already exist
	*/
	while (curRec.bytesLeft > 1) {
		GetName(moduleName);
		if (moduleName[0] == 0)
			group = group + 1;
		else if (LookupSymbol(moduleName, &hashPtr))
		{
			Log(&moduleName[1], moduleName[0]);
			LogCRLF(" - SYMBOL ALREADY IN LIBRARY", 0x23);
			longjmp(reset, 1);
		}

		WriteName(moduleName);
	}

	/* if (there are modules ) add any new ones */
	if (libModuleCnt != 0)
		for (n = oldLibHdr.modCnt; n <= libModuleCnt - 1; n++) {
			/* search the hashtable */
			for (k = 0; k <= 127; k++) {
				hashPtr = (symbol_t *)&hashTable[k];
				while (hashPtr->link != 0) {
					hashPtr = hashPtr->link;
					/* is this the correct module number */
					if (hashPtr->moduleCnt == n)
						WriteBuf(hashPtr->symbol[0] + 1, hashPtr->symbol);
				}
			}
			/* Write() the module separator */
			WriteBuf(1, (pointer)&zeroByte);
		}

	SkipCurRec(); /* ignore the rest of the LIBDICT in the original */
	FixupLen();	/* fixup the LIBDICT len */
	WriteCRC();	/* and make the CRC work */
	FinaliseLib((pointer)&oldLibHdr.idxBlk);
	ReplaceLib(libFileName);
}




static void DeleteCmd()
{
    libhdr_t tmpLibHdr;
//    address junk_6429;
    byte nothingToDelete;
//	address junk_642C;
	module_t *delmodP;

	GetFileAndModuleNames();
	if (lookAheadP[0] != CR)
		LibError(ERR203);	/* INVALID SYNTAX */

	if (argChain->modules == 0 )		/* nothing to do !! */
		LibError(ERR203);	/* INVALID SYNTAX */

	argFilename = argChain->name;
	OpenLib();
	AcceptRecord(LIBHDR);
	GetRecordBytes(6, (pointer)&tmpLibHdr);	/* Read() in the existing header */
	ReadChkCrc();			/* check not corrupt */
	LibSeek(2, &tmpLibHdr.idxBlk);	/* Seek() to names module */
	AcceptRecord(NAMES);

	delmodP = (module_t *)&moduleHead;
	while (curRec.bytesLeft > 1) {		/* pull in all the names */
		GetName(moduleName);
		delmodP->link = (module_t *)AllocUp(sizeof(module_t)-1);		/* allocate a module entry without name */
		delmodP = delmodP->link;
		if (delmodP->found = LookupModule())	/* set the flag is this is one to Delete() */
			moduleP->found = true;		/* and flagged as found in the user list */
		else
			libModuleCnt = libModuleCnt + 1;

	}
	delmodP->link = 0;
	ReadChkCrc();		/* check for good OMF */

	nothingToDelete = 0xFF;
	moduleP = (module_t *)&argChain->modules;

	while (moduleP->link != 0) {	/* check we found at least one of the user's modules */
		moduleP = moduleP->link;
		if (! moduleP->found)
			ModNotFound();
		else
			nothingToDelete = 0;
	}

	if (nothingToDelete)
		longjmp(reset, 1);

	CreateTmpFile(argFilename);
	LibSeek(2, &zeroBlk);	/* rewind library */
	AcceptRecord(LIBHDR);
	OutRecInit(curRec.len);	/* Write() the LIBHDR type and record length */
	CopyRestRec();		/* copy the rest of the record */

	/* copy the objmodules unless Delete() requested */
	delmodP = (module_t *)&moduleHead;
	while (delmodP->link != 0) {
		delmodP = delmodP->link;
		AcceptRecord(OBJHDR);
		if (delmodP->found )	/* one to Delete() */
			SkipModule();
		else			
		{			/* record where this module is going */
			SeekFile(outConn, 0, &delmodP->blk, &delmodP->byt, &status);

			/* copy until we get an end of module */
			while (curRec.type != OBJEND) {
				OutRecInit(curRec.len);
				CopyRestRec();
				PrepRecord();
			}
			/* copy the OBJEND record */
			OutRecInit(curRec.len);
			CopyRestRec();
		}
	}

	/* copy only the none deleted NAME record entries */
	delmodP = (module_t *)&moduleHead;
	AcceptRecord(NAMES);
	OutRecInit(0);

	while (delmodP->link != 0) {
		delmodP = delmodP->link;
		GetName(moduleName);
		if (! delmodP->found)
			WriteName(moduleName);
	}

	FixupLen();
	WriteCRC();
	tmpLibHdr.idxBlk = recBlk;	/* save where the NAMES record is */
	tmpLibHdr.idxByte = recByt;

	/* scan included modules writing the locations */
	delmodP = (module_t *)&moduleHead;
	AcceptRecord(LIBLOC);
	OutRecInit(0);

	while (delmodP->link != 0) {
		delmodP = delmodP->link;
		if (! delmodP->found)
			WriteBuf(4, (pointer)&delmodP->blk);
	}

	FixupLen();
	WriteCRC();

	/* scan included modules writing the dictionary */
	delmodP = (module_t *)&moduleHead;
	AcceptRecord(LIBDICT);
	OutRecInit(0);

	while (delmodP->link != 0) {
		delmodP = delmodP->link;
		GetName(moduleName);
		
		while (moduleName[0] != 0) {	/* copy the module names if (needed */
			if (! delmodP->found)
				WriteName(moduleName);

			GetName(moduleName);
		}

		/* terminate with a 0 if (the group is included */
		if (! delmodP->found)
			WriteBuf(1, (pointer)&zeroByte);

	}

	FixupLen();
	WriteCRC();
	FinaliseLib((pointer)&tmpLibHdr.idxBlk);	/* endfile & header update */
	ReplaceLib(argFilename);		/* replace the library */
}


static void CreateCmd()
{

	GetToken();
	if (lookAheadP[0] != CR )		/* should have no other data */
		LibError(ERR203);	/* INVALID SYNTAX */

	argFilename = tokenP;	/* try to Open() library */
	Open(&inConn, argFilename, readOnly, 0, &status);
	if (status == 0 )	/* if no Error() then already exists so error */
	{
		CloseFile(inConn, &status);
		FileStatusChk(0xE7, argFilename, 0xFF);	/* Error() 231 not defined !!! */
								/* should be FILE ALREADY EXISTS */
	}
	CloseFile(inConn, &status);
	OpenFile(&outConn, argFilename, writeOnly, 0, &status);
	/* initialise with a minimal content */
	WriteFile(outConn, initLibContent, 26, &status);
	CloseFile(outConn, &status);
}

static void OutName(byte leadSpaces, pointer nameP)
{
    WriteFile(outConn, "      ", leadSpaces, &status);
    WriteFile(outConn, nameP + 1, nameP[0], &status);
    WriteFile(outConn, "\r\n", 2, &status);	/* crlf */
}

static void ListCmd()
{
	bool listPublics;
    bool listThisModule;
    libhdr_t newLibHdr;




	/* collect the users list of libraries */
	GetFileAndModuleNames();		/* add the name to the chain and get any module list */
	while (lookAheadP[0] == ',') {
		GetToken();
		GetFileAndModuleNames();
	}

	libOutFile = ":CO: ";		/* assume console output */
	if (MatchLookAhead("TO", 2))
	{
		GetToken(); 	/* get the filename  to lookAhead */
		GetToken(); 	/* and again to get to token */
		libOutFile = tokenP;
	}

	GetToken();	/* lookAhead to token */
	if ((listPublics = MatchToken("PUBLICS", 7)))
		GetToken();

	if (tokenP[0] != CR )	/* should be it */
		LibError(ERR203);	/* INVALID SYNTAX */

	OpenFile(&outConn, libOutFile, 2, 0, &status);
	argChain = (arg_t *)&argHead;

	while (((argChain = argChain->link) != 0)) {	/* loop through the files */
		argFilename = argChain->name;
		OpenLib();
		/* header line "  file\n\n" */
		WriteFile(outConn, "  ", 2, &status);
		WriteFile(outConn, argFilename, (word)(PastFileName(argFilename) - argFilename), &status);
		WriteFile(outConn, "\r\n", 2, &status);

		AcceptRecord(LIBHDR);
		GetRecordBytes(6, (pointer)&newLibHdr);	/* Read() module count, block number and byte number */
		ReadChkCrc();			/* check not corrupt */
		LibSeek(2, &newLibHdr.idxBlk);	/* Seek() to the names */
		AcceptRecord(NAMES);		/* check we get the names */
		if (listPublics)
		{
			SaveCurPos();		/* save where we are */
			AcceptRecord(LIBLOC);	/* check locations and dictionary exist */
			AcceptRecord(LIBDICT);
			SwapCurSavedRec();
		}

		while (curRec.bytesLeft > 1) {		/* scan the names until the CRC */
			GetName(moduleName);	/* get the name */
			if (argChain->modules == 0 )		/* listing all ? */
				listThisModule = true;
			else
				listThisModule = LookupModule();	/* no so check if (needed */

			if (listThisModule )		/* list name with 4 leading spaces */
				OutName(4, moduleName);

			if (listPublics)
			{
				SwapCurSavedRec();	/* get to dictionary current location*/
				GetName(moduleName);

				while (moduleName[0] != 0) {	/* scan the names until the end of the group */
					if (listThisModule )	/* emit name with 6 leading spaces if required */
						OutName(6, moduleName);

					GetName(moduleName);
				}
				SwapCurSavedRec();	/* back to where we were and save the current dictionary location */
			}
		}
		CloseFile(inConn, &status);	/* Close() the library */
	}
	/* Close() the listing file */
	CloseFile(outConn, &status);
}



static byte i;

void Start()
{


		Write(0, "ISIS-II LIBRARIAN V2.1\r\n", 24, &status);
		Read(1, MEMORY, 122, &actual, &status); 	/* flush cmdline */

		while (1) {
			if (setjmp(reset) == 0)
				break;
// reset :
			fileP = (file_t *)&fileHead;

			while (((fileP = fileP->link) != 0)) {
				if (fileP->isopen)
					Close(fileP->conn, &status);
			}
		}
//main:
	while (1) {
		libModuleCnt = 0;		/* reset key vars and memory usage */
		heapTop = MEMORY;
		memTop = MemCk();
		fileP = (file_t *)&fileHead;
		fileP->link = 0;
		argChain = (arg_t *)&argHead;
		argChain->link = 0;
		moduleHead = 0;
		moduleP = (module_t *)&moduleHead;
		for (i = 0; i <= 127; i++) {
			hashTable[i] = 0;
		}

		Write(0, "*", 1, &status);
		GetCmd();
		if (MatchToken("EXIT", 4))
			Exit();
		else if (MatchToken("ADD", 3))
			AddCmd();
		else if (MatchToken("CREATE", 6))
			CreateCmd();
		else if (MatchToken("LIST", 4))
			ListCmd();
		else if (MatchToken("DELETE", 6))
			DeleteCmd();
		else if (tokenP[0] == CR)
			;
		else
			WriteErrStr(ERR232);	/* UNRECOGNIZED COMMAND */
	}

}

