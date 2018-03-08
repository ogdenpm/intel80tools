#include "plm.h"

#define BADINFO 0xffff

// forward references
static byte InGetC();
static void GetLin();


// optTable is encoded with entries as follows
// name as pstr, control bytes as per structure below
#pragma pack(push, 1)
typedef struct {
    byte tokenId;       // if none zero then token id to put in lexical stream
    bool primary;       // true if primary control
    byte controlId;     // index into controls if not 0xff
    byte controlVal;    // index used in switch in PraseExtControl
    byte primaryId;     // index into primaryCtrlSeen
}  tknflags_t;
#pragma pack(pop)

static byte optTable[] =            // name      tokenId   primary controlId   controlVal primaryid
"\x5PRINT\0\0\xFF\x7\0"         // PRINT        0         0       0xFF        7         0
"\x7NOPRINT\0\0\xFF\x8\0"       // NOPRINT      0         0       0xFF        8         0
"\x4LIST\x3\x1\0\0\0"           // LIST         L_LIST    1       0           0         0
"\x6NOLIST\x4\x1\0\0\0"         // NOLIST       L_NOLIST  1       0           0         0
"\x4" "CODE\x5\x1\0\0\0"        // CODE         L_CODE    1       0           0         0
"\x6NOCODE\x6\x1\0\0\0"         // NOCODE       L_NOCODE  1       0           0         0
"\x4XREF\0\0\x1\x1\x1"          // XREF         0         0       1           1         1
"\x6NOXREF\0\0\x1\0\x1"         // NOXREF       0         0       1           0         1
"\x7SYMBOLS\0\0\x2\x1\x2"       // SYMBOLS      0         0       2           1         2
"\x9NOSYMBOLS\0\0\x2\0\x2"      // NOSYMBOLS    0         0       2           0         2
"\x5" "DEBUG\0\0\x3\x1\x3"      // DEBUG        0         0       3           1         3
"\x7NODEBUG\0\0\x3\0\x3"        // NODEBUG      0         0       3           0         3
"\x6PAGING\0\0\x4\x1\x4"        // PAGING       0         0       4           1         4
"\x8NOPAGING\0\0\x4\0\x4"       // NOPAGING     0         0       4           0         4
"\xAPAGELENGTH\0\0\xFF\0\x5"    // PAGELENGTH   0         0       0xFF        0         5
"\x9PAGEWIDTH\0\0\xFF\x1\x6"    // PAGEWIDTH    0         0       0xFF        1         6
"\x4" "DATE\0\0\xFF\x2\x7"      // DATE         0         0       0xFF        2         7
"\x5TITLE\0\0\xFF\x3\x8"        // TITLE        0         0       0xFF        3         8
"\x5" "EJECT\x7\x1\0\0\0"       // EJECT        L_EJECT   1       0           0         0
"\xALEFTMARGIN\0\x1\xFF\x4\0"   // LEFTMARGIN   0         1       0xFF        4         0
"\x6OBJECT\0\0\xFF\x5\x9"       // OBJECT       0         0       0xFF        5         9
"\x8NOOBJECT\0\0\xFF\x9\x9"     // NOOBJECT     0         0       0xFF        9         9
"\x8OPTIMIZE\0\0\x6\x1\xA"      // OPTIMIZE     0         0       6           1         10
"\xANOOPTIMIZE\0\0\x6\0\xA"     // NOOPTIMIZE   0         0       6           0         10
"\x7INCLUDE\0\x1\xFF\x6\0"      // INCLUDE      0         1       0xFF        6         0
"\x9WORKFILES\0\0\xFF\xA\xB"    // WORKFILES    0         0       0xFF        10        11
"\x9INTVECTOR\0\0\xFF\xB\xC"    // INTVECTOR    0         0       0xFF        11        12
"\xBNOINTVECTOR\0\0\xFF\xC\xC"  // NOINTVECTOR  0         0       0xFF        12        12
"\x5IXREF\0\0\xFF\xD\xD"        // IXREF        0         0       0xFF        13        13
"\x7NOIXREF\0\0\xFF\xE\xD"      // NOIXREF      0         0       0xFF        14        13
"\x4SAVE\0\x1\xFF\xF\0"         // SAVE         0         1       0xFF        15        0
"\x7RESTORE\0\x1\xFF\x10\0"     // RESTORE      0         1       0xFF        16        0
"\x3SET\0\x1\xFF\x11\0"         // SET          0         1       0xFF        17        0
"\x5RESET\0\x1\xFF\x12\0"       // RESET        0         1       0xFF        18        0
"\x2IF\0\x1\xFF\x13\0"          // IF           0         1       0xFF        19        0
"\x6" "ELSEIF\0\x1\xFF\x14\0"   // ELSEIF       0         1       0xFF        20        0
"\x4" "ELSE\0\x1\xFF\x14\0"     // ELSE         0         1       0xFF        20        0
"\x5" "ENDIF\0\x1\xFF\x15\0"    // ENDIF        0         1       0xFF        21        0
"\x4" "COND\0\x1\xFF\x16\0"     // COND         0         1       0xFF        22        0
"\x6NOCOND\0\x1\xFF\x17\0";     // NOCOND       0         1       0xFF        23        0

