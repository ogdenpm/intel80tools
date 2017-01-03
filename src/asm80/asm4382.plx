$IF OVL4
asm43: do;
$include(:f3:asm43.ipx)
$ELSE
asm82: do;
$include(:f3:asm82.ipx)
$ENDIF

declare tokReq(*) byte data(
            /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
               0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
               0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
               0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
               0, 0),
    b3EA0(*) byte data(36h, 0, 0, 0, 6, 0, 0, 2),
        /* bit vector 55 -> 0 x 24 00000110 0 x 16 0000001 */
        /* 29, 30, 55 */
    absValueReq(*) bool data(
            /* 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F */
               0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
               0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0FFh,0,   0,   0FFh,
               0,   0FFh,0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
               0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0FFh,0FFh,0FFh,0,   0FFh,0,
               0,   0);
    /* true for DS, ORG, IF, 3A?, IRP, IRPC REPT */ 
$IF OVL4
declare    b$3F88(*) byte data(41h, 90h, 0, 0, 0, 0, 0, 0, 0, 40h);
    /* bit vector 66 -> 10010000 0 x 56 01 */

SkipWhite$2: procedure public;
    do while GetCh = ' ' or IsTab;
    end;
end;


Sub3FA9: procedure byte public;
    declare w9B5A pointer,
        wrd based w9B5A address;

    w9B5A = curTokenSym$p - 6;
    return (wrd < 4679h) or ctlMACRODEBUG or (4682h < wrd);
end;



SeekM: procedure(blk);
    declare blk address;

    if (pAddr := blk - nxtMacroBlk) <> 0 then
    do;
        kk = SEEKFWD;
        if blk < nxtMacroBlk then
        do;
            pAddr = - pAddr;
            kk = SEEKBACK;
        end;

        call Seek(macrofd, kk, .pAddr, .w$3780, .statusIO);
        call IoErrChk;
    end;
    nxtMacroBlk = blk + 1;
end;



/* read in macro from disk - located at given block */
ReadM: procedure(blk) public;
    declare blk address;
    declare actual address;

    if blk >= maxMacroBlk then
        actual = 0;
    else if blk = curMacroBlk then
        return;
    else
    do;
        call SeekM(blk);
        call Read(macrofd, .macroBuf, 128, .actual, .statusIO);
        call IoErrChk;
    end;

    tmac$blk, curMacroBlk = blk;
    macroBuf(actual) = 0FEh;    /* flag end of macro buffer */
end;

/* write the macro to disk */
WriteM: procedure public;
    if phase = 1 then
    do;
        call SeekM(maxMacroBlk);
        maxMacroBlk = maxMacroBlk + 1;
        call Write(macrofd, symHighMark, 128, .statusIO);
        call IoErrChk;
    end;
    macroBlkCnt = macroBlkCnt + 1;
end;



FlushM: procedure public;
    declare bytesLeft address;

    if b905E then
    do;	/* spool macros to disk in 128 byte blocks */
        do while (bytesLeft := macroInPtr - symHighMark) >= 128;
            call WriteM;
            symHighMark = symHighMark + 128;
        end;
    /* move the remaining bytes to start of macro buffer */
        if bytesLeft <> 0 then
            call move(bytesLeft, symHighMark, endSymTab(TID$MACRO));
        macroInPtr = (symHighMark := endSymTab(TID$MACRO)) + bytesLeft;
    end;
end;

$ENDIF

SkipWhite: procedure public;
    do while IsWhite;
        curChar = GetCh;
    end;
end;


$IF BASE
SkipWhite$2: procedure public;
    do while GetCh = ' ' or IsTab;
    end;
end;
$ENDIF

Skip2NextLine: procedure public;
    call Skip2EOL;
    call ChkLF;
end;



Tokenise: procedure public;

    Sub416B: procedure;
        if rightOp = O$NONE then
            call ExpressionError;
        inExpression = 0;
        rightOp = O$NONE;
    end;


    do while 1;
    if atStartLine then
    do;
        call ParseControlLines;
        atStartLine = 0;
    end;

    do case GetChClass;
        case0:    call IllegalCharError;        /* CC$BAD */
        ;                /* CC$WS */
        do;                /* CC$SEMI */
$IF OVL4
            if not b9058 then
$ENDIF
            do;
                inComment = TRUE;
$IF OVL4
                if GetChClass = CC$SEMI and b905E then
                do;
                    b9059 = TRUE;
                    macroInPtr = macroInPtr - 2;
                end;
$ENDIF
                call Skip2NextLine;
                effectiveToken = T$CR;
                return;
            end;
        end;
        do;                /* CC$COLON */
            if not gotLabel then
            do;
                if skipping(0)
$IF OVL4
                   or b905E
