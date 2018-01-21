#include "plm.h"

static byte ENDorSEMICOLON[2] = {T_END, T_SEMICOLON};
static byte tokenTypeTable[] = {
	T_NUMBER, T_NUMBER, T_NUMBER, T_VARIABLE,
	T_VARIABLE, T_PLUSSIGN, T_MINUSSIGN, T_STAR,
	T_SLASH, T_LPAREN, T_RPAREN, T_COMMA,
	T_COLON, T_SEMICOLON, T_STRING, T_PERIOD,
	T_EQ, T_LT, T_GT, T_VARIABLE,
	T_VARIABLE, T_VARIABLE, T_VARIABLE, T_VARIABLE};

byte typeProcIdx[] = {
	1, 1, 1, 2, 2, 3, 3, 3,
	4, 3, 3, 3, 5, 3, 6, 3,
	3, 7, 8, 0, 9, 9, 10, 0};

byte binValidate[4]  = {0, 1, 1, 1};
byte octValidate[4]  = {0, 0, 1, 1};
byte decValidate[4]  = {0, 0, 0, 1};
byte hexValidate[4]  = {0, 0, 0, 2};

bool endToggle = false;

// lifted to file scope because of nested procedures
byte saveClass;
word curOff;

static byte Toupper(byte c)
{
    if ('a' <= c && c <= 'z')
    c = c & 0x5F;
    return c;
}



// lifted to file scope because of nested procedure
byte tokenLen, ch;

static void Str2num(byte radix, pointer validateP)
{
    word trial;
    byte valId;
    bool errored;
    byte curoff, ct;

    tokenVal = trial = 0 ;
    errored = false;
    for (curoff = 1; curoff <= tokenLen; curoff++) {
        if ((ct = cClass[ch = tokenStr[curoff]]) > CC_HEXCHAR)
            valId = 1;
        else
            valId = validateP[ct];
        switch (valId) {
        case 0: ch = ch - '0'; break;
        case 1: if (! errored) {
                    TokenErrorAt(ERR5);	/* INVALID CHARACTER IN NUMERIC CONSTANT */
                    errored = true;
                }
                ch = 0;
                break;
         case 2: ch = ch - 'A' + 10; break;
        }

        if ((trial = tokenVal * radix + ch) < tokenVal)
        {
            TokenErrorAt(ERR94);	/* ILLEGAL CONSTANT, VALUE > 65535 */
            return;
        }
        tokenVal = trial;
    }
}


static void Token2Num()
{
    if (cClass[ch = tokenStr[tokenLen = tokenStr[0]]] <= CC_DECDIGIT)
        Str2num(10, decValidate);
    else 
    {
        tokenLen = tokenLen - 1;
        if (ch == 'H')
            Str2num(16, hexValidate);
        else if (ch == 'Q' || ch == 'O')
            Str2num(8, octValidate);
        else if (ch == 'B')
            Str2num(2, binValidate);
        else if (ch == 'D')
            Str2num(10, decValidate);
        else {
            TokenErrorAt(ERR4);	/* ILLEGAL NUMERIC CONSTANT TYPE */
            tokenVal = 0;
        }
    }
} /* Token2Num() */

static void NestMacro()
{
    pointer tmp;

    tmp = off2Ptr(GetLitAddr() + 2);	
    WrXrefUse();
    if (macroDepth == 10)
        TokenErrorAt(ERR7);	/* LIMIT EXCEEDED: MACROS NESTED TOO DEEPLY */
    else {
        SetType(7);
        macroPtrs[macroDepth = macroDepth + 2] = inChrP;
        macroPtrs[macroDepth + 1] = off2Ptr(curMacroInfoP);
        inChrP = tmp - 1;
        curMacroInfoP = curInfoP;
    }
} /* NestMacro() */

static bool ChkMacro()
{
    Lookup(tokenStr);
    markedSymbolP = curSymbolP;
    if (High(SymbolP(curSymbolP)->infoP) == 0xFF)	/* simple key word */
        tokenType = Low(SymbolP(curSymbolP)->infoP);
    else { 
        FindInfo();
        if (curInfoP != 0)
            if (GetType() == LIT_T) {
                NestMacro();
                return false;
            } else if (GetType() == MACRO_T) {
                TokenErrorAt(ERR6);	/* ILLEGAL MACRO REFERENCE, RECURSIVE EXPANSION */
                return false;
            }
    }
    return true;
}

