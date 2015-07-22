asm4D$OV1:
do;
/* to force the code generation this needs a non-standard definition
   of put2Hex
*/
put2Hex: procedure(arg1w, arg2w) external; declare arg1w address, arg2w address; end;

$IF OVL4
$include(asm4d.ipx)
$ELSE
$include(asmov1.ipx)
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


out2Hex: procedure(n);
    declare n byte;
    call put2Hex(.outch, n);
end;


print2Hex: procedure(n);
    declare n byte;
    call put2Hex(.printChar, n);
end;



printStr: procedure(str) reentrant;
    declare str address;
    declare ch based str byte;

    do while ch <> 0;
        call printChar(ch);
        str = str + 1;
    end;
end;

printNStr: procedure(cnt, str) reentrant;
    declare cnt byte, str address;
    declare ch based str byte;

    do while cnt > 0;
        call printChar(ch);
        str = str + 1;
        cnt = cnt - 1;
    end;
end;


printCRLF: procedure reentrant;
    call printChar(CR);
    call printChar(LF);
end;

declare aNumStr(*) byte initial('     ', 0);


itoa: procedure(n, buf);
    declare (n, buf) address;
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


printDecimal: procedure(n) reentrant public;
    declare n address;
    call itoa(n, .aNumStr);
    call printStr(.aNumStr(1));
end;

skipToEOP: procedure public;
    do while pageLineCnt <= ctlPAGELENGTH;
        call outch(LF);
        pageLineCnt = pageLineCnt + 1;
    end;
end;


newPageHeader: procedure public;
    call printStr(.topLFs);
    call printStr(.asmHeader);
    call printDecimal(pageCnt);
    call printCRLF;
    if ctlTITLE then
        call printNStr(titleLen, .ctlTITLESTR);

    call printCRLF;
    call printCRLF;
    if not b68AE then
        call printStr(.lstHeader);
    pageCnt = pageCnt + 1;
end;


newPage: procedure public;
    if ctlTTY then
        call skipToEOP;
    else
        call outch(FF);

    pageLineCnt = 1;
    if not scanCmdLine then
        call newPageHeader;
end;


doEject: procedure public;
    if showLine then
    do while ctlEJECT > 0;
        call newPage;
        ctlEJECT = ctlEJECT - 1;
    end;
end;




printChar: procedure(c) reentrant;
    declare c byte;
    declare cnt byte;

    if c = FF then
    do;
        call newPage;
        return;
    end;

    if c = LF then
        if ctlPAGING then
        do;
            if (pageLineCnt := pageLineCnt + 1) >= ctlPAGELENGTH - 2 then
            do;
                if ctlTTY then
                    call outch(LF);
                if ctlEJECT > 0 then
                    ctlEJECT = ctlEJECT - 1;
                call newPage;
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
                call printCRLF;
                call printStr(.spaces24);
                curCol = curCol + 1;
            end;
            call outch(c);
        end;
        cnt = cnt - 1;
    end;
end;

declare segChar(*) byte initial(' CDSME');	/* seg id char */

sub7041$8447: procedure public;
    declare symGrp byte,
        flagsAndType address,
        (type, flags) byte at(.flagsAndType),
        zeroAddr byte;
    declare tokBytePair based curTokenSym$p address;

    printAddr2: procedure(printFunc);
        declare printFunc address;
        declare ch based curTokenSym$p byte;

        curTokenSym$p = curTokenSym$p - 1;	/* backup into value */
        call printFunc(ch and not zeroAddr);	/* print address or 0 */
    end;


    b68AE = TRUE;
    if not ctlSYMBOLS then
        return;

    segChar(0) = 'A';		/* show A instead of space for absolute */
    do symGrp = 0 to 2;
        jj = isPhase2Print and ctlSYMBOLS;
$IF OVL4
        ctlDEBUG = ctlDEBUG or ctlMACRODEBUG;
$ENDIF
        curTokenSym$p = symTab(1) - 2;		/* address user sym(-1).type */
        call printCRLF;
        call printStr(symbolMsgTable(symGrp));

        do while (curTokenSym$p := curTokenSym$p + 8) < endSymTab(1);
            flagsAndType = tokBytePair;
            if type <> 9 then
                if type <> 6 then
$IF OVL4
                    if sub$3FA9 then
$ENDIF
                        if symGrp <> 0 or type <> 3 then
                            if symGrp = 2 or (flags and b6DC1(symGrp)) <> 0 then
                            do;
                                call unpackToken(curTokenSym$p - 6, .tokStr);
                                if jj then
                                do;
                                    if (ctlPAGEWIDTH - curCol) < 11h then
                                        call printCRLF;

                                    call printStr(.tokStr);
                                    call printChar(' ');
$IF OVL4
                                    if type = O$3A then
                                        call printChar('+');
                                    else
$ENDIF
				    if (zeroAddr := (flags and 40h) <> 0) then
                                        call printChar('E');
                                    else
                                        call printChar(segChar(flags and 7));

                                    call printChar(' ');
                                    call printAddr2(.print2Hex);
                                    call printAddr2(.print2Hex);
                                    curTokenSym$p = curTokenSym$p + 2;
                                    call printStr(.spaces4);
                                end;
                            end;
        end;
    end;

    if ctlDEBUG then
        b68AE = FALSE;

    if jj then
        call printCRLF;
