#include "asm80.h"

static byte b5666[] = {9, 0x2D, 0x80}; /* bit vector 10 -> 00101101 10 */
static byte b5669[] = {0x3A, 8, 0x80, 0, 0, 0, 0, 0, 0x20};
		/* bit vector 59 -> 00001000 1000000 00000000 0000000
							00000000 0000000 00000000 001 */
static byte op16[] = {
	/* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, /* DW,DW,EQU,SET,ORG */
	   0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1 };         /* LXI,IMM16 */
static byte chClass[] = {
	/*  0/8     1/9     2/A     3/B     4/C     5/D     6/E     7/F */
	/*00*/ CC_BAD, CC_BAD, CC_BAD, CC_BAD, CC_BAD, CC_BAD, CC_BAD, CC_BAD,
	/*08*/ CC_BAD,  CC_WS, CC_BAD, CC_BAD,  CC_WS,  CC_CR, CC_BAD, CC_BAD,
	/*10*/ CC_BAD, CC_BAD, CC_BAD, CC_BAD, CC_BAD, CC_BAD, CC_BAD, CC_BAD,
	/*18*/ CC_BAD, CC_BAD, CC_BAD, CC_ESC, CC_BAD, CC_BAD, CC_BAD, CC_BAD,
	/*20*/ CC_WS, CC_BAD, CC_BAD, CC_BAD, CC_DOLLAR, CC_BAD, CC_BAD, CC_QUOTE,
	/*28*/ CC_PUN, CC_PUN, CC_PUN, CC_PUN, CC_PUN, CC_PUN, CC_BAD, CC_PUN,
	/*30*/ CC_DIG, CC_DIG, CC_DIG, CC_DIG, CC_DIG, CC_DIG, CC_DIG, CC_DIG,
	/*38*/ CC_DIG, CC_DIG, CC_COLON, CC_SEMI, CC_BAD, CC_BAD, CC_BAD, CC_LET,
	/*40*/ CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET,
	/*48*/ CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET,
	/*50*/ CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET,
	/*58*/ CC_LET, CC_LET, CC_LET, CC_BAD, CC_BAD, CC_BAD, CC_BAD, CC_BAD,
	/*60*/ CC_BAD, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET,
	/*68*/ CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET,
	/*70*/ CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET, CC_LET,
	/*78*/ CC_LET, CC_LET, CC_LET, CC_BAD, CC_BAD, CC_BAD, CC_BAD, CC_BAD };

static byte Unpack1(word packedWord)
{
	byte ch;

	ch = packedWord % 40;
	if (ch == 0)
		ch = ' ';
	else if (ch <= 10)
		ch += '0' - 1;    /* digit */
	else
		ch += '?' - 11;    /* ? @ and letters */
	return ch;
}




void UnpackToken(apointer src, pointer dst)
{
    word packedword;

	packedword = src[1];
	dst += 5;
	*dst-- = Unpack1(packedword); packedword /= 40;
	*dst-- = Unpack1(packedword); packedword /= 40;
	*dst-- = Unpack1(packedword);
	packedword = src[0];
	*dst-- = Unpack1(packedword); packedword /= 40;
	*dst-- = Unpack1(packedword); packedword /= 40;
	*dst-- = Unpack1(packedword);
}


void InsertSym()
{
	pointer p, q;

    /* move up the top block of the symbol tables to make room */
	/* minor rewrite for c pointer arithmetic */
	symHighMark = p = (q = symHighMark) + sizeof(tokensym_t);

    if (baseMacroTbl < p)
        RuntimeError(1);    /* table Error() */

    while (q > (pointer)tokenSym.curP)   /* byte coqy */
        *--p = *--q;
    /* insert the new symbol name */
    memcpy(tokenSym.curP, tokPtr, 4);
    endSymTab[TID_MACRO]++;    /* mark new top of macro table - scaled for c pointer arithmetic */
    tokenSym.curP->type = 0;            /* clear the type */
}

static bool OutSideTable(byte tableId)
{        /* check if tokenSym.curP is outside table bounds */
    if (endSymTab[tableId] >= tokenSym.curP && tokenSym.curP >= symTab[tableId])
        return false;

    SyntaxError();
    return true;
}


