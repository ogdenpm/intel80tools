#include "plm.h"

static byte copyright[] = "(C) 1976, 1977, 1982 INTEL CORP";

static void Sub_3F3C()
{
    b7199 = PRINT | OBJECT;
    if (OBJECTSet) {
        DeletF(&objFile);
        OBJECTSet = false;
    }
    if (!lfOpen && PRINTSet) {
        DeletF(&lstFil);
        PRINTSet = false;
    }
    CloseF(&tx2File);
#ifdef _DEBUG
    copyFile(tx2File.fNam, "plmtx2.tmp_main3");
#endif
    DeletF(&tx2File);
    CreatF(&tx1File, tx1Buf, 1280, 2);
    if (b7199 || IXREF)
        CreatF(&nmsFile, nmsBuf, 1280, 1);
    CreatF(&atFile, atBuf, 1280, 1);
    if (OBJECT) {
        OpenF(&objFile, 3);
        CreatF(&objFile, objBuf, 1280, 2);
        SekEnd(&objFile);
    }
    w7197 = csegSize;
}


static void Sub_3FE2()
{
    word p;
    for (p = 1; p <= procCnt; p++) {
        curInfoP = procInfo[p] + botInfo;
        if (!TestInfoFlag(F_EXTERNAL)) {
            SetLinkVal(w7197);
            w7197 = w7197 + GetDimension2();
        }
    }
}


static void Sub_402F()
{
    word p, q;
    curInfoP = botInfo + 2;
    while (curInfoP != 0) {
        if (GetType() == LABEL_T) {
            if (!TestInfoFlag(F_LABEL))
                Sub_49BC(0xAC, curInfoP - botInfo, 0);
            else if (!TestInfoFlag(F_EXTERNAL)) {
                p = GetLinkVal();
                q = curInfoP;
                curInfoP = procInfo[High(GetScope())] + botInfo;
                p = p + GetLinkVal();
                curInfoP = q;
                SetLinkVal(p);
            }
        }
        AdvNxtInfo();
    }
}

static void Sub_40B6()
{
    word p;

    for (p = 1; p <= localLabelCnt; p++) {
        curInfoP = procInfo[ByteP(w381E)[p]] + botInfo;
        WordP(localLabelsP)[p] += GetLinkVal();
    }
}




static void Sub_4105()
{
    byte i, j, k;
    bool m;
    word p;

    if (!standAlone)
        return;

    Fwrite(&tx1File, "\xA4", 1);
    Fwrite(&tx1File, (pointer)&procInfo[1], 2);
    curInfoP = procInfo[1] + botInfo;
    p = w7197 - GetLinkVal();
    Fwrite(&tx1File, (pointer)&p, 2);
    for (i = 0; i <= 45; i++) {
        k = b42D6[i];
        j = k + b42A8[i];
        m = false;
        while (k < j) {
            if (m) {
                WordP(helpersP)[k] = w7197;
                w7197 = w7197 + b4813[k];
            } else if (WordP(helpersP)[k] != 0) {
                m = true;
                WordP(helpersP)[k] = w7197;
                w7197 = w7197 + b4813[k];
            }
            k = k + 1;
        }
    }
}



static void Sub_4201()
{
    byte i;

    curSymbolP = (topSymbol = localLabelsP - 3) - 1;
    Fread(&nmsFile, &i, 1);
    while (i != 0) {
        curSymbolP = curSymbolP - i - 1;
        SymbolP(curSymbolP)->name[0] = i;
        Fread(&nmsFile, &SymbolP(curSymbolP)->name[1], i);
        Fread(&nmsFile, &i, 1);
    }
    botSymbol = curSymbolP + 4;
    botMem = botSymbol;
}


static void Sub_426E()
{
    curInfoP = botInfo + procInfo[1];
    curSymbolP = GetSymbol();
    if (curSymbolP == 0)
        RecAddByte(rec2, 0, 0);
    else
        Sub_48BA(rec2, 0, SymbolP(curSymbolP)->name[0], &SymbolP(curSymbolP)->name[1]);
    RecAddByte(rec2, 0, 1);
    RecAddByte(rec2, 0, (version[1] << 4) | (version[3] & 0xf));
    RecAddByte(rec2, 0, 1);
    RecAddWord(rec2, 0, csegSize);
    RecAddByte(rec2, 0, 3);
    RecAddByte(rec2, 0, 2);
    RecAddWord(rec2, 0, dsegSize);
    RecAddByte(rec2, 0, 3);
    RecAddByte(rec2, 0, 3);
    RecAddWord(rec2, 0, Sub_4938());
    RecAddByte(rec2, 0, 3);
    RecAddByte(rec2, 0, 4);
    RecAddWord(rec2, 0, 0);
    RecAddByte(rec2, 0, 3);
    WriteRec(rec2, 0);
}


