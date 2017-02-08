$IF MACRO
asm2m: do;
/* force the non standard code generation of outStrN and put2Hex */
OutStrN: procedure(s, n) external; declare s address, n address; end;
Put2Hex: procedure(arg1w, arg2w) external; declare arg1w address, arg2w address; end;
$include(:f3:asm2m.ipx)
$ELSE
asm2n: do;
OutStrN: procedure(s, n) external; declare s address, n address; end;
$include(:f3:asm2n.ipx)
$ENDIF
/* 
    0    -> ?
    1    -> start single word expression
    5    -> single byte opcode no operand
    7    -> reg operand
    0Fh    -> binary topOp
    0Dh    -> unary topOp
    17h    -> IMM8 operand
    37h    -> imm16 operand
    40h    -> list
    47h    -> 2 operand topOp
    4Dh    -> start word list
    80h    -> end expression
    81h    -> rept operand
    0C0h    -> local operand

    -------x    -> getnum to acc1 & copy to acc2
    ------x-    -> getnum to acc1
    -----x--    -> collect low(acc1)
    ----x---    -> collect high(acc1)
    ---x----    -> collect low(acc2)
    --x-----    -> collect high(acc2)
    -x------    -> list
*/

declare opFlags(*) byte public data(
       /* 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F */
          0, 80h,   0,   0, 0Fh, 0Fh, 80h, 0Fh, 0Dh, 0Fh, 0Dh, 0Fh, 0Fh, 0Fh, 0Fh, 0Fh,
        0Fh, 0Dh, 0Fh, 0Fh, 0Fh, 0Fh, 0Fh, 0Fh, 0Dh, 0Dh, 40h, 4Dh,   1,   1,   1,   1,
        80h,   1,   0,   0, 47h,   7,   7,   7, 17h, 47h,   7, 47h, 37h,   5,   7,   0,
          0,   0, 40h, 40h,   0,   1
$IF MACRO
                                    , 80h, 40h, 80h,   0, 40h, 80h, 80h, 40h, 81h,0C0h,
        80h, 0Dh
$ENDIF
       ),

    noRegOperand(*) byte data(41h, 0, 0, 0, 19h, 40h, 0, 1Ch, 0, 0),
    /* bit vector 66 -> 0 x 24 00011001 01000000 00000000 00011100 00000000 00 */
    b41C1(*) byte data(1Ah, 5, 80h, 0, 0C0h),
    /* bit vector 27 -> 00000101 10000000 00000000 110 */
    opCompat(*) byte data(57h, 71h, 0F4h, 57h, 76h, 66h, 66h, 67h, 77h, 77h, 77h, 55h),
    /* bit vector 88 -> 01110001 11110100 01010111 01110110
                        01100110 01100110 01100111 01110111
                        01110111 01110111 01010101 */
    propagateFlags(*) byte data(57h, 6, 2, 20h, 0, 0, 0, 0, 0, 0, 0, 22h),
    /* bit vector 88 -> 00000110 00000010 00100000 00000000
                    00000000 00000000 00000000 00000000
                    00000000 00000000 00100010 */ 
    typeHasValue(*) byte data(3Ah, 0FFh, 80h, 0, 0, 0Fh, 0FEh, 0, 20h),
    /* bit vector 59 -> 11111111 10000000 00000000 00000000
                        00001111 11111110 00000000 001 */
    /* O$NONE, T$CR, T$LPAREN, T$RPAREN/O$LABEL, T$STAR, T$PLUS/K$SPECIAL, T$COMMA, */
    /* T$MINUS/K$REGNAME, T$UPLUS/K$SP */
    /* K$LXI, K$REG16, K$LDSTAX, K$ARITH, K$IMM8, K$MVI, K$INRDCR. K$MOV, K$IMM16, K$SINGLE */
    /* K$RST
/* precedence table */
/*
   10 - NULL
    9 - HIGH, LOW
    8 - *, /, MOD, SHL, SHR
    7 - +, -, UPLUS, UMINUS
    6 - =, <, <=, >, >=, <>
    5 - NOT
    4 - AND
    3 - OR, XOR,
    2 - not used
    1 - COMMA, DB - STKLEN, O$37, ENDM, EXITM, O$3D, REPT, LOCAL
    0 - all others
*/
    precedence(*) byte data(
     /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
        0, 0, 0, 0, 8, 7, 1, 7, 7, 8, 7, 6, 6, 6, 6, 6,
        6, 5, 4, 3, 3, 8, 8, 8, 9, 9, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1
$IF MACRO 
/* for macro ver */     , 0, 1, 1, 1, 0, 0, 0, 1, 1, 1,
        0, 0Ah
$ENDIF
     );


TestBit: procedure(bitIdx, bitVector) bool public;
    declare bitIdx byte, bitVector pointer;
    declare ch based bitVector byte;

    if ch < bitIdx then
        return FALSE;
    
    bitVector = bitVector + shr(bitIdx, 3) + 1;
    return (ch and ROR(1, (bitIdx and 7) + 1)) <> 0;
