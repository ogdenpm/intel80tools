#include "plm.h"

byte b4789[] = {
    2, 2, 3, 4, 3, 4, 2, 2, 3, 4, 2, 3, 2, 3, 3, 3,
    3, 2, 2, 3, 4, 2, 3, 2, 3, 2, 2, 2, 2, 3, 2, 2,
    2, 3, 2, 3, 2, 2, 3, 2, 2, 1, 2, 2, 3, 4 };

byte b47B7[] = {
    0, 2, 4, 7, 0xB, 0xE, 0x12, 0x14,
    0x16, 0x19, 0x1D, 0x1F, 0x22, 0x24, 0x27, 0x2A,
    0x2D, 0x30, 0x32, 0x34, 0x37, 0x3B, 0x3D, 0x40,
    0x42, 0x45, 0x47, 0x49, 0x4B, 0x4D, 0x50, 0x52,
    0x54, 0x56, 0x59, 0x5B, 0x5E, 0x60, 0x62, 0x65,
    0x67, 0x69, 0x6A, 0x6C, 0x6E, 0x71, 0x24, 0x24,
    0x24, 0x24, 0x13, 0x13, 0x18, 0x18, 0x18, 0x18,
    0x16, 0x2C, 0x15, 0x1F, 0x1F, 0x20, 0x20, 0x19,
    0x19, 0x19, 0x19, 8, 8, 9, 9, 6,
    7, 0x25, 0x25, 0x25, 0x25, 0x25, 0xA, 0xA,
    0xB, 0xB, 0x14, 0x14, 0x14, 0x14, 0x14, 0x39,
    0x1A, 0x1A, 0x1A, 0x1A };

byte b4813[] = {
    3, 7, 3, 7, 2, 3, 8, 1, 3, 1, 8, 2, 3, 8, 1, 3,
    1, 8, 3, 7, 3, 7, 2, 3, 8, 1, 3, 1, 8, 2, 0x1D, 3,
    1, 7, 2, 0x12, 2, 1, 0xA, 2, 1, 8, 2, 1, 8, 2, 1, 7,
    3, 7, 3, 7, 2, 3, 8, 1, 3, 1, 8, 1, 6, 3, 1, 0xB,
    1, 6, 3, 1, 0xB, 1, 6, 1, 6, 3, 7, 3, 7, 2, 3, 8,
    3, 8, 3, 9, 1, 6, 3, 1, 6, 1, 7, 3, 1, 0xC, 3, 7,
    3, 7, 2, 3, 8, 3, 8, 3, 9, 0xC, 3, 7, 3, 7, 2, 3,
    8, 1, 3, 1, 8 };


byte /* tx1Buf[1280], */ nmsBuf[1280], atBuf[1280], objBuf[1280];   // use buffer in plm0a.c
word w7197;
byte b7199;
byte rec2[54] = { 2, 0, 0 };
byte rec18[303] = { 0x18, 0, 0 };
byte rec16_1[304] = { 0x16, 0, 0, 0 };
byte rec16_2[304] = { 0x16, 0, 0, 1 };
byte rec16_3[304] = { 0x16, 0, 0, 2 };
byte rec16_4[304] = { 0x16, 0, 0, 4 };
byte rec12[1024] = { 0x12, 0, 0, 1 };
byte rec6[306] = { 6, 0, 0, 0 };
// byte rec22[154] = { 0x22, 0, 0, 3 }; // use the larger array from pdata4.c
// byte rec24_1[154] = { 0x24, 0, 0, 1, 3 }; // use the larger array from pdata4.c 
byte rec24_2[154] = { 0x24, 0, 0, 2, 3 };
byte rec24_3[154] = { 0x24, 0, 0, 4, 3 };
// byte rec20[153] = { 0x20, 0, 0, 3 }; // use the larger array from pdata4.c


void Sub_4889()
{
    WriteRec(rec6, 3);
    WriteRec(rec22, 1);
    WriteRec(rec24_1, 2);
    WriteRec(rec24_2, 2);
    WriteRec(rec24_3, 2);
    WriteRec(rec20, 1);
}



void Sub_48BA(pointer arg1w, byte arg2b, byte arg3b, pointer arg4bP)
{
    byte i;

    RecAddByte(arg1w, arg2b, arg3b);
    i = 0;
    while (i != arg3b) {
        RecAddByte(arg1w, arg2b, arg4bP[i]);
        i = i + 1;
    }
}




