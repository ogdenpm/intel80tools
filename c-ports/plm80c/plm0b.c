#include "plm.h"

#define BADINFO 0xffff

static byte optTable[] = 
    "\x5PRINT\0\0\xFF\x7\0"
    "\x7NOPRINT\0\0\xFF\x8\0"
    "\x4LIST\x3\x1\0\0\0"
    "\x6NOLIST\x4\x1\0\0\0"
    "\x4" "CODE\x5\x1\0\0\0"
    "\x6NOCODE\x6\x1\0\0\0"
    "\x4XREF\0\0\x1\x1\x1"
    "\x6NOXREF\0\0\x1\0\x1"
    "\x7SYMBOLS\0\0\x2\x1\x2"
    "\x9NOSYMBOLS\0\0\x2\0\x2"
    "\x5" "DEBUG\0\0\x3\x1\x3"
    "\x7NODEBUG\0\0\x3\0\x3"
    "\x6PAGING\0\0\x4\x1\x4"
    "\x8NOPAGING\0\0\x4\0\x4"
    "\xAPAGELENGTH\0\0\xFF\0\x5"
    "\x9PAGEWIDTH\0\0\xFF\x1\x6"
    "\x4" "DATE\0\0\xFF\x2\x7"
    "\x5TITLE\0\0\xFF\x3\x8"
    "\x5" "EJECT\x7\x1\0\0\0"
    "\xALEFTMARGIN\0\x1\xFF\x4\0"
    "\x6OBJECT\0\0\xFF\x5\x9"
    "\x8NOOBJECT\0\0\xFF\x9\x9"
    "\x8OPTIMIZE\0\0\x6\x1\xA"
    "\xANOOPTIMIZE\0\0\x6\0\xA"
    "\x7INCLUDE\0\x1\xFF\x6\0"
    "\x9WORKFILES\0\0\xFF\xA\xB"
    "\x9INTVECTOR\0\0\xFF\xB\xC"
    "\xBNOINTVECTOR\0\0\xFF\xC\xC"
    "\x5IXREF\0\0\xFF\xD\xD"
    "\x7NOIXREF\0\0\xFF\xE\xD"
    "\x4SAVE\0\x1\xFF\xF\0"
    "\x7RESTORE\0\x1\xFF\x10\0"
    "\x3SET\0\x1\xFF\x11\0"
    "\x5RESET\0\x1\xFF\x12\0"
    "\x2IF\0\x1\xFF\x13\0"
    "\x6" "ELSEIF\0\x1\xFF\x14\0"
    "\x4" "ELSE\0\x1\xFF\x14\0"
    "\x5" "ENDIF\0\x1\xFF\x15\0"
    "\x4" "COND\0\x1\xFF\x16\0"
    "\x6NOCOND\0\x1\xFF\x17\0";

byte ebadTail[] = "ILLEGAL COMMAND TAIL SYNTAX OR VALUE";
byte ebadcontrol[] = "UNRECOGNIZED CONTROL IN COMMAND TAIL";
byte errNotDisk[] = "INCLUDE FILE IS NOT A DISKETTE FILE";
byte errWorkFiles[] = "MISPLACED CONTROL: WORKFILES ALREADY OPEN";

byte primaryCtrlSeen[14];
static struct {
    byte code;
    byte list;
    byte cond;
    word leftMargin;
} saveStack[5];

byte saveDepth;
bool CODE = false;
bool LIST = true;
bool COND = true;
pointer curChP;
byte chrClass;
byte tknLen;
pointer optStrValP;
word optNumValue;
byte optFileName[16];
byte ixiGiven;
byte objGiven;
byte lstGiven;
byte inIFpart;
word skippingCONDDepth;

#pragma pack(push, 1)
typedef struct {
    byte tokenId;
    byte primary;
    byte controlId;
    byte controlVal;
    byte primaryId;
}  tknflags_t;
#pragma pack(pop)

tknflags_t *tknFlagsP;
    

void NxtCh()
{
    if (chrClass == CC_NEWLINE)
        return;
    curChP = curChP + 1;
    if (*curChP == CR || (offNxtCmdChM1 != 0 && *curChP == '&'))
        chrClass = CC_NEWLINE;
    else if ((chrClass = cClass[*curChP]) == CC_NONPRINT)
        *curChP = ' ';
    if (*curChP >= 'a')
        *curChP = *curChP & 0x5f;
} /* NxtCh() */
    

