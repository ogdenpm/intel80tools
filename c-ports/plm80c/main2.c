#include "plm.h"
#include <setjmp.h>

word wAF54[] = {
    0x11B, 0x14B, 0x12B, 0x12B, 0x11B, 0x14B, 0x60, 0x60,
    0x62, 0x62, 0x5B, 0x62, 0xB, 0x1E4, 0x1E7, 0x1E8,
    0x1EE, 0x1F1, 0x8D, 0xCF, 0x10B, 0xE, 0x12, 0x14,
    0xEB, 0xEB, 0xEB, 0xA9, 0x9A, 0x96, 0xA1, 0x69,
    0x68, 0x70, 0x76, 0x90, 0xEB, 0x10B, 0x15B, 0x18B,
    0x1BB, 0x19B, 0x17B, 0x16B, 0x58, 0x19, 0x20, 0x27,
    0x19, 0x20, 0x27, 0x16, 0x17, 0x18, 0x16, 0x17,
    0x18, 0xA, 0x38, 0x3E, 0x67, 0x2A, 6, 0xC,
    0x1DE, 0x80, 0x43, 0x45, 0x47, 0x1DB, 0x49, 0xC9,
    0xCD, 0xCE, 0xB7, 0xBA, 0xBD, 0x41, 0x42, 0x44,
    0x46, 0x48, 0x4A, 0x4B, 0x4C, 0x41, 0x42, 0x44,
    0x46, 0x48, 0x4A, 0x4B, 0x4C, 0x7C, 0x42, 0x44,
    0x46, 0x48, 0x48, 0x46, 0x44, 0x42, 0x7C, 0x42,
    0x44, 0x46, 0x48, 0x48, 0x46, 0x44, 0x42, 0x4F,
    0x42, 0x44, 0x46, 0x48, 0x4A, 0x4D, 0x4E, 0x57,
    0x50, 0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0xC0,
    0xC3, 0xC6, 0xCF, 0x1DC, 0, 0};	/* wB05C & wB05E assumed at end */

//static byte tx2Buf[512]; use larger buf in main6.c
// static byte tx1Buf[512]; use larger buffer in plm0a.c
offset_t blkCurInfo[20];
word wB488[20];
word wB4B0[20];
word wB4D8[20];
byte extProcId[20];
byte procChainNext[20];
word wB528[10];
word wB53C[10];
byte tx2opc[255] = {T2_SEMICOLON, T2_LOCALLABEL, T2_SEMICOLON, T2_SEMICOLON};
byte tx2Aux1b[255] = {12, 9};
byte tx2Aux2b[255];
word tx2op1[255];
word tx2op2[255];
word tx2op3[255] = {0, 0, 0, 0};
word tx2Auxw[255] = {0, 1};
byte bC045[9];
byte bC04E[9];
bool boC057[9];
bool boC060[9];
bool boC069[9];
bool boC072[9];
bool boC07B[9];
word wC084[9];
word wC096[9];
byte bC0A8[9];
byte bC0B1;
byte bC0B2;
byte bC0B3[2];
byte bC0B5[2];
byte bC0B7[2];
byte bC0B9[2];
byte bC0BB[2];
byte bC0BD[2];
byte bC0BF[2];
byte bC0C1[2];
byte bC0C3[125];
byte bC140[125];
byte bC1BD = 0;
byte tx2qp;
byte bC1BF = 4;
byte tx2qEnd = 4;
word pc = 0;
word wC1C3 = 0;
word wC1C5 = 0;
word wC1C7 = 0;
byte blkSP = 0;
byte blkOverCnt = 0;
byte procCallDepth = 0;
bool boC1CC = false;
bool boC1CD;
bool eofSeen = false;
word wC1CF = 0;
byte curOp;
byte bC1D2;
byte padC1D3;
byte curExtProcId = 1;
byte procChainId = 0;
word wC1D6;
bool boC1D8 = false;
byte bC1D9;
byte cfrag1;
byte bC1DB;
word wC1DC[5];
byte bC1E6;
byte buf_C1E7[34];
byte bC209[] = {4, 5, 3, 2, 0, 1};
bool boC20F = false;




byte copyRight[] = "(C) 1976, 1977, 1982 INTEL CORP";


static void Sub_3F27()
{
    byte emsg[] = "COMPILER ERROR: INSUFFICIENT MEMORY FOR CODE GENERATION";
    MEMORY = 0xC3BD;  // to align with ov2

    botMem = MEMORY + 256;
    if (w3822 < botMem)
        Fatal(emsg, Length(emsg));
    CreatF(&tx1File, tx1Buf, 512, 2);
    CreatF(&tx2File, tx2Buf, 512, 1);
    memset(wC1DC, 0, 10);
    blkCurInfo[0] = procInfo[1] + botInfo;
    programErrCnt = 0;
} /* Sub_3F27() */


static void Sub_3F7D()
{
    curInfoP = procInfo[1] + botInfo;
    SetDimension(pc);
    SetBaseVal(wC1C5);
    Fflush(&tx1File);
} /* Sub_3F7D() */

word Start2()
{
    if (setjmp(exception) == 0) {
        Sub_3F27();
        while (1) {
            FillTx2Q();
            Sub_67A9();
            if (tx2opc[4] == T2_EOF)
                break;
            Sub_689E();
            Sub_6BD6();
            Sub_A153();
        }
    }
    /* longjmp comes here */
    Sub_3F7D();
    return 3; // Chain(overlay[3]);
}