static void GetName(word maxlen)
{
    byte ct;

    curOff = 1;
   
    ct = cClass[lastCh]; 
    while (ct <= CC_ALPHA || lastCh == '$') {
        if (lastCh == '$')
            GNxtCh();
        else if (curOff > maxlen) {
            TokenErrorAt(ERR3);	/* IDENTIFIER, STRING, or NUMBER TOO LONG, TRUNCATED */
            while (ct <= CC_ALPHA || lastCh == '$') {
                GNxtCh();
                ct = cClass[lastCh];
            }
            curOff = maxlen + 1;
        } else {
            tokenStr[curOff] = Toupper(lastCh);
            curOff = curOff + 1;
            GNxtCh();
        }
        ct = cClass[lastCh];
    }
    
    tokenStr[0] = curOff - 1;
}


static void ParseString()
{
    bool tooLong;

    tooLong = false;
    curOff = 1;
    
    while (1) {
        GNxtCh();
        while (lastCh != '\'') {
            while (lastCh == '\r' || lastCh == '\n')
                GNxtCh();
            if (lastCh != QUOTE) {
                if (curOff != 256) {
                    tokenStr[curOff] = lastCh;
                    curOff = curOff + 1;
                } else {
                    tooLong = true;
                    if (lastCh == ';') {
                        TokenErrorAt(ERR85);	/* LONG STRING ASSUMED CLOSED at NEXT SEMICOLON or QUOTE */
                        goto l5F4E;
                    }
                }
                GNxtCh();
            }
        }
        GNxtCh();
        if (lastCh == QUOTE)
            if (curOff != 256) {
                tokenStr[curOff] = lastCh;
                curOff = curOff + 1;
            } else
                tooLong = true;
        else
           break;
    }
l5F4E:
    tokenStr[0] = curOff - 1;
    if (tokenStr[0] == 0)
        TokenErrorAt(ERR189);	/* LONG STRING ASSUMED CLOSED at NEXT SEMICOLON or QUOTE */
    if (tooLong)
        TokenErrorAt(ERR3);	/* IDENTIFIER, STRING, or NUMBER TOO LONG, TRUNCATED */
}


static void LocYylex()
{
    word tmp;
    bool inComment;

    while (1) {
        saveClass = cClass[lastCh];
        tokenType = tokenTypeTable[saveClass];
        switch (typeProcIdx[saveClass]) {
        case 0:	/* white space */
            GNxtCh();
            saveClass = cClass[lastCh];
            while (saveClass == CC_WSPACE) {
                GNxtCh();
                saveClass = cClass[lastCh];
            }
            break;
        case 1:	/* digits */
            GetName(31);
            Token2Num();
            return;
        case 2:	/* letters */
            tmp = curInfoP;
            GetName(255);
            if (ChkMacro())
                return;
            curInfoP = tmp;
            GNxtCh();
            break;
        case 3:	/* -, +, *, (,), ,, ;, = */
            GNxtCh();
            return;
        case 4:	/* slash */
            GNxtCh();
            inComment = true;
            if (lastCh != '*')
                return;
            GNxtCh();
            while (inComment) {
                while (lastCh != '*') {
                    GNxtCh();
                }
                GNxtCh();
                if (lastCh == '/') {
                    inComment = false;
                    GNxtCh();
                }
            }
            break;
        case 5:	/* : */
            GNxtCh();
            if (lastCh == '=') {
                tokenType = T_COLON_EQUALS;
                GNxtCh();
            }
            return;
        case 6:	/* quote */
            ParseString();
            return;
        case 7:	/* < */
            GNxtCh();
            if (lastCh == '>') {
                tokenType = T_NE;
                GNxtCh();
            } else if (lastCh == '=') {
                tokenType = T_LE;
                GNxtCh();
            }
            return;
        case 8:	/* > */
            GNxtCh();
            if (lastCh == '=') {
                tokenType = T_GE;
                GNxtCh();
            }
            return;
        case 9:	/* $, !, ", #, %, &, ?, @, [, \, ], ^, _, `, {, |, }, ~ */
            TokenErrorAt(ERR1);	/* INVALID PL/M-80 CHARACTER */
            GNxtCh();
            break;
        case 10:	/* non white space control chars and DEL */
            TokenErrorAt(ERR2);	/* UNPRINTABLE ASCII CHARACTER */
            GNxtCh();
            break;
        }
    }
} /* LocYylex() */