void BadCmdTail(byte err)
{
    if (offNxtCmdChM1 != 0)
        Fatal(ebadTail, sizeof(ebadTail) - 1);
    else
        SyntaxError(err);
}


void UnknownCtrl()
{

    if (offNxtCmdChM1 != 0)
        Fatal(ebadcontrol, sizeof(ebadcontrol) - 1);
    else
        SyntaxError(ERR9);  /* INVALID CONTROL */
}


void SkipWhite()
{
    while (*curChP == ' ')
        NxtCh();
}


void SkipToRPARorEOL()
{
    while (*curChP != CR && *curChP != ')') 
        NxtCh();
    if (*curChP == ')')
        NxtCh();
}   

void AcceptRP()
{
    SkipWhite();
    if (*curChP != ')')
    {
        BadCmdTail(ERR12);  /* INVALID CONTROL PARAMETER */
        SkipToRPARorEOL();
    }
    else
        NxtCh();
}

static void AccessOptStrVal()
{
    byte nesting;

    nesting = 0;
    SkipWhite();
    if (*curChP != '(')
        tknLen = 0;
    else {
        NxtCh();
        optStrValP = curChP;
        for (;;) {
            if (chrClass == CC_NEWLINE || *curChP == '\'')
                break;
            if (*curChP == '(')
                nesting = nesting + 1;
            else if (*curChP == ')') {
                if (nesting == 0)
                    break;
                nesting = nesting - 1;
            }
            NxtCh();
        }
        tknLen = (byte)(curChP - optStrValP);
        AcceptRP();
    }           

} /* AccessOptStrVal() */

static void AcceptFileName()
{
    SkipWhite();
    if (*curChP != '(')
        tknLen = 0;
    else {
        NxtCh();
        SkipWhite();
        optStrValP = curChP;
        while (*curChP != ' ' && *curChP != ')' && chrClass != CC_NEWLINE)
            NxtCh();
        tknLen = (byte)(curChP - optStrValP);
        if (tknLen > 14) {
            BadCmdTail(ERR17);  /* INVALID PATH-NAME */
            tknLen = 0;
        } else {
            memset(optFileName, ' ', 16);
            memmove(optFileName, optStrValP, tknLen);
            optStrValP = optFileName;
            tknLen = tknLen + 1;
        }
        AcceptRP();
    }
}


static word Asc2Num(pointer firstChP, pointer lastChP, byte radix)
{
    word num, trial;
    byte digit;
   
    if (lastChP < firstChP || radix == 0)
        return 0xffff;
    
    num = 0;
    while (firstChP <= lastChP) {
        if (cClass[*firstChP] <= CC_DECDIGIT)
            digit = *firstChP - '0';
        else if (cClass[*firstChP] < CC_ALPHA)
            digit = *firstChP - '7';
        else
            return 0xffff;
        if (digit >= radix)
            return 0xffff;
        if ((trial = num * radix + digit) < digit || (num * radix + digit - digit) / radix != num)
            return 0xffff;
        num = trial;
        firstChP = firstChP + 1;
    }
    return num;
}

static byte ChkRadix(pointer *pLastCh)
{
    pointer p;

    p = *pLastCh;
    if (cClass[*p] <= CC_DECDIGIT)
        return 10;
    *pLastCh = *pLastCh - 1;
    if (*p == 'B')
        return 2;
    if (*p == 'Q' || *p == 'O')
        return 8;
    if (*p == 'H')
        return 16;
    if (*p == 'D')
        return 10;
    else
        return 0;
}

static word ParseNum()
{
    pointer firstCh, lastCh;
    byte radix;

    NxtCh();
    SkipWhite();
    firstCh = curChP;
    while (chrClass <= CC_ALPHA) 
        NxtCh();
    lastCh = curChP - 1;
    SkipWhite();
    radix = ChkRadix(&lastCh);
    return Asc2Num(firstCh, lastCh, radix);
}

