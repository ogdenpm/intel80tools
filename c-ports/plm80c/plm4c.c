#include "plm.h"
#include <stdio.h>

static byte ccBits[] = {0x10, 0x18, 8, 0, 0x18, 0x10};
static byte ccCodes[] =  "\x2" "NC" "\x1" "C " "\x1" "Z " "\x2" "NZ" "\x1" "C " "\x2" "NC";

// lifted to file scope for nested procedures
static word l_arg1w;
static word wA18D;
static byte bA18F, bA190;

static void PstrCat2Line(pointer strP)
{
    if (strP != 0) {
        memmove(&line[line[0] + 1], strP + 1, strP[0]);
        line[0] = line[0] + strP[0];
    }
}

static void Sub_6175()
{
    byte i, j;
    pointer p;

    j= (bA18F >> 4) & 3;	
    bA190 = bA18F & 0xf;
    if (bA190 < 4) {
        i= (byte)wValAry[bA190];
        p= sValAry[bA190];
    } else if (j == 0) {
        i= stkRegNo[bA190 - 4];
        p= &opcodes[stkRegIdx[bA190 - 4]];
    } else {
        i= regNo[bA190 - 4];
        p= &opcodes[regIdx[bA190 - 4]];
    }

    switch (j) {
    case 0: i = (i << 4) | (i >> 4); break;
    case 1: i = (i << 3) | (i >> 5); break;
    case 2: break;
    }
    opBytes[0] |= i;
    PstrCat2Line(p);
}



static void AddWord()
{
    wpointer pw;

    dstRec = b96D6;
    pw = (wpointer)&opBytes[opByteCnt];
    *pw = wValAry[bA190];
    opByteCnt += 2;
    PstrCat2Line(sValAry[bA190]);
}



static void AddHelper()
{
    wpointer pw;
    word q;
    byte i, j;

    pw = (wpointer)&opBytes[opByteCnt];
    if (bA190 == 1)
        q = 0x69;
    else {
        i = b425D[b969D];
        j = b418C[b9692 + 11 * i];
        q = b42D6[(j >> 2)] + (j & 3);
    }
    helperStr[0] = Num2Asc(q, -4, 10, &helperStr[3]) + 2;
    PstrCat2Line(helperStr);
    if (standAlone) {
        *pw = WordP(helpersP)[q];
        dstRec = 1;
    } else {
        *pw = 0;
        dstRec = 5;
        curExtId = (byte)WordP(helpersP)[q];
    }
    opByteCnt += 2;
}


static void AddSmallNum()
{
    byte i;
    
    i = b4A78[++wA18D];
    opBytes[opByteCnt++] = i;
    /* extend to word on opBytes if not 0x84 */
    if (bA190 != 0) {
        opBytes[opByteCnt++] = 0;
    }
    line[0] += Num2Asc(i, 0, 10, &line[line[0] + 1]);
}



static void AddStackOrigin()
{
    dstRec = 3;
    opBytes[opByteCnt] = 0;
    opBytes[opByteCnt + 1] = 0;
    opByteCnt += 2;
    PstrCat2Line(stackOrigin);
}



static void AddByte()
{
    pointer str;

    opBytes[opByteCnt] = (byte)wValAry[bA190];
    opByteCnt++;
    if (wValAry[bA190] > 255) {		/* reformat number to byte Size() */
        str = sValAry[bA190];
        str[0] = Num2Asc(Low(wValAry[bA190]), 0, -16, &str[1]);
    }
    PstrCat2Line(sValAry[bA190]);
}

static void AddPCRel()
{
    wpointer pw;
    word q;

    dstRec = 1;
    pw = (wpointer)&opBytes[opByteCnt];
    wA18D = wA18D + 1;
    q = b4A78[wA18D];
    if (q > 127)	/* Sign() extend */
        q = q | 0xff00;
    *pw = baseAddr + q;
    opByteCnt += 2;
    line[line[0] + 1] = '$';
    line[0]++;
    AddWrdDisp(line, q);
}




static void AddCcCode()
{
    opBytes[0] |= ccBits[b969C];
    PstrCat2Line(&ccCodes[3 * b969C]);
}


static void EmitHelperLabel()
{
    helperStr[0] = Num2Asc(helperId, -4, 10, &helperStr[3]) + 3;
    PstrCat2Line(helperStr);
    helperId = helperId + 1;
}


static void Sub_64CF()
{
    byte i;
    switch (bA190) {
    case 0: i = b425D[b969D]; break;
    case 1: i = b475E[b969D]; break;
    case 2: i = b4774[b969D]; break;
    case 3: i = b478A[b969D]; break;
    default: fprintf(stderr, "out of bounds in Sub_64CF() bA190 = %d\n", bA190);
        Exit();
    }
    opBytes[0] = b473D[i];
    opByteCnt = 1;
    PstrCat2Line(&opcodes[b47A0[i]]);
}

