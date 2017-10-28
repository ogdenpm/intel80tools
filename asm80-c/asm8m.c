#include "asm80.h"

static byte b7183[] = {0x3F, 0, 4, 0, 0, 0, 8, 0, 0x10};
    /* bit vector 64 - 00000000 00000100 00000000 00000000 00000000 00001000 00000000 00010000 */
    /*                 CR, COMMA, SEMI */                                  

static word baseMacroBlk;
byte savedTokenIdx;


static bool IsEndParam()
{
    if (IsCR()) {		// new line forces end
        inAngleBrackets = false;
        return true;
    }

    if (inAngleBrackets)	// end of balanced list
        return argNestCnt == startNestCnt;

    if (IsLT() || (! (macro.top.mtype == M_IRP) && IsGT())) {
        IllegalCharError();
        return true;
    }

    return IsWhite() || IsComma() || IsGT() || curChar == ';';
}



static void Sub71F2()
{
    symTab[TID_MACRO] = endSymTab[TID_MACRO] = (tokensym_t *)symHighMark;
    b9065 = macro.top.b3 = bZERO;
    yyType = 0x37;
}


static void Sub720A()
{
    savedMtype = macro.top.mtype;
    if (! expandingMacro)
        expandingMacro = 1;

    if (macroDepth == 0)
        expandingMacro = 0xFF;

	if (nestMacro) {
		macro.stack[macroDepth].bufP = macro.stack[0].bufP;
		macro.stack[macroDepth].blk = macro.stack[0].blk;
	}

    nestMacro = false;
    macro.top.w12 = w9199;
    macro.top.w4 = w919B;
    w919B += macro.top.b3;
    ReadM(macro.top.w10);
    macro.top.bufP = macroBuf;
}


static bool Sub727F()
{
    if (! (mSpoolMode & 1))
        return true;
    b9064++;
    b6B2C = topOp != K_REPT;
    yyType = 0x37;
    return false;
}


void Sub72A4(byte mtype)
{    /* 1 -> IRP, 2 -> IRPC */
    if (Sub727F()) {
        Sub71F2();
        Nest(1);
        macro.top.cnt = 0;
        macro.top.mtype = mtype;
    }
}

static void Acc1ToDecimal()
{
    byte buf[6];

    PushToken(CR);
    kk = 0;
    /* build number in reverse digit order */
	do {
		buf[++kk] = accum1 % 10 + '0';
	} while ((accum1 /= 10) > 0);

    /* insert in collect buffer in order */
    while (kk > 0)
        CollectByte(buf[kk--]);
}


void Sub7327()
{
    w9199 = baseMacroTbl;
    yyType = 0x3D;
    b905D = true;
    b9060 = false;
}


static pointer EnterMacro(pointer lowAddr, pointer highAddr)
{
    while (lowAddr <= highAddr) {
        if (baseMacroTbl <= symHighMark)
            RuntimeError(1);    /* table Error() */
        *baseMacroTbl-- = *highAddr--;
    }
    return baseMacroTbl;
}


static void Sub7383()
{
    b9064 = 1;
    macroInPtr = symHighMark;
    mSpoolMode = 1;
    baseMacroBlk = macroBlkCnt;
}



static void Sub739A()
{
    if (usrLookupIsID)
        if (asmErrCode != 'U')
            LocationError();
}


void Sub73AD()
{
    byte isPercent;

    savedTokenIdx = tokenIdx;
    SkipWhite();
    if (! (isPercent = curChar == '%')) {
        startNestCnt = argNestCnt - 1;		// if nested than argNestCnt will already have been incremented for the <
        if ((inAngleBrackets = IsLT()))
            curChar = GetCh();

        PushToken(0xD);

        while (! IsEndParam()) {
            if (curChar == '\'') {
                if ((curChar = GetCh()) == '\'') {
                    curChar = GetCh();
                    SkipWhite();
					if (IsEndParam())
						break;
                    else {
                        CollectByte('\'');
                        CollectByte('\'');
                    }
                } else {
                    CollectByte('\'');
					continue;
                }
            }
            CollectByte(curChar);
            if (macro.top.mtype == M_IRPC)
                macro.top.cnt++;

            if ((curChar == '!') & (GetCh() != CR)) {
                CollectByte(curChar);
                curChar = GetCh();
            }
        }

        if (inAngleBrackets)
            curChar = GetCh();

        SkipWhite();
        if (IsGT()) {
            curChar = GetCh();
            SkipWhite();
        }

        reget = 1;
    }

    b905D = false;
    if (macro.top.mtype == M_INVOKE)
    {
        if (! inAngleBrackets && tokenSize[0] == 5)
            if (StrUcEqu("MACRO", tokPtr)) {	// nested macro definition
                yyType = K_MACRO;
                PopToken();
                w9199 = macro.top.w12;
                opSP--;
                reget = 1;
                EmitXref(XREF_DEF, name);
                rhsUserSymbol = false;
                nestedMacroSeen = true;
                return;
            }
        macro.top.mtype = savedMtype;
        Nest(1);
        macro.top.mtype = 0;
    }

    if (! isPercent)
        if (! TestBit(curChar, b7183)) {    /* ! CR, COMMA || SEMI */
            Skip2EOL();
            SyntaxError();
            reget = 1;
        }
}



void Sub7517()
{
    if (Sub727F()) {
        expectingOperands = false;
        w9068 = (pointer)&tokenSym.curP->line;
        UpdateSymbolEntry(0, T_MACRONAME);
        macro.top.mtype = 0;
        Sub71F2();
    }
}