void Sub_4908(pointer arg1wP, word arg2w, byte arg3b)
{
    arg1wP = arg1wP + 1;
    if (*(wpointer)arg1wP + arg3b >= arg2w) {
        Sub_4889();
        ((rec6_t *)rec6)->addr = w7197;
    }
}




word Sub_4938()
{
    word p, q;
    q = 0;
    for (p = 1; p <= procCnt; p++) {
        curInfoP = procInfo[p] + botInfo;
        if (q < GetBaseVal())
            q = GetBaseVal();
    }
    return q;
}




word Sub_4984()
{
    byte i;

    i = GetType() - 2;
    if (i > 2)
        return 0;
    switch (i) {
    case 0: return 1;   /* BYTE_T */
    case 1: return 2;   /* ADDRESS_T */
    case 2: return GetParentOffset();   /* STRUCT_T */
    }
    return 0;       // inserted to avoid compiler warning
}


void Sub_49BC(word arg1w, word arg2w, word arg3w)
{
    word tmp[] = { arg1w, arg2w, arg3w };
    byte a3 = 0xa3;

    if (b7199) {
        Fwrite(&tx1File, &a3, 1);
        Fwrite(&tx1File, (pointer)tmp, 6);
    } else
        programErrCnt = programErrCnt + 1;
}

#pragma pack(push, 1)
static struct {
    byte type;
    offset_t infoP;
    word stmtNum;
    offset_t atVarInfoOffset;
    word atVarArrayIndex;
    word atNestedArrayIndex;
    word atVal;
} atFData;
#pragma pack(pop)
static byte dat[255];

static word w8115, w8117, w8119, w811B;
static bool b811D;
static word w811E;


static void Sub_4B6C()
{
    if (curInfoP == 0 || !TestInfoFlag(F_MEMBER)) {
        if (w8119 > 1) {
            w8119 = w8119 - 1;
            curInfoP = atFData.infoP;
        } else if (curInfoP == 0) {
            b811D = false;
            return;
        } else {
            if (!TestInfoFlag(F_PACKED))
                b811D = false;
            if (GetType() == STRUCT_T) {
                if (TestInfoFlag(F_ARRAY))
                    w8119 = GetDimension();
                AdvNxtInfo();
                atFData.infoP = curInfoP;
            }
        }
    }
    if (TestInfoFlag(F_ARRAY))
        w811B = GetDimension();
}

static void Sub_4BF4()
{
    if (!b811D || w8117 >= w8115) {
        Fread(&atFile, &atFData.type, 1);
        switch (atFData.type - 2) {
        case 0: Fread(&atFile, (pointer)&atFData.atVal, 2); break; /* ATI_2 */
        case 1:     /* ATI_STRING */
            Fread(&atFile, (pointer)&w8115, 2);
            Fread(&atFile, dat, w8115);
            w8117 = 0;
            break;
        case 2: Fread(&atFile, (pointer)&atFData.atVarInfoOffset, 8); break; /* ATI_DATA */
        case 3: break;    /* ATI_END */
        }
    }
}



static void Sub_4CAC()
{
    if (GetType() == BYTE_T) {
        Sub_4908(rec6, 0x12c, 1);
        RecAddByte(rec6, 3, (byte)atFData.atVal);
        w7197 = w7197 + 1;
    } else {
        Sub_4908(rec6, 0x12C, 2);
        RecAddWord(rec6, 3, atFData.atVal);
        w7197 = w7197 + 2;
    }
}

static void Sub_4CF9()
{
    Sub_49BC(0xd2, curInfoP - botInfo, atFData.stmtNum);
    Sub_4CAC();
}

static void Sub_4D13()
{
    pointer w8120;

    if (GetType() == BYTE_T) {
        atFData.atVal = dat[w8117];
        w8117 = w8117 + 1;
    } else {
        w8120 = (pointer)&atFData.atVal;
        w8120[1] = dat[w8117];
        w8117 = w8117 + 1;
        if (w8117 < w8115) {
            w8120[0] = dat[w8117];
            w8117 = w8117 + 1;
        } else {
            w8120[0] = w8120[1];
            w8120[1] = 0;
        }
    }
    Sub_4CAC();
}



