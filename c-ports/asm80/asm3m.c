// vim:ts=4:expandtab:shiftwidth=4:
#include "asm80.h"

            /* 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F */
bool isExprOrMacroMap[] = {
           true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, true, 
           true, true, true, true, true, true, true, true, true, true, false,false,false,false,false,false,
           false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
           false,false,false,false,false,false,true, true, true, true, true, true, true, true, false,true, 
           true, true};
bool isInstrMap[] = {
           false,true, true, true, false,false,false,false,false,false,false,false,false,false,false,false,
           false,false,false,false,false,false,false,false,false,false,true, true, false,false,false,false,
           false,false,false,false,true, true, true, true, true, true, true, true, true, true, true, false,
           false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
           true, true};

void ChkSegAlignment(byte seg)
{    /* seg = 0 => CSEG, seg = 1 => DSEG */

    if (segHasData[seg]) {
        if (HaveTokens()) {
            accum1 = GetNumVal();
            if (alignTypes[seg] != accum1Lb)
                ExpressionError();
        }
        else if (alignTypes[seg] != 3)    /* no speficier - check byte algined */
            ExpressionError();
    } else {
        segHasData[seg] = true;
        if (HaveTokens()) {
            accum1 = GetNumVal();
            if (accum1 == 1 || accum1 == 2)    /* only allow inPage and Page */
                alignTypes[seg] = accum1 & 0xff;
            else
                ExpressionError();
        }
    }
}


void Cond2Acc(byte cond)
{    /* convert conditional result to accum1 */

    accum1 = cond;
    if (accum1)
        accum1 = 0xFFFF;
}

void UpdateHiLo(byte hilo)
{

    if ((acc1Flags & (UF_EXTRN + UF_SEGMASK)) != 0)
        acc1Flags = (acc1Flags & ~UF_BOTH) | hilo;
}

