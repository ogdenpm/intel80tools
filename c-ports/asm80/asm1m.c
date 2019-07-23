// vim:ts=4:expandtab:shiftwidth=4:
#include "asm80.h"

byte tokReq[] = {
    /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
       0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
       0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
       0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
       0, 0 };

byte definingLHS[] = { 0x36, 0, 0, 0, 6, 0, 0, 2 };
/* bit vector 55 -> 0 x 24 00000110 0 x 16 0000001 */
/* 29, 30, 55 */
bool absValueReq[] = {
    /* 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F */
       false, false, false, false, false, false, false, false, false, false, false, false, false, false, false, false,
       false, false, false, false, false, false, false, false, false, false, false, false, true,  false, false, true,
       false, true,  false, false, false, false, false, false, false, false, false, false, false, false, false, false,
       false, false, false, false, false, false, false, false, false, false, true,  true,  true, false,  true,  false,
       false, false };
/* true for DS, ORG, IF, 3A?, IRP, IRPC DoRept */

byte b3F88[] = { 0x41, 0x90, 0, 0, 0, 0, 0, 0, 0, 0x40 };
/* bit vector 66 -> 10010000 0 x 56 01 */

void SkipWhite_2()
{
    while (GetCh() == ' ' || IsTab())
        ;
}


byte NonHiddenSymbol()
{
    // name based nameP word;

    word *nameP = tokenSym.curP->tok;
    /* check name < '??0' or '??9' < name */
    return (*nameP < 0x4679) || controls.macroDebug || (0x4682 < *nameP);
}


/* seek to specified macro block (128 byte) in macro spool file */
void SeekM(word blk)
{
    if (aVar.w = blk - nxtMacroBlk) {		// if already there then nothing to do
        kk = SEEKFWD;						// check if we need to seek fwd / bwd
        if (blk < nxtMacroBlk) {
            aVar.w = -aVar.w;
            kk = SEEKBACK;
        }

        Seek(macrofd, kk, &aVar.w, &seekMZero, &statusIO);	// seek to the required block
        IoErrChk();
    }
    nxtMacroBlk = blk + 1;					// update next block info
}



/* read in macro from disk - located at given block */
void ReadM(word blk)
{
    word actual;

    if (blk >= maxMacroBlk)			// does not exist
        actual = 0;
    else if (blk == curMacroBlk)	// already correct
        return;
    else {							// load the buffer
        SeekM(blk);
        Read(macrofd, macroBuf, 128, &actual, &statusIO);
        IoErrChk();
    }

    macro.top.blk = curMacroBlk = blk;	// set relevant trackers
    macroBuf[actual] = MACROEOB;    /* flag end of macro buffer */
}

/* write the macro to disk */
void WriteM()
{
    if (phase == 1) {	// only needs writing on pass 1
        SeekM(maxMacroBlk++);	// seek to end and update marker to account for this block
        Write(macrofd, symHighMark, 128, &statusIO);
        IoErrChk();
    }
    macroBlkCnt++;		// update the buffer count for this macro
}


// flush the macro buffer to disk in full 128 byte blocks
// residual are moved to start of macro buffer endSymTab[TID_MACRO]
void FlushM()
{
    word bytesLeft;

    if (mSpoolMode & 1) { /* spool macros to disk in 128 byte blocks */
        while ((bytesLeft = (word)(macroInPtr - symHighMark)) >= 128) {
            WriteM();
            symHighMark += 128;
        }
        /* move the remaining bytes to start of macro buffer */
        if (bytesLeft != 0)
            memcpy(endSymTab[TID_MACRO], symHighMark, bytesLeft);
        macroInPtr = (symHighMark = (pointer)endSymTab[TID_MACRO]) + bytesLeft;
    }
}


void SkipWhite()
{
    while (IsWhite())
        curChar = GetCh();
}



void Skip2NextLine()
{
    Skip2EOL();
    ChkLF();
}


void gotValue()
{
    if (curOp == T_VALUE)		// if previous was value then two consecutive values so error
        ExpressionError();
    inExpression = 0;
    curOp = T_VALUE;			// record we just saw a value
}

