$IF BASE
asm801: do;
$include(:f3:asm801.ipx)
$ELSEIF OVL4
asm41: do;
$include(:f3:asm41.ipx)
$ELSE
asm51: do;
$include(:f3:asm51.ipx)
$ENDIF

$IF BASE
declare CHKOVL$0 lit    'call OvlMgr(0)',
    CHKOVL$1 lit    'call OvlMgr(1)',
    CHKOVL$2 lit    'call OvlMgr(2)',
    CHKOVL$3 lit    'call OvlMgr(3)';
$ELSE
declare CHKOVL$0 lit    ' ',
    CHKOVL$1 lit    ' ',
    CHKOVL$2 lit    ' ',
    CHKOVL$3 lit    ' ';
$ENDIF

$IF OVL4
declare w$3780 address public data(0),
    b$3782 byte public data(80h),
    b$3783 byte public data(81h);
$ENDIF

declare    spaces24(*) byte public data('         '),
    spaces15(*) byte public data('         '),
    spaces6(*) byte public data(' '),
    spaces5(*) byte public data(' '),
    spaces4(*) byte public data('  '),
    spaces2(*) byte public data('  ', 0),
    ascCRLF(*) byte public data(CR, LF, 0),
    signonMsg(*) byte public data(CR, LF),
    asmHeader(*) byte public data('ISIS-II 8080/8085 MACRO ASSEMBLER, V4.1', TAB, TAB),
    aModulePage(*) byte public data('MODULE ', TAB, ' PAGE ', 0),
    bZERO byte public data(0),
    bTRUE byte public data(TRUE),
    copyright(*) byte data('(C) 1976,1977,1979,1980 INTEL CORP'),
    aStack(*) byte public data(CR, LF, 'STACK', 0),
    aTable(*) byte public data(CR, LF, 'TABLE', 0),
    aCommand(*) byte public data(CR, LF, 'COMMAND', 0),
    aEof(*) byte public data(CR, LF, 'EOF', 0),
    aFile(*) byte public data(CR, LF, 'FILE', 0),
    aMemory(*) byte public data(CR, LF, 'MEMORY', 0),
    aError(*) byte public data(' ERROR', CR, LF, 0),
    aError$0(*) byte public data(' ERROR, ', CR,LF, 0),
    errStrs(*) address public data(.aStack, .aTable, .aCommand, .aEof, .aFile, .aMemory),
    errStrsLen(*) byte public data(7, 7, 9, 5, 6, 8),
    aBadSyntax(*) byte public data('BAD SYNTAX', CR, LF),
    aCo(*) byte public data(':CO:', 0);

$IF BASE
declare    loadedOvl byte initial(4),
    ovlFile(*) byte public initial(':F0:ASM80.OV0 ');
$ENDIF

Physmem: procedure address public;
    declare top address at(4);

    return (top - 100h) and 0FF00h;
end;


GetCmdCh: procedure byte public;
    declare cmdch based cmdch$p byte;
    declare ch byte;

    ch = cmdch;
    cmdch$p = cmdch$p + 1;
    if 'a' <= ch and ch <= 'z' then
        ch = ch and 5Fh;
    return ch;
end;    


IoErrChk: procedure public;
    if statusIO = 0 then
        return;
    call Error(statusIO);
    call Exit;
end;


Flushout: procedure public;
    call Write(outfd, .outbuf, out$p - .outbuf, .statusIO);
    out$p = .outbuf;
end;

Outch: procedure(c) public;
    declare c byte;
    declare outc based out$p byte;

    if out$p = endOutBuf then
        call Flushout;
    outc = c;
    out$p = out$p + 1;
end;

OutStrN: procedure(s, n) public;
    declare s pointer, n byte;
    declare ch based s byte;

    do while n > 0;
        call Outch(ch);
        s = s + 1;
        n = n - 1;
    end;
end;

$IF BASE
OvlMgr: procedure(ovl) public;
    declare ovl byte;
    declare entry$p address;

    if ovl > 4 then
    do;
        statusIO = 0Dh;
        call IoErrChk;
    end;
    if ovl <> loadedOvl then
    do;
        ovlFile(12) = ovl + '0';
        call Load(.ovlFile, 0, 0, .entry$p, .statusIO);
        call IoErrChk;
        loadedOvl = ovl;
    end;
end;
$ENDIF

CloseF: procedure(conn) public;
    declare conn address;

    call Close(conn, .statusIO);
end;

IsSpace: procedure byte public;
    return curChar = ' ';
end;

IsTab: procedure byte public;
    return curChar = TAB;
end;

IsWhite: procedure byte public;
    return IsSpace or IsTab;
end;

IsRParen: procedure byte public;
    return curChar = ')';
end;

IsCR: procedure byte public;
    return curChar = CR;
end;

IsComma: procedure byte public;
    return curChar = ',';
end;

$IF OVL4
IsLT: procedure byte public;
    return curChar = '<';
end;


IsGT: procedure byte public;
    return curChar = '>';
end;

$ENDIF