static void GetOptNumVal()
{
    SkipWhite();
    optNumValue = 0;
    if (*curChP != '(')
        BadCmdTail(ERR11);
    else {
        optNumValue = ParseNum();
        AcceptRP();
    }
}


static void GetToken()
{
    optStrValP = curChP;
    while (*curChP != ' ' && *curChP != '(' && chrClass != CC_NEWLINE)
        NxtCh();
    tknLen = (byte)(curChP - optStrValP);
}



static void ParseWord(byte maxLen)
{
    byte pstr[33];
    
    optStrValP = &pstr[1];
    tknLen = 0;
    SkipWhite();
    if (chrClass == CC_HEXCHAR || chrClass == CC_ALPHA)
        while (chrClass <= CC_ALPHA || chrClass == CC_DOLLAR) {
            if (chrClass != CC_DOLLAR && tknLen <= maxLen) {
                pstr[tknLen + 1] = *curChP;
                tknLen = tknLen + 1;
            }
            NxtCh();
        }
    if (tknLen > maxLen)
        pstr[0] = maxLen;
    else
        pstr[0] = tknLen;
}


static void GetVal()
{
    pointer tmp;
    tmp = curChP - 1;
    ParseWord(31);
    if (tknLen == 0) {
        curInfoP = BADINFO;
        curChP = tmp;
        chrClass = 0;
        NxtCh();
        return;
    }
    if (tknLen > 31) {
        tknLen = tknLen - 1;
        BadCmdTail(ERR184); /* CONDITIONAL COMPILATION PARAMETER NAME TOO LONG */
    }
    Lookup(optStrValP - 1);
    if (High(SymbolP(curSymbolP)->infoP) == 0xFF) {/* special */
        curInfoP = BADINFO;
        curChP = tmp;
        chrClass = 0;
        NxtCh();
        return;
    }

    FindScopedInfo(1);
}


static byte GetLogical()
{
    ParseWord(3);
    if (tknLen == 0 && chrClass == CC_NEWLINE)
        return 0;
    if (tknLen == 2) {
        if (Strncmp(optStrValP, "OR", 2) == 0)
            return 1;
    }
    else if (tknLen == 3)
        if (Strncmp(optStrValP, "AND", 3) == 0)
            return 2;
        else if (Strncmp(optStrValP, "XOR", 3) == 0)
            return 3;
    
    BadCmdTail(ERR185); /* MISSING OPERATOR IN CONDITIONAL COMPILATION EXPRESSION */
    SkipToRPARorEOL();
    return 4;
}


static byte GetTest()
{
    byte test;

    test = 0;
    SkipWhite();
    if (*curChP == '<') {
        NxtCh();
        if (*curChP == '>') {
            NxtCh();
            return 6;
        }
        test = 2;
    } else if (*curChP == '>') {
        NxtCh();
        test = 4;
    }
    if (*curChP == '=') {
        NxtCh();
        test = test + 1;
    }
    return test;
}

static bool ChkNot()
{
    bool notStatus;
    pointer tmp;

    notStatus = false;

    while ((1)) {
        tmp = curChP - 1;
        ParseWord(3);
        if (tknLen != 3 || Strncmp(optStrValP, "NOT", 3) != 0) {
            curChP = tmp;
            return notStatus;
        }
        notStatus = !notStatus;
    }
}

static word GetIfVal()
{
    word val;

    chrClass = 0;
    NxtCh();
    SkipWhite();
    if (chrClass < CC_HEXCHAR) {
        curChP = curChP - 1;
        val = ParseNum();
        if (val > 255)
            BadCmdTail(ERR186); /* INVALID CONDITIONAL COMPILATION CONSTANT, TOO LARGE */
        return Low(val);
    } else {
        GetVal();
        if (curInfoP == BADINFO) {
            BadCmdTail(ERR180); /* MISSING or INVALID CONDITIONAL COMPILATION PARAMETER */
            SkipToRPARorEOL();
            return 256;
        } else if (curInfoP == 0)
            return 0;
        else
            return GetCondFlag();
    }
}

