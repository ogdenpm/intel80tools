#include "plm.h"

static byte signonMsg[] = "\r\nISIS-II PL/M-80 COMPILER ";
static byte noMemMsg[] = "NOT ENOUGH MEMORY FOR A COMPILATION";
static byte aIxi[] = ".IXI";
static byte aObj[] = ".OBJ";
static byte aLst[] = ".LST";
static byte plmtx1[] = ":F1:PLMTX1.TMP ";
static byte plmtx2[] = ":F1:PLMTX2.TMP ";
static byte plmat[] = ":F1:PLMAT.TMP ";
static byte plmnms[] = ":F1:PLMNMS.TMP ";
static byte plmxrf[] = ":F1:PLMXRF.TMP ";
static byte aInvocationComm[] = "INVOCATION COMMAND DOES NOT END WITH <CR><LF>";
static byte aIncorrectDevice[] = "INCORRECT DEVICE SPEC";
static byte aSourceFileNotDisk[] = "SOURCE FILE NOT A DISKETTE FILE";
static byte aSourceFileName[] = "SOURCE FILE NAME INCORRECT";
static byte aSourceFileBadExt[] = "SOURCE FILE EXTENSION INCORRECT";
static byte aIllegalCommand[] = "ILLEGAL COMMAND TAIL SYNTAX";

pointer cmdTextP;


static void SkipSpace()
{
    while (*cmdTextP == ' ' || *cmdTextP == '&') {
        if (*cmdTextP == ' ')
            cmdTextP = cmdTextP + 1;
        else if (CmdP(cmdLineP)->link) {
            cmdLineP = CmdP(cmdLineP)->link;
            cmdTextP = &CmdP(cmdLineP)->pstr[1];
        }
    }
} /* SkipSpace() */


static bool TestToken(pointer str, byte len)
{
    pointer p;

    p = cmdTextP;
    while (len != 0) {
        if ((*cmdTextP & 0x5F) != *str) {
            cmdTextP = p;
            return false;
        }
        cmdTextP = cmdTextP + 1;
        str = str + 1;
        len = len - 1;
    }
    return true;
} /* TestToken() */


static void SkipAlphaNum()
{
    while ('A' <= *cmdTextP && *cmdTextP <= 'Z' || 'a' <= *cmdTextP && *cmdTextP <= 'z'
                || '0' <= *cmdTextP && *cmdTextP <= '9')
        cmdTextP = cmdTextP + 1;
} /* SkipAlphaNum() */



static void GetCmdLine()
{
    word actual, status;
    byte i;
    bool inQuote;

    Rescan(1, &status);     // no need for LocalRescan
    if (status != 0)
        FatlIO(&conFile, status);
    startCmdLineP = 0;
    cmdLineP = topMem;

    for (;;) {
        ReadF(&conFile, ioBuffer, 128, &actual);
        if (ioBuffer[actual - 1] != '\n' || ioBuffer[actual - 2] != '\r')
            Fatal(aInvocationComm, sizeof(aInvocationComm) - 1);
        topMem = cmdLineP - (sizeof(cmd_t) + actual);
        if (startCmdLineP == 0)
            startCmdLineP = topMem;
        else
            CmdP(cmdLineP)->link = topMem;
        cmdLineP = topMem;
        CmdP(cmdLineP)->pstr[0] = (byte)actual;
        memmove(&CmdP(cmdLineP)->pstr[1], ioBuffer, actual);
        inQuote = false;
        for (i = 0; i < actual; i++) {
            if (ioBuffer[i] == QUOTE)
                inQuote = ! inQuote;
            else if (ioBuffer[i] == '&')
                if (! inQuote)
                    goto extend;
        }
        CmdP(cmdLineP)->link = 0;
        cmdLineP = startCmdLineP;
        topMem = topMem - 1;
        return;
    extend:
        PrintStr("**", 2);
    }
} /* GetCmdLine() */

static void ParseInvokeName()
{
    pointer startP;
    word len;
    word p;

    SkipSpace();
    if (TestToken("DEBUG", 5))
        debugFlag = true;
    else
        debugFlag = false;
    SkipSpace();
    startP = cmdTextP;
    if (*cmdTextP == ':')
        cmdTextP = cmdTextP + 4;    // skip drive
    SkipAlphaNum();
    if ((len = (word)(cmdTextP - startP)) > 10)
        len = 10;
    /* plm had the overlay & invokename (ov0) split out
     * I have combined them to avoid spaces in address ranges
     * note invokeName is now overlay[0]
     * overlayN is now overlay[N]
     * In the end this code is not actually needed as overlays are
     * handled differently
     */
    for (p = 0; p <= 6; p++) {
        memmove(overlay[p], startP, len);               // copy fileName
        memmove(overlay[p] + len, overlay[p] + 10, 5);  // move up the ext
    }
} /* ParseInvokeName() */