void Yylex()
{
	if (yyAgain)
		yyAgain = false;
	else if (unexpectedEOF)
		tokenType = ENDorSEMICOLON[endToggle = (endToggle + 1) & 1];
	else {
		LocYylex();
		if (tokenType == T_EOF) {
		    unexpectedEOF = true;
		    tokenType = T_END;
		}
	}
}

void SetYyAgain()
{
	yyAgain = true;
}

/*
	look for matching token
*/
bool YylexMatch(byte token)
{
    Yylex();			/* get the token to check */
    if (tokenType == token)
		return true;
    else {
	    SetYyAgain();	/* not matching but push back */
	    return false;
    }
}

bool YylexNotMatch(byte token)
{
    return ! YylexMatch(token);
}

static void Sub_60F9()
{
	word i;

	i = 1;
	WrOprAndValue();
	Yylex();
    
	while (1) {
		if (tokenType == T_LPAREN)
		    i = i + 1;
		else if (tokenType == T_RPAREN) {
		    if ((i = i - 1) == 0) {
                WrOprAndValue();
                return;
		    }
		} else if (tokenType == T_SEMICOLON || (tokenType >= T_CALL && tokenType <= T_RETURN)) {
			TokenErrorAt(ERR82);	/* INVALID SYNTAX, MISMATCHED '(' */
			while (i != 0) {
				WrByte(L_RPAREN);
				i = i - 1;
			}
			SetYyAgain();
			return;
		}
		WrOprAndValue();
		Yylex();
	}
}


void ParseExpresion(byte endTok)
{
	Yylex();
	while (tokenType != endTok && tokenType != T_SEMICOLON) {
		if (T_CALL <= tokenType && tokenType <= T_RETURN)
			break;
		WrOprAndValue();
		Yylex();
	}
	SetYyAgain();
}


/*
	Error() recovery to) or end of statement
	skip to ; or
		) unless inside nested ()
*/
static void Sub_61CF()
{
	word i;

	i = 0;
	while (1) {
		if (tokenType == T_SEMICOLON)
			break;
		if (tokenType == T_LPAREN)
			i = i + 1;
		else if (tokenType == T_RPAREN)
			if (i == 0)
				break;
			else
				i = i - 1;
		Yylex();	/* get next token */
	}
	SetYyAgain();	/* push back token */
}


/*
	Error() recovery to next element in parameter list
	skip to ; or
		) or , unless inside nested ()
*/
static void Sub_621A()
{
	word i;
	i = 0;
	while (1) {
		if (tokenType == T_SEMICOLON)
			break;
		if (i == 0)
			if (tokenType == T_COMMA)
				break;
		if (tokenType == T_LPAREN)
			i = i + 1;
		else if (tokenType == T_RPAREN)
			if (i == 0)
				break;
			else
				i = i - 1;
		Yylex();	/* get next token */
	}
	SetYyAgain();	/* push back token */
}



static offset_t declNames[33];
static word declBasedNames[33];
static word declNameCnt;
static offset_t declaringName;
static word declaringBase;
static word parentStructure;
static word basedInfo;
static byte dclFlags[3];
static byte dclType;
static offset_t lastLit;
static word arrayDim;
static offset_t structMembers[33];
static word structMemDim[33];
static byte structMemType[33];
static word structMCnt;
static byte byte_9D7B;
static byte flag;




static void Sub_62B0(word errcode)
{
    TokenError((byte)errcode, declaringName);
}



static void ChkModuleLevel()
{
    if (*curProcData != 0x100)
        TokenErrorAt(ERR73);	/* INVALID ATTRIBUTE or INITIALIZATION, not at MODULE LEVEL */
}

static void ChkNotArray()
{
    if (TestFlag(dclFlags, F_ARRAY))
    {
        TokenErrorAt(ERR63);	/* INVALID DIMENSION WITH THIS ATTRIBUTE */
        ClrFlag(dclFlags, F_ARRAY);
        ClrFlag(dclFlags, F_STARDIM);
        arrayDim = 0;
    }
}


