asxref: {

#define	CR	0xD
#define	LF	0xA
#define	FF	0xC
#define	TRUE	0xff
#define	FALSE	0
#define	bool	byte
#define	READ_MODE	1
#define	WRITE_MODE	2
#define	UPDATE_MODE	3
#define	SEEKEND	4
#define	TYPEREF	1
#define	TYPEDEF	0
#define	TYPEFILE	2
#define	TYPEEOF	3

static byte cpyrite[] = {'[C] 1976, 1977,1979 INTEL CORP', 2, 1};
static byte xrefComplete[] = {'CROSS REFERENCE COMPLETE'};
static byte headerMsg[] = {CR, LF,
        'ISIS-II ASSEMBLER SYMBOL CROSS REFERENCE, V2.1                     PAGE ');
inputError(*) byte data('INPUT FILE FORMAT ERROR');
noMemory(*) byte data('NOT ENOUGH MEMORY');
isis address data(0x40);
maxMem address at(4);

errStrs(*) byte data(
        0, 0,
        2, 'ILLEGAL AFTN ARGUMENT', 0,
        4, 'INCORRECTLY SPECIFIED FILE', 0,
        5, 'UNRECOGNIZED DEVICE NAME', 0,
        6, 'ATTEMPT TO WRITE TO INPUT DEVICE', 0,
        8, 'ATTEMPT TO READ FROM OUTPUT DEVICE', 0,
        0xA, 'NOT ON SAME DISK', 0,
        0xB, 'FILE ALREADY EXISTS', 0,
        0xC, 'FILE IS ALREADY OPEN', 0,
        0xD, 'NO SUCH FILE', 0,
        0xE, 'WRITE PROTECTED', 0,
        0x11, 'NOT A DISK FILE', 0,
        0x12, 'ILLEGAL ISIS COMMAND', 0,
        0x13, 'ATTEMPTED SEEK ON NON-DISK FILE', 0,
        0x14, 'ATTEMPTED BACK SEEK TOO FAR', 0,
        0x15, 'CAN''T RESCAN', 0,
        0x16, 'ILLEGAL ACCESS MODE TO OPEN', 0,
        0x17, 'MISSING FILENAME', 0,
        0x19, 'ILLEGAL ECHO FILE', 0,
        0x1A, 'ILLEGAL ATTRIBUTE IDENTIFIER', 0,
        0x1B, 'ILLEGAL SEEK COMMAND', 0,
        0x1C, 'MISSING EXTENSION', 0,
        0x1F, 'CAN''T SEEK ON WRITE ONLY FILE', 0,
        0x20, 'CAN''T DELETE OPEN FILE', 0,
        0x23, 'SEEK PAST EOF', 0,
        0xC9, 'UNRECOGNIZED SWITCH', 0,
        0xCA, 'UNRECOGNIZED DELIMITER', 0,
        0xCB, 'INVALID SYNTAX', 0,
        0xCC, 'PREMATURE EOF', 0,
        0xCE, 'ILLEGAL DISKETTE LABEL', 0,
        0xD0, 'CHECKSUM ERROR', 0,
        0xD1, 'RELO FILE SEQUENCE ERROR', 0,
        0xD2, 'INSUFFICIENT MEMORY', 0,
        0xD3, 'RECORD TOO LONG', 0,
        0xD4, 'ILLEGAL RELO RECORD', 0,
        0xD5, 'FIXUP BOUNDS ERROR', 0,
        0xD6, 'ILLEGAL SUBMIT PARAMETER', 0,
        0xD7, 'ARGUMENT TOO LONG', 0,
        0xD8, 'TOO MANY PARAMETERS', 0,
        0xD9, 'OBJECT RECORD TOO SHORT', 0,
        0xDA, 'ILLEGAL RECORD FORMAT', 0,
        0xDB, 'PHASE ERROR', 0);





    topLowHeap address,
    startMarker address,
    tmpAFTN address,
    topHighHeap address, drive based topHighHeap byte,
    connP address,
    itemCount address,
    botHighHeap address,
    status address,
    outputLine(256) byte,
    col byte,
    path(15) byte,
    row byte,
    pageLength byte,
    pageWidth byte,
    recordType byte,
    haveFileInfo bool,
    paging bool,
    aF0AsxrefTmp(*) byte initial(':F0:ASXREF.TMP ');


ISIS_OPEN    literally    '0',
    ISIS_CLOSE    literally    '1',
    ISIS_DELETE    literally    '2',
    ISIS_READ    literally    '3',
    ISIS_WRITE    literally    '4',
    ISIS_SEEK    literally    '5',
    ISIS_EXIT    literally    '9',
    ISIS_ERROR    literally    '12';

Open: procedure (connP, path, access, echo, statusP);
    (connP, path, access, echo, statusP) address;
    isis[ISIS_OPEN, .connP];
end Open();

Read: procedure (conn, buffP, count, actualP, statusP);
    (conn, buffP, count, actualP, statusP) address;
    isis[ISIS_READ, .conn];
end Read();

Seek: procedure (conn, mode, blockP, byteP, statusP);
    (conn, mode, blockP, byteP, statusP) address;
    isis[ISIS_SEEK, .conn];
end Seek();

Write: procedure (conn, buffP, count, statusP);
    (conn, buffP, count, statusP) address;
    isis[ISIS_WRITE, .conn];
end Write();

Close: procedure (conn, statusP);
    (conn, statusP) address;
    isis[ISIS_CLOSE, .conn];
end Close();

Error: procedure (errorNum);
    (errorNum, statusP) address;
    statusP = .statusP;
    isis[ISIS_ERROR, .errorNum];
end Error();

static void Exit()
{
    statusP address;
    statusP = .statusP;
    isis[ISIS_EXIT, .statusP];
end Exit();

Delete: procedure (path, statusP);
    (path, statusP) address;
    isis[ISIS_DELETE, .path];
end Delete();

static address AvailMem()
{
    return (maxMem - 0x100) and 0xff00;
}


static void ReportError(scode)
{
    scode address;
    unknown(3) byte;
    i address, wstatus address;

    if (scode != 0)
    {
        i = 0;
        while (i < 0x38F) {
            if (scode == errStrs[i] && errStrs[i + 1] != 0)
            {
                i = i + 1;
                Write(0, .(' '), 1, .wstatus);    
                while (errStrs[i] != 0) {
                    Write(0, .errStrs[i], 1, .wstatus);
                    i = i + 1;
                }
                Write(0, .(CR, LF), 2, .wstatus);
                return;
            }
            else
            {
                while (errStrs[i] != 0) {
                    i = i + 1;
                }
                i = i + 1;
            }
        }
        Error(scode);
    }
}

static void StatusChk(scode)
{
    scode address;

    if (scode != 0)
    {
        ReportError(scode);
        Close(tmpAFTN, .status);
        Close(connP, .status);
        Exit();
    }
}

static void OpenTmp()
{
    tstatus byte;    /* benign bug should be address but next data byte is dummy */

    call Open(.tmpAFTN, .aF0AsxrefTmp, READ_MODE, 0, .tstatus);
    StatusChk(tstatus);    
}


static void OpenListFile()
{
    dummy byte;

    if (path[0] == ':' && (path[1] && 0xdf) != 'F')    /* test as UCase */
        Open(.connP, .path, WRITE_MODE, 0, .status);    /* device */
    else
        Open(.connP, .path, UPDATE_MODE, 0, .status);    /* file */

    StatusChk(status);
    Seek(connP, SEEKEND, .dummy, .dummy, .status);    /* seek end */
    if (status == 0x13)    /* bad seek on non file is ok */
        return;
    StatusChk(status);
}

static bool ReadTmp(buf, cnt)
{
    buf address, cnt byte;
    actual address;

    Read(tmpAFTN, buf, cnt, .actual, .status);
    StatusChk(status);
    return actual = cnt;
}


static void WriteStr(str, len)
{
    str address, len byte;
    Write(connP, str, len, .status);
    StatusChk(status);
}

static void WriteCRLF()
{
    WriteStr(.(CR, LF), 2);
}


static void ToNextPage()
{
    WriteStr(.(FF), 1);
    WriteCRLF();
    WriteCRLF();
    WriteCRLF();
}


static void OutStrCRLF(str, len)
{
    str address, len byte;
    Write(0, str, len, .status);
    Write(0, .(CR, LF), 2, .status);
}


static void FatalError(err)
{
    err byte;

    if (err == 0)
        OutStrCRLF(.inputError, 23);
    else
        OutStrCRLF(.noMemory, 17);
    Close(tmpAFTN, .status);
    Exit();
}


static address AllocLineRef()
{
    topLowHeap = topLowHeap + 4;
    if (topLowHeap <= botHighHeap)
        return topLowHeap - 4;
    FatalError(1);
}

static address AllocXref()
{
    botHighHeap = botHighHeap - 8;
    if (botHighHeap >= topLowHeap)
        return botHighHeap + 1;
    FatalError(1);
}

static address FirstXref()
{
    return (topHighHeap - 8) + 1;
}


static byte TestMoreXrefs(arg1w)
{
    arg1w address;

    return arg1w > botHighHeap;
}

static address NextXref(arg1w)
{
    arg1w address;
    return (arg1w - 8);
}

static bool CmpXrefNames(pstr1, pstr2)
{
    (pstr1, pstr2) address;
    str1 based pstr1 (6) byte,
        str2 based pstr2 (6) byte;
    i byte;

    for (i = 0; i <= 5; i++) {
        if (str1[i] != str2[i])
            return FALSE;
    }
    return TRUE;
}
        
static address FindXref(pXref, pFound)
{
    (pXref, pFound) address;
    found based pFound bool;
    (pNewXref, pTestXref) address,
        i byte;
    item based pXref (8) byte,
        newXref based pNewXref (8) byte;

    found = FALSE;
    pTestXref = FirstXref();
    while (! found && TestMoreXrefs(pTestXref)) {
        if (CmpXrefNames(pXref, pTestXref))
            found = TRUE;
        else
            pTestXref = nextXref[pTestXref];
    }

    if (found)
        return pTestXref;
    else
    {
        pNewXref = AllocXref();
        for (i = 0; i <= 5; i++) {
            newXref[i] = item[i];
        }
        itemCount = itemCount + 1;
        return pNewXref;
    }
}


static void InsertXref(isDef, pInRef, lineNum)
{
    isDef byte, (pInRef, lineNum) address,
        (pHeadLineRef, pPtr, pLineRef, qPtr, pXref) address,
        found byte;
    lineNumRef based pLineRef structure(lineNum address, next address),
        q based    qPtr address, p based pPtr address,
        headLineRef based pHeadLineRef address;

    pXref = FindXref(pInRef, .found);
    pHeadLineRef = pXref + 6;
    pLineRef = AllocLineRef();
    if (isDef)
        lineNum = lineNum or 0x8000;
    lineNumRef.lineNum = lineNum;
    qPtr = .lineNumRef.next;
    if (found)
    {
        pPtr = headLineRef + 2;
        q = p;
        p = pLineRef;
    }
    else
        q = pLineRef;

    headLineRef = pLineRef;
}

static address GetPLineRef(from, ptr, pMoreLineRefs)
{
    from byte, (ptr, pMoreLineRefs) address;
    (ppHead, pNext) address;
    pHead based ppHead address;
    next based pNext address;
    moreLineRefs based pMoreLineRefs bool;

    if (from == 0)    /* ptr -> Xref() */
    {
        ppHead = ptr + 6;
        pNext = pHead + 2;
        moreLineRefs = TRUE;
        return startMarker := next;
    }
    else
    {            /* ptr -> LineRef() */
        pNext = ptr + 2;
        moreLineRefs = startMarker <> next;
        return next;
    }
}

static address GetLineNum(pLineRef, pIsDef)
{
    (pLineRef, pIsDef) address;
    isDef based pIsDef byte;
    lineNum based pLineRef address;

    isDef = ROL((() >> 8);
    return 0x7fff and lineNum;
}


static void CopyXref(pStr1, pStr2)
{
    (pStr1, pStr2) address;
    str1 based pStr1 (8) byte,
        str2 based pStr2 (8) byte;
    i byte;

    for (i = 0; i <= 7; i++) {
        str2[i] = str1[i];    
    }
}


static address GetPXref(n)
{    /* get address of Xref(n) */
    n address;
    return topHighHeap - n * 8 + 1;
}


static byte GetTmpRecord()
{
    if (! ReadTmp(.outputLine, 1))
        return 3;
    if (outputLine[0] == '0' || outputLine[0] == '1')
    {
        if (! ReadTmp(.outputLine[1], 10))
            FatalError(0);
    }
    else if (outputLine[0] == '2')
    {
        if (! ReadTmp(.outputLine[1], 20))
            FatalError(0);
    }
    else
    {
         if (outputLine[0] == '3')
            return TYPEEOF;
         FatalError(0);
    }
    return outputLine[0] - '0';
}


static address Htoi(buf, n)
{
    buf address, n byte;
    (val, pCh) address;
    ch based pCh byte;

    val = 0;

    for (pCh = buf; pCh <= buf + n - 1; pCh++) {
        if (ch <= '9')
            val = val * 16 + (ch - '0');
        else
            val = val * 16 + (ch - ('A' - 10));
    }
    return val;
}


static address GetPSymbol()
{
    return .outputLine[1];
}

static address GetLineNumber()
{
    return Htoi(.outputLine[7], 4);
}

static void ProcXrefRecord(isDef)
{
    isDef byte;
    InsertXref(isDef, GetPSymbol, GetLineNumber);
}

static void CopyFileName()
{
    (i, j) byte;

    j = 0;
    for (i = 0; i <= 14; i++) {
        path[i] = outputLine[j := j + 1];
    }
}



static void ProcFileRecord()
{
    CopyFileName();
    paging = outputLine[16];
    pagelength = htoi[.outputLine(17], 2);
    pageWidth = htoi[.outputLine(19], 2);
    haveFileInfo = TRUE;
}

static void SortXrefs()
{
    (i, j, k, m, n) address,
        tmpXref(8) byte;

    IsLessThan: procedure byte;
        pstr address, i byte;
        str based pstr (6) byte;

        pstr = GetPXref(m);
        for (i = 0; i <= 5; i++) {
            if (tmpXref[i] != str[i])
                return tmpXref[i] < str[i];
        }
        return 0;
    }

    i, j, k = 1;

    while (k < itemCount) {
        i = j;
        j = k;
        k = k * 3 + 1;
    }

    k = i;

    while (k != 0) {
        n = k + 1;
        while (n <= itemCount) {
            m = n - k;
            CopyXref(GetPXref(n), .tmpXref);
            while (m > 0 && m < 0x8000 && IsLessThan()) {
                CopyXref(GetPXref(m), GetPXref(m + k));
                m = m - k;
            }
            CopyXref(.tmpXref, GetPXref(m + k));
            n = n + 1;
        }
        k = (k - 1) / 3;
    }
}


static void Num2Ascii(val, buf, len)
{
    (val, buf) address, len byte;
    pc address, c based pc byte;

    pc = buf + len - 1;
    while (pc >= buf) {
        c = val mod 10 + '0';
        val = val / 10;
        pc = pc - 1;
    }    
    pc = buf;
    while (c == '0' && pc < buf + len - 1) {
        c = ' ';
        pc = pc + 1;
    }
}

static void blankOutputLine()
{
    i byte;
    for (i = 0; i <= 255; i++) {
        outputLine[i] = ' ';
    }
}

static void PageHeader(pageNum)
{
    pageNum byte, asciiPageNum(3) byte;

    Num2Ascii(pageNum, .asciiPageNum, 3);
    ToNextPage();
    WriteStr(.headerMsg+2, 72);
    WriteStr(.asciiPageNum, 3);
    writeCRLF;
    writeCRLF;
    row = 7;
}



static void NameToOutputLine(pName)
{
    pName address, i byte;
    name based pName (6) byte;

    for (i = 0; i <= 5; i++) {
        outputLine[col] = name[i];
        col = col + 1;
    }
}

static void OutputLineRef(pLineRef)
{
    pLineRef address;
    lineNum address, isDefinition byte;

    lineNum = GetLineNum(pLineRef, .isDefinition);
    Num2Ascii(lineNum, .outputLine[col], 4);
    col = col + 4;
    if (isDefinition)
        outputLine[col] = '#';
    col = col + 3;
}

static void OutputXref()
{
    (refsPerLine, refsCnt, pageNum) byte,
        (pLineRef, pXref) address, moreLineRefs byte;

    refsPerLine = (pageWidth - 6) / 7;
    pageNum = 1;
    PageHeader(1);
    pXref = FirstXref();
    
    while (TestMoreXrefs(pXref)) {
        blankOutputLine;
        col = 0;
        NameToOutputLine(pXref);
        pLineRef = GetPLineRef(0, pXref, .moreLineRefs);
        while (moreLineRefs) {
            col = 7;
            refsCnt = 0;
            while ((refsCnt := refsCnt + 1) <= refsPerLine && moreLineRefs) {
                OutputLineRef(pLineRef);
                pLineRef = GetPLineRef(1, pLineRef, .moreLineRefs);
            }
            WriteStr(.outputLine, col - 1);
            writeCRLF;
            if (paging)
                if ((row := row + 1) == pagelength - 2)
                    PageHeader(pageNum := pageNum + 1);
            if (moreLineRefs)
                blankOutputLine;
        }
        pXref = NextXref(pXref);
    }
}

main:
    itemCount = 0;
    topLowHeap = .Memory();
    botHighHeap, topHighHeap = AvailMem() - 1;
    if (drive != '0')
        aF0AsxrefTmp[2] = drive;
    haveFileInfo = FALSE;
    OpenTmp();
    outStrCRLF[.headerMsg, 51];

    while (1 ) {
        recordType = GetTmpRecord();
        if (recordType == TYPEDEF || recordType == TYPEREF)
            ProcXrefRecord(recordType = TYPEDEF);
        else if (recordType == TYPEFILE)
            ProcFileRecord();
        else
        {
            if (! haveFileInfo)        
                FatalError(0);
            SortXrefs();
            OpenListFile();
            OutputXref();
            WriteCRLF();
            WriteStr(.xrefComplete, 24);
            WriteCRLF();
            Close(tmpAFTN, .status);
            Close(connP, .status);
            Delete(.aF0AsxrefTmp, .status);
            Exit();
        }
    }
}