static bool ParseIfCond()
{
    byte andFactor, orFactor, xorFactor;
    word val1, val2;
    byte relOp, not1, not2;

    andFactor = true;
    orFactor = false;
    xorFactor = false;
    
    while ((1)) {
        not1 = ChkNot();
        val1 = GetIfVal();
        if (val1 > 255)
            return true;
        
        relOp = GetTest();
        if (relOp > 0)
        {
            not2 = ChkNot();
            val2 = GetIfVal();
            if (val2 > 255)
                return true;
            val2 = not2 ^ val2;
            switch(relOp - 1) {
            case 0: val1 = val1 == val2; break;
            case 1: val1 = val1 < val2; break;
            case 2: val1 = val1 <= val2; break;
            case 3: val1 = val1 > val2; break;
            case 4: val1 = val1 >= val2; break;
            case 5: val1 = val1 != val2; break;
            }
        }
        val1 = (not1 ^ val1) & andFactor;
        andFactor = true;
        switch(GetLogical()) {
        case 0: return (val1 | orFactor) ^ xorFactor;
        case 1:
            orFactor = (val1 | orFactor) ^ xorFactor; 
            xorFactor = false;
            break;
        case 2: andFactor = (byte)val1; break;
        case 3:
            xorFactor = (val1 | orFactor) ^ xorFactor;
            orFactor = false;
            break;
        case 4: return true;
        }
    }
}


static void OptPageLen()
{
    GetOptNumVal();
    if (optNumValue < 4 || optNumValue == 0xFFFF)
        BadCmdTail(ERR91);  /* ILLEGAL PAGELENGTH CONTROL VALUE */
    else
        SetPageLen(optNumValue - 3);
}

static void OptPageWidth()
{
    GetOptNumVal();
    if (optNumValue < 60 || optNumValue == 0xFFFF)
        BadCmdTail(ERR92);  /* ILLEGAL PAGEWIDTH CONTROL VALUE */
    else if (optNumValue > 132)
        BadCmdTail(ERR92);  /* ILLEGAL PAGEWIDTH CONTROL VALUE */
    else
        SetPageWidth(optNumValue);
}

static void OptDate()
{
    AccessOptStrVal();
    SetDate(optStrValP, tknLen);
}

// promoted to file level
static bool LocalSetTitle()
{
    byte len;
    NxtCh();
    if (*curChP != '\'')
        return false;
    len = 0;
    while (1) {
        NxtCh();
        if (*curChP == '\r')
            break;
        if (*curChP == '\'') {
            NxtCh();
            if (*curChP != '\'')
                break;
        }
        if (len <= 59) {
            TITLE[len] = *curChP;
            len = len + 1;
        }
    }
    if (len != 0)
        TITLELEN = len;
    else {
        TITLELEN = 1;
        TITLE[0] = ' ';
    }
    if (*curChP != ')')
        return false;
    else {
        NxtCh();
        return true;
    }
}

static void OptTitle()
{

    SkipWhite();
    if (*curChP != '(') {
        BadCmdTail(ERR11);  /* MISSING CONTROL PARAMETER */
        return;
    }
    if (! LocalSetTitle()) {
        BadCmdTail(ERR12);  /* INVALID CONTROL PARAMETER */
        SkipToRPARorEOL();
    }
}


static void OptLeftMargin()
{
    GetOptNumVal();
    if (optNumValue == 0) {
        BadCmdTail(ERR12);  /* INVALID CONTROL PARAMETER */
        optNumValue = 1;
    } else if (optNumValue == 0xFFFF) {
        BadCmdTail(ERR12);  /* INVALID CONTROL PARAMETER */
        return;
    }
    LEFTMARGIN = optNumValue;
}

static void OptIXRef()
{
    AcceptFileName();
    if (tknLen != 0)
        InitF(&ixiFile, "IXREF ", optStrValP);
    else
        InitF(&ixiFile, "IXREF ", ixiFileName);
    IXREFSet = true;
    IXREF = true;
    OpenF(&ixiFile, 2);
    ixiGiven = true;
}

static void OptObject()
{
    AcceptFileName();
    if (tknLen != 0)
        InitF(&objFile, "OBJECT", optStrValP);
    else
        InitF(&objFile, "OBJECT", objFileName);
    objBlk = objByte = 0;
    OBJECT = true;
    OBJECTSet = true;
    OpenF(&objFile, 2);
    objGiven = true;
}