void Sub753E()
{
	if (HaveTokens()) {
        if (tokenType[0] == 0)
            MultipleDefError();

        Sub5CAD(++b9065, 0);
    }
    else if (! (macro.top.mtype == 0))
        SyntaxError();

    if (! macro.top.mtype == 0) {
        SkipWhite();
        if (IsComma()) {
            reget = 0;
            newOp = T_BEGIN;
            Sub7327();
            if (macro.top.mtype == M_IRP) {
                curChar = GetCh();
                SkipWhite();
                if (! IsLT()) {
                    SyntaxError();
                    reget = 1;
                }
            }
        }
        else
        {
            SyntaxError();
            Sub7383();
        }
    }
    else if (newOp == T_CR)
    {
        if (! BlankMorPAsmErrCode())
        {
            macro.top.mtype = 5;
            w9068 += 2;		// now points to type
            if ((*w9068 & 0x7F) == 0x3A)
				*w9068 = asmErrCode == 'L' ? 0x89 : 9;
        }
        Sub7383();
    }
}

void Sub75FF()
{
    if (mSpoolMode & 1) {
        if (--b9064 == 0) {
            mSpoolMode = 0;
            if (! (macro.top.mtype == 5)) {
                if (macro.top.mtype == M_IRPC)
                    w9199 = baseMacroTbl + 3;

                for (byte *p = w919D; p <= w919F - 1; p++) {	// plm reuses pAddr
                    curChar = *p;
                    if (! IsWhite())
                        SyntaxError();
                }

                macroInPtr = w919D;
                *macroInPtr = ESC;
                FlushM();
                WriteM();
				symHighMark = (pointer)(endSymTab[TID_MACRO] = symTab[TID_MACRO]);
                if (macro.top.mtype == 0) {
                    *(word *)w9068 = baseMacroBlk;
                    w9068 += 3;		// points to flags
					*w9068 = macro.top.b3;
                } else {
                    macro.top.w10 = baseMacroBlk;
                    Sub720A();
                    if (macro.top.cnt == 0)
                        UnNest(1);
                }
            }
        }
    }
    else
        NestingError();
}


void Sub76CE()
{
    if (expandingMacro) {
        if (newOp == T_CR) {
            condAsmSeen = true;
            macroCondSP = macro.top.condSP;
            ifDepth = macro.top.ifDepth;
            macro.top.cnt = 1;
            lookAhead = 0x1B;
            macroCondStk[0] = 1;
        } else
            SyntaxError();
    } else
        NestingError();
}


void Sub770B()
{
    if (savedTokenIdx + 1 != tokenIdx)
        SyntaxError();
    else if (! b9060) {
        if (tokenType[0] != 0xD) {
            accum1 = GetNumVal();
            Acc1ToDecimal();
        }

		if (macro.top.mtype == M_IRPC)
			macro.top.cnt = tokenSize[0] == 0 ? 1 : tokenSize[0];	// plm uses true->FF and byte arithmetic. True in C is 1

        CollectByte((tokenSize[0] + 1) | 0x80);
        baseMacroTbl = EnterMacro(tokPtr, tokPtr + tokenSize[0] - 1);
        PopToken();

        if (macro.top.mtype == 0 || (macro.top.mtype == M_IRP && argNestCnt > 0))
            b905D = true;
        else
            b9060 = true;

        if (macro.top.mtype == M_IRP)
            macro.top.cnt++;
    }
    else
        SyntaxError();

    if (newOp == T_CR) {
        b905D = false;
        if (argNestCnt > 0)
            BalanceError();

        if (! BlankMorPAsmErrCode()) {
            Sub739A();
            if (macro.top.mtype == 0) {
                Sub720A();
                UnNest(1);
                return;
            } else
                macro.top.cnt = 0;
        } else {
            baseMacroTbl = EnterMacro(b3782, b3782 + 1);
            if (macro.top.mtype == 0) {
                macro.top.b3 = tokenSym.curP->flags;
                macro.top.w10 = GetNumVal();
                Sub720A();
            } else if (macro.top.cnt == 0)
                SyntaxError();
        }

        if (! (macro.top.mtype == 0))
            Sub7383();
    }
}



void Sub7844()
{
    Sub72A4(M_REPT);
    if ((yyType = newOp) != T_CR)
        SyntaxError();

    if (! (mSpoolMode & 1)) {
        macro.top.cnt = accum1;
        if (! BlankMorPAsmErrCode()) {
            Sub739A();
            macro.top.cnt = 0;
        }
        Sub7383();
    }
}


void Sub787A()
{
    if (mSpoolMode == 2) {
        if (HaveTokens()) {
            if ((byte)(++macro.top.b3) == 0)
                StackError();

            if (tokenType[0] != O_NAME)
                MultipleDefError();

            Sub5CAD(macro.top.b3, 1);
            macroInPtr = symHighMark;
        }
        if (newOp == T_CR) {
            mSpoolMode = 1;
            macroInPtr = symHighMark;
        }
    } else
        SyntaxError();
}



void Sub78CE()
{
    kk = *macro.top.w12;
    accFixFlags[0] = (kk == '!' && savedMtype == M_IRPC) ? 2 : 1;	
    if (savedMtype == 0 || (macro.top.cnt -= accFixFlags[0]) == 0)
        UnNest(1);
    else {
        if (savedMtype == M_IRP)
            w9199 = macro.top.w12 - (kk & 0x7F);
        else
            w9199 = macro.top.w12 + accFixFlags[0];

        macro.top.mtype = savedMtype;
        Sub720A();
    }
    lookAhead = 0;
    b6B2C = atStartLine = bTRUE;
}