static void ParseSrcFile()
{
    pointer fullName;
    pointer fileName;
    word nameLen;

    while (*cmdTextP != ' ' && *cmdTextP != '\r' && *cmdTextP != '&')
        cmdTextP = cmdTextP + 1;
    SkipSpace();
    fullName = cmdTextP;
    if (*cmdTextP == ':') {
        if (cmdTextP[3] != ':')
            Fatal(aIncorrectDevice, sizeof(aIncorrectDevice) - 1);
        if (cmdTextP[1] >= 'a')
            cmdTextP[1] = cmdTextP[1] & 0x5F;
        if (cmdTextP[1] != 'F')
            Fatal(aSourceFileNotDisk, sizeof(aSourceFileNotDisk) - 1);
        cmdTextP = cmdTextP + 4;
    }
    fileName = cmdTextP;
    SkipAlphaNum();
    if ((nameLen = (word)(cmdTextP - fileName)) == 0 || nameLen > 6)
        Fatal(aSourceFileName, sizeof(aSourceFileName) - 1);
    srcStemLen = (byte)(cmdTextP - fullName);
    memset(srcStemName, ' ', 10);
    memmove(srcStemName, fullName, srcStemLen);
    if (*cmdTextP == '.') {
        fileName = (cmdTextP = cmdTextP + 1);
        SkipAlphaNum();
        if ((nameLen = (word)(cmdTextP - fileName)) == 0 || nameLen > 3)
            Fatal(aSourceFileBadExt, sizeof(aSourceFileBadExt) - 1);
    }
    nameLen = (word)(cmdTextP - fullName);
    srcFileIdx = 0;
    memset(srcFileTable, ' ', 16);
    memmove(srcFileTable, fullName, nameLen);
    memset(&srcFileTable[8], 0, 4);
    SkipSpace();
    if (*cmdTextP == '$')
        Fatal(aIllegalCommand, sizeof(aIllegalCommand) - 1);
    if (*cmdTextP == '\r')
        offNxtCmdChM1 = 0;
    else
        offNxtCmdChM1 = (word)(cmdTextP - ByteP(cmdLineP) - 1);
} /* ParseSrcFile() */

static void Sub_45F6()
{
    LEFTMARGIN = 1;
    memset(ixiFileName, ' ', 15);
    memmove(ixiFileName, srcStemName, srcStemLen);
    memmove(&ixiFileName[srcStemLen], aIxi, 4);
    InitF(&ixiFile, "IXREF ", ixiFileName);
    objBlk = objByte = 0;
    memset(objFileName, ' ', 15);	
    memmove(objFileName, srcStemName, srcStemLen);
    memmove(&objFileName[srcStemLen], aObj, 4);
    InitF(&objFile, "OBJECT", objFileName);
    memset(lstFileName, ' ', 15);	
    memmove(lstFileName, srcStemName, srcStemLen);
    memmove(&lstFileName[srcStemLen], aLst, 4);
    InitF(&lstFil, "LIST ", lstFileName);
    InitF(&tx1File, "UT1 ", plmtx1);
    InitF(&tx2File, "UT2 ", plmtx2);
    InitF(&atFile, "AT  ", plmat);
    InitF(&nmsFile, "NAMES ", plmnms);
    InitF(&xrfFile, "XREF ", plmxrf);
    IXREF = false;
    IXREFSet = false;
    PRINT = true;
    PRINTSet = true;
    XREF = false;
    SYMBOLS = false;
    DEBUG = false;
    PAGING = true;
    OBJECT = true;
    OBJECTSet = true;
    OPTIMIZE = true;
    SetDate(" ", 1);
    SetPageLen(57);
    SetMarkerInfo(20, 45, 21);
    SetPageNo(0);
    SetMarginAndTabW(0xFF, 4);
    SetTitle(" ", 1);
    SetPageWidth(120);
} /* Sub_45F6() */

void Sub_40AC()
{
    memmove(version, verNo, 4);
	InitF(&conFile, "CONSOL", ":CI: ");
	OpenF(&conFile, 1);
	topMem = MemCk() - 12;
	if (topMem < 0xC000)
		Fatal(noMemMsg, sizeof(noMemMsg) - 1);
	GetCmdLine();
	PrintStr(signonMsg, sizeof(signonMsg) - 1);
	PrintStr(version, 4);
	PrintStr("\r\n", 2);
	cmdTextP = &CmdP(cmdLineP)->pstr[1];
// TOFIX
//	blkSize1 = topMem - blkSize1 - 256;
//	blkSize2 = topMem - blkSize2 - 256;
	ParseInvokeName();
	ParseSrcFile();
	Sub_45F6();
} /* Sub_40AC() */