static void ChkNotStarDim()
{
    if (TestFlag(dclFlags, F_STARDIM))
    {
        TokenErrorAt(ERR62);	/* INVALID STAR DIMENSION WITH 'structure' or 'external' */
        ClrFlag(dclFlags, F_STARDIM);
        arrayDim = 1;
    }
}

static void CreateStructMemberInfo()
{
    word i, memDim;
    byte memType;

    if (structMCnt == 0)
        return;

    for (i = 1; i <= structMCnt; i++) {
        curSymbolP = structMembers[i];
        memType = structMemType[i];
        memDim = structMemDim[i];
        CreateInfo(*curProcData, memType);
        WrXrefDef();
        SetInfoFlag(F_MEMBER);
        SetInfoFlag(F_LABEL);
        if (memDim != 0) {
            SetInfoFlag(F_ARRAY);
            SetDimension(memDim);
        } else
            SetDimension(1);
        SetParentOffset(parentStructure);
    }
}

static void Sub_63B7(word v)
{
    byte cFlags, i;
    
    curSymbolP = declaringName;
    FindScopedInfo(*curProcData);
    if (curInfoP != 0) {
        WrXrefUse();
        if (TestInfoFlag(F_PARAMETER) && !TestInfoFlag(F_LABEL)) {
            cFlags = 0;
            for (i = 0; i <= 2; i++)
                cFlags = cFlags | dclFlags[i];
            if (cFlags != 0)
                Sub_62B0(ERR76);	/* CONFLICTING ATTRIBUTE WITH PARAMETER */
            if (dclType != 2 && dclType != 3)
                Sub_62B0(ERR79);	/* ILLEGAL PARAMETER TYPE, not byte or address */
            else
                SetType(dclType);
            if (declaringBase != 0) {
                Sub_62B0(ERR77);	/* INVALID PARAMETER DECLARATION, BASE ILLEGAL */
                declaringBase = 0;
            }
        } else {
            Sub_62B0(ERR78);	/* INVALID PARAMETER DECLARATION, BASE ILLEGAL */
            return;
        }
    } else {
        CreateInfo(*curProcData, dclType);
        WrXrefDef();
        CpyFlags(dclFlags);
    }
    parentStructure = curInfoP;
    if (dclType == 0) {
        if (declaringBase != 0)
            Sub_62B0(ERR81);	/* CONFLICTING ATTRIBUTE WITH 'BASE' */
        SetLitAddr(lastLit);
        return;
    } else if (dclType == 1) {
        if (declaringBase != 0)
            Sub_62B0(ERR80);	/* INVALID DECLARATION, label MAY not BE based */
        if (TestInfoFlag(F_EXTERNAL))
            SetInfoFlag(F_LABEL);
        return;
    } else {
        if (declaringBase != 0) {
            if (TestInfoFlag(F_PUBLIC) || TestInfoFlag(F_EXTERNAL)
              || TestInfoFlag(F_AT) || TestInfoFlag(F_INITIAL)
              || TestInfoFlag(F_DATA)) {
                Sub_62B0(ERR81);	/* CONFLICTING ATTRIBUTE WITH 'BASE' */
                declaringBase = 0;
            } else
                SetInfoFlag(F_BASED);
        }
        SetDimension(arrayDim);
        SetBaseOffset(declaringBase);
        curInfoP = curProcInfoP;
        if (TestInfoFlag(F_REENTRANT)) {
            curInfoP = parentStructure;
            if (!( TestInfoFlag(F_DATA) || TestInfoFlag(F_BASED) || TestInfoFlag(F_AT)))
                SetInfoFlag(F_AUTOMATIC);
        }
        curInfoP = parentStructure;
    }
    if (TestInfoFlag(F_PARAMETER))
        byte_9D7B = byte_9D7B + 1;
    else if (v - byte_9D7B != 1)
        SetInfoFlag(F_PACKED);
    SetInfoFlag(F_LABEL);
    if (dclType == 4)
        CreateStructMemberInfo();
}

static void Sub_65AF()
{
    word i;

    byte_9D7B = 0;
    for (i = 1; i <= declNameCnt; i++) {
        declaringName = declNames[i];
        declaringBase = declBasedNames[i];
        Sub_63B7(i);
    }
}

