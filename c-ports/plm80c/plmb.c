#include "plm.h"

// plmc.c
static byte vtext[] = "program_version_number=";
byte verNo[] = "V4.0";

// plmf.c
static byte builtins[] = {
    "\x5" "CARRY\0\0\x2"
    "\x3" "DEC\x1\x1\x2"
    "\x6" "DOUBLE\x2\x1\x3"
    "\x4HIGH\x3\x1\x2"
    "\x5INPUT\x4\x1\x2"
    "\x4LAST\x5\x1\x3"
    "\x6LENGTH\x6\x1\x3"
    "\x3LOW\x7\x1\x2"
    "\x4MOVE\x8\x3\0"
    "\x6OUTPUT\x9\x1\0"
    "\x6PARITY\xA\0\x2"
    "\x3ROL\xB\x2\x2"
    "\x3ROR\xC\x2\x2"
    "\x3SCL\xD\x2\x2"
    "\x3SCR\xE\x2\x2"
    "\x3SHL\xF\x2\x2"
    "\x3SHR\x10\x2\x2"
    "\x4SIGN\x11\0\x2"
    "\x4SIZE\x12\x1\x2"
    "\x8STACKPTR\x13\0\x3"
    "\x4TIME\x14\x1\0"
    "\x4ZERO\x15\0\x2" };

static byte keywords[] = {
    "\x7" "ADDRESS\x28"
    "\x3" "AND\xA"
    "\x2" "AT\x29"
    "\x5" "BASED\x2A"
    "\x2" "BY\x35"
    "\x4" "BYTE\x2B"
    "\x4" "CALL\x1C"
    "\x4" "CASE\x36"
    "\x4" "DATA\x2C"
    "\x7" "DECLARE\x1D"
    "\x7" "DISABLE\x1E"
    "\x2" "DO\x1F"
    "\x4" "ELSE\x37"
    "\x6" "ENABLE\x20"
    "\x3" "END\x21"
    "\x3" "EOF\x38"
    "\x8" "EXTERNAL\x2D"
    "\x2GO\x22"
    "\x4GOTO\x23"
    "\x4HALT\x24"
    "\x2IF\x25"
    "\x7INITIAL\x2E"
    "\x9INTERRUPT\x2F"
    "\x5LABEL\x30"
    "\x9LITERALLY\x31"
    "\x5MINUS\x9"
    "\x3MOD\x7"
    "\x3NOT\xD"
    "\x2OR\xB"
    "\x4PLUS\x8"
    "\x9PROCEDURE\x26"
    "\x6PUBLIC\x32"
    "\x9REENTRANT\x33"
    "\x6RETURN\x27"
    "\x9STRUCTURE\x34"
    "\x4THEN\x39"
    "\x2TO\x3A"
    "\x5WHILE\x3B"
    "\x3XOR\xC" };



static void InstallBuiltins()
{
    pointer p;

    p = builtins;
    while (*p != 0) {
        Lookup(p);
        CreateInfo(0, BUILTIN_T);
        SetBuiltinId(p[*p + 1]);
        SetParamCnt(p[*p + 2]);
        SetDataType(p[*p + 3]);
        p += *p + 4;
    }	
    Lookup("\x6MEMORY");
    CreateInfo(0, BYTE_T);
    SetInfoFlag(F_LABEL);
    SetInfoFlag(F_MEMORY);
    SetInfoFlag(F_ARRAY);
} /* InstallBuiltins() */


static void InstallKeywords()
{
    pointer p;

    p = keywords;
    while (*p != 0) {
        Lookup(p);
        SymbolP(curSymbolP)->infoP = 0xFF00 | p[*p + 1];
        p += *p + 2;
    }
} /* InstallKeywords() */


static void Sub_4845()
{
    word i;

    if (w3C44 > w3C34 )
        botMem = w3C44;
    else
        botMem = w3C34;
    botMem = botMem + 256;
    botSymbol = (topSymbol = (hashChainsP = (topMem + 1 - 64 * sizeof(offset_t))) - 1) + 1;
    topInfo = (botInfo = botMem) + 1;
    for (i = 0; i <= 63; i++)
        WordP(hashChainsP)[i] = 0;
    SetPageNo(1);
    localLabelCnt = 0;
    w382A++;
    procChains[0] = procChains[1] = blockDepth = 0;
} /* Sub_4845() */

void Sub_4767()
{
	Sub_4845();
	InstallKeywords();
	InstallBuiltins();
} /* Sub_4767() */

void SetDate(pointer str, byte len)
{
    if (len > 9)
        len = 9;
    memset(DATE, ' ', 9);
    memmove(DATE, str, len);
} /* SetDate() */


void SetPageLen(word len)
{
    PAGELEN = (byte)len;
} /* SetPageLen() */


void SetPageNo(word v)
{
    pageNo = v - 1;
}


void SetMarginAndTabW(byte startCol, byte width)
{
    margin = startCol - 1;
    tWidth = width;
}


void SetTitle(pointer str, byte len)
{
    if (len > 60)
        len = 60;
    memmove(TITLE, str, len);
    TITLELEN = len;
} /* SetTitle() */


void SetPageWidth(word width)
{
    PWIDTH = (byte)width;
} /* SetPageWidth() */