void HandleOp()
{
    switch (topOp) {
    case 0:	break;
    case 1:	FinishLine();        /* CR */
            break;
    case 2:	                    /* ( */
    case 3:	                    /* ) */
            if (! (topOp == T_LPAREN && newOp == T_RPAREN))
                BalanceError();

            if (tokenType[0] == O_DATA) {
                tokenSize[0] = 1;
                tokenAttr[0] = 0;
                b6B36 = true;
            }

            expectOp = inNestedParen;
            if (newOp == T_RPAREN)
                b6B2C = true;
            break;
    case 4:	accum1 *= accum2;    /* * */
            break;
    case 5:	accum1 += accum2;    /* + */
            break;
    case 6:	                    /* , */
            SyntaxError();
            PopToken();
            break;
    case 7:	accum1 -= accum2;    /* - */
            break;
    case 8:	;                /* unary + */
            break;
    case 9:	                /* / */
        if (accum2 == 0) {
            ValueError();
            accum1 = 0xffff;	/* synthesise what 8085 does on / 0 */
        } 
        else
                accum1 /= accum2;
            break;
    case 10:	accum1 = -accum1;            /* unary - */
            break;
    case 11:	Cond2Acc(accum1 == accum2);        /* EQ */
            break;
    case 12:	Cond2Acc(accum1 < accum2);        /* LT */
            break;
    case 13:	Cond2Acc(accum1 <= accum2);    /* LE */
            break;
    case 14:	Cond2Acc(accum1 > accum2);        /* GT */
            break;
    case 15:	Cond2Acc(accum1 >= accum2);    /* GE */
            break;
    case 16:	Cond2Acc(accum1 != accum2);    /* NE */
            break;
    case 17:	accum1 = ~accum1;            /* NOT */
            break;
    case 18:	accum1 &= accum2;        /* AND */
            break;
    case 19:	accum1 |= accum2;        /* OR */
            break;
    case 20:	accum1 ^= accum2;        /* XOR */
            break;
    case 21:	                    /* MOD */
        if (accum2 == 0) {
            ValueError();
            accum1 = accum2;		// this is what the 8080 mod code would do
        } else
            accum1 %= accum2;
            break;
    case 22:	                    /* SHL */
            if (accum2Lb != 0)
                accum1 <<= accum2;
            break;
    case 23:	                    /* SHR */
            if (accum2Lb != 0)
                accum1 >>= accum2;
            break;
    case 24:	                    /* HIGH */
            accum1 >>= 8;
            UpdateHiLo(UF_HIGH);
            break;
    case 25:	                    /* LOW */
            accum1 &= 0xff;
            UpdateHiLo(UF_LOW);
            break;
    case 26:	                    /* DB ? */
            if (tokenType[0] != O_STRING)
            {
                accum1 = GetNumVal();
                if ((byte)(accum1Hb - 1) < 0xFE)    /* ! 0 or FF */
                    ValueError();
                curOpFlags = 0x44;   /* adjusted from PLM as revised code doesn't use rotate */
                if ((acc1Flags & UF_BOTH) == UF_BOTH) {
                    ValueError();
                    acc1Flags = (acc1Flags & ~UF_BOTH) | UF_LOW;
                }
            } else {
                acc1Flags = 0;
                tokenType[0] = O_DATA;
            }

            if (IsReg(acc1ValType))
                OperandError();
            nextTokType = O_DATA;
            inDB = true;
            break;
    case 27:                    /* DW ? */
            nextTokType = O_DATA;
            inDW = true;
            break;
    case 28:	                    /* DS ? */
            segSize[activeSeg] += accum1;
            showAddr = true;
            break;
    case 29:            	/* EQU ? */
    case 30:                /* SET ? */
            showAddr = true;
            if ((acc1Flags & UF_EXTRN) == UF_EXTRN) {   /* cannot SET or EQU to external */
                ExpressionError();
                acc1Flags = 0;
            }
            labelUse = 1;
            UpdateSymbolEntry(accum1, (K_SET + 4) - topOp);    /* 4 for set, 5 for equ */
            expectingOperands = false;
            break;
    case 31:	                    /* ORG ? */
            showAddr = true;
            if ((acc1Flags & UF_EXTRN) != UF_EXTRN) {
                if ((acc1Flags & UF_BOTH) != 0)
                    if ((acc1Flags & UF_SEGMASK) != activeSeg
                      || (acc1Flags & UF_BOTH) != UF_BOTH)
                        ExpressionError();
            } else
                ExpressionError();

            if (controls.object)
                if (segSize[activeSeg] > maxSegSize[activeSeg])
                    maxSegSize[activeSeg] = segSize[activeSeg];
            segSize[activeSeg] = accum1;
            break;
    case 32:	                    /* END ? */
            if (tokenIdx > 0) {
                startOffset = GetNumVal();
                startDefined = 1;
                startSeg = acc1Flags & 7;
                if ((acc1Flags & UF_EXTRN) == UF_EXTRN)
                    ExpressionError();
                if (IsReg(acc1ValType))
                    OperandError();

                showAddr = true;
            }
            kk = mSpoolMode;
            mSpoolMode = 0;

            if (macroCondSP > 0 || (kk & 1))
                NestingError();
            if (newOp != T_CR)
                SyntaxError();
            if (expectOp)
                b6B33 = true;
            else
                SyntaxError();
            break;
    case 33:	                    /* IF ? */
            if (expectOp)
            {
                condAsmSeen = true;
                Nest(2);        /* push current skip/else status */
                xRefPending = true;        /* push current skip/else status */
                if (!skipIf[0])    /* if ! skipping set new status */
                    skipIf[0] = ! (accum1 & 1);
                inElse[0] = false;        /* ! in else at this nesting level */
            }
            break;
    case 34:	                    /* ELSE ? */
            condAsmSeen = true;
            if (macroCondStk[0] != 2)    /* check ! mid macro nest */
                NestingError();
            else if (! inElse[0])    /* shouldn't be in else at this level */
            {
                if (! skipIf[0])    /* IF was active so ELSE forces skip */
                    skipIf[0] = true;
                else            /* IF inactive so revert to previous skipping status */
                    skipIf[0] = skipIf[ifDepth];
                inElse[0] = true;    /* in else at this nesting level */
            }
            else
                NestingError();    /* multiple else !! */
            break;
    case 35:	                    /* ENDIF ? */
            if (expectOp)
            {
                condAsmSeen = true;
                UnNest(2);    /* revert to previous status */
            }
            break;
        /* in the following topOp = 36 and nextTokType = O_DATA
           except where noted on return from MkCode */
    case 36:	                    /* LXI ? */
            if (nameLen == 1)
                if (name[0] == 'M')
                    SyntaxError();
            MkCode(0x85);    /* topOp = 2Ch on return */
            break;
    case 37:	                /* REG16 ops POP DAD PUSH INX DCX ? */
            if (nameLen == 1)
                if (name[0] == 'M')
                    SyntaxError();
            MkCode(5);
            break;
    case 38:	MkCode(7);        /* LDAX STAX ? */
            break;
    case 39:	MkCode(2);        /* ARITH ops ADC ADD SUB ORA SBB XRA ANA CMP ? */
            break;
    case 40:	MkCode(8);        /* IMM8 ops ADI OUT SBI ORI IN CPI SUI XRI ANI ACI ? */
            break;
    case 41:	MkCode(0x46);    /* MVI ?  topOp = 40 on return */
            break;
    case 42:	MkCode(6);        /* INR DCR ? */
            break;
    case 43:	MkCode(0x36);    /* MOV   topOp = 39 on return*/
            break;
    case 44:	MkCode(0);        /* IMM16 ops CZ CNZ JZ STA JNZ JNC LHLD */
                        /* CP JC SHLD CPE CPO CM LDA JP JM JPE */
                        /* CALL JPO CC CNC JMP */
            break;
    case 45:	MkCode(0);        /* SINGLE ops RNZ STC DAA DI SIM SPHL RLC */
                        /* RP RAL HLT RM RAR RPE RET RIM */
                        /* PCHL CMA CNC RPO EI XTHL NOP */
                        /* RC RNX XCHG RZ RRC */
            break;
    case 46:	MkCode(6);        /* RST */
            break;
    case 47:	activeSeg = 0;            /* ASEG ? */
            break;
    case 48:	                /* CSEG ? */
            activeSeg = 1;
            ChkSegAlignment(0);
            break;
    case 49:	                /* DSEG ? */
            activeSeg = 2;
            ChkSegAlignment(1);
            break;
    case 50:	                /* PUBLIC */
            inPublic = true;
            labelUse = 0;
            UpdateSymbolEntry(0, O_REF);
            break;
    case 51:	                /* EXTRN ? */
            inExtrn = true;
            if (externId == 0 && IsPhase1 && controls.object) 
                WriteModhdr();
            labelUse = 0;
            UpdateSymbolEntry(externId, O_TARGET);
            if (IsPhase1 && controls.object && ! badExtrn)
                WriteExtName();
            if (! badExtrn)
                externId++;
            badExtrn = false;
            break;
    case 52:	                /* NAME */
            if (tokenIdx != 0 && noOpsYet) {
                /* set the module name in the header - padded to 6 chars */
                move(6, spaces6, aModulePage);
                move(moduleNameLen = nameLen, name, aModulePage);
            } else
                SourceError('R');
            PopToken();
            break;
    case 53:	segSize[SEG_STACK] = accum1;    /* STKLN ? */
            break;
    case 54:	DoMacro();            /* MACRO ? */
            break;
    case 55:	DoMacroBody();			  /* MACRO BODY */
            break;
    case 56:	DoEndm();            /* ENDM */
            break; 
    case 57:	DoExitm();            /* EXITM */
            break;
    case 58:						/* MACRONAME */
            macro.top.mtype = M_INVOKE;
            initMacroParam();
            break;
    case 59:	DoIrpX(M_IRP);        /* IRP ? */
            break;
    case 60:	DoIrpX(M_IRPC);        /* IRPC */
            break;
    case 61:	DoIterParam();			/* MACRO PARAMETER */
            break;
    case 62:	DoRept();            /* REPT ? */
            break;
    case 63:	DoLocal();            /* LOCAL */
            break;
    case 64:	Sub78CE();
            break;
    case 65:	                /* NUL */
            Cond2Acc(tokenType[0] = K_NUL);
            PopToken();
            acc1Flags = 0;
    }

    if (topOp != T_CR)
        noOpsYet = false;
}