void Tokenise()
{
    while (true) {
        if (atStartLine) {
            ParseControlLines();
            atStartLine = false;
        }

        switch (GetChClass()) {
		case CC_ESC:	// moved to allow fall through to error case rather than use goto
			if (expandingMacro) {
				skipIf[0] = false;
				yyType = O_NULVAL;
				return;
			}
        case CC_BAD:
		    IllegalCharError(); break;
        case CC_WS: break;
        case CC_SEMI:
            if (!inQuotes) {
                inComment = true;
                if (GetChClass() == CC_SEMI && (mSpoolMode & 1)) {
                    excludeCommentInExpansion = true;
                    macroInPtr -= 2;		// remove the ;;
                }
                Skip2NextLine();			// process the rest of the line
                yyType = T_CR;
                return;
            }
            break;
        case CC_COLON:
            if (!gotLabel) {				// ok if only label on line
                if (skipIf[0] || (mSpoolMode & 1))	// skipping or spooling so junk tokens
                    PopToken();
                else {
                    labelUse = L_TARGET;			// note label usage and update symbol table with location
                    UpdateSymbolEntry(segLocation[activeSeg], O_TARGET);
                }
                expectingOperands = false;			// should see an opcode first
				gotLabel = expectingOpcode = bTRUE;
            }
            else {
                SyntaxError();				// two labels is an error
                PopToken();
            }
            EmitXref(XREF_DEF, name);		// this is a x-ref definition
            haveUserSymbol = false;			// not a user symbol but a label
            curOp = O_LABEL;
            break;
        case CC_CR:
            ChkLF();
            yyType = T_CR;
            inQuotes = false;		// auto close string for spooling
            return;
        case CC_PUN:
            if (curChar == '+' || curChar == '-')
                if (!TestBit(curOp, b3F88)) /* not T_BEGIN, T_RPAREN or K_NUL */
                    curChar += (T_UPLUS - T_PLUS);    /* make unary versions */
            yyType = curChar - '(' + T_LPAREN;
            return;
        case CC_DOLLAR:
            PushToken(O_NUMBER);        // $ is treated as a number
            CollectByte(segLocation[activeSeg] & 0xff); /* its value is the current seg's size*/
            CollectByte(segLocation[activeSeg] >> 8);
            if (activeSeg != SEG_ABS)   // if not abs set seg and relocatable flags
                tokenAttr[0] |= activeSeg | UF_RBOTH;
            gotValue();
            break;
        case CC_QUOTE:
            if (yyType == O_MACROPARAM) {		// quote not allowed in macro parameter (unless escaped)
                IllegalCharError();
                return;
            }
            if (mSpoolMode & 1)					// spooling passes through string, record whether in / out of string
                inQuotes = !inQuotes;
            else {
                GetStr();						// normal processing so collect string
                if (expectingOpcode)			// can't be an opcode
                    SetExpectOperands();
                gotValue();
            }
            break;
        case CC_DIG:
            GetNum();
            if (expectingOpcode)				// can't be an opcode
                SetExpectOperands();
            gotValue();
            break;
        case CC_LET:
            startMacroToken = macroInPtr - 1;
            GetId(O_NAME);    /* assume it's a name */
            if (tokenSize[0] > MAXSYMSIZE)  /* cap length */
                tokenSize[0] = MAXSYMSIZE;

            if (controls.xref) {
                memcpy(savName, name, MAXSYMSIZE);
                memcpy(name, spaces6, MAXSYMSIZE);
            }
            /* copy the token to name */
            memcpy(name, tokPtr, tokenSize[0]);
            nameLen = tokenSize[0];
            PackToken();        /* make into 4 byte name */
            if (haveUserSymbol) {			// user symbol not followed by a colon
                haveNonLabelSymbol = true;
                haveUserSymbol = false;
            }


            if (Lookup(TID_MACRO) != O_NAME && (mSpoolMode & 1)) {
                kk = tokenType[0] == 0; // assignment pulled out to allow short circuit tests
                if (!inQuotes || (kk && (curChar == '&' || startMacroToken[-1] == '&'))) {
                    macroInPtr = startMacroToken;
                    InsertCharInMacroTbl(kk ? 0x80 : 0x81);
                    InsertByteInMacroTbl((byte)GetNumVal());
                    InsertCharInMacroTbl(curChar);
                    yyType = O_NAME;			// reuse of yyType?
                }
            }
            else if (yyType != O_MACROPARAM && mSpoolMode != 2) {		// skip if capturing macro parameter or local names
                if (Lookup(TID_KEYWORD) == O_NAME) {       /* not a key word */
                    tokenType[0] = Lookup(TID_SYMBOL);    /* look up in symbol space */
                    haveUserSymbol = true;        /* note we have a user symbol */
                }

                yyType = tokenType[0];
                needsAbsValue = absValueReq[tokenType[0]]; /* DS, ORG, IF, K_MACRONAME, IRP, IRPC DoRept */
                if (!tokReq[tokenType[0]]) /* i.e. not instruction, reg or K_MACRONAME or punctuation */
                    PopToken();

                // for name seen to lhs of op emit xref, for SET/EQU/MACRO PARAM then this is defining
                // else it is reference
                if (haveNonLabelSymbol) {               /* EQU, SET or O_MACROPARAM */
                    EmitXref(!TestBit(yyType, definingLHS), savName);   // maps to XREF_DEF or XREF_REF
                    haveNonLabelSymbol = false;
                }
            }
            if (mSpoolMode == 1) {					// start of macro spooling
                if (yyType == K_LOCAL) {			// move to capture locals
                    mSpoolMode = 2;
                    if (spooledControl)				// error if there are any controls before local in macro
                        SyntaxError();
                    spooledControl = false;
                } else {
                    spooledControl = false;			// clear spooled control flag
                    mSpoolMode = 0xff;				// capture body
                }
            }

            if (yyType == K_NUL)
                PushToken(O_NULVAL);
            if (yyType < 10 /* | yyType == 9 | 80h*/) { /* !! only first term contributes */
                gotValue();
                if (expectingOpcode)
                    SetExpectOperands();
            }
            else {
                expectingOpcode = false;
                return;
            }
            break;
        case CC_MAC:
            nestedMacroSeen = false;
            GetMacroToken();
            if (nestedMacroSeen)
                return;
            break;
        }
    }
}