static byte ebadTail[] = "ILLEGAL COMMAND TAIL SYNTAX OR VALUE";
static byte ebadcontrol[] = "UNRECOGNIZED CONTROL IN COMMAND TAIL";
static byte errNotDisk[] = "INCLUDE FILE IS NOT A DISKETTE FILE";
static byte errWorkFiles[] = "MISPLACED CONTROL: WORKFILES ALREADY OPEN";

byte primaryCtrlSeen[14];   // C defaults to all false
static struct {
    byte code;
    byte list;
    byte cond;
    word leftMargin;
} saveStack[5];

static byte saveDepth;
static bool CODE = false;
static bool LIST = true;
static bool COND = true;
static pointer curChP;
static byte chrClass;
static byte tknLen;
static pointer optStrValP;
static word optNumValue;
static byte optFileName[16];
static bool ixiGiven;
static bool objGiven;
static bool lstGiven;
static bool inIFpart;
static word skippingIfDepth;

static tknflags_t *tknFlagsP;

// NxtCh()
// on input curChP points to current char and chrClass is set to the correspondingly
//
// if more chars on line returns next char - converting to uppercase
// and converting none printable chars to space
// end of line is at CR and if on command line also & the continuation marker
// also sets chrClass


static void NxtCh()
{
    if (chrClass == CC_NEWLINE)
        return;
    curChP++;
    if (*curChP == CR || (offFirstChM1 != 0 && *curChP == '&')) // end of line or & on the command line
        chrClass = CC_NEWLINE;
    else if ((chrClass = cClass[*curChP]) == CC_NONPRINT)
        *curChP = ' ';
    if (*curChP >= 'a')
        *curChP &= 0x5f;
} /* NxtCh() */


static void BadCmdTail(byte err)
{
    if (offFirstChM1 != 0)      // processing command line
        Fatal(ebadTail, Length(ebadTail));
    else
        SyntaxError(err);
}


static void UnknownCtrl()
{

    if (offFirstChM1 != 0)      // processing command line
        Fatal(ebadcontrol, Length(ebadcontrol));
    else
        SyntaxError(ERR9);  /* INVALID CONTROL */
}


static void SkipWhite()     // also skips non print characters
{
    while (*curChP == ' ')  // PMO: should possibly also skip tab
        NxtCh();
}


static void SkipToRPARorEOL()
{
    while (*curChP != CR && *curChP != ')')
        NxtCh();
    if (*curChP == ')')
        NxtCh();
}

static void AcceptRP()
{
    SkipWhite();
    if (*curChP != ')') {
        BadCmdTail(ERR12);  /* INVALID CONTROL PARAMETER */
        SkipToRPARorEOL();
    } else
        NxtCh();
}

static void AcceptOptStrVal()
{
    byte nesting = 0;

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
                nesting++;
            else if (*curChP == ')') {
                if (nesting-- == 0)
                    break;
            }
            NxtCh();
        }
        tknLen = (byte)(curChP - optStrValP);
        AcceptRP();
    }

} /* AcceptOptStrVal() */

static void AcceptFileName()
{
    SkipWhite();
    if (*curChP != '(')
        tknLen = 0;
    else {
        NxtCh();
        SkipWhite();
        optStrValP = curChP;
        while (*curChP != ' ' && *curChP != ')' && chrClass != CC_NEWLINE)  // doesn't allow tab
            NxtCh();
        tknLen = (byte)(curChP - optStrValP);
        if (tknLen > 14) {
            BadCmdTail(ERR17);  /* INVALID PATH-NAME */
            tknLen = 0;
        } else {
            memset(optFileName, ' ', 16);
            memcpy(optFileName, optStrValP, tknLen);
            optStrValP = optFileName;
            tknLen = tknLen + 1;    // include trailing space
        }
        AcceptRP();
    }
}