IsPhase1: procedure byte public;
    return phase = 1;
end;

Skip2EOL: procedure public;
    if not IsCR then
        do while GetCh <> 0Dh;
        end;
end;


ChkGenObj: procedure byte public;
$IF BASE
    return (phase > 2) and ctlOBJECT;
$ELSE
    return (phase = 2) and ctlOBJECT;
$ENDIF
end;


IsPhase2Print: procedure byte public;
    return phase = 2 and ctlPRINT;
end;


WrConsole: procedure(buf$p, count) public;
    declare (buf$p, count) address;

    call Write(0, buf$p, count, .statusIO);
    call IoErrChk;
end;


RuntimeError: procedure(errCode) public;
    declare errCode byte;

    if b6BD9 then
        return;

    if IsPhase1 and errCode = 0 then
    do;
        b6B33 = TRUE;
        return;
    end;

    pAddr = .aError;        /* assume " ERROR\r\n" */
    if errCode = 4 then        /* file Error */
        pAddr = .aError$0;    /* replace with " ERROR, " */

    call WrConsole(errStrs(errCode), errStrsLen(errCode));    /* Write the ERROR type */
    call WrConsole(pAddr, 8);    /* Write the ERROR string */
    if IsPhase2Print then        /* repeat to the print file if required */
    do;
        call OutStrN(errStrs(errCode), errStrsLen(errCode));
        call OutStrN(pAddr, 8);
    end;

    if errCode = 4 or errCode = 3 then    /* file or EOF Error */
    do;
        if tokBufIdx = 0 then
        do;
            call WrConsole(.aBadSyntax, 12);
            if not scanCmdLine then
            do;
                call Skip2NextLine;
                outfd = 0;
                CHKOVL$1;
                call PrintDecimal(srcLineCnt);    /* overlay 1 */
                call Outch(LF);
            end;
        end;
        else
        do;
            call WrConsole(curFileName$p, tokBufIdx);
            call WrConsole(.ascCRLF, 2);
        end;
    end;

    if errCode = 0 then    /* stack Error */
    do;
        b6BD9 = TRUE;
        return;
    end;

    call Exit;
end;

IoError: procedure(s) public;
    declare s pointer;
    declare ch based s byte;

    tokBufIdx = 0;
    curFileName$p = s;

    do while ch <> ' ' and ch <> CR and ch <> TAB;
        tokBufIdx = tokBufIdx + 1;
        s = s + 1;
    end;
    if missingEnd then
        call RuntimeError(3);    /* EOF Error*/
    call RuntimeError(4);        /* file Error */
end;

InOpen: procedure(path$p, access) address public;
    declare (path$p, access) address;
    declare Open$infd address;

    call Open(.Open$infd, path$p, access, 0, .openStatus);
    if openStatus <> 0 then
        call IoError(path$p);
    return Open$infd;
end;


Nibble2Ascii: procedure(n) byte public;
    declare n byte;
    n = (n and 0Fh) + '0';
    if n > '9' then
        n = n + 7;
    return n;
end;

Put2Hex: procedure(pfunc, val) public;
    declare pfunc address, val byte;

    call pfunc(Nibble2Ascii(ror(val, 4)));
    call pfunc(Nibble2Ascii(val));
end;

BlankAsmErrCode: procedure byte public;
    return asmErrCode = ' ';
end;

BlankMorPAsmErrCode: procedure byte public;
    return BlankAsmErrCode or asmErrCode = 'M' or asmErrCode = 'P';
end;


GetNibble: procedure(bp, idx) byte public;
    declare bp pointer, idx byte;
    declare b based bp byte;
    declare n byte;

    bp = bp + shr(idx, 1);    /* index into buffer the number of nibbles */
    n = b;            /* pick up the byte there */
    if not idx then        /* pick up the right nibble */
        n = ror(n, 4);
    return n and 0Fh;    /* mask to leave just the nibble */
end;

SourceError: procedure(errCh) public;
    declare errCh byte;

    if not IsSkipping or leftOp = K$ELSE then    /* ELSE */
    do;
        if inExtrn then
            badExtrn = TRUE;
        if BlankAsmErrCode then
            errCnt = errCnt + 1;

        if BlankMorPAsmErrCode or errCh = 'L' or errCh = 'U' then    /* no Error or M, P L or U */
            if asmErrCode <> 'L' then    /* override unless already location counter Error */
                asmErrCode = errCh;

    end;
end;

$IF OVL4

InsertByteInMacroTbl: procedure(c) public;
    declare c byte;
    declare ch based macroInPtr byte;

    ch = c;
    if (macroInPtr := macroInPtr + 1) > baseMacroTbl then
        call RuntimeError(1);    /* table Error */
end;


InsertCharInMacroTbl: procedure(c) public;
    declare c byte;

    call InsertByteInMacroTbl(c);
    if c = CR then
        call InsertByteInMacroTbl(LF);
end;

$ENDIF


ParseControlLines: procedure public;

    do while GetCh = '$';
        if IsSkipping then
        do;
            call Skip2NextLine;
            isControlLine = TRUE;