void Sub5CAD(word line, byte type)
{
    if (OutSideTable(TID_MACRO))
        return;
    InsertSym();
    tokenSym.curP->line = line;        /* fill in the rest of the new entry */
    tokenSym.curP->type = type;
    tokenSym.curP->flags = 0;
    PopToken();
//	DumpSymbols(TID_MACRO);
}

byte labelUse;


void SetTokenType(byte type, bool isSetOrEqu)
{
	tokenType[0] = type;
	if ((acc1ValType == K_REGNAME || acc1ValType == K_SP) && isSetOrEqu)
		tokenType[0] = 12 - type;    /* set-> K_SP, equ->K_REGNAME */
}

void UpdateSymbolEntry(word line, byte type)
{
	byte flags, absFlag;
	bool lineSet, isSetOrEqu;
    byte origType;

    /* type = 2 -> target
          4 -> set
          5 -> equ
          6 -> public
          9 -> word ref
         0x3a-> ??
             0x8x-> needs absolute value
    */


    origType = tokenType[0];
    isSetOrEqu = type == 5 || type == 4;
    absFlag = 0;
    
	flags = tokenSym.curP->flags;
    lineSet = false;
    if (OutSideTable(TID_SYMBOL))        /* oops if outside normal symbol table */
        return;

    if (tokenIdx > 1)
        SyntaxError();

    if (IsPhase1())
        if (tokenType[0] == O_NAME) {
            if (createdUsrSym) {
                if (tokenSym.curP->type >= 0x80 || (type == K_MACRONAME && tokenSym.curP->line != srcLineCnt)) {
                    LocationError();
                    absFlag = 0x80;
                }
            } else {
                InsertSym();
                symTab[TID_MACRO]++;        /* adjust the base of the macro table */
                endSymTab[TID_SYMBOL]++;    /* adjust the end of the user symbol table */
                flags = 0;
            }

            flags = (activeSeg == SEG_ABS ? 0 : UF_BOTH) | (inPublic ? UF_PUBLIC : 0) | (inExtrn ? (UF_EXTRN + UF_BOTH) : 0);
            if (labelUse == 1)    /* set || equ */
                flags = acc1Flags;

            if (labelUse == 2)    /* label: */
                flags |= activeSeg;

            if (hasVarRef && isSetOrEqu)
                tokenType[0] = O_MACROID;
            else
                SetTokenType(type, isSetOrEqu);

            goto endUpdateSymbol;
        }

    if (passCnt == 2)
        if (tokenType[0] == O_NAME)
            if (acc1ValType != O_NAME)
                if (isSetOrEqu)
                {
                    SetTokenType(type, isSetOrEqu);
                    if (tokenSym.curP->type < 128)
                    {
                        tokenSym.curP->type = tokenType[0];
                        tokenSym.curP->line = line;
                        flags = acc1Flags;
                        lineSet = true;
                    }
                    goto endUpdateSymbol;
                }

    if (IsPhase1())
        if (tokenType[0] == O_REF)
            if (TestBit(type, b5666))
            {
                if (inExtrn)
                    tokenType[0] = 3;
                else
                {
                    tokenType[0] = type;
                    flags &= 0xE0;    /* mask off seg, low, high */
                    if (labelUse == 1) /* set || equ */
                        flags = acc1Flags | UF_PUBLIC;

                    if (labelUse == 2) /* label: */
                        if (activeSeg != 0)
                            flags |= activeSeg | (UF_PUBLIC + UF_BOTH);
                }
                goto endUpdateSymbol;
            }

    if (IsPhase1())
        if (type == O_REF)
            if (TestBit(tokenType[0], b5666))
            {
                if ((flags && 0x60) != 0)
                    tokenType[0] = O_LABEL;
                else
                    flags |= UF_PUBLIC;
                goto endUpdateSymbol;
            }

    if (IsPhase1())
        if (tokenType[0] != type && tokenType[0] != 8 || type == 5)
            tokenType[0] = 3;

    if (! inPublic && TestBit(tokenType[0], b5669))
        flags = acc1Flags | (tokenType[0] != K_MACRONAME ? flags & UF_PUBLIC : 0);
    else
    {
        if (IsPhase1())
            tokenType[0] = 3;

        if (! (inPublic || inExtrn))
            if (tokenSym.curP->line != line)
                PhaseError();
    }

endUpdateSymbol:
    absFlag |= (tokenSym.curP->type & 0x80);

    if (IsPhase1() && (type == 9 || type == 6 || origType != tokenType[0]))
        tokenSym.curP->type = tokenType[0] | absFlag;

    kk = tokenSym.curP->type;
    if (tokenType[0] == 3 || kk == 3)
        MultipleDefError();

    if (kk >= 0x80)
        LocationError();

    if (IsPhase1() && (tokenType[0] == type || (type == 5 && tokenType[0] == 7))
       || (type == 4 && BlankAsmErrCode()) || lineSet
       || type == K_MACRONAME)
        tokenSym.curP->line = line;

    tokenSym.curP->flags = flags;
    inPublic = 0;
    inExtrn = 0;
    if (tokenSym.curP->type == 6)
        UndefinedSymbolError();

    hasVarRef = false;
    if (createdUsrSym)
        PopToken();
//	DumpSymbols(TID_SYMBOL);
//	DumpSymbols(TID_MACRO);
//	DumpTokenStack();
}

