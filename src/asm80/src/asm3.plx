$IF MACRO
asm3m: do;
$include(:f3:asm3m.ipx)
$ELSEIF BIG
asm3b: do;
$include(:f3:asm3b.ipx)
$ELSE
asm3s: do;
$include(:f3:asm3s.ipx)
$ENDIF

$IF SMALL
declare    CHKOVL$2 lit    'call OvlMgr(2)';
$ELSE
declare    CHKOVL$2 lit    ' ';
$ENDIF

            /* 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F */
declare isExprOrMacroMap(*) bool data(
               0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,
               0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0,   0,   0,   0,   0,   0,
               0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
               0,   0,   0,   0,   0,   0,   0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh, 0,  0FFh,
               0FFh,0FFh),
   isInstrMap(*) bool data(
               0,   0FFh,0FFh,0FFh,0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
               0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0FFh,0FFh,0,   0,   0,   0,
               0,   0,   0,   0,   0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0FFh,0,
               0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
               0FFh,0FFh);

declare pad byte;

declare    accum(4) byte at(.accum1);


ChkSegAlignment: procedure(seg) public;    /* seg = 0 => CSEG, seg = 1 => DSEG */
    declare seg byte;

    if segHasData(seg) then
    do;
        if HaveTokens then
        do;
            accum1 = GetNumVal;
            if alignTypes(seg) <> accum1$lb then
                call ExpressionError;
        end;
        else if alignTypes(seg) <> 3 then    /* no speficier - check byte algined */
            call ExpressionError;
    end;
    else
    do;
        segHasData(seg) = TRUE;
        if HaveTokens then
        do;
            accum1 = GetNumVal;
            if accum1 = 1 or accum1 = 2 then    /* only allow inPage and Page */
                alignTypes(seg) = low(accum1);
            else
                call ExpressionError;
        end;
    end;
end;


Cond2Acc: procedure(cond) public;    /* convert conditional result to accum1 */
    declare cond byte;        /* 0 or 0ffffh */

    accum1 = cond;
    if accum1 then
        accum1 = 0FFFFh;
end;

UpdateHiLo: procedure(hilo) public;
    declare hilo byte;

    if (acc1Flags and (UF$EXTRN + UF$SEGMASK)) <> 0 then
        acc1Flags = (acc1Flags and not UF$BOTH) or hilo;
end;

HandleOp: procedure public;
    do case topOp;
/* 0 */        ;
/* 1 */        call FinishLine;        /* CR */
/* 2 */        goto case3;            /* ( */
/* 3 */ case3:    do;                /* ) */
            if not (topOp = T$LPAREN and newOp = T$RPAREN) then
                call BalanceError;

            if tokenType(0) = O$DATA then
            do;
                tokenSize(0) = 1;
                tokenAttr(0) = 0;
                b6B36 = TRUE;
            end;

            expectOp = inNestedParen;
            if newOp = T$RPAREN then
                b6B2C = TRUE;
        end;
/* 4 */        accum1 = accum1 * accum2;    /* * */
/* 5 */        accum1 = accum1 + accum2;    /* + */
/* 6 */        do;                /* , */
            call SyntaxError;
            call PopToken;
        end;
/* 7 */        accum1 = accum1 - accum2;    /* - */
/* 8 */        ;                /* unary + */
/* 9 */        do;                /* / */
            if accum2 = 0 then
                call ValueError;

            accum1 = accum1 / accum2;
        end;
/* 10 */    accum1 = -accum1;            /* unary - */
/* 11 */    call Cond2Acc(accum1 = accum2);        /* EQ */
/* 12 */    call Cond2Acc(accum1 < accum2);        /* LT */
/* 13 */    call Cond2Acc(accum1 <= accum2);    /* LE */
/* 14 */    call Cond2Acc(accum1 > accum2);        /* GT */
/* 15 */    call Cond2Acc(accum1 >= accum2);    /* GE */
/* 16 */    call Cond2Acc(accum1 <> accum2);    /* NE */
/* 17 */    accum1 = not accum1;            /* NOT */
/* 18 */    accum1 = accum1 and accum2;        /* AND */
/* 19 */    accum1 = accum1 or accum2;        /* OR */
/* 20 */    accum1 = accum1 xor accum2;        /* XOR */
/* 21 */    do;                    /* MOD */
            if accum2 = 0 then
                call ValueError;
            accum1 = accum1 mod accum2;
        end;
