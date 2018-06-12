#include "asm80.h"

word seekMZero = 0;
byte b3782[2] = { 0x80, 0x81 };
byte spaces24[] = "                        ";
byte ascCRLF[] = "\r\n";
byte signonMsg[] = "\r\nISIS-II 8080/8085 MACRO ASSEMBLER, V4.1\t\tMODULE \t PAGE ";
byte bZERO = 0;
byte bTRUE = true;
static byte copyright[] = "(C) 1976,1977,1979,1980 INTEL CORP";

pointer aErrStrs[] = { "\r\nSTACK", "\r\nTABLE", "\r\nCOMMAND", "\r\nEOF", "\r\nFILE", "\r\nMEMORY" };
byte aErrStrsLen[] = {7, 7, 9, 5, 6, 8};


pointer Physmem()
{
    return (MemCk() - 0x100);	// top of memory
}


byte GetCmdCh()
{
    byte ch;

    ch = *cmdchP++;
    if ('a' <= ch && ch <= 'z')
        ch = ch & 0x5F;
    return ch;
}    


void IoErrChk()
{
    if (statusIO == 0)
        return;
    Error(statusIO);
    Exit();
}


void Flushout()
{
    Write(outfd, outbuf, (word)(outP - outbuf), &statusIO);
    outP = outbuf;
}

void Outch(byte c)
{
    if (outP == endOutBuf)
        Flushout();
    *outP++ = c;
}

void OutStrN(pointer s, byte n)
{

    while (n-- > 0)
        Outch(*s++);
}


void CloseF(word conn)
{
    Close(conn, &statusIO);
}

bool IsSpace()
{
    return curChar == ' ';
}

bool IsTab()
{
    return curChar == TAB;
}

bool IsWhite()
{
    return IsSpace() || IsTab();
}

bool IsRParen()
{
    return curChar == ')';
}

bool IsCR()
{
    return curChar == CR;
}

bool IsComma()
{
    return curChar == ',';
}

bool IsLT()
{
    return curChar == '<';
}


bool IsGT()
{
    return curChar == '>';
}


bool IsPhase1()
{
    return phase == 1;
}

void Skip2EOL()
{
    if (!IsCR())
        while (GetCh() != CR)
            ;
}


bool ChkGenObj()
{
    return phase == 2 && controls.object;
}


bool IsPhase2Print()
{
    return phase == 2 && controls.print;
}


void WrConsole(pointer bufP, word count)
{
    Write(0, bufP, count, &statusIO);
    IoErrChk();
}


void RuntimeError(byte errCode)
{
    if (skipRuntimeError)
        return;

    if (IsPhase1() && errCode == RTE_STACK) {
        b6B33 = true;
        return;
    }

    aVar.bp = " ERROR\r\n";        /* assume " ERROR\r\n" */
    if (errCode == RTE_FILE)        /* file Error() */
        aVar.bp = " ERROR, ";    /* replace message */

    WrConsole(aErrStrs[errCode], aErrStrsLen[errCode]);    /* Write() the ERROR type */
    WrConsole(aVar.bp, 8);    /* Write() the ERROR string */
    if (IsPhase2Print()) {       /* repeat to the print file if required */
        OutStrN(aErrStrs[errCode], aErrStrsLen[errCode]);
        OutStrN(aVar.bp, 8);
    }

    if (errCode == RTE_FILE || errCode == RTE_EOF) {    /* file or EOF Error() */
        if (tokBufIdx == 0) {
            WrConsole("BAD SYNTAX\r\n", 12);
            if (! scanCmdLine) {
                Skip2NextLine();
                outfd = 0;		// :CO:
                PrintDecimal(srcLineCnt);    /* overlay 1 */
                Outch(LF);
            }
        } else {
            WrConsole(curFileNameP, tokBufIdx);
            WrConsole(ascCRLF, 2);
        }
    }

    if (errCode == RTE_STACK) {   /* stackError - suppress cascade errors */
        skipRuntimeError = true;
        return;
    }

    Exit();
}

void IoError(pointer path)
{
    tokBufIdx = 0;
    curFileNameP = path;

    while (*path != ' ' && *path != CR && *path != TAB) {
        tokBufIdx++;
        path++;
    }
    if (missingEnd)
        RuntimeError(RTE_EOF);    /* EOF Error*/
    RuntimeError(RTE_FILE);        /* file Error() */
}

/* open file for read with status check */
word SafeOpen(pointer pathP, word access)
{
    word conn;

    Open(&conn, pathP, access, 0, &openStatus);
    if (openStatus != 0)
        IoError(pathP);
    return conn;
}


byte Nibble2Ascii(byte n)
{
    n = (n & 0xF) + '0';
    if (n > '9')
        n = n + 7;
    return n;
}

void Put2Hex(void (*pfunc)(byte), byte val)
{
    pfunc(Nibble2Ascii(val >> 4));
    pfunc(Nibble2Ascii(val));
}