end;

printCmdLine: procedure public;
    declare ch based actRead byte;

    call outch(FF);
    call doEject;
    ch = 0;
    call printStr(.cmdLineBuf);
    call newPageHeader;
end;


outStr: procedure(s) reentrant public;
    declare s address;
    declare ch based s byte;

    do while ch <> 0;
        call outch(ch);
        s = s + 1;
    end;
end;

outNStr: procedure(cnt, s) reentrant;
    declare cnt byte, s address,
        ch based s byte;

    do while cnt > 0;
        call outch(ch);
        s = s + 1;
        cnt = cnt - 1;
    end;
end;


moreBytes: procedure byte public;
    return startItem < endItem;
end;



sub$7229: procedure public;
    declare ch based startItem byte;
    declare i byte;

    if (showAddr := moreBytes or showAddr) then
    do;	/* print the address */
        call out2Hex(high(effectiveAddr));
        call out2Hex(low(effectiveAddr));
    end;
    else
        call outStr(.spaces4);

    call outch(' ');
    do  i = 1 to 4;
        if moreBytes and b6B34 then
        do;
            effectiveAddr = effectiveAddr + 1;
            call out2Hex(ch);
        end;
        else
            call outStr(.spaces2);

        startItem = startItem + 1;
    end;

    call outch(' ');
    if shr(jj := tokenAttr(spIdx), 6) then
        call outch('E');
    else if not showAddr then
        call outch(' ');
    else
        call outch(segChar(jj and 7));
end;


sub$72D8: procedure public;
    if not b689B then
        return;

    call printStr(.ascLParen);    /* " (" */
    call printNStr(4, .b6A57);
    call printStr(.ascRParen);    /* ")" */
    call printCRLF;
    call move(4, .asciiLineNo, .b6A57);
end;



printLine: procedure public;
    declare ch based inCh$p byte;
$IF OVL4
    declare ch1 based macro$p byte;
$ENDIF
loop:
    endItem = (startItem := tokStart(spIdx)) + tokenSize(spIdx);
    if isSkipping then
        endItem = startItem;

    call outch(asmErrCode);
$IF OVL4
    if b905E = 0FFh then
        call outch('-');
    else
$ENDIF
        call outch(' ');

    if not blankAsmErrCode then
    do;
        asmErrCode = ' ';
        b689B = TRUE;
    end;
    if isControlLine then
        call outStr(.spaces15);
    else
        call sub$7229;

    if fileIdx > 0 then
    do;
	/* note byte arith used so needToOpenFile = TRUE(255h) treated as -1 */
        call outch(a1234(ii := needToOpenFile + fileIdx));
        if ii > 0 then    
            call outch('=');
        else
            call outch(' ');
    end;
    else
        call outStr(.spaces2);

    if b68AD then
    do;
        call outStr(.spaces4);
        call printCRLF;
    end;
    else
    do;
        b68AD = 0FFh;
        call outNStr(4, .asciiLineNo);
$IF OVL4
        if expandingMacro > 1 then
            call outch('+');
        else
$ENDIF
            call outch(' ');
$IF OVL4
        if expandingMacro > 1 then
        do;
            curCol = 24;
            ch1 = 0;
            call printStr(.macroLine);
            call printChar(LF);
        end;
        else
        do;
$ENDIF
            curCol = 24;
            call printNStr(lineChCnt, startLine$p);
	    if ch <> LF then
                 call printChar(LF);
$IF OVL4
        end;
$ENDIF
    end;

    if isControlLine then
    do;
        if ctlPAGING then
            call doEject;
    end;
    else
    do;
        do while moreBytes;
            call outStr(.spaces2);
            call sub$7229;
            call printCRLF;
        end;

        if spIdx > 0 and (inDB or inDW) then
        do;
            call sub$546F;
            goto loop;
        end;
    end;

    call sub$72D8;
end;

asmComplete: procedure public;
    if errCnt > 0 then
        call itoa(errCnt, .aNoErrors);
    call printNStr((errCnt = 1) + 32, .aAssemblyComple);
    if errCnt > 0 then
    do;
        call move(4, .b6A57, .space5RP);
        call printNStr(8, .spaceLP);
    end;
    call outch(CR);
    call outch(LF);
end;

ovl9: procedure public;
    if ctlPRINT then
        call closeF(outfd);
    outfd = 0;
    pageLineCnt = 1;
    call asmComplete;
    call flushout;
end;

ovl10: procedure public;
    declare ch based effectiveAddr byte;

    call closeF(infd);
$IF OVL4
    call closeF(macrofd);
    call delete(.asmax$ref, .statusIO);
    if ctlOBJECT then
        call closeF(objfd);
$ENDIF
    if ctlXREF then
    do;
        effectiveAddr = physmem - 1;
        ch = '0';
        if asxref$tmp(0) = ':' then
            ch = asxref$tmp(2);
    
        call load(.asxref, 0, 1, 0, .statusIO);
        call ioErrChk;
    end;

    call exit;
end;
end;
