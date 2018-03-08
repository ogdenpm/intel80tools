#include "plm.h"

byte lstBuf[1280];
byte maxSymLen;
word dictSize;
word w66D2;
word w66D4;
offset_t xrefItemP;
byte b66D8 = 0;
offset_t dictionaryP;
offset_t dictTopP;


static byte copyright[] = "(C) 1976, 1977, 1982 INTEL CORP";
static byte dots[] = ". . . . . . . . . . . . . . . . . . . . ";
static byte dashes[] = "------------------------------------";
byte b3F0B = 0xFF;	/* ixi module header */



void Sub_4121(pointer str)
{
    NewLineLst();
    Xputstr2cLst("*** WARNING -- ", 0);
    Xputstr2cLst(str, 0);
    NewLineLst();
}

void LoadDictionary()
{
    dictionaryP = dictTopP = botMem;
    dictSize = maxSymLen = 0;
    curInfoP = botInfo + 2;

    while (1) {
        if (GetType() < MACRO_T && GetSymbol() != 0) {
            dictTopP = dictTopP + 2;
            if (dictTopP >= botSymbol) {
                Sub_4121("INSUFFICIENT MEMORY FOR FULL DICTIONARY LISTING");
                return;
            }
            dictSize = dictSize + 1;
            *WordP(dictTopP) = curInfoP;
            SetScope(0);		/* used for xref Chain() */
            curSymbolP = GetSymbol();
            if (PstrP(curSymbolP)->len > maxSymLen)
                maxSymLen = PstrP(curSymbolP)->len;
        }
        AdvNxtInfo();
        if (curInfoP == 0)
            return;
    }
}

byte CmpSym(offset_t dictItem1, offset_t dictItem2)
{
    pointer sym1, sym2;
    pointer str1, str2;
    byte i, j;

    curInfoP = dictItem1;
    sym1 = ByteP(GetSymbol());
    str1 = sym1 + 1;
    curInfoP = dictItem2;
    sym2 = ByteP(GetSymbol());
    str2 = sym2 + 1;
    if (*sym1 < *sym2) {
        i = *sym1;
        j = 0;
    } else if (*sym1 > *sym2) {
        i = *sym2;
        j = 2;
    } else {
        i = *sym1;
        j = 1;
    }

    while (i != 0) {
        if (*str1 < *str2)
            return 0;
        if (*str1 > *str2)
            return 2;
        str1++;
        str2++;
        i--;
    }
    return j;
}


void SortDictionary()
{
    word p, q, r, s, t, u, v;
    offset_t w, x;
    word y, z;

    if (dictSize == 1)
        return;
    r = dictSize / 2;
    s = r + 2;
    t = 1;
    p = 2;
    q = r;
    y = dictSize;
    u = q;
    w = WordP(dictionaryP)[u];

    while (1) {
        v = u * 2;
        if (v > y) {
            WordP(dictionaryP)[u] = w;
            if (t == 2) {
                x = WordP(dictionaryP)[1];
                WordP(dictionaryP)[1] = WordP(dictionaryP)[q];
                WordP(dictionaryP)[q] = x;
                if (p >= dictSize)
                    break;
                else
                {
                    p = p + 1;
                    q = y = z - p;
                    u = 1;
                }
            } else if (p >= r) { 
                t = 2;
                z = dictSize + 2;
                p = 2;
                q = y = dictSize;
                u = 1;
            } else {
                p = p + 1;
                q = s - p;
                y = dictSize;
                u = q;
            }
            w = WordP(dictionaryP)[u];
        } else {
            if (v != y)
                if (CmpSym(WordP(dictionaryP)[v + 1], WordP(dictionaryP)[v]) > 1)
                    v = v + 1;
            if (CmpSym(WordP(dictionaryP)[v], w) <= 1) {
                WordP(dictionaryP)[u] = w;
                if (t == 2) {
                    x = WordP(dictionaryP)[1];
                    WordP(dictionaryP)[1] = WordP(dictionaryP)[q];
                    WordP(dictionaryP)[q] = x;
                    if (p >= dictSize)
                        break;
                    p = p + 1;
                    q = y = z - p;
                    u = 1;
                } else if (p >= r) {
                    t = 2;
                    z = dictSize + 2;
                    p = 2;
                    q = y = dictSize;
                    u = 1;
                } else {
                    p = p + 1;
                    q = s - p;
                    y = dictSize;
                    u = q;
                }
                w = WordP(dictionaryP)[u];
            } else {
                WordP(dictionaryP)[u] = WordP(dictionaryP)[v];
                u = v;
            }
        }
    }
}


