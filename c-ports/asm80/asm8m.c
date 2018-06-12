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

    if (inAngleBrackets)	// return whether end of balanced list
        return argNestCnt == startNestCnt;

    if (IsLT() || (macro.top.mtype != M_IRP && IsGT())) {
        IllegalCharError();
        return true;
    }

    return IsWhite() || IsComma() || IsGT() || curChar == ';';
}



static void InitParamCollect()
{
    symTab[TID_MACRO] = endSymTab[TID_MACRO] = (tokensym_t *)symHighMark;	// init macro parameter table
    paramCnt = macro.top.localsCnt = bZERO;	// no params yet
    yyType = O_MACROBODY;
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
    macro.top.pCurArg = pNextArg;
    macro.top.localIdBase = localIdCnt;
    localIdCnt += macro.top.localsCnt;		
    ReadM(macro.top.savedBlk);
    macro.top.bufP = macroBuf;
}


static bool Sub727F()
{
    if (! (mSpoolMode & 1))
        return true;
    macroSpoolNestDepth++;
    b6B2C = topOp != K_REPT;
    yyType = 0x37;
    return false;
}


void DoIrpX(byte mtype)
{    /* 1 -> IRP, 2 -> IRPC */
    if (Sub727F()) {
        InitParamCollect();
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


void initMacroParam()
{
    pNextArg = baseMacroTbl;
    yyType = O_ITERPARAM;
    inMacroBody = true;
    b9060 = false;
}


static pointer AddMacroText(pointer lowAddr, pointer highAddr)
{
    while (lowAddr <= highAddr) {
        if (baseMacroTbl <= symHighMark)
            RuntimeError(RTE_TABLE);    /* table Error() */
        *baseMacroTbl-- = *highAddr--;
    }
    return baseMacroTbl;
}


static void InitSpoolMode()
{
    macroSpoolNestDepth = 1;
    macroInPtr = symHighMark;
    mSpoolMode = 1;
    baseMacroBlk = macroBlkCnt;
}



static void ChkForLocationError()
{
    if (usrLookupIsID)
        if (asmErrCode != 'U')
            LocationError();
}


void GetMacroToken()
{
    byte isPercent;

    savedTokenIdx = tokenIdx;
    SkipWhite();
    if (! (isPercent = curChar == '%')) {
        startNestCnt = argNestCnt - 1;		// if nested than argNestCnt will already have been incremented for the <
        if ((inAngleBrackets = IsLT()))
            curChar = GetCh();

        PushToken(O_PARAM);

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

    inMacroBody = false;
    if (macro.top.mtype == M_INVOKE)
    {
        if ((! inAngleBrackets) && tokenSize[0] == 5)
            if (StrUcEqu("MACRO", tokPtr)) {	// nested macro definition
                yyType = K_MACRO;
                PopToken();
                pNextArg = macro.top.pCurArg;
                opSP--;
                reget = 1;
                EmitXref(XREF_DEF, name);
                rhsUserSymbol = false;
                nestedMacroSeen = true;
                return;
            }
        macro.top.mtype = savedMtype;
        Nest(1);
        macro.top.mtype = M_MACRO;
    }

    if (! isPercent)
        if (! TestBit(curChar, b7183)) {    /* ! CR, COMMA or SEMI */
            Skip2EOL();
            SyntaxError();
            reget = 1;
        }
}



void DoMacro()
{
    if (Sub727F()) {
        expectingOperands = false;
        pMacro = (pointer)&tokenSym.curP->blk;
        UpdateSymbolEntry(0, T_MACRONAME);
        macro.top.mtype = M_MACRO;
        InitParamCollect();
    }
}

void DoMacroBody()
{
    if (HaveTokens()) {
        if (tokenType[0] == 0)		// saved parameters have type 0
            MultipleDefError();		// so if found we have a multiple definition

        InsertMacroSym(++paramCnt, 0);	// add the parameter setting type to 0
    }
    else if (! (macro.top.mtype == M_MACRO))	// only MACRO is allowed to have no parameters
        SyntaxError();

    if (! macro.top.mtype == M_MACRO) {	// none MACRO have parameters -> dummy,value
        SkipWhite();					// as dummy is entered above look for , value
        if (IsComma()) {
            reget = 0;
            newOp = T_BEGIN;		   // mark beginning of expression
            initMacroParam();
            if (macro.top.mtype == M_IRP) {		/* if IRP then expression begins with < */
                curChar = GetCh();
                SkipWhite();
                if (! IsLT()) {
                    SyntaxError();		/* report error but assume < was given */
                    reget = 1;			/* make sure the non < is read again */
                }
            }
        }
        else
        {
            SyntaxError();				/* report error and assume param list complete */
            InitSpoolMode();			/* spool rest of definition */
        }
    }
    else if (newOp == T_CR)		// got the parameters
    {
        if (! MPorNoErrCode())	// skip if multiple defined, phase or no error 
        {
            macro.top.mtype = M_BODY;
            pMacro += 2;		// now points to type
            if ((*pMacro & 0x7F) == T_MACRONAME)
                *pMacro = asmErrCode == 'L' ? (O_NAME + 0x80) : O_NAME;	// location error illegal forward ref
        }
        InitSpoolMode();
    }
}

void DoEndm()
{
    if (mSpoolMode & 1) {
        if (--macroSpoolNestDepth == 0) {	// Reached end of spooling ?
            mSpoolMode = 0;		// spooling done
            if (! (macro.top.mtype == M_BODY)) {
                if (macro.top.mtype == M_IRPC)
                    pNextArg = baseMacroTbl + 3;

                /* endm cannot have a label */
                for (byte *p = startMacroLine; p <= startMacroToken - 1; p++) {	// plm reuses aVar
                    curChar = *p;
                    if (! IsWhite())
                        SyntaxError();
                }

                /* replace line with the ESC char to mark end */
                macroInPtr = startMacroLine;
                *macroInPtr = ESC;
                FlushM();		// write to disk
                WriteM();
                symHighMark = (pointer)(endSymTab[TID_MACRO] = symTab[TID_MACRO]);	// reset macro parameter symbol table
                if (macro.top.mtype == M_MACRO) {
                    *(word *)pMacro = baseMacroBlk;
                    pMacro += 3;		// points to flags
                    *pMacro = macro.top.localsCnt;		// number of locals
                } else {
                    macro.top.savedBlk = baseMacroBlk;
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


void DoExitm()
{
    if (expandingMacro) {
        if (newOp == T_CR) {
            condAsmSeen = true;
            macroCondSP = macro.top.condSP;
            ifDepth = macro.top.ifDepth;
            macro.top.cnt = 1;		// force apparent endm - last repitition
            lookAhead = ESC;		// and endm marker
            macroCondStk[0] = 1;	// and any if else
        } else
            SyntaxError();
    } else
        NestingError();
}


void DoIterParam()
{
    if (savedTokenIdx + 1 != tokenIdx)
        SyntaxError();
    else if (! b9060) {
        if (tokenType[0] != O_PARAM) {
            accum1 = GetNumVal();
            Acc1ToDecimal();
        }

        if (macro.top.mtype == M_IRPC)
            macro.top.cnt = tokenSize[0] == 0 ? 1 : tokenSize[0];	// plm uses true->FF and byte arithmetic. Replaced here for clarity

        CollectByte((tokenSize[0] + 1) | 0x80);						// append a byte to record the token length + 0x80
        baseMacroTbl = AddMacroText(tokPtr, tokPtr + tokenSize[0] - 1);
        PopToken();

        if (macro.top.mtype == M_MACRO || (macro.top.mtype == M_IRP && argNestCnt > 0))
            inMacroBody = true;
        else
            b9060 = true;

        if (macro.top.mtype == M_IRP)
            macro.top.cnt++;
    }
    else
        SyntaxError();

    if (newOp == T_CR) {
        inMacroBody = false;
        if (argNestCnt > 0)
            BalanceError();

        if (! MPorNoErrCode()) {
            ChkForLocationError();
            if (macro.top.mtype == M_MACRO) {
                Sub720A();
                UnNest(1);
                return;
            } else
                macro.top.cnt = 0;
        } else {
            baseMacroTbl = AddMacroText(b3782, b3782 + 1);		// append 0x80 0x81
            if (macro.top.mtype == M_MACRO) {
                macro.top.localsCnt = tokenSym.curP->flags;
                macro.top.savedBlk = GetNumVal();
                Sub720A();
            } else if (macro.top.cnt == 0)
                SyntaxError();
        }

        if (! (macro.top.mtype == M_MACRO))
            InitSpoolMode();
    }
}



void DoRept()
{
    DoIrpX(M_REPT);
    if ((yyType = newOp) != T_CR)
        SyntaxError();

    if (! (mSpoolMode & 1)) {
        macro.top.cnt = accum1;		// get the repeat count
        if (! MPorNoErrCode()) {	// skip processing if error other than M or P
            ChkForLocationError();
            macro.top.cnt = 0;
        }
        InitSpoolMode();
    }
}


void DoLocal()
{
    if (mSpoolMode == 2) {
        if (HaveTokens()) {
            if ((byte)(++macro.top.localsCnt) == 0)
                StackError();

            if (tokenType[0] != O_NAME)
                MultipleDefError();

            InsertMacroSym(macro.top.localsCnt, 1);
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
    kk = *macro.top.pCurArg;
    aVar.lb = (kk == '!' && savedMtype == M_IRPC) ? 2 : 1;	// size arg (2 if escaped char)
    if (savedMtype == M_MACRO || (macro.top.cnt -= aVar.lb) == 0)	// all done
        UnNest(1);
    else {
        if (savedMtype == M_IRP)
            pNextArg = macro.top.pCurArg - (kk & 0x7F);		// skip foward to start of arg (stored as arg, len)
        else
            pNextArg = macro.top.pCurArg + aVar.lb;	// skip char or escaped char

        macro.top.mtype = savedMtype;
        Sub720A();
    }
    lookAhead = 0;
    b6B2C = atStartLine = bTRUE;
}