end;

IsReg: procedure(topOp) bool public;
    declare topOp byte;

    return topOp = K$REGNAME or topOp = K$SP;
end;

ChkInvalidRegOperand: procedure public;
    if TestBit(topOp, .noRegOperand) then
        if IsReg(acc1ValType) then
            call OperandError;
end;

Sub4291: procedure public;
    if IsReg(acc1ValType) then
        call OperandError;
    if (opFlags(topOp) and 2) = 0 then
        acc2Flags = 0;
    else if IsReg(acc2ValType) then
        call OperandError;

    acc1ValType = O$NUMBER;
    accFixFlags(0) = (acc1Flags and UF$BOTH) <> 0;
    accFixFlags(1) = (acc2Flags and UF$BOTH) <> 0;
    if (acc1Flags and UF$SEGMASK) <> SEG$ABS then
        if (acc2Flags and UF$SEGMASK) <> SEG$ABS then
            if ((acc1Flags xor acc2Flags) and 1Fh) <> 0 then
                call ExpressionError;
    if (ii := (acc1Flags and UF$EXTRN) <> 0) or (jj := (acc2Flags and UF$EXTRN) <> 0) then
    do;
        if topOp = K$SPECIAL then    /* +? (PAGE INPAGE)? */
            if not (ii or accFixFlags(0)) then
            do;
                acc1NumVal = acc2NumVal;
                acc1Flags = acc2Flags;
                return;
            end;
        if jj or accFixFlags(1) or not TestBit(topOp, .b41C1) then
            goto L4394;
        else
            return;
    end;
    kk = shl(topOp - 4, 2) or (accFixFlags(0) and 2) or (accFixFlags(1) and 1);
    if TestBit(kk, .opCompat) then
L4394:    do;
        call ExpressionError;
        acc1Flags = 0;
        return;
    end;
    if TestBit(kk, .propagateFlags) then
    do;
        if not accFixFlags(0) then
            acc1Flags = acc2Flags;
        return;
    end;
    acc1Flags = 0;
end;


SwapAccBytes: procedure public;
    declare tmp byte;
    tmp = accum1$lb;
    accum1$lb = accum1$hb;
    accum1$hb = tmp;
end;



SetExpectOperands: procedure public;
    expectingOperands = TRUE;
    expectingOpcode = FALSE;
end;



GetNumVal: procedure address public;
    declare tokByte based tokPtr (1) byte,    /* ptr into token info */
        val$p pointer,
        val based val$p address;
$IF MACRO
    LogError: procedure(ch);
        declare ch byte;

        if tokenType(tokenIdx) <> O$OPTVAL then
        do;
            call SourceError(ch);
            return;
        end;
        if tokenSize(0) = 0 then
            tokenType(tokenIdx) = K$NUL;
    end;
$ENDIF

    acc1Flags = 0;
    accum1 = 0;
    acc1ValType = O$ID;
$IF MACRO
    if tokenType(0) = O$OPTVAL then
        call PushToken(CR);
$ENDIF
    if tokenIdx = 0 or tokenType(0) = O$DATA and not b6B36 then
$IF MACRO
        call LogError('Q');
$ELSE
        call SyntaxError;
$ENDIF
    else
    do;
        if tokenType(0) = O$ID or tokenType(0) = T$COMMA then
$IF MACRO
            call LogError('U');
$ELSE
            call UndefinedSymbolError;
$ENDIF
        else
        do;
            acc1ValType = tokenType(0);
            if TestBit(acc1ValType, .typeHasValue) then
            do;
                tokPtr = curTokenSym$p + 7;    /* point to flags */
                acc1Flags = tokByte(0) and not UF$PUBLIC; /* remove public attribute */
                tokPtr, val$p = curTokenSym$p + 4;    /* point to value */
                acc1NumVal = val;            /* pick up value */
                tokenSize(0) = 2;        /* word value */

            end;
            else if tokenSize(0) = 0 then
$IF MACRO
                call LogError('V');
$ELSE
                call ValueError;
$ENDIF
            else
            do;
                if tokenSize(0) > 2 then
$IF MACRO
                    call LogError('V');
$ELSE
                    call ValueError;
$ENDIF
                acc1Flags = tokenAttr(0) and not UF$PUBLIC;    /* remove public attribute */
                acc1NumVal = tokenSymId(0);        /* use the symbol Id */
            end;

            if tokenSize(0) > 0 then    /* get low byte */
                accum1$lb = tokByte(0);
            if tokenSize(0) > 1 then    /* and high byte if not a register */
                accum1$hb = tokByte(1) and tokenType(0) <> 7;
        end;    

        if has16bitOperand then
            if tokenSize(0) = 2 then
                if tokenType(0) = O$STRING then
                    call SwapAccBytes;

        if (acc1Flags and UF$EXTRN) <> 0 then
            if tokenType(0) < 9 then
                accum1 = 0;

        call PopToken;
    end;

    b6B36 = FALSE;
    return accum1;