/* 22 */    do;                    /* SHL */
            if accum2$lb <> 0 then
                accum1 = shl(accum1, accum2);
        end;
/* 23 */    do;                    /* SHR */
            if accum2$lb <> 0 then
                accum1 = shr(accum1, accum2);
        end;
/* 24 */    do;                    /* HIGH */
            accum1 = high(accum1);
            call UpdateHiLo(UF$HIGH);
        end;
/* 25 */    do;                    /* LOW */
            accum1 = low(accum1);
            call UpdateHiLo(UF$LOW);
        end;
/* 26 */    do;                    /* DB ? */
            if tokenType(0) <> O$STRING then
            do;
                accum1 = GetNumVal;
                if accum1$hb - 1 < 0FEh then    /* not 0 or FF */
                    call ValueError;
                curOpFlags = 22h;	/* never used -- getnum to acc1 & collect high(acc2) */
                if (acc1Flags and UF$BOTH) = UF$BOTH then
                do;
                    call ValueError;
                    acc1Flags = (acc1Flags and not UF$BOTH) or UF$LOW;
                end;
            end;
            else
            do;
                acc1Flags = 0;
                tokenType(0) = O$DATA;
            end;

            if IsReg(acc1ValType) then
                call OperandError;
            nextTokType = O$DATA;
            inDB = TRUE;
        end;
/* 27 */    do;                    /* DW ? */
            nextTokType = O$DATA;
            inDW = TRUE;
        end;
/* 28 */    do;                    /* DS ? */
            segSize(activeSeg) = segSize(activeSeg) + accum1;
            showAddr = TRUE;
        end;
/* 29 */ case29:                    /* EQU ? */
        do;
            showAddr = TRUE;
            if (acc1Flags and UF$EXTRN) = UF$EXTRN then    /* cannot SET or EQU to external */
            do;
                call ExpressionError;
                acc1Flags = 0;
            end;
            labelUse = 1;
            call UpdateSymbolEntry(accum1, (K$SET + 4) - topOp);    /* 4 for set, 5 for equ */
            expectingOperands = FALSE;
        end;
/* 30 */    goto case29;                /* SET ? */
/* 31 */    do;                    /* ORG ? */
            showAddr = TRUE;
            if (acc1Flags and UF$EXTRN) <> UF$EXTRN then
            do;
                if (acc1Flags and UF$BOTH) <> 0 then
                    if (acc1Flags and UF$SEGMASK) <> activeSeg
                        or (acc1Flags and UF$BOTH) <> UF$BOTH then
                        call ExpressionError;
            end;
            else
                call ExpressionError;

            if ctlOBJECT then
                if segSize(activeSeg) > maxSegSize(activeSeg) then
                    maxSegSize(activeSeg) = segSize(activeSeg);
            segSize(activeSeg) = accum1;
        end;
/* 32 */    do;                    /* END ? */
            if tokenIdx > 0 then
            do;
                startOffset = GetNumVal;
                startDefined = 1;
                startSeg = acc1Flags and 7;
                if (acc1Flags and UF$EXTRN) = UF$EXTRN then
                    call ExpressionError;
                if IsReg(acc1ValType) then
                    call OperandError;

                showAddr = TRUE;
            end;
$IF MACRO
            kk = b905E;
            b905E = 0;

            if macroCondSP > 0 or kk then
$ELSE
            if ifDepth > 0 then
$ENDIF
                call NestingError;
            if newOp <> T$CR then
                call SyntaxError;
            if expectOp then
                b6B33 = TRUE;
            else
                call SyntaxError;

        end;
/* 33 */    do;                    /* IF ? */
            if expectOp then
            do;
                condAsmSeen = TRUE;
                call Nest(2);        /* push current skip/else status */
                xRefPending = TRUE;        /* push current skip/else status */
                if skipIf(0) = FALSE then    /* if not skipping set new status */
                    skipIf(0) = not ((low(accum1) and 1) = 1);
                inElse(0) = FALSE;        /* not in else at this nesting level */
            end;
        end;
/* 34 */    do;                    /* ELSE ? */
            condAsmSeen = TRUE;
$IF MACRO
            if macroCondStk(0) <> 2 then	/* check not mid macro nest */
