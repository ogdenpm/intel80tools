#include "asm80.h"

static byte controlTable[] = {
/* format of entries
    byte -> 00PNxxxx where
        P -> 0 general control 1 -> primary control
        N -> 0 NO prefix not supported 1-> NO prefix supported
        xxxx -> number of chars in name string
    name string
*/
            "\x35" "DEBUG"     "\x3A" "MACRODEBUG"
            "\x34" "XREF"      "\x37" "SYMBOLS"
            "\x36" "PAGING"    "\x33" "TTY"
            "\x25" "MOD85"     "\x35" "PRINT"
            "\x36" "OBJECT"    "\x39" "MACROFILE"
            "\x29" "PAGEWIDTH" "\x2A" "PAGELENGTH"
            "\x7"  "INCLUDE"   "\x5"  "TITLE"
            "\x4"  "SAVE"      "\x7"  "RESTORE"
            "\x5"  "EJECT"     "\x14" "LIST"
            "\x13" "GEN"       "\x14" "COND"};

byte tokVal, savedCtlList, savedCtlGen;
bool controlError;


static bool ChkParen(byte arg1b)
{
    SkipWhite();
    reget = 0;
    return arg1b + '(' == curChar;
}



static byte GetTok()
{
    tokBufLen = 0;
    tokType = TT_ID;
    if (IsCR())
        return curChar;

    SkipWhite_2();
    if (curChar > 'A'-1 && 'Z'+1 > curChar || curChar > 'a'-1 && 'z'+1 > curChar) {  /* letter */
        GetId(O_ID);
        if (BlankAsmErrCode() && tokenSize[0] < 14)
            memcpy(tokBuf, lineBuf, tokBufLen = tokenSize[0]);
    } else if (curChar > '0'-1  &&  '9'+1 > curChar) {   /* digit ? */
        GetNum();
        if (BlankAsmErrCode()) {
            tokNumVal = GetNumVal();
            tokType = TT_NUM;
            tokBuf[0] = ' ';
        }
    } else if (curChar == '\'') {   /* string ? */
        GetStr();
        if (BlankAsmErrCode()) {
            tokBufLen = 64;         /* cap at 64 chars */
            if (tokenSize[0] < 64)
                tokBufLen = tokenSize[0];
            tokType = TT_STR;
            if (tokBufLen > 0)
                memcpy(tokBuf, lineBuf, tokBufLen);
        }
    } else {
        tokBufLen = 1;
        tokBuf[0] = curChar;
        return curChar;
    }
    PopToken();      /* restore the stack from GetId/GetNum/GetStr() calls */
    return tokBuf[0];
}



static bool FinaliseFileNameOpt(pointer arg1w)
{
//    word pad;

    if (tokBufIdx == 0)
        return false;

    tokBuf[tokBufIdx] = ' ';
    tokBufLen = tokBufIdx;
    if (IsWhite())
        return ChkParen(1);
    return true;
}

static void GetFileNameOpt()
{
    SkipWhite_2();

    while (1) {
        if (IsRParen() || IsWhite()) {
            if (FinaliseFileNameOpt(tokBuf))
                return;
           break;
        }

        tokBuf[tokBufIdx] = curChar;
        tokBufIdx = tokBufIdx + 1;
        if (tokBufIdx > 14)
            break;
        curChar = GetCh();
    }
    curFileNameP = tokBuf;
    FileError();
}


static void GetFileParam()
{
    tokBufIdx = 0;
    if (! ChkParen(0))    /* ( */
        FileError();
    else {
        GetFileNameOpt();
        memcpy(curFileNameP, tokBuf, tokBufIdx + 1);
        if (! ChkParen(1))    /* ) */
            FileError();
    }
}


static void GetMacroFileDrive()
{
    SkipWhite_2();
    tokBufIdx = 13;     /* leave room for max file name */
    ii = 0;

    while (! IsRParen() && ii < 4) {
        asmacRef[ii] = curChar;
        ii = ii + 1;
        curChar = GetCh();
    }

    if (IsRParen() || IsWhite())
        if (FinaliseFileNameOpt(asmacRef))
            return;
    curFileNameP = asmacRef;
    tokBufIdx = 0;
    FileError();
}




static bool GetControlNumArg()
{
    if (ChkParen(0)) {   /* ( */
        tokVal = GetTok();
        if (tokType == TT_NUM)
            return ChkParen(1);    /* ) */
    }
    return false;
}

static void SetControl(byte ctlVal, byte noInvalid)
{
    if (!noInvalid) {
        controls.all[controlId] = ctlVal;
        if (!ctlVal)
            controlId = 17;    /* no action needed */
    }
}