/*
    parse at, data or initial argument
*/
static void Sub_65FA(byte lexItem, byte locflag)
{
    if (TestFlag(dclFlags, F_EXTERNAL))
        TokenErrorAt(ERR41);	/* CONFLICTING ATTRIBUTE */
    if (YylexMatch(T_LPAREN)) {
        WrByte(lexItem);
        if (flag)
            WrOffset(topInfo + 1);
        else
            WrOffset(botInfo);
        SetFlag(dclFlags, locflag);
        Sub_60F9();
    } else
        TokenErrorAt(ERR75);	/* MISSING ARGUMENT OF 'at' , 'data' , or 'initial' */
}


static void ParseDclInitial()
{
    if (YylexMatch(T_INITIAL)) {
        ChkModuleLevel();
        Sub_65FA(L_INITIAL, F_INITIAL);
    } else if (YylexMatch(T_DATA))
        Sub_65FA(L_DATA, F_DATA);
    else if (TestFlag(dclFlags, F_STARDIM)) {
        TokenErrorAt(ERR74);	/* INVALID STAR DIMENSION, not WITH 'data' or 'initial' */
        ClrFlag(dclFlags, F_STARDIM);
        arrayDim = 1;
    }
}

static void ParseDclAt()
{
    if (YylexMatch(T_AT))
        Sub_65FA(L_AT, F_AT);
}

static void ParseDclScope()
{
    if (YylexMatch(T_PUBLIC)) {
        ChkModuleLevel();
        SetFlag(dclFlags, F_PUBLIC);
    } else if (YylexMatch(T_EXTERNAL)) {
        ChkNotStarDim();
        ChkModuleLevel();
        SetFlag(dclFlags, F_EXTERNAL);
    }
}

static void ParseStructMType()
{
    word type;
    
    if (YylexMatch(T_BYTE))
        type = 2;
    else if (YylexMatch(T_ADDRESS))
        type = 3;
    else {
        type = 2;
        if (YylexMatch(T_STRUCTURE)) {
            TokenErrorAt(ERR70);	/* INVALID MEMBER TYPE, 'structure' ILLEGAL */
            if (YylexMatch(T_LPAREN)) {
                Sub_61CF();
                Yylex();
            }
        }
        else if (YylexMatch(T_LABEL))
            TokenErrorAt(ERR71);	/* INVALID MEMBER TYPE, 'label' ILLEGAL */
        else
            TokenErrorAt(ERR72);	/* MISSING TYPE FOR structure MEMBER */
    }
    structMemType[structMCnt] = (byte)type;
}

static void ParseStructMDim()
{
    word dim;
    
    if (YylexMatch(T_LPAREN))
    {
        if (YylexMatch(T_NUMBER))
            dim = tokenVal;
        else if (YylexMatch(T_STAR)) {
            dim = 1;
            TokenErrorAt(ERR69);	/* INVALID STAR DIMENSION WITH structure MEMBER */
        } else {
            dim = 1;
            TokenErrorAt(ERR59);	/* ILLEGAL DIMENSION ATTRIBUTE */
        }
        if (dim == 0) {
            dim = 1;
            TokenErrorAt(ERR57);	/* ILLEGAL DIMENSION ATTRIBUTE */
        }
        if (YylexNotMatch(T_RPAREN)) {
            TokenErrorAt(ERR60);	/* MISSING ') ' at end OF DIMENSION */
            Sub_61CF();
            Yylex();
        }
        structMemDim[structMCnt] = dim;
    }
}


static void LocParseStructMem()
{
    byte mcnt;

    if (YylexNotMatch(T_VARIABLE))
        TokenErrorAt(ERR66);	/* INVALID structure MEMBER, not AN IDENTIFIER */
    else {
        for (mcnt = 1; mcnt <= structMCnt; mcnt++) {
            if (curSymbolP == structMembers[mcnt])
                TokenErrorAt(ERR67);	/* DUPLICATE structure MEMBER NAME */
        }
        if (structMCnt == 32)
            TokenErrorAt(ERR68);	/* LIMIT EXCEEDED: NUMBER OF structure MEMBERS */
        else
            structMCnt = structMCnt + 1;
        structMembers[structMCnt] = curSymbolP;
        structMemType[structMCnt] = 0;
        structMemDim[structMCnt] = 0;
        ParseStructMDim();
        ParseStructMType();
    }
}

