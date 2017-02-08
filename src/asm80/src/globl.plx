$IF SMALL
globls: do;
$include(:f3:globls.ipx)
$ELSEIF MACRO
globlm: do;
$include(:f3:globlm.ipx)
$ELSE
globlb: do;
$include(:f3:globlb.ipx)
$ENDIF

/* force rescan to be included from system.lib */
rescan: procedure (conn, status$p) external;
    declare (conn, status$p) address;
end rescan;

$IF MACRO
declare IN$BUF$SIZE lit '512',
        OUT$BUF$SIZE lit '512';
$ELSEIF BIG
declare IN$BUF$SIZE lit '2048',
        OUT$BUF$SIZE lit '2048';
$ELSE
declare IN$BUF$SIZE lit '200',
        OUT$BUF$SIZE lit '128';
$ENDIF

$IF MACRO
declare    byteAt(1) byte public at(0),
    macroLine(127) byte public,
    endMacroLine(2) byte public,            /* space for 0 and a guard byte */
    macro$p pointer public initial(.macroLine),
    b9058 bool public initial(FALSE),
    b9059 bool public,
    b905A byte public,
    expandingMacro byte public,
    b905C byte public,
    b905D bool public initial(FALSE),
    b905E byte public,
    b905F byte,
    b9060 bool public,
    b9061 bool public,
    b9062 byte public,
    macroDepth byte public,
    b9064 byte public,
    b9065 byte public,
    b9066 byte public,
    argNestCnt byte public initial(0),
    w9068 pointer public,
    macroInPtr pointer public,
    /*
        mtype has the following values
        1 -> IRP
        2 -> IRPC
        3 -> REPT
        4 -> ???
        5 -> ???
    */
        
    macroStk(10) MACRO$T public,
    (tmac$macroCondSP, tmac$ifDepth,  tmac$mtype, tmac$b3) byte public at (.macroStk(0)),
    (tmac$w4, tmac$buf$p) address public at (.macroStk(0).w4),
    tmac$blk address public  at (.macroStk(0).blk) initial(0FFFFh),
    (tmac$w10, tmac$w12, tmac$w14) address public at (.macroStk(0).w10),
    w910C address,
    curMacroBlk address public initial(0FFFFh),
    nxtMacroBlk address public initial(0),
    maxMacroBlk address public initial(0),
    macroBlkCnt address public,
    macroBuf(129) byte public,
    w9197 address public,
    w9199 address public,
    w919B address public,
    w919D address public,
    w919F address public,
    b91A1 byte public initial(0),
    b91A2 byte public initial(0),

    b91A3 byte public initial(81h),
    b91A4(*) byte public initial(3Fh, 3Fh, 0, 0, 0, 0, 80h);
$ENDIF
/* ov4 compat 2C8C */
declare    contentBytePtr pointer public,
    fixupSeg byte public,
    fixOffset address public,
    curFixupHiLoSegId byte public,
    curFixupType byte public,
    fixIdxs(4) byte public initial(0, 0, 0, 0),
    (fix22Idx, fix24Idx, fix20Idx, fix6Idx) byte public at (.fixIdxs),
    extNamIdx byte public initial(0),
    initFixupReq(4) bool public initial(TRUE, TRUE, TRUE, TRUE),
    firstContent byte public initial(TRUE),
    r$eof EOF$T public initial(0Eh, 0),
    r$extnames EXTNAMES$T public initial(18h, 0, 0),
    moduleNameLen byte public initial(6),
    r$content CONTENT$T public initial(18h, 0, 0),	/* seems odd would expect 6 not 18h */
    r$publics PUBLICS$T public initial (16h, 1, 0, 0, 0),
    r$reloc RELOC$T public at(.r$publics),
    r$interseg INTERSEG$T public,
    r$extref EXTREF$T public,
    r$modend MODEND$T public initial( 4, 4, 0),
    wZERO address public initial(0),
    pad6741 byte initial(0ah),
    inComment bool public initial(FALSE),
    noOpsYet bool public initial(FALSE),
    nameLen byte public,
    startSeg byte public initial(1),
    padb6746 byte,
    activeSeg byte public,
    inPublic byte public initial(0),
    inExtrn byte public initial(0),
    segHasData(2) byte public,
    alignTypes(4) byte public initial(3, 3, 3, 3),
    externId address public,
    itemOffset address public,
    badExtrn byte public initial(0),
    startDefined byte public initial(0),
    startOffset address public initial(0),
    tokenIdx byte public initial(0),
    lineBuf(128) byte public,
    tokStart(9) address public initial(.lineBuf),
    tokPtr pointer public at(.tokStart),
    tokenSym(9) address public initial(0),
    curTokenSym$p pointer public at(.tokenSym(0)),
    tokenType(9) byte public,
    tokenSize(9) byte public initial(0),
    tokenAttr(9) byte public,
    tokenSymId(9) address public,