$ENDIF
                then
                    call PopToken;
                else
                do;
                    labelUse = 2;
                    call UpdateSymbolEntry(segSize(activeSeg), O$TARGET);
                end;

                expectingOperands = FALSE;
                gotLabel, expectingOpcode = bTRUE;
            end;
            else
            do;
                call SyntaxError;
                call PopToken;
            end;

            call EmitXref(XREF$DEF, .name);
            rhsUserSymbol = FALSE;
            rightOp = O$LABEL;
        end;
        do;                /* CC$CR */
            call ChkLF;
            effectiveToken = T$CR;
$IF OVL4
            b9058 = 0;
$ENDIF
            return;
        end;
        do;                /* CC$PUNCT */
            if curChar = '+' or curChar = '-' then
$IF OVL4
                if not TestBit(rightOp, .b$3F88) then /* not 0, 3 or 41h */
$ELSE
                if rightOp <> O$NONE and rightOp <> T$RPAREN then
$ENDIF
                    curChar = curChar + (T$UPLUS - T$PLUS);    /* make unary versions */
            effectiveToken = curChar - '(' + T$LPAREN;
            return;
        end;
        do;                /* CC$DOLLAR */
            call PushToken(O$NUMBER);
            call CollectByte(low(segSize(activeSeg)));
            call CollectByte(high(segSize(activeSeg)));
            if activeSeg <> SEG$ABS then
                tokenAttr(0) = tokenAttr(0) or activeSeg or 18h;
            call Sub416B;
        end;
        do;                /* CC$QUOTE */
$IF OVL4
            if effectiveToken = 37h then
            do;
                call IllegalCharError;
                return;
            end;
            if b905E then
                b9058 = not b9058;
            else
$ENDIF
            do;
                call GetStr;
                if expectingOpcode then
                    call SetExpectOperands;
                call Sub416B;
            end;
        end;
        do;                /* CC$DIGIT */
            call GetNum;
            if expectingOpcode then
                call SetExpectOperands;
            call Sub416B;
        end;
        do;                /* CC$LET */
$IF OVL4
            w919F = macroInPtr - 1;
$ENDIF
            call GetId(O$ID);    /* assume it's an id */
            if tokenSize(0) > 6 then    /* cap length */
                tokenSize(0) = 6;

            if ctlXREF then
            do;
                call move(6, .name, .savName);
                call move(6, .spaces6, .name);
            end;
            /* copy the token to name */
            call move(tokenSize(0), tokPtr, .name);
            nameLen = tokenSize(0);
            call PackToken;        /* make into 4 byte name */
            if rhsUserSymbol then
            do;
                lhsUserSymbol = TRUE;
                rhsUserSymbol = FALSE;
            end;


$IF OVL4
            if Lookup(TID$MACRO) <> O$ID and b905E then
            do;
                if not b9058 or (kk := tokenType(0) = 0) and (curChar = '&' or byteAt(w919F-1) = '&') then
                do;
                    macroInPtr = w919F;
                    call InsertCharInMacroTbl(kk + 81h);
                    call InsertByteInMacroTbl(GetNumVal);
                    call InsertCharInMacroTbl(curChar);
                    effectiveToken = O$ID;
                end;
            end;
            else if effectiveToken <> O$37 and not b905E = 2 then
$ENDIF
            do;
                if Lookup(TID$KEYWORD) = O$ID then        /* not a key word */
                do;
                    tokenType(0) = Lookup(TID$SYMBOL);    /* look up in symbol space */
                    rhsUserSymbol = TRUE;        /* note we have a used symbol */
                end;

                effectiveToken = tokenType(0);
                needsAbsValue = absValueReq(tokenType(0)); /* DS, ORG, IF, O$3A, IRP, IRPC REPT */
                if not tokReq(tokenType(0)) then /* i.e. not instruction, reg or O$37 or 1,2,3,4,6,A */
                    call PopToken;

                if lhsUserSymbol then
                do;               /* EQU, SET or O$37 */
                    call EmitXref((not TestBit(effectiveToken, .b3EA0)) and 1, .savName);
                    lhsUserSymbol = FALSE;
                end;
            end;
$IF OVL4
            if b905E = 1 then
            do;
                if effectiveToken = K$LOCAL then
                do;
                    b905E = 2;
                    if b6897 then
                        call SyntaxError;
                    b6897 = FALSE;
                end;
                else
                do;
                    b6897 = FALSE;
                    b905E = 0FFh;
                end;
            end;

            if effectiveToken = K$NUL then
                call PushToken(40h);
$ENDIF
            if effectiveToken < 10 or effectiveToken = 9 or 80h then /* !! only first term contributes */
            do;
                call Sub416B;
                if expectingOpcode then
                    call SetExpectOperands;
            end;
            else
            do;
                expectingOpcode = FALSE;
                return;
            end;
        end;
$IF OVL4
        do;                /* 10? */
            b6BDA = FALSE;
            call Sub73AD;
            if b6BDA then
                return;
        end;
        do;                /* CC$ESC */
            if expandingMacro then
            do;
                skipping(0) = FALSE;
                effectiveToken = 40h;
                return;
            end;
            else
                goto case0;
        end;
$ENDIF
    end;
    end;
end;

end;
