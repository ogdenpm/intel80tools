// vim:ts=4:expandtab:shiftwidth=4:
//
#include "asm80.h"

static byte fixupInitialLen[] = {1, 2, 1, 3};
static wpointer fixupRecLenPtrs[] = {&rPublics.len, &rInterseg.len, &rExtref.len, &rContent.len};
static byte fixupRecLenChks[] = {123, 58, 57, 124};
static byte b6D7E[] = {10, 0x12, 0x40}; /* 11 bits 00010010010 index left to right */


static modhdr_t rModhdr = {2};
static pointer dtaP, recSymP;

void WriteRec(pointer recP)
{
    pointer lenP;
    word recLen;
    byte i, crc;

    lenP = recP + 1;		// point to the length word
    recLen = ++*(wpointer)lenP + 3;    /* include crc byte + type + len word */
    crc = 0;            /* crc */
    lenP--;
    for (i = 2; i <= recLen; i++)
        crc -= *lenP++;
    *lenP = crc;            /* insert crc byte */
    Write(objfd, recP, recLen, &statusIO);
    IoErrChk();
}


static byte GetFixupType()
{
    byte attr;
    if (((attr = tokenAttr[spIdx]) & 0x5F) == 0)
        return 3;
    if ((attr & UF_EXTRN) != 0)    /* external */
        return 2;
    if ((fixupSeg = attr & UF_SEGMASK) == 0)    /* absolute */
        return 3;
    return (fixupSeg != activeSeg);
}


void ReinitFixupRecs()
{
    byte i;
    wpointer dtaP;	// to check for conflicts with global dtaP usage in plm

    for (i = 0; i <= 3; i++) {
        ii = (i - 1) & 3; /* order as content, publics, interseg, externals */
        dtaP = fixupRecLenPtrs[ii];
        if (*dtaP > fixupInitialLen[ii])
            WriteRec((pointer)dtaP - 1);

        *dtaP = fixupInitialLen[ii];
        fixIdxs[ii] = 0;
        if (curFixupType !=  ii)
            initFixupReq[ii] = true;
    }
    rContent.offset = itemOffset + segLocation[rContent.segid = activeSeg];
    rPublics.segid = curFixupHiLoSegId;
    rInterseg.segid = tokenAttr[spIdx] & 7;
    rInterseg.hilo = rExtref.hilo = curFixupHiLoSegId;
}



static void AddFixupRec()
{
    word effectiveOffset;
    wpointer dtaP;				// to check doesn't conflict with plm global usage 

    dtaP = fixupRecLenPtrs[curFixupType = GetFixupType()];
    if (*dtaP > fixupRecLenChks[curFixupType] || rContent.len + tokenSize[spIdx] > 124)
        ReinitFixupRecs();

    if (firstContent) {
        firstContent = false;
        rContent.offset = segLocation[rContent.segid = activeSeg] + itemOffset;
    } else {
        effectiveOffset = rContent.offset + fix6Idx;		// lifted out to make sure it is calculated
        if (rContent.segid != activeSeg || effectiveOffset != segLocation[activeSeg] + itemOffset || effectiveOffset < rContent.offset)
            ReinitFixupRecs();
    }

    switch(curFixupType) {
    case 0:
            if (initFixupReq[0]) {
                initFixupReq[0] = false;
                rPublics.segid = curFixupHiLoSegId;
            } else if (rPublics.segid != curFixupHiLoSegId)
                ReinitFixupRecs();
        break;
    case 1:
            if (initFixupReq[1]) {
                initFixupReq[1] = false;
                rInterseg.segid = tokenAttr[spIdx] & 7;
                rInterseg.hilo = curFixupHiLoSegId;
            } else if (rInterseg.hilo != curFixupHiLoSegId || (tokenAttr[spIdx] & 7) != rInterseg.segid)
                ReinitFixupRecs();
        break;
    case 2:
            if (initFixupReq[2]) {
                initFixupReq[2] = false;
                rExtref.hilo = curFixupHiLoSegId;
            } else if (rExtref.hilo != curFixupHiLoSegId)
                ReinitFixupRecs();
    break;
    case 3:	break;	        /* abs no fixup */
    }
}


static void RecAddContentBytes()
{
    for (byte i = 1; i <= tokenSize[spIdx]; i++)
        rContent.dta[fix6Idx++] = *contentBytePtr++;

    rContent.len = rContent.len + tokenSize[spIdx];
}



static void IntraSegFix()
{
    rReloc.len = rReloc.len + 2;
    rReloc.dta[fix22Idx++] = fixOffset;
}


static void InterSegFix()
{
    rInterseg.len += 2;
    rInterseg.dta[fix24Idx++] = fixOffset;
}

static void ExternalFix()
{
    rExtref.dta[fix20Idx++] = tokenSymId[spIdx];
    rExtref.dta[fix20Idx++] = fixOffset;
    rExtref.len += 4;
}

static void Sub7131()
{
    curFixupHiLoSegId = (tokenAttr[spIdx] & 0x18) >> 3;
    fixOffset = segLocation[activeSeg] + itemOffset;
    if (! (inDB || inDW) && (tokenSize[spIdx] == 2 || tokenSize[spIdx] == 3))
        fixOffset++;
    AddFixupRec();
    contentBytePtr = startItem;
    RecAddContentBytes();
    switch (GetFixupType()) {
    case 0:	IntraSegFix(); break;
    case 1: InterSegFix(); break;
    case 2: ExternalFix(); break;
    case 3:	break;					/* no fixup as absolute */
    }
}