/* end */
    endLineBuf address public initial(.tokStart),
    ifDepth byte public initial(0),
    skipIf(9) bool public,
    inElse(9) bool public;
$IF MACRO
declare    macroCondSP byte public initial(0),
    macroCondStk(17) byte public,
    b94DD(2) byte;
$ENDIF
declare    opSP byte public,
    opStack(17) byte public,
/* ov4 compat 2CA0 */
    (accum1, accum2) address public,
    (accum1$lb, accum1$hb, accum2$lb, accum2$hb) byte public at(.accum1),
    acc1Flags byte public,
    acc2Flags byte public,
    hasVarRef byte public,
    acc1ValType byte public,
    acc2ValType byte public,
    acc1NumVal address public,
    acc2NumVal address public,
    curChar byte public initial(0),
    reget byte public initial(0),
    lookAhead byte public,
    pad6861 byte initial(0),
    symTab(3) address public,
    endSymTab(3) address public,
    symHighMark address public,
    baseMacroTbl pointer public,
    gotLabel byte public initial(0),
    name(6) byte public,
    savName(6) byte public,
    lhsUserSymbol bool public,
    rhsUserSymbol bool public,
    xRefPending bool public initial(FALSE),
    passCnt byte public initial(0),
    createdUsrSym bool public initial(FALSE),
    usrLookupIsID bool public initial(FALSE),
    needsAbsValue bool public initial(FALSE),
    objfd address public,
    xreffd address public,
    infd address public,
    outfd address public,
    macrofd address public,
    statusIO address public,
    openStatus address public,
    pad6894 address initial(0FFFFh),
    asmErrCode byte public initial(' '), 
    b6897 bool public initial(FALSE),
    primaryValid byte public initial(TRUE);
/* end */
$IF SMALL
declare    pad6899 byte initial(0);
$ENDIF
/* ov4 compat 2C9F */
declare    tokI byte public,
    errorOnLine bool public,
    atStartLine byte public,
    pad689D(2) byte,
    curCol byte public initial(1),
    endItem address public,
    startItem address public,
    pageLineCnt address public,
    effectiveAddr address public,
    pageCnt address public,
    pad68AA byte,
    showAddr bool public,
    pad68AC byte,
    lineNumberEmitted bool public initial(FALSE),
    b68AE bool public initial(FALSE),
    tokStr(7) byte public initial(0, 0, 0, 0, 0, 0, 0),
    sizeInBuf address public initial(IN$BUF$SIZE),
    inBuf(IN$BUF$SIZE) byte public,
    outbuf(OUT$BUF$SIZE) byte public,
/* end */
    b6A00 byte public,
