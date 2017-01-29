$IF MACRO
listm: do;
/* to force the code generation this needs a non-standard definition of put2Hex */
Put2Hex: procedure(arg1w, arg2w) external; declare arg1w address, arg2w address; end;
$include(:f3:listm.ipx)
$ELSE
listn: do;
Put2Hex: procedure(arg1w, arg2w) external; declare arg1w address, arg2w address; end;
$include(:f3:listn.ipx)
$ENDIF

declare aAssemblyComple(*) byte initial(CR, LF, 'ASSEMBLY COMPLETE,'),
    aNoErrors(*) byte initial('   NO ERRORS'),
    spaceLP(*) byte initial(' ('),
    space5RP(*) byte initial('     )'),
    aPublicSymbols(*) byte initial(CR, LF, 'PUBLIC SYMBOLS', CR, LF, 0),
    aExternalSymbol(*) byte initial(CR, LF, 'EXTERNAL SYMBOLS', CR, LF, 0),
    pad754E address,

    aUserSymbols(*) byte data(CR, LF, 'USER SYMBOLS', CR, LF, 0),
    lstHeader(*) byte data('  LOC  OBJ         LINE        SOURCE STATEMENT', CR, LF, LF, 0),
    symbolMsgTable(*) address data(.aPublicSymbols, .aExternalSymbol, .aUserSymbols),
    aCRLFLF(*) byte data(CR, LF, LF, 0),
    aCR(*) byte data(CR),
    topLFs(*) byte data(LF, LF, LF, 0),
    b6DC1(2) byte data(20h, 40h),
    ascLParen(*) byte data(' (', 0),
    ascRParen(*) byte data(')', 0),
    a1234(*) byte data('  1234');


Out2Hex: procedure(n);
    declare n byte;
    call Put2Hex(.outch, n);
end;


Print2Hex: procedure(n);
    declare n byte;
    call Put2Hex(.printChar, n);
end;



PrintStr: procedure(str) reentrant;
    declare str pointer;
    declare ch based str byte;

    do while ch <> 0;
        call PrintChar(ch);
        str = str + 1;
    end;
end;

PrintNStr: procedure(cnt, str) reentrant;
    declare cnt byte, str pointer;
    declare ch based str byte;

    do while cnt > 0;
        call PrintChar(ch);
        str = str + 1;
        cnt = cnt - 1;
    end;
end;


PrintCRLF: procedure reentrant;
    call PrintChar(CR);
    call PrintChar(LF);
end;

declare aNumStr(*) byte initial('     ', 0);


Itoa: procedure(n, buf);
    declare n address, buf pointer;
    declare ch based buf byte;

    call move(5, .spaces5, buf);
    buf = buf + 4;

    do while 1;
        ch = n mod 10 + '0';
        buf = buf - 1;
        if (n := n /10) = 0 then
            return;
    end;
end;


PrintDecimal: procedure(n) reentrant public;
    declare n address;
    call Itoa(n, .aNumStr);
    call PrintStr(.aNumStr(1));
end;

SkipToEOP: procedure public;
    do while pageLineCnt <= ctlPAGELENGTH;
        call Outch(LF);
        pageLineCnt = pageLineCnt + 1;
    end;
end;


NewPageHeader: procedure public;
    call PrintStr(.topLFs);
    call PrintStr(.asmHeader);
    call PrintDecimal(pageCnt);
    call PrintCRLF;
    if ctlTITLE then
        call PrintNStr(titleLen, .ctlTITLESTR);

    call PrintCRLF;
    call PrintCRLF;
    if not b68AE then
        call PrintStr(.lstHeader);
    pageCnt = pageCnt + 1;
end;


NewPage: procedure public;
    if ctlTTY then
        call SkipToEOP;
    else
        call Outch(FF);

    pageLineCnt = 1;
    if not scanCmdLine then
        call NewPageHeader;
end;


DoEject: procedure public;
    if ShowLine then
    do while ctlEJECT > 0;
        call NewPage;
        ctlEJECT = ctlEJECT - 1;
    end;
end;