static word Asc2Num(pointer firstChP, pointer lastChP, byte radix)
{
    word num;
    unsigned trial;  // use unsigned (>= 32 bit) to smiplify test for overflow
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
//        if ((trial = num * radix + digit) < digit || ((word)(num * radix + digit) - digit) / radix != num)
          if ((trial = num * radix + digit) > 0xffff)
            return 0xffff;
        num = (word)trial;
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
    pointer firstCh, nextCh;
    byte radix;

    NxtCh();
    SkipWhite();
    firstCh = curChP;
    while (chrClass <= CC_ALPHA)
        NxtCh();
    nextCh = curChP - 1;
    SkipWhite();
    radix = ChkRadix(&nextCh);
    return Asc2Num(firstCh, nextCh, radix);
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



static void ParseId(byte maxLen)
{
    byte pstr[33];

    optStrValP = &pstr[1];
    tknLen = 0;
    SkipWhite();
    if (chrClass == CC_HEXCHAR || chrClass == CC_ALPHA)     // A-Z
        while (chrClass <= CC_ALPHA || chrClass == CC_DOLLAR) { // 0-9 A-Z $
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


static void GetVar()
{
    pointer tmp;
    tmp = curChP - 1;
    ParseId(31);
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

// return logical operator
// none (0), OR (1), AND (2), XOR (3), bad (4)
static byte GetLogical()
{
    ParseId(3);
    if (tknLen == 0 && chrClass == CC_NEWLINE)
        return 0;
    if (tknLen == 2) {
        if (Strncmp(optStrValP, "OR", 2) == 0)
            return 1;
    } else if (tknLen == 3)
        if (Strncmp(optStrValP, "AND", 3) == 0)
            return 2;
        else if (Strncmp(optStrValP, "XOR", 3) == 0)
            return 3;

    BadCmdTail(ERR185); /* MISSING OPERATOR IN CONDITIONAL COMPILATION Expression */
    SkipToRPARorEOL();
    return 4;
}

// returns test operation
// = (1), < (2), <= (3), > (4), >= (5), <> (6)
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

static byte ChkNot()    // checks for potentially multiple NOT prefixes
{
    byte notStatus;
    pointer tmp;

    notStatus = 0;

    while ((1)) {
        tmp = curChP - 1;
        ParseId(3);
        if (tknLen != 3 || Strncmp(optStrValP, "NOT", 3) != 0) {
            curChP = tmp;
            return notStatus;
        }
        notStatus = ~notStatus;
    }
}

static word GetIfVal()
{
    word val;

    chrClass = 0;
    NxtCh();
    SkipWhite();
    if (chrClass < CC_HEXCHAR) {    // starts with a digit
        curChP = curChP - 1;
        val = ParseNum();
        if (val > 255)
            BadCmdTail(ERR186); /* INVALID CONDITIONAL COMPILATION CONSTANT, TOO LARGE */
        return Low(val);
    } else {
        GetVar();                   // variable
        if (curInfoP == BADINFO) {
            BadCmdTail(ERR180); /* MISSING OF INVALID CONDITIONAL COMPILATION PARAMETER */
            SkipToRPARorEOL();
            return 256;             // error value
        } else if (curInfoP == 0)
            return 0;               // default to false if name not defined
        else
            return GetCondFlag();   // else return current value
    }
}

static bool ParseIfCond()
{
    byte andFactor, orFactor, xorFactor;
    word val1, val2;
    byte relOp, not1, not2;

    andFactor = 0xff;
    orFactor = 0;
    xorFactor = 0;

    while ((1)) {
        not1 = ChkNot();
        val1 = GetIfVal();
        if (val1 > 255)         // illegal name
            return true;

        relOp = GetTest();
        if (relOp > 0) {        // we have a rel op
            not2 = ChkNot();    // check for not prefixes
            val2 = GetIfVal();
            if (val2 > 255)
                return true;    // illegal name
            val2 = not2 ^ val2; 
            switch (relOp) {
            case 1: val1 = val1 == val2 ? 0xff : 0; break;
            case 2: val1 = val1 < val2 ? 0xff : 0; break;
            case 3: val1 = val1 <= val2 ? 0xff : 0; break;
            case 4: val1 = val1 > val2 ? 0xff : 0; break;
            case 5: val1 = val1 >= val2 ? 0xff : 0; break;
            case 6: val1 = val1 != val2 ? 0xff : 0; break;
            }
        }
        val1 = (not1 ^ val1) & andFactor;
        andFactor = 0xff;
        switch (GetLogical()) {
        case 0: // apply final OR/XOR and convert to bool using PLM rule
            return ((val1 | orFactor) ^ xorFactor) & 1;
        case 1:
            orFactor = (val1 | orFactor) ^ xorFactor;
            xorFactor = 0;
            break;
        case 2: andFactor = (byte)val1; break;
        case 3:
            xorFactor = (val1 | orFactor) ^ xorFactor;
            orFactor = 0;
            break;
        case 4: return true;        // bad so assume true
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
    AcceptOptStrVal();
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
    if (!LocalSetTitle()) {
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
            if (offFirstChM1 != 0)
                Fatal(errNotDisk, Length(errNotDisk));
            else
                FatalError(ERR98);  /* INCLUDE FILE IS not A DISKETTE FILE */
    }
    if (srcFileIdx >= 50)
        SyntaxError(ERR13); /* LIMIT EXCEEDED: INCLUDE NESTING */
    else {
        TellF(&srcFil, (loc_t *)&srcFileTable[srcFileIdx + 8]);
        Backup((loc_t *)&srcFileTable[srcFileIdx + 8], offLastCh - offCurCh);
        srcFileIdx = srcFileIdx + 10;
        memmove(&srcFileTable[srcFileIdx], optStrValP, tknLen);
        CloseF(&srcFil);
        InitF(&srcFil, "SOURCE", optStrValP);
        OpenF(&srcFil, 1);
        offCurCh = offLastCh;
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

static bool isOK;
static pointer lFname;

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
        if (!AcceptDrive(wrkFiles1, ','))
            return;
        if (!AcceptDrive(wrkFiles2, ')'))
            return;
        if (tx1File.aftn != 0)
            Fatal(errWorkFiles, Length(errWorkFiles));
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
            GetVar();
            if (curInfoP == BADINFO) {
                BadCmdTail(ERR180); /* MISSING or INVALID CONDITIONAL COMPILATION PARAMETER */
                SkipToRPARorEOL();
                return;
            }
            if (curInfoP == 0)
                CreateInfo(1, CONDVAR_T);
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
    ifDepth++;
    if (!ParseIfCond()) {   // if not true so skip
        skippingCOND = true;
        inIFpart = true;
        skippingIfDepth = ifDepth;    // record depth for unwind
        if (!COND)          // if COND false and currently listing - surpress listing
            if (LIST)
                WrByte(L_NOLIST);
    }
}

static void OptElseElseIf()
{
    if (ifDepth == 0)      // no corresponding if!!
        BadCmdTail(ERR182); /* MISPLACED else or ELSEIF OPTION */
    else {
        skippingCOND = true;
        inIFpart = false;
        skippingIfDepth = ifDepth;
        if (!COND)
            if (LIST)
                WrByte(L_NOLIST);
    }
    while (*curChP != '\r') {
        NxtCh();
    }
}


static void OptEndIf()
{
    if (ifDepth == 0)       // no if
        BadCmdTail(ERR183); /* MISPLACED ENDIF OPTION */
    else
        ifDepth--;
}


static void ParseControlExtra()
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



static void ParseControl()
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

                if (tknFlagsP->controlId != 0xFF)       // simple control
                    if (tknFlagsP->tokenId == 0)        // simple update to control value only
                        controls[tknFlagsP->controlId] = tknFlagsP->controlVal;
                    else {                              // write control to lexical stream
                        WrByte(tknFlagsP->tokenId);
                        if (tknFlagsP->tokenId == L_CODE)       // update values for CODE and LIST
                            CODE = true;
                        else if (tknFlagsP->tokenId == L_NOCODE)
                            CODE = false;
                        else if (tknFlagsP->tokenId == L_LIST)
                            LIST = true;
                        else if (tknFlagsP->tokenId == L_NOLIST)
                            LIST = false;
                    } else
                        ParseControlExtra();      // not a simple control do further processing
        }
    }
}

void ParseControlLine(pointer pch)
{
    curChP = pch;
    chrClass = 0;
    ixiGiven = objGiven = lstGiven = false;
    NxtCh();        // skip $ or to first char of current cmd line string

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
        ParseControl();
    }
}


static void ChkEndSkipping(pointer pch)
{
    curChP = pch;
    if (*curChP == '$') {
        chrClass = 0;
        NxtCh();
        SkipWhite();
        GetToken();
        if (tknLen == 2 && Strncmp(optStrValP, "IF", 2) == 0)   // nested IF
            ifDepth++;
        else if (tknLen == 5 && Strncmp(optStrValP, "ENDIF", 5) == 0) {
            if (--ifDepth < skippingIfDepth)                  // end of skipping
                skippingCOND = false;
        } else if (skippingIfDepth == ifDepth && inIFpart)    // else/elseif at same level as initial IF
            if (tknLen == 4 && Strncmp(optStrValP, "ELSE", 4) == 0)
                skippingCOND = false;                         // else so now not skipping
            else if (tknLen == 6 && Strncmp(optStrValP, "ELSEIF", 6) == 0)
                skippingCOND = !ParseIfCond();                // still skipping if condition false
        if (!skippingCOND)          // no longer skipping so record any change of listing status
            if (!COND)
                if (LIST)
                    WrByte(L_LIST);
    }
}


// converted from plm0c.asm
bool trunc;

void GNxtCh()
{
    inChrP++;
    while (*inChrP == '\n')
        GetLin();
    nextCh = *inChrP;
}


// get next char into *inChrP
// char is 0x81 on EOF, cr is discarded

static byte InGetC()
{
    byte c;
    do {
        if (offCurCh == offLastCh) {
            offCurCh = 0;
            ReadF(&srcFil, inbuf, 512, &offLastCh);
            if (offLastCh == 0) {
                return *inChrP = ISISEOF;	// EOF
            } else
                offLastCh--;	// base from 0
        } else
            offCurCh++;
    } while ((c = inbuf[offCurCh] & 0x7f) == '\r'); // make sure parity stripped & skip cr
    return *inChrP = c;
}


static void RSrcLn()
{
    byte i = 127;		// max chars;

    for (;;) {
        if (*inChrP == '\n' || *inChrP == ISISEOF)
            return;     // got a line (or eof)
        if (--i == 0)   // not reached line limit
            break;
        inChrP++;
        InGetC();
    }
    trunc = true;       // oops too long
    while (InGetC() != '\n' && *inChrP != ISISEOF)	// junk rest of long line
        ;
}


static void GetSrcLine()
{
    WriteLineInfo();        // make sure linfo is written
    inChrP = lineBuf;
    InGetC();
    if (*inChrP == ISISEOF)	/* eof */
        return;
    trunc = false;          // reset truncation flag
    RSrcLn();               // get line
    *inChrP++ = '\r';       // add cr lf
    *inChrP = '\n';
    inChrP = lineBuf;       // point to start
    linfo.lineCnt++;        // account for new line
    linfo.stmtCnt = linfo.blkCnt = 0;   // no stmt or blkcnt for this line yet
    lineInfoToWrite = true; // note linfo needs to be written
}




static void GetCodeLine()
{
    pointer startOfLine;

    while (1) {
        GetSrcLine();
        if (*inChrP != ISISEOF) {
            startOfLine = inChrP + LEFTMARGIN - 1;  // skip over leftmargin (tabs are 1 char !!)
            while (inChrP < startOfLine) {
                if (*inChrP == '\r')
                    return;                         // effectively a blank line
                inChrP = inChrP + 1;
            }
            if (skippingCOND)               // is skipping check to see if finished
                ChkEndSkipping(inChrP);
            else if (*inChrP == '$') {      // control line
                WrByte(L_STMTCNT);          // note zero stmts on this line
                WrWord(0);
                if (trunc) {
                    SyntaxError(ERR86);	/* LIMIT EXCEEDED: SOURCE LINE LENGTH */
                    trunc = false;
                }
                ParseControlLine(inChrP);   // process controls
            } else {
                isNonCtrlLine = true;       // first none control line (even a comment) stops primary controls 
                return;                     // we have a line
            }
        } else if (srcFileIdx == 0) {       // EOF at end of main file
            if (ifDepth != 0)               // oops we are nested (error code seems to be incorrect)
                SyntaxError(ERR188);	    /* MISPLACED RESTORE OPTION */
            inChrP = "/*'/**/EOF   ";       // string to make sure any comments, strings are closed and EOF
            return;
        } else {
            CloseF(&srcFil);                // close nested file & reload caller file at saved position
            srcFileIdx = srcFileIdx - 10;
            InitF(&srcFil, "SOURCE", (pointer)&srcFileTable[srcFileIdx]);
            OpenF(&srcFil, 1);
            SeekF(&srcFil, (loc_t *)&srcFileTable[srcFileIdx + 8]);
            offCurCh = offLastCh;           // force buffer reload
        }
    }
}

static void GetLin()
{
    word tmp;

    if (macroDepth != 0) {      // last line was a nested lit expansion
        tmp = curInfoP;
        curInfoP = curMacroInfoP;
        SetType(LIT_T);         // reset info entry to LIT_T from MACRO_T
        curMacroInfoP = ptr2Off(macroPtrs[macroDepth + 1]); // get the caller lit's infoP
        inChrP = macroPtrs[macroDepth]; // get the curent loc in the expansion
        macroDepth -= 2;    // prep for next line
        curInfoP = tmp;
    } else
        GetCodeLine();
}