/* ov4 compat 2F57 */
    out$p pointer public initial(.outbuf),
    endOutBuf address public,
    pad6A05 address initial(.outbuf),
    pad6A07 byte initial(0),
    objFile(15) byte public initial('               '), /* 15 spaces */
    lstFile(15) byte public initial('               '), /* 15 spaces */
    asxref$tmp(*) byte public initial(':F0:ASXREF.TMP '),
    asxref(*) byte public initial(':F0:ASXREF '),
    asmax$ref(*) byte public initial(':F0:ASMAC.TMP '),
    srcLineCnt address public initial(1),
    pad6A50(2) byte initial('  '),        /* protects for very big files */
    asciiLineNo(4) byte public initial('   0'),
    spIdx byte public,
    lastErrorLine(4) byte public initial('   0'),
    ctlDEBUG bool public initial(FALSE),    /* DEBUG */
    controls(1) byte public at(.ctlDEBUG),
    ctlMACRODEBUG bool public initial(FALSE),    /* MACRODEBUG */
    ctlXREF bool public initial(FALSE),    /* XREF */
    ctlSYMBOLS bool public initial(TRUE),    /* SYMBOLS */
    ctlPAGING bool public initial(TRUE),    /* PAGING */
    ctlTTY bool public initial(FALSE),    /* TTY */
    ctlMOD85 bool public initial(FALSE),    /* MOD85 */
    ctlPRINT bool public initial(TRUE),    /* PRINT */
    ctlOBJECT bool public initial(TRUE),    /* OBJECT */
    ctlMACROFILE bool public initial(FALSE),    /* MACROFILE */
    ctlPAGEWIDTH byte public initial(120),    /* PAGEWIDTH */
    ctlPAGELENGTH byte public initial(66),    /* PAGELENGTH */
    ctlINLCUDE byte initial(0),        /* INCLUDE */
    ctlTITLE bool public initial(FALSE),    /* TITLE */
    ctlSAVE byte,                /* SAVE */
    ctlRESTORE byte,            /* RESTORE */
    ctlEJECT byte public initial(0),    /* EJECT */
    ctlLIST bool public initial(TRUE),    /* LIST */    /* SAVE/RESTORE act on next 3 opts */
    ctlGEN bool public initial(TRUE),    /* GEN */
    ctlCOND bool public initial(TRUE),    /* COND */
    ctlLISTChanged bool public initial(TRUE),
    titleLen byte public initial(0),
    pad6A71 byte,
    pad6A72(3) byte initial(120, TRUE),
    controlSeen(12) bool public initial(0,0,0,0,0,0,0,0,0,0,0,0),
    saveStack(24) byte public,
    saveIdx byte public initial(0),
    ctlTITLESTR(64) byte public,
    tokBufLen byte public,
    tokType byte public,
    controlId byte public,
    tokBuf(64) byte public,
    tokBufIdx byte public initial(0),
    tokNumVal address public,
/* end */
    isControlLine byte public initial(FALSE);
$IF NOT MACRO
declare    MacroDebugOrGen byte public initial(0);
$ENDIF
/* ov4 compat 2F56 */
declare    scanCmdLine byte public,
    inDB byte public,
    inDW byte public,
    inExpression bool public,
    has16bitOperand byte public,
    phase byte public,
    curOpFlags byte public,
    effectiveToken byte public,
    newOp byte public,
    topOp byte public,
    b6B2C bool public,
    nextTokType byte public,
    finished bool public,
    inNestedParen byte public,
    expectingOperands bool public,
    expectingOpcode bool public,
    condAsmSeen bool public,    /* true when IF, ELSE, ENDIF seen (also macro to check) */
    b6B33 bool public,
    isInstr bool public initial(TRUE),
    expectOp bool public initial(TRUE),
    b6B36 bool public initial(FALSE),
    segSize(5) address public initial(0, 0, 0, 0, 0),    /* ABS, CODE, DATA, STACK, MEMORY */
    maxSegSize(3) address public initial(0, 0, 0),        /* seg is only ABS, CODE or DATA */
    cmdLineBuf(129) byte public,
    actRead pointer public,
    errCnt address public,
    padw6BCB address,    /* not used */
    w6BCE address public,
    pad6BD0(3) byte initial(0),
    azero address public initial(0),
    cmdch$p pointer public initial(.cmdLineBuf),
    controls$p address public,
    b6BD9 bool public initial(FALSE),
    b6BDA bool public,
    ii byte public,
    jj byte public,
    kk byte public;
/* end */
$IF MACRO
declare    b9B34 byte initial(0);
$ENDIF
/* ov4 compat 2F57 */
declare    curFileName$p address public,
    pAddr pointer public,
    accFixFlags(2) byte public at(.pAddr),
    controlFileType address public;     /* 1->INCLUDE 2->PRINT, 3->OBJECT or MACROFILE */
/* end */
end;