static void OptInclude()
{
    AcceptFileName();
    if (tknLen == 0) {
        BadCmdTail(ERR15);  /* MISSING INCLUDE CONTROL PARAMETER */
        return;
    }
    if (optFileName[0] == ':') {
        if (optFileName[1] != 'F')
            if (offNxtCmdChM1 != 0)
                Fatal(errNotDisk, sizeof(errNotDisk) - 1);
            else
                FatalError(ERR98);  /* INCLUDE FILE IS not A DISKETTE FILE */
    }
    if (srcFileIdx >= 50)
        SyntaxError(ERR13); /* LIMIT EXCEEDED: INCLUDE NESTING */
    else {
        TellF(&srcFil, (loc_t *)&srcFileTable[srcFileIdx + 8]);
        Backup((loc_t *)&srcFileTable[srcFileIdx + 8], olstch - ocurch);
        srcFileIdx = srcFileIdx + 10;
        memmove(&srcFileTable[srcFileIdx], optStrValP, tknLen);
        CloseF(&srcFil);
        InitF(&srcFil, "SOURCE", optStrValP);
        OpenF(&srcFil, 1);
        ocurch = olstch;
        WrByte(L_INCLUDE);
        WrBuf(optStrValP + 12, 6);
        WrByte(L_INCLUDE);
        WrBuf(optStrValP + 6, 6);
        WrByte(L_INCLUDE);
        WrBuf(optStrValP, 6);
    }
    SkipWhite();
    if (*curChP != '\r')
        BadCmdTail(ERR14);  /* INVALID CONTROL FORMAT, INCLUDE not last() CONTROL */
}


static void OptPrint()
{
    AcceptFileName();
    if (lfOpen)
        BadCmdTail(ERR16);  /* ILLEGAL PRINT CONTROL */
    else {
        if (tknLen != 0)
            InitF(&lstFil, "LIST ", optStrValP);
        else
            InitF(&lstFil, "PRINT ", lstFileName);
        PRINTSet = true;
    }
    PRINT = true;
    OpenF(&lstFil, 2);
    lstGiven = true;
}


static void OptIntVector()
{
    byte vecNum;
    word vecLoc;

    SkipWhite();
    if (*curChP != '(')
        BadCmdTail(ERR11);  /* MISSING CONTROL PARAMETER */
    else {
        vecNum = (byte)ParseNum();
        if (vecNum != 4 && vecNum != 8) {
            BadCmdTail(ERR176); /* INVALID INTVECTOR INTERVAL VALUE */
            SkipToRPARorEOL();
        } else if (*curChP != ',') {
            BadCmdTail(ERR177); /* INVALID INTVECTOR LOCATION VALUE */
            SkipToRPARorEOL();
        } else {
            vecLoc = ParseNum();
            if (vecLoc > 0xFFE0 || vecLoc % (vecNum * 8) != 0 || *curChP != ')') {
                BadCmdTail(ERR177); /* INVALID INTVECTOR LOCATION VALUE */
                SkipToRPARorEOL();
            } else {
                NxtCh();
                intVecNum = vecNum;
                intVecLoc = vecLoc;
            }
        }
    }
}

// lifted to filescope

bool isOK;
pointer lFname;

static void AcceptRangeChrs(byte lch, byte hch)
{
    if (isOK)
        if (*curChP < lch || hch < *curChP) {
            BadCmdTail(ERR12);  /* INVALID CONTROL PARAMETER */
            SkipToRPARorEOL();
            isOK = false;
        } else {
            *lFname = *curChP;
            lFname = lFname + 1;
            NxtCh();
        }
}

static bool AcceptDrive(pointer fname, byte follow)
{
    lFname = fname;

    isOK = true;
    SkipWhite();
    AcceptRangeChrs(':', ':');
    AcceptRangeChrs('F', 'F');
    AcceptRangeChrs('0', '9');
    AcceptRangeChrs(':', ':');
    SkipWhite();
    AcceptRangeChrs(follow, follow);
    return isOK;
}


