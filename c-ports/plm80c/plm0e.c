#include "plm.h"

static byte ENDorSEMICOLON[2] = { T_END, T_SEMICOLON };
static byte tokenTypeTable[] = {
    T_NUMBER, T_NUMBER, T_NUMBER, T_IDENTIFIER,
    T_IDENTIFIER, T_PLUSSIGN, T_MINUSSIGN, T_STAR,
    T_SLASH, T_LPAREN, T_RPAREN, T_COMMA,
    T_COLON, T_SEMICOLON, T_STRING, T_PERIOD,
    T_EQ, T_LT, T_GT, 0,
    0, 0, 0, 0 };

byte typeProcIdx[] = {
    1, 1, 1, 2, 2, 3, 3, 3,
    4, 3, 3, 3, 5, 3, 6, 3,
    3, 7, 8, 0, 9, 9, 10, 0 };

byte binValidate[4] = { 0, 1, 1, 1 };
byte octValidate[4] = { 0, 0, 1, 1 };
byte decValidate[4] = { 0, 0, 0, 1 };
byte hexValidate[4] = { 0, 0, 0, 2 };



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

    tokenVal = trial = 0;
    errored = false;
    for (curoff = 1; curoff <= tokenLen; curoff++) {
        if ((ct = cClass[ch = tokenStr[curoff]]) > CC_HEXCHAR)
            valId = 1;
        else
            valId = validateP[ct];
        switch (valId) {
        case 0: ch = ch - '0'; break;
        case 1:
            if (!errored) {
                TokenErrorAt(ERR5);	/* INVALID CHARACTER IN NUMERIC CONSTANT */
                errored = true;
            }
            ch = 0;
            break;
        case 2: ch = ch - 'A' + 10; break;
        }

        if ((trial = tokenVal * radix + ch) < tokenVal) {
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
    else {
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
        SetType(MACRO_T);   // mark the type as  MACRO_T to spot recursive expansion
        macroPtrs[macroDepth = macroDepth + 2] = inChrP;    // push the current location
        macroPtrs[macroDepth + 1] = off2Ptr(curMacroInfoP); // and infoP
        inChrP = tmp - 1;               // adjust for initial increment in GNxtCh()
        curMacroInfoP = curInfoP;       // set up the new infoP
    }
} /* NestMacro() */

static bool IsNotLit()
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
    word curOff = 1;

    ct = cClass[nextCh];
    while (ct <= CC_ALPHA || nextCh == '$') {
        if (nextCh == '$')
            GNxtCh();
        else if (curOff > maxlen) {
            TokenErrorAt(ERR3);	/* IDENTIFIER, STRING, or NUMBER TOO LONG, TRUNCATED */
            while (ct <= CC_ALPHA || nextCh == '$') {   // junk rest of identifier
                GNxtCh();
                ct = cClass[nextCh];
            }
            curOff = maxlen + 1;                        // lenght will exceed limit
        } else {
            tokenStr[curOff++] = Toupper(nextCh);       // save name in upper case
            GNxtCh();
        }
        ct = cClass[nextCh];
    }

    tokenStr[0] = curOff - 1;       // save pstr length
}


static void ParseString()
{
    bool tooLong = false;
    word curOff = 1;

    // code simplified in port to C
    while (1) {
        GNxtCh();
        if (nextCh == '\r' || nextCh == '\n')   // strings can go over more than one line but cr lf ignored
            continue;
        if (nextCh == QUOTE) {  // double quote passes through as single quote
            GNxtCh();
            if (nextCh != QUOTE)    // single quote finishes string
                break;
        }
        if (curOff != 256)
            tokenStr[curOff++] = nextCh;
        else {
            tooLong = true;
            if (nextCh == ';') {
                TokenErrorAt(ERR85);	/* LONG STRING ASSUMED CLOSED AT NEXT SEMICOLON OR QUOTE */
                break;
            }
        }
    }
    tokenStr[0] = curOff - 1;   // record length of pstr
    if (tokenStr[0] == 0)
        TokenErrorAt(ERR189);	/* NULL STRING NOT ALLOWED */
    if (tooLong)
        TokenErrorAt(ERR3);	/* IDENTIFIER, STRING, or NUMBER TOO LONG, TRUNCATED */
}