$ELSE
            if ifDepth = 0 then			/* invalid if not in if */
$ENDIF
                call NestingError;
            else if not inElse(0) then    /* shouldn't be in else at this level */
            do;
                if not skipIf(0) then    /* IF was active so ELSE forces skip */
                    skipIf(0) = TRUE;
                else            /* IF inactive so revert to previous skipping status */
                    skipIf(0) = skipIf(ifDepth);
                inElse(0) = TRUE;    /* in else at this nesting level */
            end;
            else
                call NestingError;    /* multiple else !! */
        end;
/* 35 */    do;                    /* ENDIF ? */
            if expectOp then
            do;
                condAsmSeen = TRUE;
                call UnNest(2);    /* revert to previous status */
            end;
        end;
        /* in the following topOp = 36 and nextTokType = O$DATA
           except where noted on return from MkCode */
/* 36 */    do;                    /* LXI ? */
            if nameLen = 1 then
                if name(0) = 'M' then
                    call SyntaxError;
            call MkCode(85h);    /* topOp = 2Ch on return */
        end;
/* 37 */    do;                /* POP DAD PUSH INX DCX ? */
            if nameLen = 1 then
                if name(0) = 'M' then
                    call SyntaxError;
            call MkCode(5);
        end;
/* 38 */    call MkCode(7);        /* LDAX STAX ? */
/* 39 */    call MkCode(2);        /* ADC ADD SUB ORA SBB XRA ANA CMP ? */
/* 40 */    call MkCode(8);        /* ADI OUT SBI ORI IN CPI SUI XRI ANI ACI ? */
/* 41 */    call MkCode(46h);    /* MVI ?  topOp = 40 on return */
/* 42 */    call MkCode(6);        /* INR DCR ? */
/* 43 */    call MkCode(36h);    /* MOV   topOp = 39 on return*/
/* 44 */    call MkCode(0);        /* CZ CNZ JZ STA JNZ JNC LHLD */
                        /* CP JC SHLD CPE CPO CM LDA JP JM JPE */
                        /* CALL JPO CC CNC JMP */
/* 45 */    call MkCode(0);        /* RNZ STC DAA DI SIM SPHL RLC */
                        /* RP RAL HLT RM RAR RPE RET RIM */
                        /* PCHL CMA CNC RPO EI XTHL NOP */
                        /* RC RNX XCHG RZ RRC */
/* 46 */    call MkCode(6);        /* RST */
/* 47 */    activeSeg = 0;            /* ASEG ? */
/* 48 */    do;                /* CSEG ? */
            activeSeg = 1;
            call ChkSegAlignment(0);
        end;
/* 49 */    do;                /* DSEG ? */
            activeSeg = 2;
            call ChkSegAlignment(1);
        end;

/* 50 */    do;                /* PUBLIC */
            inPublic = TRUE;
            labelUse = 0;
            call UpdateSymbolEntry(0, O$REF);
        end;
/* 51 */    do;                /* EXTRN ? */
            inExtrn = TRUE;
            if externId = 0 and IsPhase1 and ctlOBJECT then
            do;
                CHKOVL$2;
                call WriteModhdr;
            end;
            labelUse = 0;
            call UpdateSymbolEntry(externId, O$TARGET);
            if IsPhase1 and ctlOBJECT and not badExtrn then
            do;
                CHKOVL$2;
                call WriteExtName;
            end;
            if not badExtrn then
                externId = externId + 1;
            badExtrn = 0;
        end;
/* 52 */    do;                /* NAME */
            if tokenIdx <> 0 and noOpsYet then
            do;
                /* set the module name in the header - padded to 6 chars */
                call move(6, .spaces6, .aModulePage);
                call move(moduleNameLen := nameLen, .name, .aModulePage);
            end;
            else
                call SourceError('R');

            call PopToken;
        end;
/* 53 */    segSize(SEG$STACK) = accum1;    /* STKLN ? */
$IF MACRO
/* 54 */    call Sub7517;            /* MACRO ? */
/* 55 */    call Sub753E;
/* 56 */    call Sub75FF;            /* ENDM */
/* 57 */    call Sub76CE;            /* EXITM */
/* 58 */    do;
            tmac$mtype = 4;
            call Sub7327;
        end;