static void OptWorkFiles()
{
    byte wrkFiles1[5], wrkFiles2[5];

    SkipWhite();
    if (*curChP != '(')
        BadCmdTail(ERR11);  /* MISSING CONTROL PARAMETER */
    else {
        NxtCh();
        if (! AcceptDrive(wrkFiles1, ','))
            return;
        if (! AcceptDrive(wrkFiles2, ')'))
            return;
        if (tx1File.aftn != 0)
            Fatal(errWorkFiles, sizeof(errWorkFiles) - 1);
        memmove(atFile.fNam, wrkFiles1, 4);
        memmove(nmsFile.fNam, wrkFiles1, 4);
        memmove(tx1File.fNam, wrkFiles1, 4);
        memmove(xrfFile.fNam, wrkFiles1, 4);
        memmove(tx2File.fNam, wrkFiles2, 4);
    }
}

static void OptSave()
{
    if (saveDepth >= 5)
        BadCmdTail(ERR187); /* LIMIT EXCEEDED: NUMBER OF SAVE LEVELS > 5 */
    else {
        saveStack[saveDepth].code = CODE;
        saveStack[saveDepth].list = LIST;
        saveStack[saveDepth].cond = COND;
        saveStack[saveDepth].leftMargin = LEFTMARGIN;
        saveDepth = saveDepth + 1;
    }
}

static void OptRestore()
{
    if (saveDepth == 0)
        BadCmdTail(ERR188); /* MISPLACED RESTORE OPTION */
    else {
        saveDepth = saveDepth - 1;
        CODE = saveStack[saveDepth].code;
        LIST = saveStack[saveDepth].list;
        COND = saveStack[saveDepth].cond;
        LEFTMARGIN = saveStack[saveDepth].leftMargin;
        if (CODE)
            WrByte(L_CODE);
        else
            WrByte(L_NOCODE);
        if (LIST)
            WrByte(L_LIST);
        else
            WrByte(L_NOLIST);
    }
}


static void OptSetReset(bool isSet)
{
    word val;

    SkipWhite();    
    if (*curChP != '(')
        BadCmdTail(ERR11);  /* MISSING CONTROL PARAMETER */
    else 
        while ((1)) {
            NxtCh();
            GetVal();
            if (curInfoP == BADINFO) {
                BadCmdTail(ERR180); /* MISSING or INVALID CONDITIONAL COMPILATION PARAMETER */
                SkipToRPARorEOL();
                return;
            }
            if (curInfoP == 0)
                CreateInfo(1, TEMP_T);
            SkipWhite();
            if (*curChP == '=' && isSet) {
                val = ParseNum();
                if (val > 255) {
                    BadCmdTail(ERR181); /* MISSING or INVALID CONDITIONAL COMPILATION CONSTANT */
                    SkipToRPARorEOL();
                    return;
                }
                SetCondFlag((byte)val);
            } else
                SetCondFlag(isSet);
            if (*curChP != ',') {
                AcceptRP();
                return;
            }
        }
}

static void OptIf()
{
    ifDepth = ifDepth + 1;
    if (! ParseIfCond())
    {
        skippingCOND = true;
        inIFpart = true;
        skippingCONDDepth = ifDepth;
        if (! COND)
            if (LIST)
                WrByte(L_NOLIST);
    }
}

static void OptElseElseIf()
{
    if (ifDepth == 0)
        BadCmdTail(ERR182); /* MISPLACED else or ELSEIF OPTION */
    else {
        skippingCOND = true;
        inIFpart = false;
        skippingCONDDepth = ifDepth;
        if (! COND)
            if (LIST)
                WrByte(L_NOLIST);
    }
    while (*curChP != '\r') {
        NxtCh();
    }
}


static void OptEndIf()
{
    if (ifDepth == 0)
        BadCmdTail(ERR183); /* MISPLACED ENDIF OPTION */
    else
        ifDepth = ifDepth - 1;
}