static void Sub_603C()
{
    wA18D = w506F[l_arg1w];
    if (b4A78[wA18D] == 0)
        opByteCnt = 0;
    else {
        opBytes[0] = b4A78[wA18D];
        opByteCnt = 1;
    }

    dstRec = 0;
    line[0] = 0;

    while (1) {
        wA18D = wA18D + 1;
        bA18F = b4A78[wA18D];
        if (bA18F < 0x80) {
            line[line[0]++ + 1] = bA18F;
        } else if (bA18F >= 0xc0) 
            Sub_6175();
        else {
            bA190 = (bA18F >> 4) & 3;
            switch (bA18F & 0xf) {
            case 0: return;
            case 1: PstrCat2Line(sValAry[bA190]); break;
            case 2: AddWord(); break;
            case 3: AddHelper(); break;
            case 4: AddSmallNum(); break;
            case 5: AddStackOrigin(); break;
            case 6: AddByte(); break;
            case 7: AddPCRel(); break;
            case 8: AddCcCode(); break;
            case 9: EmitHelperLabel(); break;
            case 10: PstrCat2Line(w969E); break;
            case 11: Sub_64CF(); break;
            }
        }
    }
}




static void Sub_654F()
{
    word p;
    byte i;

    if (opByteCnt == 0 || ! OBJECT)
        return;
    if (((rec_t *)rec6_4)->len + opByteCnt >= 1018)
        FlushRecs();
    p = baseAddr + opByteCnt - 2;	
    switch (dstRec) {
    case 0: break;
    case 1:
            if (((rec_t *)rec22)->len + 2 >= 1018)
                FlushRecs();
            RecAddWord(rec22, 1, p);
            break; 
    case 2:
            if (((rec_t *)rec24_1)->len + 2 >= 1017)
                FlushRecs();
            RecAddWord(rec24_1, 2, p);
            break;
    case 3:
            if (((rec_t *)rec24_2)->len + 2 >= 99)
                FlushRecs();
            RecAddWord(rec24_2, 2, p);
            break;
    case 4:
            if (((rec_t *)rec24_3)->len + 2 >= 99)
                FlushRecs();
            RecAddWord(rec24_3, 2, p);
            break;
    case 5:
            if (((rec_t *)rec20)->len + 4 >= 1018)
                FlushRecs();
            RecAddWord(rec20, 1, curExtId);
            RecAddWord(rec20, 1, p);
            break;
    }
    for (i = 0; i <= opByteCnt - 1; i++) {
        RecAddByte(rec6_4, 3, opBytes[i]);
    }
} /* Sub_654F() */

void Sub_5FE7(word arg1w, byte arg2b)
{
    word p;

    l_arg1w = arg1w;    // local copy to support nested proceedures

	while (arg2b > 0) {
		Sub_603C();
		Sub_654F();
		ListCodeBytes();
		l_arg1w++;
		arg2b--;
		p = baseAddr + opByteCnt;
		if (baseAddr > p) {
			errData.stmt = errData.info = 0;
			errData.num = 0xCE;
			EmitError();
		} 
		baseAddr = p;
	}
}



static byte i;
static byte bA1AB;

Sub_66F1()
{

    if (cfCode >= 0xAE) { 
        i = cfCode - 0xAE;
        cfCode = b4602[i];
        i = b413B[i];
        b9692 = b4128[i];
    }
}



static byte arg1b_67AD, arg2b_67AD;


static void Sub_685C(byte arg1b, byte arg2b, byte arg3b)
{
    wValAry[arg1b] = arg2b;
    sValAry[arg1b] = &opcodes[arg3b];
}



static void RdBVal()
{
    Fread(&tx1File, (pointer)&wValAry[arg2b_67AD], 1);
    wValAry[arg2b_67AD] &= 0xff;
    b96B0[0] = Num2Asc(wValAry[arg2b_67AD], 0, -16, &b96B0[1]);
    sValAry[arg2b_67AD] = b96B0;
}



static void RdWVal()
{
    Fread(&tx1File, (pointer) &wValAry[arg2b_67AD], 2);
    b96B0[0] = Num2Asc(wValAry[arg2b_67AD], 0, -16, &b96B0[1]);
    sValAry[arg2b_67AD] = b96B0;
}

static void RdLocLab()
{

    Fread(&tx1File, (pointer)&w96D7, 2);
    wValAry[arg2b_67AD] = WordP(localLabelsP)[w96D7];
    locLabStr[1] = '@';
    locLabStr[0] = Num2Asc(w96D7, 0, 10, &locLabStr[2]) + 1;
    sValAry[arg2b_67AD] = locLabStr;
    b96D6 = 1;
}