static void ParseStructMem()
{
    structMCnt = 0;
    if (YylexNotMatch(T_LPAREN))
        TokenErrorAt(ERR64);	/* MISSING structure MEMBERS */
    else {
        while (1) {
            LocParseStructMem();
            if (YylexNotMatch(T_COMMA))
                break;
        }
        if (YylexNotMatch(T_RPAREN)) {
            TokenErrorAt(ERR65);	/* MISSING ') ' at end OF structure MEMBER LIST */
            Sub_61CF();
            Yylex();
        }
    }
}

static void ParseDclDataType()
{
    if (YylexMatch(T_BYTE))
        dclType = 2;
    else if (YylexMatch(T_ADDRESS))
        dclType = 3;
    else if (YylexMatch(T_STRUCTURE)) {
        dclType = 4;
        ChkNotStarDim();
        ParseStructMem();
    } else if (YylexMatch(T_LABEL)) {
        dclType = 1;
        ChkNotArray();
    } else {
        TokenErrorAt(ERR61);	/* MISSING TYPE */
        dclType = 2;
    }
}

static void ParseArraySize()
{
    if (YylexMatch(T_LPAREN)) {
        SetFlag(dclFlags, F_ARRAY);
        if (YylexMatch(T_NUMBER)) {
            if (tokenVal == 0) {
                TokenErrorAt(ERR57);	/* INVALID DIMENSION, Zero() ILLEGAL */
                arrayDim = 1;
            }
            else
                arrayDim = tokenVal;
        } else if (YylexMatch(T_STAR)) {
            if (declNameCnt > 1) {
                TokenErrorAt(ERR58);	/* INVALID STAR DIMENSION IN FACTORED DECLARATION */
                arrayDim = 1;
            } else
                SetFlag(dclFlags, F_STARDIM);
        } else {
            TokenErrorAt(ERR59);	/* ILLEGAL DIMENSION ATTRIBUTE */
            arrayDim = 1;
        }
        if (YylexNotMatch(T_RPAREN)) {
            TokenErrorAt(ERR60);	/* MISSING ') ' at end OF DIMENSION */
            Sub_61CF();
            Yylex();
        }
    }
}

static void ParseDeclType()
{
    ParseArraySize();
    ParseDclDataType();
    ParseDclScope();
    ParseDclAt();
    ParseDclInitial();
}


static void ParseLiterally()
{
    if (YylexNotMatch(T_STRING)) {
        TokenErrorAt(ERR56);	/* INVALID MACRO TEXT, not A STRING CONSTANT */
        tokenStr[0] = 1;
        tokenStr[1] = ' ';
    }
    lastLit = CreateLit(tokenStr);
    dclType = 0;
}


static void ParseLitOrType()
{
    arrayDim = 1;
    ClrFlags(dclFlags);
    dclType = 0;
    if (YylexMatch(T_LITERALLY))
        ParseLiterally();
    else
        ParseDeclType();
}

static void GetBaseInfo()
{
    offset_t base1Name, base2Name;

    basedInfo = 0;
    if (YylexNotMatch(T_VARIABLE))
        TokenErrorAt(ERR52);	/* INVALID BASE, MEMBER OF based structure or ARRAY OF STRUCTURES */
    else {
        base1Name = curSymbolP;
        if (YylexMatch(T_PERIOD))
            if (YylexMatch(T_VARIABLE))
                base2Name = curSymbolP;
            else {
                TokenErrorAt(ERR53);	/* INVALID structure MEMBER IN BASE */
                return;
            }
        else
            base2Name = 0;
        curSymbolP = base1Name;
        FindInfo();
        if (curInfoP == 0) {
            TokenErrorAt(ERR54);	/* UNDECLARED BASE */
            return;
        }
        WrXrefUse();
        if (base2Name == 0)
            basedInfo = curInfoP;
        else {
            curSymbolP = base2Name;
            FindMemberInfo();
            if (curInfoP == 0) {
                TokenErrorAt(ERR55);	/* UNDECLARED structure MEMBER IN BASE */
                return;
            }
            WrXrefUse();
            basedInfo = curInfoP;
        }
    }
} /* GetBaseInfo() */