PrintChar: procedure(c) reentrant;
    declare c byte;
    declare cnt byte;

    if c = FF then
    do;
        call NewPage;
        return;
    end;

    if c = LF then
        if ctlPAGING then
        do;
            if (pageLineCnt := pageLineCnt + 1) >= ctlPAGELENGTH - 2 then
            do;
                if ctlTTY then
                    call Outch(LF);
                if ctlEJECT > 0 then
                    ctlEJECT = ctlEJECT - 1;
                call NewPage;
                return;
            end;
        end;

    if c = CR then
        curCol = 0;

    cnt = 1;
    if c = TAB then
    do;
        cnt = 8 - (curCol and 7);
        c = ' ';
    end;

    do while cnt <> 0;
        if curCol < 132 then
        do;
            if c >= ' ' then
                curCol = curCol + 1;
            if curCol > ctlPAGEWIDTH then
            do;
                call PrintCRLF;
                call PrintStr(.spaces24);
                curCol = curCol + 1;
            end;
            call Outch(c);
        end;
        cnt = cnt - 1;
    end;
end;

declare segChar(*) byte initial(' CDSME');    /* seg id char */

Sub7041$8447: procedure public;
    declare symGrp byte,
        flagsAndType address,
        (type, flags) byte at(.flagsAndType),
        zeroAddr byte;
    declare tokBytePair based curTokenSym$p address;

    PrintAddr2: procedure(printFunc);
        declare printFunc address;
        declare ch based curTokenSym$p byte;

        curTokenSym$p = curTokenSym$p - 1;    /* backup into value */
        call printFunc(ch and not zeroAddr);    /* print address or 0 */
    end;


    b68AE = TRUE;
    if not ctlSYMBOLS then
        return;

    segChar(0) = 'A';        /* show A instead of space for absolute */
    do symGrp = 0 to 2;
        kk = IsPhase2Print and ctlSYMBOLS;
$IF MACRO
        ctlDEBUG = ctlDEBUG or ctlMACRODEBUG;
$ENDIF
        curTokenSym$p = symTab(TID$SYMBOL) - 2;        /* address user sym(-1).type */
        call PrintCRLF;
        call PrintStr(symbolMsgTable(symGrp));

        do while (curTokenSym$p := curTokenSym$p + 8) < endSymTab(TID$SYMBOL);
            flagsAndType = tokBytePair;
            if type <> 9 then
                if type <> 6 then
$IF MACRO
                    if Sub3FA9 then
$ENDIF
                        if symGrp <> 0 or type <> 3 then
                            if symGrp = 2 or (flags and b6DC1(symGrp)) <> 0 then
                            do;
                                call UnpackToken(curTokenSym$p - 6, .tokStr);
                                if kk then
                                do;
                                    if (ctlPAGEWIDTH - curCol) < 17 then
                                        call PrintCRLF;

                                    call PrintStr(.tokStr);
                                    call PrintChar(' ');
$IF MACRO
                                    if type = O$3A then
                                        call PrintChar('+');
                                    else
$ENDIF
                    if (zeroAddr := (flags and UF$EXTRN) <> 0) then
                                        call PrintChar('E');
                                    else
                                        call PrintChar(segChar(flags and UF$SEGMASK));

                                    call PrintChar(' ');
                                    call PrintAddr2(.print2Hex);
                                    call PrintAddr2(.print2Hex);
                                    curTokenSym$p = curTokenSym$p + 2;
                                    call PrintStr(.spaces4);
                                end;
                            end;
        end;
    end;

    if ctlDEBUG then
        b68AE = FALSE;

    if kk then
        call PrintCRLF;
end;

PrintCmdLine: procedure public;
    declare ch based actRead byte;

    call Outch(FF);
    call DoEject;
    ch = 0;
    call PrintStr(.cmdLineBuf);
    call NewPageHeader;
end;


OutStr: procedure(s) reentrant public;
    declare s address;
    declare ch based s byte;

    do while ch <> 0;
        call Outch(ch);
        s = s + 1;
    end;
end;

OutNStr: procedure(cnt, s) reentrant;
    declare cnt byte, s address,
        ch based s byte;

    do while cnt > 0;
        call Outch(ch);
        s = s + 1;
        cnt = cnt - 1;
    end;
end;


MoreBytes: procedure byte public;
    return startItem < endItem;
end;