static void Sub_436C()
{
    pointer p;
    word q, r, s;
    byte i, j, k, m, t[6];

    s = 0;
    curInfoP = botInfo + 2;
    while (curInfoP != 0) {
        curSymbolP = GetSymbol();
        if (LABEL_T <= GetType() && GetType() <= PROC_T && curSymbolP != 0) {
            if (TestInfoFlag(F_EXTERNAL) && !TestInfoFlag(F_AT)) {
                if (((rec_t *)rec18)->len + SymbolP(curSymbolP)->name[0] + 2 >= 299)
                    WriteRec(rec18, 0);
                s = s + 1;
                Sub_48BA(rec18, 0, SymbolP(curSymbolP)->name[0], &SymbolP(curSymbolP)->name[1]);
                RecAddByte(rec18, 0, 0);
            } else if (!(TestInfoFlag(F_AUTOMATIC) || TestInfoFlag(F_BASED) || TestInfoFlag(F_MEMBER))) {
                if (TestInfoFlag(F_DATA) || GetType() == LABEL_T || GetType() == PROC_T) {
                    p = rec16_2;
                    q = ((rec_t *)rec16_2)->len;
                    i = 1;
                } else if (TestInfoFlag(F_MEMORY)) {
                    p = rec16_4;
                    q = ((rec_t *)rec16_4)->len;
                    i = 4;
                } else if (TestInfoFlag(F_ABSOLUTE)) {
                    p = rec16_1;
                    q = ((rec_t *)rec16_1)->len;
                    i = 0;
                } else {
                    p = rec16_3;
                    q = ((rec_t *)rec16_3)->len;
                    i = 2;
                }

                if (TestInfoFlag(F_PUBLIC)) {
                    if (q + SymbolP(curSymbolP)->name[0] + 4 >= 299)
                        WriteRec(p, 1);
                    RecAddWord(p, 1, GetLinkVal());
                    Sub_48BA(p, 1, SymbolP(curSymbolP)->name[0], &SymbolP(curSymbolP)->name[1]);
                    RecAddByte(p, 1, 0);
                }
                if (DEBUG) {
                    if (curInfoP == procInfo[1] + botInfo)
                        j = true;
                    else if (!TestInfoFlag(F_PARAMETER))
                        j = TestInfoFlag(F_EXTERNAL);
                    else {
                        r = curInfoP;
                        curInfoP = procInfo[High(GetScope())] + botInfo;
                        j = TestInfoFlag(F_EXTERNAL);
                        curInfoP = r;
                    }
                    if (!j) {
                        if (i != ((rec_t *)rec12)->val[0] || ((rec_t *)rec12)->len + SymbolP(curSymbolP)->name[0] + 4 >= 1019)
                            WriteRec(rec12, 1);
                        ((rec_t *)rec12)->val[0] = i;
                        RecAddWord(rec12, 1, GetLinkVal());
                        Sub_48BA(rec12, 1, SymbolP(curSymbolP)->name[0], &SymbolP(curSymbolP)->name[1]);
                        RecAddByte(rec12, 1, 0);
                    }
                }
            }
        }
        AdvNxtInfo();
    } /* of while */

    t[0] = '@';
    t[1] = 'P';
    if (!standAlone) {
        for (k = 0; k <= 116; k++) {
            if (WordP(helpersP)[k] != 0) {
                WordP(helpersP)[k] = s;
                s = s + 1;
                if (((rec_t *)rec18)->len + 8 >= 299)
                    WriteRec(rec18, 0);
                m = Num2Asc(k, 0xfc, 10, &t[2]);
                Sub_48BA(rec18, 0, 6, t);
                RecAddByte(rec18, 0, 0);
            }
        }
    }
    WriteRec(rec18, 0);
    WriteRec(rec16_1, 1);
    WriteRec(rec16_2, 1);
    WriteRec(rec16_3, 1);
    WriteRec(rec16_4, 1);
    WriteRec(rec12, 1);
}



static void Sub_46B7()
{
    word p;

    if (intVecNum == 0)
        return;
    for (p = 1; p <= procCnt; p++) {
        curInfoP = procInfo[p] + botInfo;
        if (TestInfoFlag(F_INTERRUPT)) {
            ((rec6_t *)rec6)->addr = intVecLoc + intVecNum * GetIntrNo();
            RecAddByte(rec6, 3, 0xC3);
            RecAddWord(rec6, 3, GetLinkVal());
            WriteRec(rec6, 3);
            RecAddWord(rec24_1, 2, ((rec6_t *)rec6)->addr + 1);
            WriteRec(rec24_1, 2);
        }
    }
}



static void Sub_4746()
{
    if (b7199 || IXREF) {
        Fwrite(&tx1File, "\x9c", 1);
        Fflush(&tx1File);
        Rewind(&tx1File);
        CloseF(&nmsFile);
#ifdef _DEBUG
        copyFile(nmsFile.fNam, "plmnms.tmp_main3");
#endif
        DeletF(&nmsFile);
        Fflush(&objFile);
    }
    CloseF(&atFile);
#ifdef _DEBUG
    copyFile(atFile.fNam, "plmat.tmp_main3");
#endif
    DeletF(&atFile);
}

word Start3()
{
    Sub_3F3C();
    if (b7199 || IXREF)
        Sub_3FE2();
    Sub_402F();
    if (b7199 || IXREF) {
        Sub_40B6();
        Sub_4105();
        csegSize = w7197;
        Sub_4201();
#ifdef _DEBUG
        symMode = 2;    // now info points to pstr symbols
#endif
    }
    if (OBJECT) {
        Sub_426E();
        Sub_436C();
        Sub_46B7();
    }
    Sub_49F9();
    Sub_4746();
    if (b7199)
        return 4; // Chain(overlay[4]);
    else {
        CloseF(&tx1File);
#ifdef _DEBUG
        copyFile(tx1File.fNam, "plmtx1.tmp_main3");
#endif
        DeletF(&tx1File);
        if (IXREF)
            return 5; // Chain(overlay[5]);
        else {
            EndCompile();
            Exit();
        }
    }
}