/*
    two different tables are used in lookup
    table 0: static keyword lookup the individual entries are coded as
        packed keyword - byte * 4 - 3 chars per word
        opcode base - byte
        offset to next entry || 0 if end - byte
        type - byte (add 0x80 if abs value)
        flags - byte
    initial entry is determined by hashing the name

    table 1: is a dynamic symbol table 8 bytes per entry kept sorted to allow binary chop search
    individual entries encoded as
        

    table 2: is a dynamic macro table 8 bytes per entry kept sorted to allow binary chop search

*/    

byte Lookup(byte tableId)
{
	tokensym_t *lowOffset, *highOffset, *midOffset;
	word deltaToNext;
	tokensym_t *entryOffset;
	apointer packedTokP;
    byte i, gt;
//    symEntry based entryOffset KEYWORD_T,
//    addr based pAddr word;

    packedTokP = (apointer)tokPtr;
    /* Keyword() lookup */
    if (tableId == TID_KEYWORD)        /* hash chain look up key word */
    {
		/* code modified to keep deltaToNext as offset */
        entryOffset = symTab[TID_KEYWORD] /* is 0 in plm */;    /* offset to current symbol to compare */
                    /* offset of first to use - hashes packed symbol name */
        deltaToNext = /* symTab[TID_KEYWORD] + */ ((packedTokP[0] + packedTokP[1] & 0xffff)) % 151 /* * 8 */;	// C pointers auto scale

        while (deltaToNext != 0) {    /* while ! end of chain */
            entryOffset += deltaToNext;    /* point to the next in chain */
            if (entryOffset->tok[0] == packedTokP[0])        /* check for exact match */
                if (entryOffset->tok[1] == packedTokP[1]) {
                    tokenSym.curP = entryOffset;
                    tokenType[0] = tokenSym.curP->type;
                    if (tokenType[0] < K_SINGLE)    /* instruction with arg */
                        if (op16[tokenType[0]])
                            has16bitOperand = true;

                    if (tokenSym.curP->flags == 2 && !controls.mod85)    /* RIM/SIM only valid on 8085 */
                        SourceError('O');

                    if (tokenType[0] == K_SP) {       /* SP */
                        if (!(newOp == K_LXI || newOp == K_REG16)) 
                            SourceError('X');
                        tokenType[0] = K_REGNAME;    /* reg */
                    }
                    return tokenType[0] & 0x7F;
                }
            deltaToNext = entryOffset->delta / sizeof(tokensym_t);	// scale for C pointer arithmetic
        }
        return O_NAME;
    }

    /* MACRO and User() tables are stored sorted 8 bytes per entry */
    /* use binary chop to find entry */
    lowOffset = symTab[tableId];
	highOffset = entryOffset = endSymTab[tableId];

    /* binary chop search for id */

    while ((midOffset = lowOffset + ((highOffset - lowOffset) >> 1)) != entryOffset) {
        entryOffset = midOffset;
        if (packedTokP[0] == entryOffset->tok[0])
        {
            if (packedTokP[1] == entryOffset->tok[1])
            {
                tokenSym.curP = entryOffset;
                tokenType[0] = tokenSym.curP->type;
                if (tokenType[0] == O_MACROID)
                    tokenType[0] = O_NAME;

                if ((usrLookupIsID = (kk = (tokenType[0] & 0x7F)) == O_NAME))
                    if (needsAbsValue)
                        tokenSym.curP->type = 0x80 + O_NAME;    /* update ABSVALUE flag */
                return kk;
            }
            else
                gt = entryOffset->tok[1] > packedTokP[1];
        }
        else
            gt = entryOffset->tok[0] > packedTokP[0];

        entryOffset = midOffset;
        if (gt)
            highOffset = entryOffset;
        else
            lowOffset = entryOffset;
    }

    tokenSym.curP = highOffset;
    if (tableId == TID_SYMBOL && !IsSkipping())
    {
        createdUsrSym = false;
        labelUse = 0;
        UpdateSymbolEntry(srcLineCnt, (needsAbsValue & 0x80) | O_NAME);
        /* update symbol stack to adjust pointers for entries above insert point */
        tokensym_t **p = tokenSym.stack;		// plm uses pAddr
        for (i = 1; i <= tokenIdx; i++) {
            if (*++p >= tokenSym.curP)
                ++*p;
        }

        createdUsrSym = true;
    }
    return O_NAME;
}