void WriteExtName()
{
    byte i;

    if (rExtnames.len + 9 > 125) {   /* check room for name */
        WriteRec((pointer)&rExtnames);    /* flush existing extNam Record() */
        rExtnames.type = OMF_EXTNAMES;
        rExtnames.len = 0;
        extNamIdx = 0;
    }
    rExtnames.len += nameLen + 2;    /* update length for this ref */
    rExtnames.dta[extNamIdx] = nameLen;        /* Write() len */
    extNamIdx++;
    for (i = 0; i <= nameLen; i++)            /* and name */
        rExtnames.dta[extNamIdx + i] = name[i];

    rExtnames.dta[extNamIdx + nameLen] = 0;    /* and terminating 0 */
    extNamIdx += nameLen + 1;    /* update where next ref writes */
}



void AddSymbol()
{

    if ((tokenSym.curP->flags & UF_EXTRN) != 0)
        return;

    *(wpointer)recSymP = tokenSym.curP->offset;
    UnpackToken(tokenSym.curP->tok, (dtaP = (recSymP += 2) + 1));
    dtaP[6] = ' ';    /* trailing space to ensure end */
    *recSymP = 0;	  /* length of symbol */

    while (dtaP[0] != ' ') {    /* find *recSymPgth of name */
        ++*recSymP;
        dtaP++;
    }
    dtaP[0] = 0;            /* terminate name with 0 */
    recSymP = dtaP + 1;
}

void FlushSymRec(byte segId, byte isPublic)			/* args to because procedure is no longer nested */
{
    if ((rPublics.len = (word)(recSymP - &rPublics.segid)) > 1)    /* something to Write() */
        WriteRec((pointer)&rPublics);
    rPublics.type = isPublic ? OMF_PUBLICS : OMF_LOCALS;        /* PUBLIC or DoLocal */
    rPublics.segid = segId;
    recSymP = rPublics.dta;
}

static void WriteSymbols(byte isPublic)	/* isPublic= true -> PUBLICs else LOCALs */
{
    byte segId;

    recSymP = rPublics.dta;
    for (segId = 0; segId <= 4; segId++) {
        FlushSymRec(segId, isPublic);    /* also sets up segid for new record */
        tokenSym.curP = symTab[TID_SYMBOL] - 1;        /* point to type byte of user symbol (-1) */

        while (++tokenSym.curP < endSymTab[TID_SYMBOL]) {	// converted for C pointer arithmetic */
            if (recSymP > &rPublics.crc - (MAXSYMSIZE + 4)) /* make sure there is room offset, len, symbol, 0 */
                FlushSymRec(segId, isPublic);

            if ((tokenSym.curP->flags & UF_SEGMASK) == segId
               && tokenSym.curP->type != T_MACRONAME && NonHiddenSymbol()
               && !TestBit(tokenSym.curP->type, b6D7E) &&		// not O_LABEL, O_REF or O_NAME
               (! isPublic || (tokenSym.curP->flags & UF_PUBLIC) != 0))
                   AddSymbol();
        }
        FlushSymRec(segId, isPublic);
    }
}



void WriteModhdr()
{
    byte i;

    /* fill the module name */
    memcpy(rModhdr.dta + 1, aModulePage, (rModhdr.dta[0] = moduleNameLen));
    dtaP = &rModhdr.dta[moduleNameLen + 1];
    *(wpointer)dtaP = 0;    /* the trnId & trnVn bytes */
    dtaP++;					/* past trnId byte */
    if (segLocation[SEG_CODE] < maxSegSize[SEG_CODE])    /* code segment */
        segLocation[SEG_CODE] = maxSegSize[SEG_CODE];
    if (segLocation[SEG_DATA] < maxSegSize[SEG_DATA])    /* data segment */
        segLocation[SEG_DATA] = maxSegSize[SEG_DATA];

    for (i = 1; i <= 4; i++) {
        *++dtaP = i;        /* seg id */
        *(word *)(++dtaP) = segLocation[i];    /* seg size */
        dtaP += 2;
        *dtaP = alignTypes[i - 1];    /* aln typ */
    }
    rModhdr.len = moduleNameLen + (2 + 4 * 4 + 1);    /* set record length (trnId/trnVn 4 * (segid, segLocation, align), crc) */
    WriteRec((pointer)&rModhdr);
}

void WriteModend()
{
    rModend.modtyp = startDefined;
    rModend.segid = startSeg;
    rModend.offset = startOffset;
    WriteRec((pointer)&rModend);
    *(byte *)&rEof.len = 0;
    WriteRec((pointer)&rEof);
}

void Ovl8()
{
    itemOffset = 0;
    tokI = 1;
    spIdx = 1;
    if (b6B33)
        ;
    else
    while (spIdx != 0) {
        spIdx = NxtTokI();
        endItem = tokStart[spIdx] + tokenSize[spIdx];
        startItem = tokStart[spIdx];
        if (IsSkipping() || !isInstr)
            endItem = startItem;
        if (endItem > startItem) {
            Sub7131();
            itemOffset = itemOffset + tokenSize[spIdx];
        }
        if (!(inDB || inDW))
            spIdx = 0;
    }
}


void Ovl11()
{
    if (externId != 0) {
        Seek(objfd, SEEKABS, &azero, &azero, &statusIO);    /* rewind */
        WriteModhdr();
        Seek(objfd, SEEKEND, &azero, &azero, &statusIO);    /* back to end */
    }
    rPublics.type = OMF_PUBLICS;          /* public declarations record */
    rPublics.len = 1;
    rPublics.segid = SEG_ABS;
    rPublics.dta[0] = 0;
    WriteSymbols(true);      /* EMIT PUBLICS */
    if (controls.debug)
        WriteSymbols(false); /* EMIT LOCALS */
}
