#include "plm.h"

static byte copyright[] = "(C) 1976, 1977, 1982 INTEL CORP";
static byte objEOF[] = {0xe, 1, 0, 0xf1};

static byte b4304[] = {
    0x24,0x24,0x24,0x24,0x13,0x13,0x18,0x18,
    0x18,0x18,0x16,0x2C,0x15,0x1F,0x1F,0x20,
    0x20,0x19,0x19,0x19,0x19,   8,   8,   9,
    9,   6,   7,0x25,0x25,0x25,0x25,0x25,
    0xA, 0xA, 0xB, 0xB,0x14,0x14,0x14,0x14,
    0x14,0x39,0x1A,0x1A,0x1A,0x1A };


static void Sub_3FC8()
{
    if (PRINT) {
        NewPageNextChLst();
        Xputstr2cLst("ISIS-II PL/M-80 ", 0);
        XwrnstrLst(version, 4);
        Xputstr2cLst(" COMPILATION OF MODULE ", 0);
        curInfoP = botInfo + procInfo[1];
        curSymbolP = GetSymbol();
        if (curSymbolP != 0)
            XwrnstrLst(&SymbolP(curSymbolP)->name[1], SymbolP(curSymbolP)->name[0]);
        NewLineLst();
        if (OBJECT) { 
            Xputstr2cLst("OBJECT MODULE PLACED IN ", 0);
            Xputstr2cLst(objFile.fNam, ' ');
        }
        else
            Xputstr2cLst("NO OBJECT MODULE REQUESTED", 0);

        NewLineLst();
        if (w382A == 1)
        {
            Xputstr2cLst("COMPILER INVOKED by:  ", 0);
            cmdLineP = startCmdLineP;
            while (cmdLineP != 0) {
                TabLst(-23);
                Xputstr2cLst(&CmdP(cmdLineP)->pstr[1], '\r');
                cmdLineP = CmdP(cmdLineP)->link;
            }
            NewLineLst();
        }
    }
}

void Sub_408B()
{
    if (PRINT) { 
        lBufP = lstBuf;
        lBufSz = 639;
    }

    Sub_3FC8();
    MEMORY = 0xA30A;
    if (MEMORY + 256 > botMem)
        Fatal("COMPILER ERROR: INSUFFICIENT MEMORY FOR FINAL ASSEMBLY", 54);
    stmtNo = 0;
    if (PRINT) {
        srcFileIdx = 0;
        InitF(&srcFil, "SOURCE", (pointer)&srcFileTable[srcFileIdx]);
        OpenF(&srcFil, 1);
    }

    CreatF(&tx1File, tx1Buf, 640, 1);
    CreatF(&objFile, objBuf, 640, 2);
    curInfoP = procInfo[1] + botInfo;
    baseAddr = ((rec6_t *)rec6_4)->addr = GetLinkVal();
    SetSkipLst(3);
    SetMarkerInfo(11, '-', 15);
    if (fatalErrorCode > 0) {
        errData.stmt = errData.info = 0;
        errData.num = fatalErrorCode;
        EmitError();
        SetSkipLst(2);
    }
    listing = PRINT;
    listOff = false;
    codeOn = false;
    programErrCnt = linesRead = 0;
}



void Sub_4162()
{
    byte helperModId, endHelperId;

    if (! standAlone)
        return;
    for (helperModId = 0; helperModId <= 45; helperModId++) {
        helperId = b42D6[helperModId];
        endHelperId = helperId + b42A8[helperModId];
        while (helperId < endHelperId) {
            if (WordP(helpersP)[helperId] != 0) {
                baseAddr = WordP(helpersP)[helperId];
                b969C = b4304[helperModId];
                b969D = b4273[b969C];
                Sub_5FE7(w4919[helperId], b4A03[helperId]);
                break;
            }
            helperId = helperId + 1;
        }
    }
} /* Sub_4162() */

void Sub_4208()
{
    if (haveModule) { 
        ((rec4_t *)rec4)->subType = 1;
        curInfoP = procInfo[1] + botInfo;
        ((rec4_t *)rec4)->addr = GetLinkVal();
    } else {
        ((rec4_t *)rec4)->subType = 0;
        ((rec4_t *)rec4)->addr = 0;
    }
    WriteRec(rec4, 0);
}


void Sub_423C()
{
    linesRead = w812F;
    Sub_4208();
    CloseF(&tx1File);
#ifdef _DEBUG
    copyFile(tx1File.fNam, "plmtx1.tmp_main4");
#endif
    if (OBJECT) {
        Fwrite(&objFile, objEOF, 4);
        Fflush(&objFile);
        CloseF(&objFile);
    }

    if (PRINT) {
        TellF(&srcFil, (loc_t *)&srcFileTable[srcFileIdx + 8]);
        Backup((loc_t *)&srcFileTable[srcFileIdx + 8], offLastCh - offCurCh);
        CloseF(&srcFil);
        FlushLstBuf();
    }

    DeletF(&tx1File);
} /* Sub_423C() */


word Start4()
{
// rec24_2 is has different seg c.f. plm3a.c
    ((rec_t *)rec24_1)->val[0] = 2; // data seg
    ((rec_t *)rec24_2)->val[0] = 3; // stack seg

    if (setjmp(exception) == 0) {
        Sub_408B();

        while (bo812B) {
            Sub_54BA();
        }
        Sub_4162();
        FlushRecs();
        FindErrStr();
    }
	Sub_423C();
    if (IXREF)
        return 5; // Chain(overlay5);
	if (PRINT) {
		if (XREF || SYMBOLS)
			return 5; // Chain(overlay5);
		else
			LstModuleInfo();
	}
	EndCompile();
	Exit();

}