static void Sub_6A68()
{
    if (YylexNotMatch(T_VARIABLE))
        TokenErrorAt(ERR48);	/* ILLEGAL DECLARATION STATEMENT SYNTAX */
    else {
        if (declNameCnt == 32)
            TokenErrorAt(ERR49);	/* LIMIT EXCEEDED: NUMBER OF ITEMS IN FACTORED declare */
        else
            declNameCnt = declNameCnt + 1;

        declNames[declNameCnt] = curSymbolP;
        declBasedNames[declNameCnt] = 0;
        if (! flag) {
            FindScopedInfo(*curProcData);
            if (curInfoP == 0)
                flag = true;
        }
        if (YylexMatch(T_BASED)) {
            GetBaseInfo();
            if (basedInfo != 0) {
                curInfoP = basedInfo;
                if (TestInfoFlag(F_BASED) || TestInfoFlag(F_ARRAY) || GetType() != ADDRESS_T) { 
                    TokenErrorAt(ERR50);	/* INVALID ATTRIBUTES FOR BASE */
                    basedInfo = 0;
                } else if (TestInfoFlag(F_MEMBER)) {
                    curInfoP = GetParentOffset();
                    if (TestInfoFlag(F_ARRAY) || TestInfoFlag(F_BASED)) {
                        TokenErrorAt(ERR52);	/* INVALID BASE, MEMBER OF based structure or ARRAY OF STRUCTURES */
                        basedInfo = 0;
                    }
                }
                declBasedNames[declNameCnt] = basedInfo;
            }
        }
    }
}


void GetDclNames()
{
    declNameCnt = 0;
    flag = false;
    if (YylexMatch(T_LPAREN)) {
        while (1) {
            Sub_6A68();
            if (YylexNotMatch(T_COMMA))
                break;
        }
        if (YylexNotMatch(T_RPAREN)) {
            TokenErrorAt(ERR47);	/* MISSING ') ' at end OF FACTORED DECLARATION */
            Sub_61CF();
            Yylex();
        }
    }
    else
        Sub_6A68();
}

void ParseDcl()
{
	if (stmtLabelCnt != 0)
		SyntaxError(ERR46);	/* ILLEGAL USE OF label */
	while (1) {
		GetDclNames();
		if (declNameCnt != 0) {
			ParseLitOrType();
			Sub_65AF();
		}
		if (YylexNotMatch(T_COMMA))
			return;
    }
}

// lifted to file scope for nested procedures
bool hasParams;


static void SetPublic()
{
    if (GetScope() != 0x100)
        TokenErrorAt(ERR39);	/* INVALID ATTRIBUTE or INITIALIZATION, not at MODULE LEVEL */
    else if (TestInfoFlag(F_PUBLIC))
        TokenErrorAt(ERR40);	/* DUPLICATE ATTRIBUTE */
    else if (TestInfoFlag(F_EXTERNAL))
        TokenErrorAt(ERR41);	/* CONFLICTING ATTRIBUTE */
    else
        SetInfoFlag(F_PUBLIC);
}


static void SetExternal()
{
    if (GetScope() != 0x100)
        TokenErrorAt(ERR39);	/* INVALID ATTRIBUTE or INITIALIZATION, not at MODULE LEVEL */
    else if (TestInfoFlag(F_EXTERNAL))
        TokenErrorAt(ERR40);	/* DUPLICATE ATTRIBUTE */
    else if (TestInfoFlag(F_REENTRANT) || TestInfoFlag(F_PUBLIC))
        TokenErrorAt(ERR41);	/* CONFLICTING ATTRIBUTE */
    else
        SetInfoFlag(F_EXTERNAL);
}

static void SetReentrant()
{
    if (GetScope() != 0x100)
        TokenErrorAt(ERR39);	/* INVALID ATTRIBUTE or INITIALIZATION, not at MODULE LEVEL */
    else if (TestInfoFlag(F_REENTRANT))
        TokenErrorAt(ERR40);	/* DUPLICATE ATTRIBUTE */
    else if (TestInfoFlag(F_EXTERNAL))
        TokenErrorAt(ERR41);	/* CONFLICTING ATTRIBUTE */
    else
        SetInfoFlag(F_REENTRANT);
}

