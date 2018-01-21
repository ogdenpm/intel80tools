#include "plm.h"

static byte copyright[] = "(C) 1976, 1977, 1982 INTEL CORP";

void Sub_3F96()
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
        if (OBJECT)
            Xputstr2cLst("NO OBJECT MODULE GENERATED", 0);
        else
            Xputstr2cLst("NO OBJECT MODULE REQUESTED", 0);
        NewLineLst();
        Xputstr2cLst("COMPILER INVOKED by:  ", 0);
        cmdLineP = startCmdLineP;
        while (cmdLineP != 0) {
            TabLst(-23);
            Xputstr2cLst(&CmdP(cmdLineP)->pstr[1], '\r');
            cmdLineP = CmdP(cmdLineP)->link;
        }
        NewLineLst();
        SetSkipLst(3);
    }
}



void Sub_404A()
{
    byte tx2Buf[2048];
    byte nmsBuf[2048];
    byte lstBuf[2048];

    if (PRINT) {
        lBufP = lstBuf;
        lBufSz = 2047;
    }
    b7AD9 = PRINT | OBJECT;
    if (OBJECT)
        DeletF(&objFile);
    if (! lfOpen && PRINTSet) {
        DeletF(&lstFil);
        PRINTSet = false;
    }
    CloseF(&tx1File);
    DeletF(&tx1File);
    CreatF(&tx2File, tx2Buf, 0x800, 1);
    if (b7AD9 || IXREF)
        CreatF(&nmsFile, nmsBuf, 0x800, 1);
    w7AEB = 0;
    if (PRINT) {
        srcFileIdx = 0;
        InitF(&srcFil, "SOURCE", (pointer)&srcFileTable[srcFileIdx]); /* note word array used */
        OpenF(&srcFil, 1);
    }
    curInfoP = procInfo[1] + botInfo;
    SetSkipLst(3);
    SetMarkerInfo(11, '-', 15);
    if (fatalErrorCode > 0) {
        STMTNum = w7AE0 = 0;
        errNum = fatalErrorCode;
        EmitError();
        SetSkipLst(2);
    }
    listing = PRINT;
    listOff = false;
    codeOn = false;
    programErrCnt = linesRead = csegSize = 0;
}

void Sub_4149()     // similar to Sub_4201 in main3.c
{
    topSymbol = localLabelsP - 3;
    curSymbolP = topSymbol - 1;
    Fread(&nmsFile, &b7ADA, 1);
    while (b7ADA != 0) {
        curSymbolP = curSymbolP - b7ADA - 1;
        SymbolP(curSymbolP)->name[0] = b7ADA;
        Fread(&nmsFile, &SymbolP(curSymbolP)->name[1], b7ADA);
        Fread(&nmsFile, &b7ADA, 1);
    }
    botSymbol = curSymbolP + 4;
    botMem = botSymbol;
}


void Sub_41B6()
{
    CloseF(&atFile);
    DeletF(&atFile);
    CloseF(&tx2File);
    DeletF(&tx2File);
    if (b7AD9 || IXREF) {
        CloseF(&nmsFile);
        DeletF(&nmsFile);
    }
    linesRead = w7AE5;
    if (PRINT) {
        TellF(&srcFil, (loc_t *)&srcFileTable[srcFileIdx + 8]);
        Backup((loc_t *)&srcFileTable[srcFileIdx + 8], offLastCh - offCurCh);
        CloseF(&srcFil);
        FlushLstBuf();
    }
}

word Start6()
{
    if (setjmp(errCont) == 0) {
        Sub_404A();
        if (b7AD9 || IXREF) {
            Sub_4149();
#ifdef _DEBUG
            symMode = 2;
#endif
        }
        Sub_3F96();
        while (b7AE4) {
            Sub_42E7();
        }

        Sub_6550();
    }
	Sub_41B6();
	if (PRINT || IXREF)
	{
		if (XREF ||  SYMBOLS || IXREF)
			return 5; // Chain(&overlay5);
		else
			LstModuleInfo();
	}
	EndCompile();
	Exit();
}
/* split file */