static void Sub_550C()
{

    switch (tknFlagsP->controlVal) {
    case 0: OptPageLen(); break;
    case 1: OptPageWidth(); break;
    case 2: OptDate(); break;
    case 3: OptTitle(); break;
    case 4: OptLeftMargin(); break;
    case 5: OptObject(); break;
    case 6: OptInclude(); break;
    case 7: OptPrint(); break;
    case 8: PRINT = false; PRINTSet = false; break;
    case 9: OBJECT = false; OBJECTSet = false; break;
    case 10: OptWorkFiles(); break;
    case 11: OptIntVector(); break;
    case 12: intVecNum = 0; break;
    case 13: OptIXRef(); break;
    case 14: IXREF = false; IXREFSet = false; break;
    case 15: OptSave(); break;
    case 16: OptRestore(); break;
    case 17: OptSetReset(true); break;
    case 18: OptSetReset(false); break;
    case 19: OptIf(); break;
    case 20: OptElseElseIf(); break;
    case 21: OptEndIf(); break;
    case 22: COND = true; break;
    case 23: COND = false; break;
    }
}

static void FindOption()
{
    word off, clen;
    off = 0;
    clen = optTable[0]; 

    while (clen != 0) {
        if (clen == tknLen)
            if (Strncmp(optStrValP, &optTable[off] + 1, (byte)clen) == 0) {
                tknFlagsP = (tknflags_t *)(&optTable[off] + clen + 1);
                return;
            }
        clen = optTable[off = off + clen + 6];
    }
    tknFlagsP = 0;
}


static void SkipControlParam()
{
    SkipWhite();
    if (*curChP == '(') {
        NxtCh();
        SkipToRPARorEOL();
    }
}



static void DoControls()
{
    GetToken();
    if (tknLen == 0) {
        BadCmdTail(ERR8);   /* INVALID CONTROL FORMAT */
        SkipControlParam();
    } else {
        FindOption();
        if (tknFlagsP == 0) {
            UnknownCtrl();
            SkipControlParam();
        } else {
            if (tknFlagsP->primary == 0)
                if (isNonCtrlLine) {
                    SyntaxError(ERR10); /* ILLEGAL USE OF PRIMARY CONTROL AFTER NON-CONTROL LINE */
                    SkipControlParam();
                    return;
                } else if (primaryCtrlSeen[tknFlagsP->primaryId]) {
                    BadCmdTail(ERR95);  /* ILLEGAL RESPECIFICATION OF PRIMARY CONTROL IGNORED */
                    SkipControlParam();
                    return;
                } else
                    primaryCtrlSeen[tknFlagsP->primaryId] = true;

            if (tknFlagsP->controlId != 0xFF)
                if (tknFlagsP->tokenId == 0)
                    CONTROLS[tknFlagsP->controlId] = tknFlagsP->controlVal;
                else {
                    WrByte(tknFlagsP->tokenId);
                    if (tknFlagsP->tokenId == 5)
                        CODE = true;
                    else if (tknFlagsP->tokenId == 6)
                        CODE = false;
                    else if (tknFlagsP->tokenId == 3)
                        LIST = true;
                    else if (tknFlagsP->tokenId == 4)
                        LIST = false;
                }
            else
                Sub_550C();
        }
    }
}

void DoControl(pointer pch)
{
    curChP = pch;
    chrClass = 0;
    ixiGiven = objGiven = lstGiven = false;
    NxtCh();

    while (1) {
        SkipWhite();
        if (chrClass == CC_NEWLINE) {
            if (ixiGiven)
                CloseF(&ixiFile);
            if (objGiven)
                CloseF(&objFile);
            if (lstGiven)
                CloseF(&lstFil);
            return;
        }
        DoControls();
    }
}


void DoCondCompile(pointer pch)
{
    curChP = pch;
    if (*curChP == '$') {
        chrClass = 0;
        NxtCh();
        SkipWhite();
        GetToken();
        if (tknLen == 2 && Strncmp(optStrValP, "IF", 2) == 0)
            ifDepth = ifDepth + 1;
        else if (tknLen == 5 && Strncmp(optStrValP, "ENDIF", 5) == 0)
        {
            if ((ifDepth = ifDepth - 1) < skippingCONDDepth)
                skippingCOND = false;
        }
        else if (skippingCONDDepth == ifDepth && inIFpart)
            if (tknLen == 4 && Strncmp(optStrValP, "ELSE", 4) == 0)
                skippingCOND = false;
            else if (tknLen == 6 && Strncmp(optStrValP, "ELSEIF", 6) == 0)
                skippingCOND = ! ParseIfCond();
        if (! skippingCOND)
            if (! COND)
                if (LIST)
                    WrByte(L_LIST);
    }
}