static void SetInterruptNo()
{
    if (YylexMatch(T_NUMBER)) {
        if (tokenVal > 255) {
            TokenErrorAt(ERR42);	/* INVALID interrupt VALUE */
            tokenVal = 0;
        }
    } else {
        TokenErrorAt(ERR43);	/* MISSING interrupt VALUE */
        tokenVal = 0;
    }
    if (GetScope() != 0x100)
        TokenErrorAt(ERR39);	/* INVALID ATTRIBUTE or INITIALIZATION, not at MODULE LEVEL */
    else if (TestInfoFlag(F_INTERRUPT))
        TokenErrorAt(ERR40);	/* DUPLICATE ATTRIBUTE */
    else if (hasParams)
        TokenErrorAt(ERR44);	/* ILLEGAL ATTRIBUTE, 'interrupt' WITH PARAMETERS */
    else if (GetDataType() != 0)
        TokenErrorAt(ERR45);	/* ILLEGAL ATTRIBUTE, 'interrupt' WITH TYPED procedure */
    else if (TestInfoFlag(F_EXTERNAL))
        TokenErrorAt(ERR41);	/* CONFLICTING ATTRIBUTE */
    else {
        SetInfoFlag(F_INTERRUPT);
        SetIntrNo((byte)tokenVal);
    }
}


static void ParseProcAttrib()
{
    curInfoP = curProcInfoP;
    
    while (1) {
        if (YylexMatch(T_PUBLIC))
            SetPublic();
        else if (YylexMatch(T_EXTERNAL))
            SetExternal();
        else if (YylexMatch(T_REENTRANT))
            SetReentrant();
        else if (YylexMatch(T_INTERRUPT))
            SetInterruptNo();
        else
            return;
    }
}

static void ParseRetType()
{
    curInfoP = curProcInfoP;
    if (YylexMatch(T_BYTE))
        SetDataType(2);
    else if (YylexMatch(T_ADDRESS))
        SetDataType(3);
}

word paramCnt;

static void Sub_6E4B()
{
    FindScopedInfo(*curProcData);
    if (curInfoP != 0)
        TokenErrorAt(ERR38);	/* DUPLICATE PARAMETER NAME */
    CreateInfo(*curProcData, BYTE_T);
    WrXrefDef();
    SetInfoFlag(F_PARAMETER);
    paramCnt = paramCnt + 1;
}

static void ParseParams()
{
    paramCnt = 0;
    if (YylexMatch(T_LPAREN)) {
        while (1) {
            hasParams = true;
            if (YylexMatch(T_VARIABLE))
                Sub_6E4B();
            else {
                TokenErrorAt(ERR36);	/* MISSING PARAMETER */
                Sub_621A();
            }
            if (YylexNotMatch(T_COMMA))
                break;
        }
        if (YylexNotMatch(T_RPAREN)) {
            TokenErrorAt(ERR37);	/* MISSING ') ' at end OF PARAMETER LIST */
            Sub_61CF();
            Yylex();
        }
        curInfoP = curProcInfoP;
        SetParamCnt((byte)paramCnt);
    }
    else
        hasParams = false;
}

void ParseProcDcl()
{
    word tmp;

	tmp = curProcInfoP;
	curSymbolP = stmtLabels[1];
	FindScopedInfo(*curProcData);
	if (curInfoP != 0)
		SyntaxError(ERR34);	/* DUPLICATE procedure DECLARATION */
	CreateInfo(*curProcData, PROC_T);
	SetInfoFlag(F_LABEL);
	WrXrefDef();
	if (procCnt == 254)
		FatalError(ERR35);	/* LIMIT EXCEEDED: NUMBER OF PROCEDURES */
	procInfo[procCnt = procCnt + 1] = curInfoP;
	procData.hb = (byte)procCnt;
	curProcInfoP = curInfoP;
	procData.lb = 0;
	doBlkCnt = 0;
	PushBlock(*curProcData);
	ParseParams();
	ParseRetType();
	ParseProcAttrib();
	curInfoP = curProcInfoP;
	if (! TestInfoFlag(F_EXTERNAL)) {
		WrByte(L_PROCEDURE);
		WrOffset(curInfoP);
		WrByte(L_SCOPE);
		WrWord(*curProcData);
	} else {
		WrByte(L_EXTERNAL);
		WrOffset(curInfoP);
	}
	SetProcId(procData.hb);
	if (tmp != 0) {
		curInfoP = tmp;
		if (TestInfoFlag(F_REENTRANT))
			SyntaxError(ERR88);	/* INVALID procedure NESTING, ILLEGAL IN reentrant procedure */
		curInfoP = curProcInfoP;
	}
}