end;


GetPrec: procedure(topOp) byte public;
    declare topOp byte;
    return precedence(topOp);
end;

/*
   arg1b
   xxxxxxx1    acc2 -> 16 bit reg
   xxxxxx1x    acc1 = acc1 | acc2
   xxxxx1xx    acc2 <<= 3
   xxxx1xxx    acc2 -> 8 bit value
   nnnnxxxx    topOp = 24h + nnnn
   
*/   
MkCode: procedure(arg1b) public;
    declare arg1b byte;

    if (arg1b and 3) <> 0 then    /* lxi, ldax, stax, regarith, mvi, mov, rst */
    do;
        if accum2$hb <> 0    /* reg or rst num <= 7 */
           or accum2$lb > 7     
           or arg1b and accum2$lb    /* only B D H SP if lxi, ldax or stax */
           or (arg1b and 3) = 3 and accum2$lb > 2    /* B or D if ldax or stax */
           or (not IsReg(acc2ValType) and topOp <> K$RST) then    /* reg unless rst */
            call OperandError;
        else if IsReg(acc2ValType) and topOp = K$RST then         /* cannot be reg for rst */
            call OperandError;
        if ror(arg1b, 2) then
            accum2$lb = rol(accum2$lb, 3);
        accum1$lb = accum1$lb or accum2$lb;
    end;
    else if topOp <> K$SINGLE then        /* single byte topOp */
        if IsReg(acc2ValType) then
            call OperandError;

    if shr(arg1b, 3) then
    do;
        if (acc2Flags and UF$BOTH) = UF$BOTH then
        do;
            call ValueError;
            acc2Flags = (acc2Flags and not UF$BOTH)  or UF$LOW;
        end;
        if accum2$hb + 1 > 1 then    /* Error if not FF or 00 */
            call ValueError;
    end;
    if topOp = K$IMM8 or topOp = K$IMM16 then    /* Imm8 or imm16 */
    do;
        acc1Flags = acc2Flags;
        acc1NumVal = acc2NumVal;
    end;
    else
        acc1Flags = 0;

    if topOp <> K$SINGLE then             /* single byte topOp */
        if accum1$lb = 76h then         /* mov m,m is actually Halt */
            call OperandError;
    if (topOp := shr(arg1b, 4) + 24h) = 24h then
        nextTokType = O$DATA;
end;

NxtTokI: procedure byte public;
    if tokI >= tokenIdx then
        return 0;
    return (tokI := tokI + 1);
end;



ShowLine: procedure byte public;
    return ((not isControlLine) and ctlLIST or ctlLISTChanged and isControlLine)
$IF MACRO
            and (not (expandingMacro > 1) or ctlGEN)
$ENDIF
        and (not(condAsmSeen or skipIf(0)) or ctlCOND);
end;

/*
    xrefMode= 0 -> defined
        = 1 -> used
        = 2 -> finalise
*/
EmitXref: procedure(xrefMode, name) public;
    declare xrefMode byte, name address;
    declare (i, byteval) byte;
    declare (srcLineLow, srcLineHigh) byte at(.srcLineCnt);

    if not IsPhase1 or not ctlXREF or IsSkipping and not xRefPending then
        return;

    call Outch(xrefMode + '0');    /* convert to hex char */
    if xrefMode <> XREF$FIN then    /* not finalise */
    do;
        call OutStrN(name, 6);        /* label ref */
        xRefPending = FALSE;
        byteval = srcLineHigh;    /* line number in hex - high byte first */
        i = 0;
        do while i < 4;
            i = i + 1;
            if i then    /* high nibble ? */
            do;
                if i = 3 then    /* get low byte */
                    byteval = srcLineLow;
                /* emit high nibble */
                call Outch(Nibble2Ascii(shr(byteval, 4)));
            end;
            else    /* emit low nibble */
                call Outch(Nibble2Ascii(byteval));
        end;
    end;
    else    /* finalise */
    do;
        call OutStrN(.lstFile, 15);    /* listing file name */
        if ctlPAGING then        /* whether paging '1' or '0' */
            call Outch('1');
        else
            call Outch('0');
        /* page length and page width as 2 hex chars */
        call Outch(Nibble2Ascii(ror(ctlPAGELENGTH, 4)));
        call Outch(Nibble2Ascii(ctlPAGELENGTH));
        call Outch(Nibble2Ascii(ror(ctlPAGEWIDTH, 4)));
        call Outch(Nibble2Ascii(ctlPAGEWIDTH));
        call Outch('3');    /* end of file */
        call Flushout;
        call CloseF(xreffd);
    end;
end;
end;