static void LoadXref()
{
    byte xrfbuf[128];
    byte b6783;
    word pa[2];

    OpenF(&xrfFile, 1);
    CreatF(&xrfFile, xrfbuf, 128, 1);
    while (1) {
        Fread(&xrfFile, &b6783, 1);
        if (b6783 == 0)
            break;
        Fread(&xrfFile, (pointer)pa, 4);
        if (b6783 == 0x42 || XREF) {
            curInfoP = pa[0] + botInfo;
            xrefItemP = w66D4 + 1;
            w66D4 = w66D4 + 4;
            if (w66D4 > botSymbol) {
                Sub_4121("INSUFFICIENT MEMORY FOR FULL XREF PROCESSING");
                break;
            }
            WordP(xrefItemP)[0] = GetScope();
            SetScope(xrefItemP);
            if (b6783 == 0x42)
                WordP(xrefItemP)[1] = -pa[1];	/* make defn line -ve */
            else
                WordP(xrefItemP)[1] = pa[1];
        }
    }
    CloseF(&xrfFile);
}


static void XrefDef2Head()
{
    word p;
    offset_t q, r;

    for (p = 1; p <= dictSize; p++) {
        curInfoP = WordP(dictionaryP)[p];
        xrefItemP = GetScope();
        if (xrefItemP != 0) { 
            q = 0;
            SetScope(0);
            while (xrefItemP != 0) { 
                r = WordP(xrefItemP)[0];
                if ((WordP(xrefItemP)[1] & 0x8000) != 0)
                    q = xrefItemP;	/* definition */
                else {
                    WordP(xrefItemP)[0] = GetScope();
                    SetScope(xrefItemP);
                }
                xrefItemP = r;
            }

            if (q != 0)	 {	/* insert definition at head */
                xrefItemP = q;
                WordP(xrefItemP)[0] = GetScope();
                SetScope(xrefItemP);
            }
        }
    }
}



void PrepXref()
{

    w66D2 = dictTopP + 2;
    if (w66D2 >= botSymbol) {
        Sub_4121("INSUFFICIENT MEMORY FOR ANY XREF PROCESSING");
        return;
    }
    w66D4 = w66D2 - 1;
    LoadXref();
    XrefDef2Head();
}

// file scope due to nested procedure lift;
static byte defnCol, addrCol, sizeCol, nameCol, attribCol, refContCol;


static void Sub_480A()
{
    word p;

    if (! XREF) { 
        NewLineLst();
        return;
    }

    xrefItemP = GetScope();
    if (xrefItemP == 0) {
        NewLineLst();
        return;
    }
    XwrnstrLst(": ", 2);
    p = 0;

    while (xrefItemP != 0) {
        if (p != WordP(xrefItemP)[1]) {
            if (PWIDTH < col + 5) {
                NewLineLst();
                TabLst(-refContCol);
            }
            TabLst(1);
            XnumLst(WordP(xrefItemP)[1], 0, 10);
            p = WordP(xrefItemP)[1];
        }
        xrefItemP = WordP(xrefItemP)[0];
    }
    if (col != 0)
        NewLineLst();
}




static void Sub_48A7()
{
    curSymbolP = GetSymbol();
    TabLst(-nameCol);
    XwrnstrLst(PstrP(curSymbolP)->str, PstrP(curSymbolP)->len);
    XwrnstrLst(&dots[PstrP(curSymbolP)->len], attribCol - col - 2);
    TabLst(1);
}

static void Sub_48E2(word arg1w, word arg2w)
{
    TabLst(-addrCol);
    XnumLst(arg1w, 0xFC, 0xF0);
    if (arg2w != 0)
    {
        TabLst(-sizeCol);
        XnumLst(arg2w, 5, 10);
    }
}


static void Sub_4921()
{
    xrefItemP = GetScope();
    if (GetType() == BUILTIN_T)
        return;
    if (xrefItemP != 0 && (WordP(xrefItemP)[1] & 0x8000) != 0)
    {
        TabLst(-defnCol);
        XnumLst(-WordP(xrefItemP)[1], 5, 10); /* defn stored as -ve */
        SetScope(WordP(xrefItemP)[0]);
    }
    else if (! TestInfoFlag(F_LABEL))
    {
        TabLst(-defnCol);
        XwrnstrLst(dashes, 5);
    }
}

static void Sub_499C()
{
    Xputstr2cLst(" EXTERNAL(", 0);
    XnumLst(GetExternId(), 0, 10);
    XwrnstrLst(")", 1);
}


static void Sub_49BB()
{
    offset_t p, q, r;

    p = curInfoP;
    Xputstr2cLst(" BASED(", 0);
    curInfoP = GetBaseOffset();
    if (TestInfoFlag(F_MEMBER)) {
        r = GetSymbol();
        curInfoP = GetParentOffset();
        q = GetSymbol();
    } else {
        q = GetSymbol();
        r = 0;
    }

    curSymbolP = q;
    XwrnstrLst(PstrP(curSymbolP)->str, PstrP(curSymbolP)->len);
    if (r != 0) {
        XwrnstrLst(".", 1);
        curSymbolP = r;
        XwrnstrLst(PstrP(curSymbolP)->str, PstrP(curSymbolP)->len);
    }
    XwrnstrLst(")", 1);
    curInfoP = p;
}