static void Sub_6982()
{
#pragma pack(push, 1)
    struct { byte i; word p; } s;
#pragma pack(pop)
    Fread(&tx1File, (pointer)&s, 3);
    w969E = commentStr;
    commentStr[0] = Num2Asc(s.i, 0, 10, &commentStr[3]) + 2;
    wValAry[arg2b_67AD] = s.p;
    b96B0[0] = Num2Asc(s.p, 0, 10, &b96B0[1]);
    sValAry[arg2b_67AD] = b96B0;
}


static void Sub_69E1(word disp)
{
    Fread(&tx1File, (pointer)&curInfoP, 2);
    curInfoP = curInfoP + botInfo;
    wValAry[arg2b_67AD] = GetLinkVal() + disp;
    curSymbolP = GetSymbol();
    if (curSymbolP != 0) {
        b96B0[0] = SymbolP(curSymbolP)->name[0];
        memmove(&b96B0[1], &SymbolP(curSymbolP)->name[1], b96B0[0]); 
    } else {
        b96B0[0] = 1;
        b96B0[1] = '$';
        disp = wValAry[arg2b_67AD] - baseAddr;
    }
    sValAry[arg2b_67AD] =  b96B0;
    AddWrdDisp(sValAry[arg2b_67AD], disp);
    if (TestInfoFlag(F_EXTERNAL)) {
        b96D6 = 5;
        curExtId = GetExternId();
    } else if (GetType() == PROC_T)
        b96D6 = 1;
    else if (GetType() == LABEL_T) 
        b96D6 = 1;
    else if (TestInfoFlag(F_MEMBER))
        ;
    else if (TestInfoFlag(F_BASED))
        ;
    else if (TestInfoFlag(F_DATA))
        b96D6 = 1;
    else if (TestInfoFlag(F_MEMORY))
        b96D6 = 4;
    else if (! TestInfoFlag(F_ABSOLUTE))
        b96D6 = 2;
}




static void Sub_6B0E()
{
    word p[3];

    Fread(&tx1File, (pointer)p, 6);
    curInfoP = p[1] + botInfo;
    wValAry[arg2b_67AD] = p[2];
    b96B0[0] = Num2Asc(p[2], 0, -16, b96B0 + 1);
    sValAry[arg2b_67AD] = b96B0;
    w969E = commentStr;
    curSymbolP = GetSymbol();
    commentStr[0] = SymbolP(curSymbolP)->name[0] + 2;
    memmove(commentStr + 3, &SymbolP(curSymbolP)->name[1], SymbolP(curSymbolP)->name[0]);
    AddWrdDisp(w969E, p[0]);
}



static void Sub_6B9B()
{
    word wA1BD;
    switch (arg1b_67AD - 8) {
    case 0:  RdBVal(); break;
    case 1:  RdWVal(); break;
    case 2:  Sub_6982(); break;
    case 3:
            Fread(&tx1File, (pointer)&wA1BD, 2);
            Sub_69E1(wA1BD);
            break;
    case 4: Sub_6B0E(); break;
    }
}

static void Sub_67AD(byte arg1b, byte arg2b)
{
    // copy for nested procedures
    // below we can use arg1b, arg2b directly as they are not modified
    arg1b_67AD = arg1b;
    arg2b_67AD = arg2b;

    switch (bA1AB) {
    case 0:  return;
    case 1:
            Sub_685C(arg2b_67AD, regNo[arg1b_67AD], regIdx[arg1b_67AD]);
            Sub_685C(arg2b_67AD + 2, regNo[4 + arg1b_67AD], regIdx[4 + arg1b_67AD]);
            break;
    case 2: Sub_685C(arg2b_67AD, stkRegNo[arg1b_67AD], stkRegIdx[arg1b_67AD]); break;
    case 3: Sub_6B9B(); break;
    case 4: RdBVal(); break;
    case 5: RdWVal(); break;
    case 6: RdLocLab(); break;
    case 7: Sub_69E1(0); break;
    }
} /* Sub_67AD() */

Sub_6720()
{
    static byte i;

    b96D6 = 0;
    if (b4029[cfCode] & 0x80) {
        Fread(&tx1File, &b969C, 1);
        b969D = b4273[b969C];
    }
    w969E = 0;
    bA1AB = (b4029[cfCode] >> 4) & 7;
    if (bA1AB != 0) {
        if (bA1AB <= 3)
            Fread(&tx1File, &i, 1);
        Sub_67AD((i >> 4) & 0xf, 0);
        bA1AB = (b4029[cfCode] >> 1) & 7;
        Sub_67AD(i & 0xf, 1);
    }
} /* Sub_6720() */

void Sub_668B()
{

	Sub_66F1();
	Sub_6720();
	if (cfCode == 0x87) { 
		baseAddr = GetLinkVal();
		if (DEBUG) {
			((rec_t *)rec8)->len -= 4;
			RecAddWord(rec8, 1, baseAddr);
			((rec_t *)rec8)->len += 2;
		}
		FlushRecs();
	}
	FindErrStr();
	Sub_5FE7(w47C1[cfCode] & 0xfff, w47C1[cfCode] >> 12);
}