static void Sub_4D85()
{
    if (atFData.atVal > 255 && GetType() == BYTE_T)
        Sub_4CF9();
    else
        Sub_4CAC();
}

static void Sub_4DA8()
{
    byte i, j;
    offset_t p;
    pointer q;

    if (atFData.atVarInfoOffset == 0)
        Sub_4D85();
    else if (GetType() == BYTE_T)
        Sub_4CF9();
    else {
        p = curInfoP;
        curInfoP = botInfo + atFData.atVarInfoOffset;
        if (TestInfoFlag(F_MEMBER)) {
            atFData.atVal = atFData.atVal + Sub_4984() * atFData.atNestedArrayIndex + GetLinkVal();
            curInfoP = GetParentOffset();
        }

        atFData.atVal = atFData.atVal + Sub_4984() * atFData.atVarArrayIndex + GetLinkVal();
        if (TestInfoFlag(F_EXTERNAL)) {
            i = GetExternId();
            curInfoP = p;
            Sub_4908(rec20, 0x95, 4);
            Sub_4CAC();
            RecAddWord(rec20, 1, i);
            RecAddWord(rec20, 1, w7197 - 2);
        } else if (TestInfoFlag(F_ABSOLUTE)) {
            curInfoP = p;
            Sub_4CAC();
        } else {
            if (GetType() == PROC_T || GetType() == LABEL_T || TestInfoFlag(F_DATA)) {
                q = rec24_1;
                i = 1;
            } else if (TestInfoFlag(F_MEMORY)) {
                q = rec24_3;
                i = 4;
            } else {
                q = rec24_2;
                i = 2;
            }

            if (i == ((rec6_t *)rec6)->seg) {
                q = rec22;
                j = 1;
            } else
                j = 2;

            curInfoP = p;
            Sub_4908(q, 0x95, 2);
            Sub_4CAC();
            RecAddWord(q, j, w7197 - 2);
        }
    }
} /* Sub_4DA8() */

static void Sub_4C7A()
{
    if (b811D) {
        switch (atFData.type - 2) {
        case 0: Sub_4D85(); break; /* ATI_2 */
        case 1: Sub_4D13(); break; /* ATI_STRING */
        case 2: Sub_4DA8(); break;   /* ATI_DATA */
        }
    }
}

static void Sub_4A31()
{
    Fread(&atFile, (pointer)&atFData.infoP, 4);
    curInfoP = (w811E = atFData.infoP) + botInfo;
    w8119 = w811B = w8115 = w8117 = 0;
    if (TestInfoFlag(F_DATA))
        ((rec6_t *)rec6)->seg = 1;
    else if (TestInfoFlag(F_MEMORY))
        ((rec6_t *)rec6)->seg = 4;
    else if (TestInfoFlag(F_ABSOLUTE))
        ((rec6_t *)rec6)->seg = 0;
    else
        ((rec6_t *)rec6)->seg = 2;

    ((rec6_t *)rec6)->addr = GetLinkVal();
    w7197 = ((rec6_t *)rec6)->addr;
    if (curInfoP == botInfo)
        b811D = false;
    else if (TestInfoFlag(F_EXTERNAL)) {
        Sub_49BC(0xd9, w811E, atFData.stmtNum);
        b811D = false;
    } else {
        Sub_4B6C();
        b811D = true;
    }

    Sub_4BF4();
    if (atFData.type == ATI_END) {
        Sub_4889();
        return;
    }

    while (1) {
        Sub_4C7A();
        Sub_4BF4();
        if (atFData.type == ATI_END) {
            Sub_4889();
            return;
        } else if (b811D) {
            if (w811B > 1)
                w811B = w811B - 1;
            else {
                AdvNxtInfo();
                while (curInfoP != 0) {
                    if (BYTE_T <= GetType() && GetType() <= STRUCT_T)
                        break;
                    AdvNxtInfo();
                }
                Sub_4B6C();
                if (!b811D)
                    Sub_49BC(0xd1, w811E, atFData.stmtNum);
            }
        }
    }
}
void Sub_49F9()
{
    while (1) {
        Fread(&atFile, &atFData.type, 1);
        if (atFData.type == ATI_AHDR)
            Fread(&atFile, (pointer)&atFData.infoP, 12);
        else if (atFData.type == ATI_DHDR)
            Sub_4A31();
        else
            return;
    }
}