static byte LookupControl()
{
    byte cmdIdx, cmdStartIdx;
    bool ctlVal;
    byte cmdLen, ctlFlags, noInvalid;
    pointer controlP, nextControlP;
    bool *ctlSeenP;

    cmdLen = tokBufLen;
    cmdStartIdx = 0;
    ctlVal = true;
    if (tokBuf[0] == 'N' && tokBuf[1] == 'O') {   /* check for NO prefix */
        cmdStartIdx = 2;    /* don't match the NO in the table */
        ctlVal = false;        /* control will be set as false */
        cmdLen = tokBufLen - 2;    /* length of string to match excludes the NO */
    }

    controlP = controlTable;
    controlId = 0;
    cmdIdx = cmdStartIdx;

    while (controlId < sizeof(controls)) {
        nextControlP = controlP + (*controlP & 0xF) + 1;
        if ((*controlP & 0xF) == cmdLen)
        {
            ctlFlags = *controlP;
            while (cmdIdx < tokBufLen) {
                controlP++;
                if (*controlP != tokBuf[cmdIdx])
                    cmdIdx = tokBufLen + 1;    /* cause early Exit() */
                else
                    cmdIdx = cmdIdx + 1;    /* check next character */
            }

            if (cmdIdx == tokBufLen)        /* found it */
                goto found;
        }
        controlP = nextControlP;
        controlId = controlId + 1;
        cmdIdx = cmdStartIdx;
    }
    return 255;                        /* not found */

found:
    if ((noInvalid = (ctlFlags & 0x10) != 0x10) && !ctlVal)
        return 255;    /* NO not supported */

    if ((ctlFlags & 0x20) != 0x20)        /* general control */
    {
        SetControl(ctlVal, noInvalid);
        return controlId;
    }

    if (! primaryValid)            /* is a PRIMARY control */
        return 255;

    ctlSeenP = &controlSeen[controlId];        /* make sure we only see a primary once */
    if (*ctlSeenP)
        return 255;
    *ctlSeenP = true;
    SetControl(ctlVal, noInvalid);
    return controlId;
}

static void ProcessControl()
{
    /* simple controls already processed */
    if (controlId >= 17 || controlId < 5)
        return;

    switch (controlId - 5) {
    case 0:            /* TTY */
            controls.tty = true;
    case 1:            /* MOD85 */
            controls.mod85 = true;
            return;
    case 2:            /* PRINT */
            controlFileType = 2;
            curFileNameP = lstFile;
            GetFileParam();
            return;
    case 3:            /* OBJECT */
            controlFileType = 3;
            curFileNameP = objFile;
            GetFileParam();
            return;
    case 4:            /* MACROFILE */
            controlFileType = 3;
            if (ChkParen(0))    /* optional drive for tmp file */
                GetMacroFileDrive();
            else
                reget = 1;
            controls.macroFile = true;
            return;
    case 5:            /* PAGEWIDTH */
            if (GetControlNumArg()) {
                controls.pageWidth = (byte) tokNumVal;
                if (controls.pageWidth > 132)
                    controls.pageWidth = 132;
                if (controls.pageWidth < 72)
                    controls.pageWidth = 72;
                return;
            }
            break;
    case 6:            /* PAGELENGTH */
            if (GetControlNumArg()) {
                controls.pageLength = (byte) tokNumVal;
                if (controls.pageLength < 15)
                    controls.pageLength = 15;
                return;
            }
            break;
    case 7:            /* INCLUDE */
            if (! pendingInclude) {		// multiple includes on control line not supported
                controlFileType = 1;
                if (fileIdx == 5)
                    StackError();
                else
                {
                    fileIdx = fileIdx + 1;
                    curFileNameP = files[fileIdx].name;
                    GetFileParam();
                    pendingInclude = true;
                    if (scanCmdLine)
                        includeOnCmdLine = true;
                    return;
                }
            }
            break;
    case 8:            /* TITLE */
            if (ChkParen(0)) {
                tokVal = GetTok();
                if (tokType == TT_STR && tokBufLen != 0) {
                    if (phase != 1 || IsPhase1() && primaryValid) {
                        memcpy(titleStr, tokBuf, tokBufLen);
                        titleStr[titleLen = tokBufLen] = 0;
                        if (ChkParen(1)) {
                            controls.title = true;
                            return;
                        }
                    }
                }
            }
            break;
    case 9:            /* SAVE */
            if (saveIdx > 7)
                StackError();
            else {
                memcpy(&saveStack[saveIdx++], &controls.list, 3);
                return;
            }
            break;
    case 10:            /* RESTORE */
            if (saveIdx > 0) {
                memcpy(&controls.list, &saveStack[--saveIdx], 3);
                return;
            }
            break;
    case 11:            /* EJECT */
        controls.eject++;
            return;
    }
    controlError = true;
}

void ParseControls()
{
    isControlLine = true;
    ctlListChanged = savedCtlList = controls.list;
    savedCtlGen = controls.gen;
    controlError = false;

    while (GetTok() != CR && ! controlError) {
        if (tokBuf[0] == ';')        /* skip comments */
            Skip2EOL();
        else if (LookupControl() == 255)    /* Error() ? */
            controlError = true;
        else
            ProcessControl();
    }

    if (controlError) {
        if (tokBuf[0] != CR) {
            reget = 0;
            Skip2EOL();
        }

        if (scanCmdLine)
            RuntimeError(RTE_CMDLINE);    /* command line error */
        else
            CommandError();
    }

    ChkLF();            /* eat the LF */
    if (controls.list != savedCtlList)
        ctlListChanged = true;
    else if (controls.gen != savedCtlGen && expandingMacro)
        ctlListChanged = false;

    reget = 0;
}