bool BlankAsmErrCode()
{
    return asmErrCode == ' ';
}

bool MPorNoErrCode()
{
    return BlankAsmErrCode() || asmErrCode == 'M' || asmErrCode == 'P';
}


byte GetNibble(pointer bp, byte idx)	// not used
{
    byte n;

    bp += (idx >> 1);    /* index into buffer the number of nibbles */
    n = *bp;            /* pick up the byte there */
    if (!(idx & 1))        /* pick up the right nibble */
        n >>= 4;
    return n & 0xF;    /* mask to leave just the nibble */
}

void SourceError(byte errCh)
{
    if (! IsSkipping() || topOp == K_ELSE) {   /* ELSE */
        if (inExtrn)
            badExtrn = true;
        if (BlankAsmErrCode())
            errCnt++;

        if (MPorNoErrCode() || errCh == 'L' || errCh == 'U')    /* no Error() || M, P L || U */
            if (asmErrCode != 'L')    /* override unless already location counter Error() */
                asmErrCode = errCh;
    }
}


void InsertByteInMacroTbl(byte c)
{
    *macroInPtr++ = c;
    if (macroInPtr > baseMacroTbl)
        RuntimeError(RTE_TABLE);    /* table Error() */
}


void InsertCharInMacroTbl(byte c)
{
    InsertByteInMacroTbl(c);
    if (c == CR)
        InsertByteInMacroTbl(LF);
}



void ParseControlLines()
{
    while (GetCh() == '$') {
        if (IsSkipping()) {
            Skip2NextLine();
            isControlLine = true;
            if (mSpoolMode == 1)
                b6897 = true;
        } else
            ParseControls();
        FinishLine();
    }
    reget = 1;    /* push back character */
}


void InitialControls()
{
    cmdchP = controlsP;
    scanCmdLine = true;
    ParseControls();
    if (IsPhase2Print())
        PrintCmdLine();
    if (pendingInclude)
        OpenSrc();
    
    pendingInclude = isControlLine = scanCmdLine = bZERO;
    ParseControlLines();            /* initial control lines allow primary controls */
    primaryValid = false;            /* not allowed from now on */
    controls.debug == controls.debug && controls.object;    /* debug doesn't make sense if no object code */
    controls.xref == controls.xref && controls.print;        /* disable controls if ! printing */
    controls.symbols = controls.symbols && controls.print;
    controls.paging = controls.paging && controls.print;
}


void InitLine()
{
    startLineP = inChP + 1;    
    lineChCnt = 0;
    if (pendingInclude)
        OpenSrc();
#ifdef SHOWLINE
    for (char *s = startLineP; s < endInBufP && *s != '\r' && *s != '\n';)
        putchar(*s++);
    putchar('\r');
    putchar('\n');
#endif
    lineNumberEmitted = has16bitOperand = isControlLine = errorOnLine = lhsUserSymbol =
    inExpression = expectingOperands = xRefPending = gotLabel = rhsUserSymbol =
    inDB = inDW = condAsmSeen = showAddr = usrLookupIsID =
    excludeCommentInExpansion = b9060 = needsAbsValue = bZERO;

    atStartLine = expectingOpcode = isInstr = expectOp = bTRUE;
    controls.eject = hasVarRef = tokenIdx = inQuotes = argNestCnt =
    tokenSize[0] = tokenType[0] = acc1ValType = acc2ValType = inComment = acc1Flags = bZERO;

    asmErrCode = ' ';
    macroP = macroLine;
    startMacroLine = macroInPtr;
    expandingMacro = expandingMacro > 0 ? 0xff : 0;
    tokI = 1;
    srcLineCnt++;
    macroP = macroLine;
    skipIf[0] = skipIf[0] > 0 ? 0xff : 0;
}

void Start()
{
    GetAsmFile();
    phase = 1;
    ResetData();
    InitialControls();
    macrofd = SafeOpen(asmacRef, UPDATE_MODE);

    if (controls.object) {
        Delete(objFile, &statusIO);
        objfd = SafeOpen(objFile, UPDATE_MODE);
    }

    if (controls.xref) {
        xreffd = SafeOpen(asxrefTmp, WRITE_MODE);
        outfd = xreffd;
    }

    DoPass();
    phase = 2;
    if (controls.object) {
        if (rExtnames.len > 0)
            WriteRec((pointer)&rExtnames);    /* in overlay 2 */

        if (externId == 0)
            WriteModhdr();        /* in overlay 2 */
        InitRecTypes();
    }
    if (controls.print)
        outfd = SafeOpen(lstFile, WRITE_MODE);
    ResetData();
    InitialControls();
    DoPass();
    if (controls.print) {
        AsmComplete();
        Flushout();
    }

    if (controls.object) {
        Ovl11();
        WriteModend();
    }

    if (! StrUcEqu(":CO:", lstFile))
        FinishPrint();
    FinishAssembly();
}