$IF OVL4
            if b905E = 1 then
                b6897 = TRUE;
$ENDIF
        end;
        else
        do;
            CHKOVL$0;
            call ParseControls;
        end;
        call FinishLine;
    end;
    reget = 1;    /* push back character */
end;


InitialControls: procedure public;
    cmdch$p = controls$p;
    scanCmdLine = TRUE;
    CHKOVL$0;
    call ParseControls;
    if IsPhase2Print then
    do;
        CHKOVL$1;
        call PrintCmdLine;
    end;
    if needToOpenFile then
        call OpenSrc;

    needToOpenFile, isControlLine, scanCmdLine = bZERO;
    call ParseControlLines;            /* initial control lines allow primary controls */
    primaryValid = FALSE;            /* not allowed from now on */
    ctlDEBUG = ctlDEBUG and ctlOBJECT;    /* debug doesn't make sense if no object code */
    ctlXREF = ctlXREF and ctlPRINT;        /* disable controls if not printing */
    ctlSYMBOLS = ctlSYMBOLS and ctlPRINT;
    ctlPAGING = ctlPAGING and ctlPRINT;
end;


InitLine: procedure public;
    startLine$p = inCh$p + 1;    
    lineChCnt = 0;
    if needToOpenFile then
        call OpenSrc;

    lineNumberEmitted, has16bitOperand, isControlLine, errorOnLine, lhsUserSymbol,
    inExpression, expectingOperands, xRefPending, gotLabel, rhsUserSymbol,
    inDB, inDW, condAsmSeen, showAddr, usrLookupIsID,
$IF OVL4
    b9059, b9060, 
$ENDIF
    needsAbsValue = bZERO;

    atStartLine, expectingOpcode, isInstr, expectOp = bTRUE;
    ctlEJECT, hasVarRef, tokenIdx,
$IF OVL4
    b9058, argNestCnt,
$ENDIF
    tokenSize(0), tokenType(0), acc1ValType, acc2ValType, inComment, acc1Flags = bZERO;

    asmErrCode = ' ';
$IF OVL4
    macro$p = .macroLine;
    w919D = macroInPtr;
    expandingMacro = expandingMacro > 0;
$ENDIF
    tokI = 1;
    srcLineCnt = srcLineCnt + 1;
$IF OVL4
    macro$p = .macroLine;
$ENDIF
    skipping(0) = skipping(0) > 0;
end;


start:
    call GetAsmFile;
    phase = 1;
    call ResetData;
    call InitialControls;
$IF BASE
    if ctlMACROFILE then
    do;
        if Physmem < 8001h then
            call RuntimeError(5);     /* memory Error */
        if srcfd <> rootfd then
            call CloseF(srcfd);
        call CloseF(infd);
        ovlFile(12) = '4';        /* use macro asm version */
        call Load(.ovlFile, 0, 1, 0, .statusIO);
        call IoErrChk;
    end;
    if Physmem > 8001h then
    do;
        if srcfd <> rootfd then
            call CloseF(srcfd);
        call CloseF(infd);
        ovlFile(12) = '5';        /* use big memory asm version */
        call Load(.ovlFile, 0, 1, 0, .statusIO);
        call IoErrChk;
    end;

    if MacroDebugOrGen then            /* attempt to use macro features */
        call RuntimeError(2);        /* command Error */
$ELSEIF OVL4
    macrofd = InOpen(.asmax$ref, 3);
$ENDIF

    if ctlOBJECT then
    do;
        call Delete(.objFile, .statusIO);
        objfd = InOpen(.objFile, 3);
    end;

    if ctlXREF then
    do;
        xreffd = InOpen(.asxref$tmp, 2);
        outfd = xreffd;
    end;

    call DoPass;
    phase = 2;
    if ctlOBJECT then
    do;
        CHKOVL$2;    /* for small version Load in overlay 2 for writeRec & WriteModhdr */
        if r$extnames.len > 0 then
            call WriteRec(.r$extnames);    /* in overlay 2 */

        if externId = 0 then
            call WriteModhdr;        /* in overlay 2 */
$IF NOT BASE
        call InitRecTypes;
$ENDIF
    end;
$IF BASE
    if not ctlOBJECT or ctlPRINT then
$ENDIF
    do;
        if ctlPRINT then
            outfd = InOpen(.lstFile, 2);
        CHKOVL$3;
        call ResetData;
        call InitialControls;
        call DoPass;
    end;
    if ctlPRINT then
    do;
        CHKOVL$1;
        call AsmComplete;
        call Flushout;
    end;

    if ctlOBJECT then
    do;
$IF BASE
        phase = 3;
        CHKOVL3;
        call ResetData;
        call InitRecTypes;
        call InitialControls;
        call DoPass;
        CHKOVL2;
$ENDIF
        call Ovl11;
        call WriteModend;
    end;

    if not StrUcEqu(.aCo, .lstFile) then
    do;
        CHKOVL$1;
        call Ovl9;
    end;
    CHKOVL$1;
    call Ovl10;
end;