/* 59 */    call Sub72A4(1);        /* IRP ? */
/* 60 */    call Sub72A4(2);        /* IRPC */
/* 61 */    call Sub770B;
/* 62 */    call Sub7844;            /* REPT ? */
/* 63 */    call Sub787A;            /* LOCAL */
/* 64 */    call Sub78CE;
/* 65 */    do;                /* NUL */
            call Cond2Acc(tokenType(0) = K$NUL);
            call PopToken;
            acc1Flags = 0;
        end;
$ENDIF
    end;

    if topOp <> T$CR then
        noOpsYet = FALSE;
end;

Parse: procedure public;

    IsExpressionOp: procedure byte;
        if effectiveToken > T$RPAREN then
            if effectiveToken <> T$COMMA then
                if effectiveToken < K$DB then
                    return TRUE;
        return FALSE;
    end;

    IsVar: procedure(type) byte;
        declare type byte;
        return type = O$ID or type = O$MACROID;
    end;


    UpdateIsInstr: procedure;
        if not isInstrMap(topOp) then
            isInstr = FALSE;
    end;



    do while 1;
        if not (effectiveToken = T$CR or effectiveToken >= K$END and effectiveToken <= K$ENDIF)
           and skipIf(0)
$IF MACRO
               or (opFlags(effectiveToken) < 128 or b9058) and b905E

$ENDIF
            then
        do;
            needsAbsValue = FALSE;
            call PopToken;
            return;
        end;
        
        if phase <> 1 then
            if inExpression then
                if IsExpressionOp then
                    if GetPrec(effectiveToken) <= GetPrec(opStack(opSP)) then
                        call ExpressionError;

        if GetPrec(newOp := effectiveToken) > GetPrec(topOp := opStack(opSP)) or newOp = T$LPAREN then
        do;    /* SHIFT */
            if opSP >= 16 then
            do;
                opSP = 0;
                call StackError;
            end;
            else
                opStack(opSP := opSP + 1) = newOp;
            if newOp = T$LPAREN then
            do;
                inNestedParen = expectOp;
                expectOp = TRUE;
            end;
            if phase > 1 then
                inExpression = IsExpressionOp;
            return;
        end;

	/* REDUCE */
        inExpression = 0;
        if not expectOp and topOp > T$RPAREN then
            call SyntaxError;

        if topOp = O$NONE then	/* topOp used so set to newOp */
            topOp = newOp;
        else
            opSP = opSP - 1;	/* pop Op */
        

        if (curOpFlags := opFlags(topOp)) then		/* -------x -> getnum to acc1 & copy to acc2 */
        do;
            accum2 = GetNumVal;
            acc2Flags = acc1Flags;
            acc2NumVal = acc1NumVal;
            acc2ValType = acc1ValType;
        end;

        if (curOpFlags := ror(curOpFlags, 1)) then	/* ------x- -> getnum to acc1 */
            accum1 = GetNumVal;

        if not hasVarRef then
            hasVarRef = IsVar(acc1ValType) or IsVar(acc2ValType);

        nextTokType = O$NUMBER;
        if topOp > T$RPAREN and topOp < K$DB then    /* expression topOp */
            call Sub4291;
        else
        do;
            call UpdateIsInstr;
            call ChkInvalidRegOperand;
        end;

        call HandleOp;
        if not isExprOrMacroMap(topOp) then
            expectOp = FALSE;

        if b6B2C then
        do;
            b6B2C = FALSE;
            return;
        end;

        if topOp <> K$DS and showAddr then
            effectiveAddr = accum1;

        if (curOpFlags and 1Eh) <> 0 then		/* -xxxxx-- -> collect list or bytes */
            call PushToken(nextTokType);

        do ii = 0 to 3;
            if (curOpFlags := ror(curOpFlags, 1)) then	/* --xxxx-- -> collect high/low acc1/acc2 */
                call CollectByte(accum(ii));
        end;

        tokenAttr(0) = acc1Flags;
        tokenSymId(0) = acc1NumVal;
        if ror(curOpFlags, 1) then			/* -x------ -> list */
            if newOp = T$COMMA then
            do;
                effectiveToken = topOp;
                expectOp = TRUE;
            end;
    end;
end;



DoPass: procedure public;
    do while finished = FALSE;
        call Tokenise;
        call Parse;
    end;
end;

end;