static void Sub_4A42()
{
    offset_t p;

    p = curInfoP;
    curInfoP = GetParentOffset();
    Xputstr2cLst(" MEMBER(", 0);
    curSymbolP = GetSymbol();
    XwrnstrLst(PstrP(curSymbolP)->str, PstrP(curSymbolP)->len);
    XwrnstrLst(")", 1);
    curInfoP = p;
}

static void Sub_4A78(pointer str)
{
    Sub_4921();
    Sub_48A7();
    Xputstr2cLst(str, 0);
    Sub_480A();
}


static void Sub_4A92()
{
    pointer p;

    Sub_4921();
    Sub_48E2(GetLinkVal(), GetDimension2());
    Sub_48A7();
    Xputstr2cLst("PROCEDURE", 0);
    if (GetDataType() != 0) {
        if (GetDataType() == 2)
            p = " BYTE";
        else
            p = " ADDRESS";
        Xputstr2cLst(p, 0);
    }
    if (TestInfoFlag(F_PUBLIC))
        Xputstr2cLst(" PUBLIC", 0);

    if (TestInfoFlag(F_EXTERNAL))
        Sub_499C();

    if (TestInfoFlag(F_REENTRANT))
        Xputstr2cLst(" REENTRANT",0);

    if (TestInfoFlag(F_INTERRUPT)) {
        Xputstr2cLst(" INTERRUPT(", 0);
        XnumLst(GetIntrNo(), 0, 10);
        XwrnstrLst(")", 1);
    }
    if (! TestInfoFlag(F_EXTERNAL)) { 
        Xputstr2cLst(" STACK=", 0);
        XnumLst(GetBaseVal(), 0xfc, 0xf0);
    }
    Sub_480A();
}

static void Sub_4B4A(pointer str)
{
    word p;
    byte i;

    Sub_4921();
    i = GetType();
    if (i == BYTE_T)
        p = 1;
    else if (i == ADDRESS_T)
        p = 2;
    else if (i == LABEL_T)
        p = 0;
    else
        p = GetParentOffset();

    if (TestInfoFlag(F_ARRAY))
        p = p * GetDimension();
    Sub_48E2(GetLinkVal(), p);
    Sub_48A7();
    Xputstr2cLst(str, 0);
    if (TestInfoFlag(F_BASED))
        Sub_49BB();
    if (TestInfoFlag(F_ARRAY)) {
        Xputstr2cLst(" ARRAY(", 0);
        XnumLst(GetDimension(), 0, 10);
        XwrnstrLst(")", 1);
    }
    if (TestInfoFlag(F_PUBLIC))
        Xputstr2cLst(" PUBLIC", 0);

    if (TestInfoFlag(F_EXTERNAL))
        Sub_499C();

    if (TestInfoFlag(F_AT))
        Xputstr2cLst(" AT", 0);
    if (TestInfoFlag(F_DATA))
        Xputstr2cLst(" DATA", 0);
    if (TestInfoFlag(F_INITIAL))
        Xputstr2cLst(" INITIAL", 0);

    if (TestInfoFlag(F_MEMBER))
        Sub_4A42();

    if (TestInfoFlag(F_PARAMETER))
        Xputstr2cLst(" PARAMETER", 0);

    if (TestInfoFlag(F_AUTOMATIC))
        Xputstr2cLst(" AUTOMATIC", 0);

    if (TestInfoFlag(F_ABSOLUTE))
        Xputstr2cLst(" ABSOLUTE",0);

    Sub_480A();
}

static void Sub_4C84()
{
    curSymbolP = GetSymbol();
    if (b66D8 != PstrP(curSymbolP)->str[0]) {
        NewLineLst();
        b66D8 = PstrP(curSymbolP)->str[0];
    }
    if (GetType() < MACRO_T)
        switch(GetType()) {
        case 0: Sub_4A78("LITERALLY"); break;
        case 1: Sub_4B4A("LABEL"); break;
        case 2: Sub_4B4A("BYTE"); break;
        case 3: Sub_4B4A("ADDRESS"); break;
        case 4: Sub_4B4A("STRUCTURE"); break;
        case 5: Sub_4A92(); break;
        case 6: Sub_4A78("BUILTIN"); break;
        }
} /* Sub_4C84() */