PrintCodeBytes: procedure public;
    declare ch based startItem byte;
    declare i byte;

    if (showAddr := MoreBytes or showAddr) then
    do;    /* print the address */
        call Out2Hex(high(effectiveAddr));
        call Out2Hex(low(effectiveAddr));
    end;
    else
        call OutStr(.spaces4);

    call Outch(' ');
    do  i = 1 to 4;
        if MoreBytes and isInstr then
        do;
            effectiveAddr = effectiveAddr + 1;
            call Out2Hex(ch);
        end;
        else
            call OutStr(.spaces2);

        startItem = startItem + 1;
    end;

    call Outch(' ');
    if shr(kk := tokenAttr(spIdx), 6) then    /* UF$EXTRN */
        call Outch('E');
    else if not showAddr then
        call Outch(' ');
    else
        call Outch(segChar(kk and 7));
end;


PrintErrorLineChain: procedure public;
    if not errorOnLine then
        return;

    call PrintStr(.ascLParen);    /* " (" */
    call PrintNStr(4, .lastErrorLine);
    call PrintStr(.ascRParen);    /* ")" */
    call PrintCRLF;
    call move(4, .asciiLineNo, .lastErrorLine);
end;



PrintLine: procedure public;
    declare ch based inCh$p byte;
$IF MACRO
    declare ch1 based macro$p byte;
$ENDIF
loop:
    endItem = (startItem := tokStart(spIdx)) + tokenSize(spIdx);
    if IsSkipping then
        endItem = startItem;

    call Outch(asmErrCode);
$IF MACRO
    if b905E = 0FFh then
        call Outch('-');
    else
$ENDIF
        call Outch(' ');

    if not BlankAsmErrCode then
    do;
        asmErrCode = ' ';
        errorOnLine = TRUE;
    end;
    if isControlLine then
        call OutStr(.spaces15);
    else
        call PrintCodeBytes;

    if fileIdx > 0 then
    do;
    /* note byte arith used so needToOpenFile = TRUE(255) treated as -1 */
        call Outch(a1234(ii := needToOpenFile + fileIdx));
        if ii > 0 then    
            call Outch('=');
        else
            call Outch(' ');
    end;
    else
        call OutStr(.spaces2);

    if lineNumberEmitted then
    do;
        call OutStr(.spaces4);
        call PrintCRLF;
    end;
    else
    do;
        lineNumberEmitted = TRUE;
        call OutNStr(4, .asciiLineNo);
$IF MACRO
        if expandingMacro > 1 then
            call Outch('+');
        else
$ENDIF
            call Outch(' ');
$IF MACRO
        if expandingMacro > 1 then
        do;
            curCol = 24;
            ch1 = 0;
            call PrintStr(.macroLine);
            call PrintChar(LF);
        end;
        else
        do;
$ENDIF
            curCol = 24;
            call PrintNStr(lineChCnt, startLine$p);
        if ch <> LF then
                 call PrintChar(LF);
$IF MACRO
        end;
$ENDIF
    end;

    if isControlLine then
    do;
        if ctlPAGING then
            call DoEject;
    end;
    else
    do;
        do while MoreBytes;
            call OutStr(.spaces2);
            call PrintCodeBytes;
            call PrintCRLF;
        end;

        if spIdx > 0 and (inDB or inDW) then
        do;
            call Sub546F;
            goto loop;
        end;
    end;

    call PrintErrorLineChain;
end;

AsmComplete: procedure public;
    if errCnt > 0 then
        call Itoa(errCnt, .aNoErrors);
    call PrintNStr((errCnt = 1) + 32, .aAssemblyComple);
    if errCnt > 0 then
    do;
        call move(4, .lastErrorLine, .space5RP);
        call PrintNStr(8, .spaceLP);
    end;
    call Outch(CR);
    call Outch(LF);
end;

Ovl9: procedure public;
    if ctlPRINT then
        call CloseF(outfd);
    outfd = 0;
    pageLineCnt = 1;
    call AsmComplete;
    call Flushout;
end;

Ovl10: procedure public;
    declare ch based effectiveAddr byte;

    call CloseF(infd);
$IF MACRO
    call CloseF(macrofd);
    call Delete(.asmax$ref, .statusIO);
    if ctlOBJECT then
        call CloseF(objfd);
$ENDIF
    if ctlXREF then
    do;
        effectiveAddr = Physmem - 1;
        ch = '0';
        if asxref$tmp(0) = ':' then
            ch = asxref$tmp(2);
    
        call Load(.asxref, 0, 1, 0, .statusIO);
        call IoErrChk;
    end;

    call Exit;
end;
end;
