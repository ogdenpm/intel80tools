#include "plm.h"

/* plm4a.plm */
static byte digits[] = " 123456789";
static byte errStrings[] = {
    "\xAC" "INVALID LABEL: UNDEFINED\0"
    "\xC8" "LIMIT EXCEEDED: STATEMENT SIZE\0"
    "\xC9" "INVALID DO CASE BLOCK, AT LEAST ONE CASE REQUIRED\0"
    "\xCA" "LIMIT EXCEEDED: NUMBER OF ACTIVE CASES\0"
    "\xCB" "LIMIT EXCEEDED: NESTING OF TYPED PROCEDURE CALLS\0"
    "\xCC" "LIMIT EXCEEDED: NUMBER OF ACTIVE PROCEDURES AND DO CASE GROUPS\0"
    "\xCD" "ILLEGAL NESTING OF BLOCKS, ENDS NOT BALANCED\0"
    "\xCE" "LIMIT EXCEEDED: CODE SEGMENT SIZE\0"
    "\xD1" "ILLEGAL INITIALIZATION OF MORE SPACE THAN DECLARED\0"
    "\xD2" "ILLEGAL INITIALIZATION OF A BYTE TO A VALUE > 255\0"
    "\xD6" "COMPILER ERROR: OPERAND CANNOT BE TRANSFORMED\0"
    "\xD7" "COMPILER ERROR: EOF READ IN FINAL ASSEMBLY\0"
    "\xD8" "COMPILER ERROR: BAD LABEL ADDRESS\0"
    "\xD9" "ILLEGAL INITIALIZATION OF AN EXTERNAL VARIABLE\0"
    };


void FlushRecs()
{
    WriteRec(rec8, 1);
    WriteRec(rec6_4, 3);
    WriteRec(rec22, 1);
    WriteRec(rec24_1, 2);
    WriteRec(rec24_2, 2);
    WriteRec(rec24_3, 2);
    WriteRec(rec20, 1);
    ((rec6_t *)rec6_4)->addr = baseAddr;
}



void AddWrdDisp(pointer str, word arg2w)
{
    if (arg2w != 0) { 
        str[0] = str[0] + 1;
        if (arg2w > 0x8000) {
            str[str[0]] = '-';
            arg2w = -arg2w;
        } else
            str[str[0]] = '+';
        str[0] = str[0] + Num2Asc(arg2w, 0, -16, &str[str[0] + 1]);
    }
}


void Sub_5BD3()
{
    if (! bo813B && listing) {
        SetStartAndTabW(15, 4);
        if (lineNo != 0)
            LstLineNo();
        TabLst(-7);
        if (depth < 10) {
            PutLst(' ');
            PutLst(digits[depth]);
        }
        else
            XnumLst(depth, 2, 10);
        if (srcFileIdx != 0) { 
            TabLst(-11);
            XwrnstrLst("=", 1);
            if (srcFileIdx != 10) 
                XnumLst(srcFileIdx / 10 - 1, 1, 10);
        }
        if (lstLineLen > 0) {
            TabLst(-15);
            XwrnstrLst(lstLine, lstLineLen);
        }

        NewLineLst();
        bo813C = true;
    }
    bo813B = true;
    listing = ! listOff && PRINT;
}



void EmitStatementNo()
{
    Sub_5BD3();
    TabLst(-50);
    XwrnstrLst("; STATEMENT # ", 14);
    XnumLst(stmtNo, 0, 0xA);
    NewLineLst();
}


void EmitLabel()
{
    if (codeOn) { 
        Sub_5BD3();
        TabLst(-26);
        XwrnstrLst(&locLabStr[1], locLabStr[0]);
        XwrnstrLst(":", 1);
        NewLineLst();
    }
}

static  word errOff, errLen;

static void Sub_5DB7()
{
    word p, q;

    q = 0;
    while (errStrings[q] != 0) {
        p = q;
        while (errStrings[q] != 0)
            q = q + 1;
        if (errStrings[p] == wa8125[0]) {
            errOff = p + 1;
            errLen = q - errOff;
            return;
        }
        q = q + 1;
    }
    errLen = 0;
}

void EmitError()
{

    programErrCnt = programErrCnt + 1;
    if (PRINT) {
        bo813B = bo813C;
        listing = true;
        Sub_5BD3();
        XwrnstrLst("*** ERROR #", 11);
        XnumLst(wa8125[0], 0, 10);
        XwrnstrLst(", ", 2);
        if (wa8125[2] != 0)
        {
            XwrnstrLst("STATEMENT #", 11);
            XnumLst(wa8125[2], 0, 10);
            XwrnstrLst(", ", 2);
        }
        if (wa8125[1] != 0)
        { 
            XwrnstrLst("NEAR '", 6);
            curInfoP = wa8125[1] + botInfo;
            curSymbolP = GetSymbol();
            if (curSymbolP != 0)
                XwrnstrLst(&SymbolP(curSymbolP)->name[1], SymbolP(curSymbolP)->name[0]);
            else
                Xputstr2cLst("<LONG CONSTANT>", 0);
            XwrnstrLst("', ", 3);
        }
        Sub_5DB7();
        TabLst(2);
        if (errLen == 0) 
            XwrnstrLst("UNKNOWN ERROR", 13);
        else
            XwrnstrLst(&errStrings[errOff], (byte)errLen);
        NewLineLst();
    }
}




void Sub_5E1B(byte arg1b)       // same as FatalError_ov6
{
    wa8125[0] = fatalErrorCode = arg1b;
    wa8125[1] = 0;
    wa8125[2] = stmtNo;
    EmitError();
    longjmp(errCont, -1);
}

void Sub_5E3E()
{
    byte i;
    if (codeOn) { 
        if (opByteCnt > 0) {
            TabLst(-12);
            XnumLst(baseAddr, -4, 0x10);
            i = 0;
            TabLst(-18);
            while (i < opByteCnt) {
                XnumLst(opBytes[i], -2, 0x10);
                i = i + 1;
            }
        }
        TabLst(-26);
        SetStartAndTabW(26, 8);
        XwrnstrLst(line, lineLen);
        NewLineLst();
    }
}



static byte bA187;


static byte GetSourceCh()
{
    if (ocurch == olstch) {
        while (1) {
            ReadF(&srcFil, srcbuf, 640, &olstch);
            ocurch = 0;
            if (olstch > 0)
                break;
            if (lstLineLen != 0)
                return '\n';

            if (srcFileIdx == 0) {
                if (bA187 != 0)
                    return '\n';
                else
                    Sub_5E1B(0xD7);
            }

            bA187 = 0;
            CloseF(&srcFil);
            srcFileIdx = srcFileIdx - 10;
            InitF(&srcFil, "SOURCE", (pointer)&srcFileTable[srcFileIdx]);
            OpenF(&srcFil, 1);
            SeekF(&srcFil, (loc_t *)&srcFileTable[srcFileIdx + 8]);
        }
        olstch = olstch - 1;
    } else
        ocurch = ocurch + 1;
    return srcbuf[ocurch] & 0x7f;
}


void GetSourceLine()
{
    lstLineLen = 0;
    bA187 = 0;

    while (1) {
        lstLine[lstLineLen] = GetSourceCh();
        if (lstLine[lstLineLen] == '\n') {
            bo813B = 0;
            bo813C = 0;
            return;
        } else if (lstLine[lstLineLen] == '\r')
            bA187 = bA187 + 1;
        else if (lstLine[lstLineLen] != '\r' && lstLineLen < 128)
            lstLineLen = lstLineLen +1;
    }
}