byte IsExpressionOp()
{
    if (yyType > T_RPAREN)
        if (yyType != T_COMMA)
            if (yyType < K_DB)
                return true;
    return false;
}

byte IsVar(byte type)
{
    return type == O_NAME || type == O_MACROID;
}


void UpdateIsInstr()
{
    if (! isInstrMap[topOp])
        isInstr = false;
}


void Parse()
{
    while (1) {
        if (! (yyType == T_CR ||( yyType >= K_END && yyType <= K_ENDIF))
           && skipIf[0] || ((opFlags[yyType] < 128 || inQuotes) && (mSpoolMode & 1))) {
            needsAbsValue = false;
            PopToken();
            return;
        }
        
        if (phase != 1)
            if (inExpression)
                if (IsExpressionOp())
                    if (GetPrec(yyType) <= GetPrec(opStack[opSP]))
                        ExpressionError();

        if (GetPrec(newOp = yyType) > GetPrec(topOp = opStack[opSP]) || newOp == T_LPAREN) {    /* SHIFT */
            if (opSP >= 16) {
                opSP = 0;
                StackError();
            } else
                opStack[++opSP] = newOp;
            if (newOp == T_LPAREN) {
                inNestedParen = expectOp;
                expectOp = true;
            }
#if _DEBUG
			DumpOpStack();
			DumpTokenStack(false);
#endif
            if (phase > 1)
                inExpression = IsExpressionOp();
            return;
        }

    /* REDUCE */
#if _DEBUG
		DumpOpStack();
        DumpTokenStack(false);
#endif
        inExpression = 0;
        if ((! expectOp) && topOp > T_RPAREN)
            SyntaxError();

        if (topOp == T_BEGIN) /* topOp used so set to newOp */
            topOp = newOp;
        else
            opSP--;    /* pop Op */
        

        if ((curOpFlags = opFlags[topOp]) & 1) {     /* -------x -> getnum to acc1 & copy to acc2 */
            accum2 = GetNumVal();
            acc2Flags = acc1Flags;
            acc2NumVal = acc1NumVal;
            acc2ValType = acc1ValType;
        }

        if (curOpFlags & 2) /* removed ror */ /* ------x- -> getnum to acc1 */
            accum1 = GetNumVal();

        if (! hasVarRef)
            hasVarRef = IsVar(acc1ValType) || IsVar(acc2ValType);

        nextTokType = O_NUMBER;
        if (topOp > T_RPAREN && topOp < K_DB)    /* expression topOp */
            Sub4291();
        else {
            UpdateIsInstr();
            ChkInvalidRegOperand();
        }

        HandleOp();
        if (! isExprOrMacroMap[topOp])
            expectOp = false;

        if (b6B2C) {
            b6B2C = false;
            return;
        }

        if (topOp != K_DS && showAddr)
            effectiveAddr.w = accum1;

        if ((curOpFlags & 0x3C))       /* -xxxxx-- -> collect list or bytes */
            PushToken(nextTokType);

        for (ii = 0; ii <= 3; ii++) 
            if (curOpFlags & (4 << ii))  /* --xxxx-- -> collect high/low acc1/acc2 */
                CollectByte(((byte *)&accum1)[ii]);

        tokenAttr[0] = acc1Flags;
        tokenSymId[0] = acc1NumVal;
        if (curOpFlags & 0x40)          /* -x------ -> list */
            if (newOp == T_COMMA) {
                yyType = topOp;
                expectOp = true;
            }
    }
}



void DoPass()
{
    while (!finished) {
        Tokenise();
//		DumpOpStack();
//		DumpTokenStack();
        Parse();
    }
}