static void LocYylex()
{
    word tmp;
    bool inComment;
    byte saveClass;

    while (1) {
        saveClass = cClass[nextCh];     // the lookahead char
        tokenType = tokenTypeTable[saveClass];
        switch (typeProcIdx[saveClass]) {
        case 0:	/* white space */
            do {
                GNxtCh();
                saveClass = cClass[nextCh];
            } while (saveClass == CC_WSPACE);
            break;
        case 1:	/* digits */
            GetName(31);
            Token2Num();
            return;
        case 2:	/* letters */
            tmp = curInfoP;
            GetName(255);
            if (IsNotLit())     // will cause lit to expand
                return;
            curInfoP = tmp;
            GNxtCh();           // carry on processing
            break;
        case 3:	/* -, +, *, (,), ,, ;, = */
            GNxtCh();
            return;
        case 4:	/* slash */
            GNxtCh();
            inComment = true;       // assume we have a comment
            if (nextCh != '*')      // no so return /
                return;
            GNxtCh();
            while (inComment) {
                while (nextCh != '*') {
                    GNxtCh();
                }
                GNxtCh();
                if (nextCh == '/') {    // found end of comment
                    inComment = false;
                    GNxtCh();
                }
            }
            break;
        case 5:	/* : */
            GNxtCh();
            if (nextCh == '=') {
                tokenType = T_COLON_EQUALS;
                GNxtCh();
            }
            return;
        case 6:	/* quote */
            ParseString();
            return;
        case 7:	/* < */
            GNxtCh();
            if (nextCh == '>') {
                tokenType = T_NE;
                GNxtCh();
            } else if (nextCh == '=') {
                tokenType = T_LE;
                GNxtCh();
            }
            return;
        case 8:	/* > */
            GNxtCh();
            if (nextCh == '=') {
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
    static bool endToggle = false;  // relies on C's false = 0, true = 1

    if (yyAgain)
        yyAgain = false;
    else if (afterEOF)  // return alternating END and ; to allow recovery
        tokenType = ENDorSEMICOLON[endToggle = !endToggle];
    else {
        LocYylex();
        if (tokenType == T_EOF) {
            afterEOF = true;
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
    return !YylexMatch(token);
}

static void WrNestedExpression()
{
    word nesting;

    nesting = 1;
    WrLexToken();
    Yylex();

    while (1) {
        if (tokenType == T_LPAREN)
            nesting++;
        else if (tokenType == T_RPAREN) {
            if ((--nesting) == 0) {
                WrLexToken();
                return;
            }
        } else if (tokenType == T_SEMICOLON || (tokenType >= T_CALL && tokenType <= T_RETURN)) {
            TokenErrorAt(ERR82);	/* INVALID SYNTAX, MISMATCHED '(' */
            while (nesting != 0) {
                WrByte(L_RPAREN);
                nesting--;
            }
            SetYyAgain();
            return;
        }
        WrLexToken();
        Yylex();
    }
}


// convert and write expression into lex tokens
// stop when endTok, semicolon or new start symbol for <declaration> or <unit> seen
void ParseExpresion(byte endTok)
{
    Yylex();
    while (tokenType != endTok && tokenType != T_SEMICOLON) {
        if (T_CALL <= tokenType && tokenType <= T_RETURN)
            break;
        WrLexToken();
        Yylex();
    }
    SetYyAgain();
}


/*
    Error() recovery to) or end of statement
    skip to ; or
        ) unless inside nested ()
*/
static void RecoverToSemiOrRP()
{
    word nesting;

    nesting = 0;
    while (1) {
        if (tokenType == T_SEMICOLON)
            break;
        if (tokenType == T_LPAREN)
            nesting++;
        else if (tokenType == T_RPAREN && nesting-- == 0)
            break;
        Yylex();	/* get next token */
    }
    SetYyAgain();	/* push back token */
}


/*
    Error() recovery to next element in parameter list
    skip to ; or
        ) or , unless inside nested ()
*/
static void RecoverMissingParam()
{
    word nesting;
    nesting = 0;
    while (1) {
        if (tokenType == T_SEMICOLON)
            break;
        if (nesting == 0 && tokenType == T_COMMA)
            break;
        if (tokenType == T_LPAREN)
            nesting++;
        else if (tokenType == T_RPAREN && nesting-- == 0)
            break;
        Yylex();	/* get next token */
    }
    SetYyAgain();	/* push back token */
}



static offset_t declNames[33];
static word declBasedNames[33];
static word declNameCnt;
static offset_t declaringName;
static word declaringBase;
static word savedCurInfo;
static word basedInfo;
static byte dclFlags[3];
static byte dclType;
static offset_t lastLit;
static word arrayDim;
static offset_t structMembers[33];
static word structMemDim[33];
static byte structMemType[33];
static word structMCnt;
static byte factoredParamCnt;
static bool isNewVar;




static void DeclarationError(word errcode)
{
    TokenError((byte)errcode, declaringName);
}



static void ChkModuleLevel()
{
    if (*curScopeP != 0x100)
        TokenErrorAt(ERR73);	/* INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL */
}

static void ChkNotArray()
{
    if (TestFlag(dclFlags, F_ARRAY)) {
        TokenErrorAt(ERR63);	/* INVALID DIMENSION WITH THIS ATTRIBUTE */
        ClrFlag(dclFlags, F_ARRAY); // can't be any form of array
        ClrFlag(dclFlags, F_STARDIM);
        arrayDim = 0;               // dim not applicable
    }
}


static void ChkNotStarDim()
{
    if (TestFlag(dclFlags, F_STARDIM)) {
        TokenErrorAt(ERR62);	/* INVALID STAR DIMENSION WITH 'STRUCTURE' OR 'EXTERNAL' */
        ClrFlag(dclFlags, F_STARDIM);   // reset to single instance
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
        CreateInfo(*curScopeP, memType);
        WrXrefDef();
        SetInfoFlag(F_MEMBER);
        SetInfoFlag(F_LABEL);
        if (memDim != 0) {
            SetInfoFlag(F_ARRAY);
            SetDimension(memDim);
        } else
            SetDimension(1);
        SetParentOffset(savedCurInfo);      // this is the infoP of the structure
    }
}

static void ChkAndSetAttributes(word factoredIdx)
{
    byte cFlags, i;

    curSymbolP = declaringName;
    FindScopedInfo(*curScopeP);
    if (curInfoP != 0) {        // identifier alredy in scope  - only valid for parameter
        WrXrefUse();
        if (TestInfoFlag(F_PARAMETER) && !TestInfoFlag(F_LABEL)) {
            cFlags = 0;                     /* check for existing flags */
            for (i = 0; i <= 2; i++)
                cFlags |= dclFlags[i];
            if (cFlags != 0)
                DeclarationError(ERR76);	/* CONFLICTING ATTRIBUTE WITH PARAMETER */
            if (dclType != BYTE_T && dclType != ADDRESS_T)
                DeclarationError(ERR79);	/* ILLEGAL PARAMETER TYPE, not byte or address */
            else
                SetType(dclType);
            if (declaringBase != 0) {
                DeclarationError(ERR77);	/* INVALID PARAMETER DECLARATION, BASE ILLEGAL */
                declaringBase = 0;
            }
        } else {
            DeclarationError(ERR78);	/* DUPLICATE DECLARATION */
            return;
        }
    } else {
        CreateInfo(*curScopeP, dclType);
        WrXrefDef();
        CpyFlags(dclFlags);
    }
    savedCurInfo = curInfoP;
    if (dclType == LIT_T) {
        if (declaringBase != 0)         // lits cannot be based
            DeclarationError(ERR81);	/* CONFLICTING ATTRIBUTE WITH 'BASE' */
        SetLitAddr(lastLit);
        return;
    } else if (dclType == LABEL_T) {
        if (declaringBase != 0)
            DeclarationError(ERR80);	/* INVALID DECLARATION, LABEL MAY NOT BE BASED */
        if (TestInfoFlag(F_EXTERNAL))
            SetInfoFlag(F_LABEL);
        return;
    } else {
        if (declaringBase != 0) {
            if (TestInfoFlag(F_PUBLIC) || TestInfoFlag(F_EXTERNAL)
                || TestInfoFlag(F_AT) || TestInfoFlag(F_INITIAL)
                || TestInfoFlag(F_DATA)) {
                DeclarationError(ERR81);	/* CONFLICTING ATTRIBUTE WITH 'BASE' */
                declaringBase = 0;
            } else
                SetInfoFlag(F_BASED);
        }
        SetDimension(arrayDim);
        SetBaseOffset(declaringBase);
        curInfoP = curProcInfoP;
        if (TestInfoFlag(F_REENTRANT)) {
            curInfoP = savedCurInfo;
            if (!(TestInfoFlag(F_DATA) || TestInfoFlag(F_BASED) || TestInfoFlag(F_AT)))
                SetInfoFlag(F_AUTOMATIC);
        }
        curInfoP = savedCurInfo;
    }
    if (TestInfoFlag(F_PARAMETER))
        factoredParamCnt++;
    else if (factoredIdx - factoredParamCnt != 1)   // isNewVar first non parameter var as packed
        SetInfoFlag(F_PACKED);
    SetInfoFlag(F_LABEL);
    if (dclType == STRUCT_T)
        CreateStructMemberInfo();
}

static void SetFactoredAttributes()
{
    word i;

    factoredParamCnt = 0;
    for (i = 1; i <= declNameCnt; i++) {
        declaringName = declNames[i];
        declaringBase = declBasedNames[i];
        ChkAndSetAttributes(i);
    }
}

/*
    parse at, data or initial argument
*/
static void AttributeExpression(byte lexItem, byte locflag)
{
    if (TestFlag(dclFlags, F_EXTERNAL))
        TokenErrorAt(ERR41);	/* CONFLICTING ATTRIBUTE */
    if (YylexMatch(T_LPAREN)) {
        WrByte(lexItem);
        if (isNewVar)
            WrInfoOffset(topInfo + 1);
        else
            WrInfoOffset(botInfo);
        SetFlag(dclFlags, locflag);
        WrNestedExpression();
    } else
        TokenErrorAt(ERR75);	/* MISSING ARGUMENT OF 'AT' , 'DATA' , OR 'INITIAL' */
}


static void ParseDclInitial()
{
    if (YylexMatch(T_INITIAL)) {
        ChkModuleLevel();
        AttributeExpression(L_INITIAL, F_INITIAL);
    } else if (YylexMatch(T_DATA))
        AttributeExpression(L_DATA, F_DATA);
    else if (TestFlag(dclFlags, F_STARDIM)) {
        TokenErrorAt(ERR74);	/* INVALID STAR DIMENSION, not WITH 'data' or 'initial' */
        ClrFlag(dclFlags, F_STARDIM);
        arrayDim = 1;
    }
}

static void ParseDclAt()
{
    if (YylexMatch(T_AT))
        AttributeExpression(L_AT, F_AT);
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
        type = BYTE_T;
    else if (YylexMatch(T_ADDRESS))
        type = ADDRESS_T;
    else {
        type = BYTE_T;
        if (YylexMatch(T_STRUCTURE)) {
            TokenErrorAt(ERR70);	/* INVALID MEMBER TYPE, 'STRUCTURE' ILLEGAL */
            if (YylexMatch(T_LPAREN)) {
                RecoverToSemiOrRP();
                Yylex();
            }
        } else if (YylexMatch(T_LABEL))
            TokenErrorAt(ERR71);	/* INVALID MEMBER TYPE, 'LABEL' ILLEGAL */
        else
            TokenErrorAt(ERR72);	/* MISSING TYPE FOR STRUCTURE MEMBER */
    }
    structMemType[structMCnt] = (byte)type;
}

static void ParseStructMDim()
{
    word dim;

    if (YylexMatch(T_LPAREN)) {
        if (YylexMatch(T_NUMBER))
            dim = tokenVal;
        else if (YylexMatch(T_STAR)) {
            dim = 1;
            TokenErrorAt(ERR69);	/* INVALID STAR DIMENSION WITH STRUCTURE MEMBER */
        } else {
            dim = 1;
            TokenErrorAt(ERR59);	/* ILLEGAL DIMENSION ATTRIBUTE */
        }
        if (dim == 0) {
            dim = 1;
            TokenErrorAt(ERR57);	/* ILLEGAL DIMENSION ATTRIBUTE */
        }
        if (YylexNotMatch(T_RPAREN)) {
            TokenErrorAt(ERR60);	/* MISSING ') ' AT END OF DIMENSION */
            RecoverToSemiOrRP();
            Yylex();
        }
        structMemDim[structMCnt] = dim;
    }
}


static void ParseStructMemElement()
{
    byte mcnt;

    if (YylexNotMatch(T_IDENTIFIER))
        TokenErrorAt(ERR66);	/* INVALID STRUCTURE MEMBER, NOT AN IDENTIFIER */
    else {
        for (mcnt = 1; mcnt <= structMCnt; mcnt++) {
            if (curSymbolP == structMembers[mcnt])
                TokenErrorAt(ERR67);	/* DUPLICATE STRUCTURE MEMBER NAME */
        }
        if (structMCnt == 32)
            TokenErrorAt(ERR68);	/* LIMIT EXCEEDED: NUMBER OF STRUCTURE MEMBERS */
        else
            structMCnt++;
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
        TokenErrorAt(ERR64);	/* MISSING STRUCTURE MEMBERS */
    else {
        while (1) {
            ParseStructMemElement();
            if (YylexNotMatch(T_COMMA))
                break;
        }
        if (YylexNotMatch(T_RPAREN)) {
            TokenErrorAt(ERR65);	/* MISSING ') ' AT END OF STRUCTURE MEMBER LIST */
            RecoverToSemiOrRP();
            Yylex();
        }
    }
}

static void ParseDclDataType()
{
    if (YylexMatch(T_BYTE))
        dclType = BYTE_T;
    else if (YylexMatch(T_ADDRESS))
        dclType = ADDRESS_T;
    else if (YylexMatch(T_STRUCTURE)) {
        dclType = STRUCT_T;
        ChkNotStarDim();        // illegal for structure to have * dim
        ParseStructMem();
    } else if (YylexMatch(T_LABEL)) {
        dclType = LABEL_T;
        ChkNotArray();
    } else {
        TokenErrorAt(ERR61);	/* MISSING TYPE */
        dclType = BYTE_T;       // assume byte
    }
}

static void ParseArraySize()
{
    if (YylexMatch(T_LPAREN)) {
        SetFlag(dclFlags, F_ARRAY);
        if (YylexMatch(T_NUMBER)) {
            if (tokenVal == 0) {
                TokenErrorAt(ERR57);	/* INVALID DIMENSION, ZERO ILLEGAL */
                arrayDim = 1;
            } else
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
            TokenErrorAt(ERR60);	/* MISSING ') ' AT END OF DIMENSION */
            RecoverToSemiOrRP();
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
        TokenErrorAt(ERR56);	/* INVALID MACRO TEXT, NOT A STRING CONSTANT */
        tokenStr[0] = 1;        // give default of a single space
        tokenStr[1] = ' ';
    }
    lastLit = CreateLit(tokenStr);
    dclType = LIT_T;
}


static void ParseLitOrType()
{
    arrayDim = 1;
    ClrFlags(dclFlags);
    dclType = LIT_T;
    if (YylexMatch(T_LITERALLY))
        ParseLiterally();
    else
        ParseDeclType();
}

static void ParseBaseSpecifier()
{
    offset_t base1Name, base2Name;

    basedInfo = 0;
    if (YylexNotMatch(T_IDENTIFIER))
        TokenErrorAt(ERR52);	/* INVALID BASE, MEMBER OF BASED STRUCTURE OR ARRAY OF STRUCTURES */
    else {
        base1Name = curSymbolP;
        if (YylexMatch(T_PERIOD)) {
            if (YylexMatch(T_IDENTIFIER))
                base2Name = curSymbolP;
            else {
                TokenErrorAt(ERR53);	/* INVALID STRUCTURE MEMBER IN BASE */
                return;
            }
        } else
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
                TokenErrorAt(ERR55);	/* UNDECLARED STRUTURE MEMBER IN BASE */
                return;
            }
            WrXrefUse();
            basedInfo = curInfoP;
        }
    }
} /* ParseBaseSpecifier() */

// parse <variable name specifier>
static void ParseVariableNameSpecifier()
{
    if (YylexNotMatch(T_IDENTIFIER))
        TokenErrorAt(ERR48);	/* ILLEGAL DECLARATION STATEMENT SYNTAX */
    else {
        if (declNameCnt == 32)
            TokenErrorAt(ERR49);	/* LIMIT EXCEEDED: NUMBER OF ITEMS IN FACTORED DECLARE */
        else
            declNameCnt++;

        declNames[declNameCnt] = curSymbolP;
        declBasedNames[declNameCnt] = 0;
        if (!isNewVar) {
            FindScopedInfo(*curScopeP);
            if (curInfoP == 0)      // new var definition
                isNewVar = true;
        }
        if (YylexMatch(T_BASED)) {  // check for BASED variant
            ParseBaseSpecifier();
            if (basedInfo != 0) {
                curInfoP = basedInfo;
                // base var has to be basic address not a based var or array var
                if (TestInfoFlag(F_BASED) || TestInfoFlag(F_ARRAY) || GetType() != ADDRESS_T) {
                    TokenErrorAt(ERR50);	/* INVALID ATTRIBUTES FOR BASE */
                    basedInfo = 0;
                } else if (TestInfoFlag(F_MEMBER)) {    // for structure var base, structure cannot be array or based
                    curInfoP = GetParentOffset();
                    if (TestInfoFlag(F_ARRAY) || TestInfoFlag(F_BASED)) {
                        TokenErrorAt(ERR52);	/* INVALID BASE, MEMBER OF BASED STRUCTURE OR ARRAY OF STRUCTURES */
                        basedInfo = 0;
                    }
                }
                declBasedNames[declNameCnt] = basedInfo;
            }
        }
    }
}


void ParseDeclareNames()
{
    declNameCnt = 0;
    isNewVar = false;
    if (YylexMatch(T_LPAREN)) { // factored names list?
        while (1) {     // collect the variable names
            ParseVariableNameSpecifier();
            if (YylexNotMatch(T_COMMA))
                break;
        }
        if (YylexNotMatch(T_RPAREN)) {
            TokenErrorAt(ERR47);	/* MISSING ') ' AT END OF FACTORED DECLARATION */
            RecoverToSemiOrRP();
            Yylex();
        }
    } else
        ParseVariableNameSpecifier();
}

// parse <declare element>
void ParseDeclareElementList()
{
    if (stmtLabelCnt != 0)  // declare cannot have label prefix
        SyntaxError(ERR46);	/* ILLEGAL USE OF LABEL */
    while (1) {
        ParseDeclareNames();
        if (declNameCnt != 0) {
            ParseLitOrType();
            SetFactoredAttributes();
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
        TokenErrorAt(ERR39);	/* INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL */
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
        TokenErrorAt(ERR39);	/* INVALID ATTRIBUTE OR INITIALIZATION, NOT AT MODULE LEVEL */
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
        SetDataType(BYTE_T);
    else if (YylexMatch(T_ADDRESS))
        SetDataType(ADDRESS_T);
}

word paramCnt;

static void AddParam()
{
    FindScopedInfo(*curScopeP);
    if (curInfoP != 0)
        TokenErrorAt(ERR38);	/* DUPLICATE PARAMETER NAME */
    CreateInfo(*curScopeP, BYTE_T); // for now assume byte
    WrXrefDef();
    SetInfoFlag(F_PARAMETER);
    paramCnt++;
}

static void ParseParams()
{
    paramCnt = 0;
    if (YylexMatch(T_LPAREN)) {
        while (1) {
            hasParams = true;
            if (YylexMatch(T_IDENTIFIER))
                AddParam();
            else {
                TokenErrorAt(ERR36);	/* MISSING PARAMETER */
                RecoverMissingParam();
            }
            if (YylexNotMatch(T_COMMA))
                break;
        }
        if (YylexNotMatch(T_RPAREN)) {
            TokenErrorAt(ERR37);	/* MISSING ') ' AT END OF PARAMETER LIST */
            RecoverToSemiOrRP();
            Yylex();
        }
        curInfoP = curProcInfoP;    // note number of params. info's follow procInfo
        SetParamCnt((byte)paramCnt);
    } else
        hasParams = false;
}


// parse <procedure statement> (label already parsed)

void ProcProcStmt()
{
    word tmp;

    tmp = curProcInfoP;
    curSymbolP = stmtLabels[1];
    FindScopedInfo(*curScopeP);   // look up procedure
    if (curInfoP != 0)              // error if already exists
        SyntaxError(ERR34);	/* DUPLICATE procedure DECLARATION */
    CreateInfo(*curScopeP, PROC_T);   // create a new procedure info block with current scope
    SetInfoFlag(F_LABEL);
    WrXrefDef();
    if (procCnt == 254)     // oops too many procedures
        FatalError(ERR35);	/* LIMIT EXCEEDED: NUMBER OF PROCEDURES */
    procInfo[++procCnt] = curInfoP;     // save procedure info
    curScope[PROCID] = (byte)procCnt;   // set scope procedure id
    curProcInfoP = curInfoP;
    curScope[DOBLKCNT] = 0;     // reset do block count for this procedure
    doBlkCnt = 0;
    PushBlock(*curScopeP);      // push current scope
    ParseParams();
    ParseRetType();
    ParseProcAttrib();
    /* write info to tx1 stream */
    curInfoP = curProcInfoP;            // accessors use curInfoP
    if (!TestInfoFlag(F_EXTERNAL)) {    // not external
        WrByte(L_PROCEDURE);
        WrInfoOffset(curInfoP);
        WrByte(L_SCOPE);
        WrWord(*curScopeP);
    } else {
        WrByte(L_EXTERNAL);
        WrInfoOffset(curInfoP);
    }
    SetProcId(curScope[PROCID]);
    if (tmp != 0) { // had valid entry curInfoP
        curInfoP = tmp;
        if (TestInfoFlag(F_REENTRANT))  // were we in re-entrant proc?
            SyntaxError(ERR88);	/* INVALID procedure NESTING, ILLEGAL IN reentrant procedure */
        curInfoP = curProcInfoP;    // set curInfoP to new proc's infoP
    }
}