byte GetCh()
{
    static byte curCH, prevCH;

reGetCh:
    if (! reget) {
        prevCH = curCH;
    nextCh:
        curCH = lookAhead;
        if (expandingMacro) {
            while ((lookAhead = *macro.top.bufP) == MACROEOB) {
                ReadM(curMacroBlk + 1);
                macro.top.bufP = macroBuf;
            }

            macro.top.bufP++;
        }
        else if (scanCmdLine)
            lookAhead = GetCmdCh();
        else
            lookAhead = GetSrcCh();

//        if (chClass[curCH] == CC_BAD)		// check may access beyond end of array. also not needed
            if (curCH == 0 || curCH == 0x7F || curCH == FF)
                goto nextCh;
        if (expandingMacro) {
            if (curCH == ESC)
                goto doneGetCh;
            else if (curCH == '&') {
                if (prevCH >= 0x80 || lookAhead == 0x80)
                    goto reGetCh;
            } else if (curCH == '!' && prevCH != 0) {
                if (! (b905D || (b905E & 1)) && b905C) {
                    curCH = 0;
                    goto reGetCh;
                }
            } else if (curCH >= 128) {
                if (! (b905C = ! b905C))
                    macro.top.bufP = w9197;
                else {
                    w9197 = macro.top.bufP;
                    if (curCH == 0x80)
                    {
                        macro.top.bufP = macro.top.w12;
                        if (b9062 == 2) {
                            b91A1[1] = *macro.top.bufP;
                            macro.top.bufP = &b91A1[1];
                            if (*macro.top.bufP == '!') {
                                b91A1[0] = '!';
                                b91A1[1] = macro.top.w12[1];
                                macro.top.bufP--;
                            }
                        } else {
                            while ((byte)(--lookAhead) != 0xFF) {
                                macro.top.bufP -= (*macro.top.bufP & 0x7F);
                            }
							macro.top.bufP++;
                        }
                    } else {
                        macro.top.bufP = b91A4;
                        word tmp = lookAhead + macro.top.w4;		// plm reuses pAddr
                        for (ii = 1; ii <= 4; ii++) {
                            b91A4[6 - ii] = tmp % 10 + '0';
                            tmp /= 10;
                        }
                    }
                }

                lookAhead = 0;
                goto reGetCh;
            }
        }

        if (expandingMacro > 1)
            if (IsPhase2Print())
                if (macroP < macroLine + 127)    /* append character */
                {
                    *macroP = curCH;    
                    macroP = macroP + 1;
                }

        if (b905E & 1)
            if (w919D != macroInPtr && curCH == CR || ! excludeCommentInExpansion)
            InsertCharInMacroTbl(curCH);

        if (!(prevCH == '!' || inComment))
        {
            if (curCH == '>')
                argNestCnt = argNestCnt - 1;

            if (curCH == '<')
                argNestCnt = argNestCnt + 1;
        }
    }
doneGetCh:
    reget = 0;
    return (curChar = curCH);
}

byte GetChClass()
{
    curChar = GetCh();
    if (b905D)
        return CC_MAC;

    return chClass[curChar];
}



void ChkLF()
{
    if (lookAhead == LF)
        lookAhead = 0;
    else
    {
        b905E &= 0xFE;
        IllegalCharError();
        b905E = b905E > 0 ? 0xff : 0;
    }
}