void PrintRefs()
{
    word p;

/* PrintRefs() */
    defnCol = 3;
    addrCol = defnCol + 6;
    sizeCol = addrCol + 6;
    nameCol = sizeCol + 7;
    attribCol = nameCol + maxSymLen + 2;
    refContCol = attribCol + 1;
    SetMarkerInfo(attribCol, '-', 3);
    NewPageNextChLst();
    if (XREF) {
        Xputstr2cLst("CROSS-REFERENCE LISTING", 0);
        NewLineLst();
        XwrnstrLst(dashes, 23);
        NewLineLst();
    } else {
        Xputstr2cLst("SYMBOL LISTING", 0);
        NewLineLst();
        XwrnstrLst(dashes, 14);
        NewLineLst();
    }
    SetSkipLst(2);
    TabLst(-defnCol);
    Xputstr2cLst(" DEFN", 0);
    TabLst(-addrCol);
    Xputstr2cLst(" ADDR", 0);
    TabLst(-sizeCol);
    Xputstr2cLst(" SIZE", 0);
    TabLst(-nameCol);
    Xputstr2cLst("NAME, ATTRIBUTES, AND REFERENCES", 0);
    NewLineLst();
    TabLst(-defnCol);
    XwrnstrLst(dashes, 5);
    TabLst(-addrCol);
    XwrnstrLst(dashes, 5);
    TabLst(-sizeCol);
    XwrnstrLst(dashes, 5);
    TabLst(-nameCol);
    XwrnstrLst(dashes, 32);
    NewLineLst();
    NewLineLst();

    for (p = 1; p <= dictSize; p++) {
        curInfoP = WordP(dictionaryP)[p];
        if (GetType() == BUILTIN_T)
        {
            if (GetScope() != 0)
                Sub_4C84();
        }
        else
            Sub_4C84();
    }
} /* PrintRefs() */


// lifted
static void Sub_4EAA(pointer buf, word cnt)
{
    Fwrite(&ixiFile, buf, cnt);
}




void CreateIxrefFile()
{
    offset_t p, q, r;
    byte i, j, ixibuf[128], k;
    byte zero = 0, one = 1;

    OpenF(&ixiFile, 2);
    CreatF(&ixiFile, ixibuf, 128, 2);
    curInfoP = botInfo + procInfo[1];
    curSymbolP = GetSymbol();
    if (curSymbolP != 0) {		/* Write() the module info */
        Sub_4EAA(&b3F0B, 1);
        k = 22 + PstrP(curSymbolP)->len;
        Sub_4EAA(&k, 1);
        Sub_4EAA(&PstrP(curSymbolP)->len, 1);		/* module name len */
        Sub_4EAA(PstrP(curSymbolP)->str, PstrP(curSymbolP)->len);	/* module name */
    }
    if (Low(srcFileTable[0]) == ':')
        j = 2;
    else
        j = 0;
    Sub_4EAA((pointer)&srcFileTable[j], 10);		/* fileName minus any :fx: */
    Sub_4EAA("---------", 9);

    for (p = 1; p <= dictSize; p++) {
        curInfoP = WordP(dictionaryP)[p];
        i = GetType();
        if (LABEL_T <= i && i <= PROC_T && 
           (TestInfoFlag(F_PUBLIC) || (TestInfoFlag(F_EXTERNAL) && !TestInfoFlag(F_AT)))) {
            if (TestInfoFlag(F_PUBLIC))
                Sub_4EAA(&zero, 1);
            else
                Sub_4EAA(&one, 1);

            curSymbolP = GetSymbol();
            k = 6 + PstrP(curSymbolP)->len;
            Sub_4EAA(&k, 1);
            Sub_4EAA(&PstrP(curSymbolP)->len, 1);
            Sub_4EAA(PstrP(curSymbolP)->str, PstrP(curSymbolP)->len);
            Sub_4EAA(&i, 1);
            if (GetType() == PROC_T) {
                r = GetDataType();
                Sub_4EAA((pointer)&r, 2);
            } else {
                if (TestInfoFlag(F_ARRAY))
                    q = GetDimension();
                else
                    q = 0;
                Sub_4EAA((pointer)&q, 2);
            }
        }
    }
    Fflush(&ixiFile);
    CloseF(&ixiFile);
}



void Sub_4EC5()
{
    LoadDictionary();
    SortDictionary();
    PrepXref();
    if ((XREF || SYMBOLS) && PRINT)
        PrintRefs();
    if (IXREF)
        CreateIxrefFile();
}

word Start5()
{
    MEMORY = 0x6936;
	botMem = MEMORY + 0x100;
	topSymbol = topSymbol + 4;
	if (PRINT) {
		lBufP = lstBuf;
		lBufSz = 1279;
	}
	Sub_4EC5();
	if (PRINT)
		LstModuleInfo();
	DeletF(&xrfFile);
	EndCompile();
	Exit();
}

