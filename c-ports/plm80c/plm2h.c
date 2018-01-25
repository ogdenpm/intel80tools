#include "plm.h"

static byte b9BA8[2] = {12, 13};
static byte b9BAA[2] = {1, 2};
static byte b9BAC[2] = {12, 13};
static byte b9BAE[2] = {1, 2};



static bool Sub_9C33()
{
    byte i, j, k;

    i = (byte)tx2op1[tx2qp];
    if ((b5124[tx2opc[i]] & 0xc0) == 0) {
        if (tx2Auxw[i] > 1)
            return false;
        if (tx2op3[i] != 0)
            bC140[tx2op3[i]] = tx2qp;
    }
    k = (byte)tx2Auxw[tx2qp];
    Sub_56A0(i, tx2qp);
    tx2Auxw[tx2qp] = k;
    tx2Auxw[i] = tx2Auxw[i] - 1;  
    for (j = 0; j <= 3; j++) {
        if (bC04E[j] == i)
            bC04E[j] = tx2qp;
    }
    return true;
}

void Sub_9BB0()
{
    bC0B7[0] = (byte)tx2op1[tx2qp];
    bC0B7[1] = (byte)tx2op2[tx2qp];
    if (T2_DOUBLE <= curOp && curOp <= T2_ADDRESSOF)
        Sub_717B();
    if (curOp <= T2_MEMBER) {
        Sub_7550();
        if (curOp == T2_65)
            if (Sub_9C33())
                return;
    }
    if ((bC1D2 & 0xc0) == 0) {
        Sub_87CB();
        if (curOp == T2_MOVE)
            procCallDepth = 0;
    } else if ((bC1D2 & 0xc0) == 0x80)
        Sub_994D();
}



void Sub_9D06()
{
    byte i, j, k;
    pointer pbyt;
    byte m;

    if (procCallDepth <= 10) {
        curInfoP = tx2op3[tx2qp];
        i = GetDataType();
        if (i == 3)
            wAF54[132] = 1;
        else
            wAF54[132] = 0;
        j = m = GetParamCnt();
        pbyt = &b44F7[wAF54[132]];
        k = 0;

        while (j > 0) {
            AdvNxtInfo();
            j = j - 1;
            if (j < 2) {
                *pbyt = (*pbyt << 4) & 0xf0;
                if (GetType() == ADDRESS_T)
                    *pbyt = *pbyt | b9BA8[k];
                else
                    *pbyt = *pbyt | b9BAA[k];
                k = 1;
            }
        }

        if (m == 1)
            *pbyt = (*pbyt << 4) & 0xf0;
        Sub_9BB0();
        wC1C3 = wB528[procCallDepth];
    }
    procCallDepth = procCallDepth - 1;  
}


static pointer pb_C2EB;


static void Sub_9EAA(byte arg1b, byte arg2b)
{
    
    *pb_C2EB = (*pb_C2EB << 4) & 0xf0;
    if (arg1b != 0)
        if (tx2Aux1b[arg1b] == 0)
            *pb_C2EB = *pb_C2EB | b9BAE[arg2b];
        else
            *pb_C2EB = *pb_C2EB | b9BAC[arg2b];
}

void Sub_9DD7()
{
    byte i;

    if (procCallDepth <= 10) {
        i = (byte)tx2op3[tx2qp];
        if (tx2opc[i] == T2_VARIABLE) {
            curInfoP = tx2op1[i];
            if (TestInfoFlag(F_AUTOMATIC))
                wAF54[133] = 3;
            else
                wAF54[133] = 4;
        } else if (tx2op3[i] == wB53C[procCallDepth]) {
            wAF54[133] = 5;
            wB528[procCallDepth] = wB528[procCallDepth] - 1;  
        } else
            wAF54[133] = 2;

        pb_C2EB = &b44F7[wAF54[133]];
        Sub_9EAA((byte)tx2op1[tx2qp], 0);
        Sub_9EAA((byte)tx2op2[tx2qp], 1);
        Sub_9BB0();
        wC1C3 = wB528[procCallDepth];
    }
    procCallDepth = procCallDepth - 1;  
}




void Sub_9EF8()
{
    procCallDepth = 1;
    Sub_9BB0();
    wB53C[procCallDepth] = wC1C3;
}



void Sub_9F14()
{
    if (EnterBlk())
        blkCurInfo[blkSP] = wC1CF;
}


void Sub_9F2F()
{
    word p, q;
    p = q = (word)blkCurInfo[blkSP];
    if (ExitBlk()) {
        while (p < wC1CF) {
            wC1DC[0] = 14;
            wC1DC[1] = WordP(botMem)[p];
            EncodeFragData(CF_DW);
            pc = pc + 2;
            p = p + 1;  
        }
        if (wC1CF == q) {
            Tx2SyntaxError(ERR201); /*  Invalid() do CASE block, */
                        /*  at least on case required */
            EmitTopItem();
        }
        wC1CF = q;
    }
}



void Sub_9F9F()
{
    if (ExitBlk()) {
        curInfoP = blkCurInfo[procChainId];
        if (! boC1CC) {
            Sub_5EE8();
            EncodeFragData(CF_RET);
            pc = pc + 1;  
        }
        if (TestInfoFlag(F_INTERRUPT))
            wC1C5 = wC1C5 + 8;

        SetDimension(pc);
        SetBaseVal(wC1C5 + wC1C7);
        pc = wB488[procChainId = procChainNext[procChainId]];
        bC1E6 = 0;
        PutTx1Byte(0xa4);
        PutTx1Word(blkCurInfo[procChainId] - botInfo);
        PutTx1Word(pc);
        WrFragData();
        wC1C3 = wB4B0[procChainId];
        wC1C5 = wB4D8[procChainId];
        wC1C7 = 0;
        curExtProcId = extProcId[procChainId];
    }
}

void Sub_A072(byte arg1b)
{
    word p;
    curInfoP = tx2op1[tx2qp] + botInfo;
    p = GetDimension() - arg1b;
    if (p < 0x100)
        Sub_5F4B(p, 0, 0, 8);
    else
        Sub_5F4B(p, 0, 1, 8);
}


void Sub_A0C4()
{
    word p;
    p = Sub_575E(tx2op1[tx2qp] + botInfo);
    if (p < 0x100)
        Sub_5F4B(p, 0, 0, 8);
    else
        Sub_5F4B(p, 0, 1, 8);
}

void Sub_A10A()
{
    procCallDepth = procCallDepth + 1;
    if (procCallDepth <= 10) {
        Sub_5E66(0xf);
        wB528[procCallDepth] = wC1C3;
        wB53C[procCallDepth] = wC1C3;
    } else if (procCallDepth == 11) {
        Tx2SyntaxError(ERR203); /*  LIMIT EXCEEDED: NESTING OF TYPED */
                    /*  procedure CALLS */
        EmitTopItem();
    }
}




static void Sub_A266()
{
    byte i;

    boC1CD = false;
    for (i = 0; i <= 3; i++) {
        bC045[i] = 0xc;
        bC04E[i] = 0;
        boC057[i] = false;
    }
}

void Sub_A153()
{
    Sub_A266();
    for (tx2qp = 4; tx2qp <= bC1BF - 1; tx2qp++) {
        curOp = tx2opc[tx2qp];
        bC1D2 = b5124[curOp];
        switch (bC1D2 >> 6) {
        case 0:
                if (curOp == T2_CALL)
                    Sub_9D06();
                else if (curOp == T2_CALLVAR)
                    Sub_9DD7();
                else if (curOp == T2_BEGMOVE)
                    Sub_9EF8();
                else
                    Sub_9BB0();
                break;
        case 1:
                if (curOp == T2_LENGTH)
                    Sub_A072(0);
                else if (curOp == T2_LAST)
                    Sub_A072(1);
                else if (curOp == T2_SIZE)
                    Sub_A0C4();
                break;  
        case 2: 
                if (curOp == T2_PROCEDURE)
                    Sub_9457();
                else
                    Sub_994D();
                break;
        case 3:
                if (curOp == T2_CASE)
                    Sub_9F14();
                else if (curOp == T2_ENDCASE)
                    Sub_9F2F();
                else if (curOp == T2_ENDPROC)
                    Sub_9F9F();
                else if (curOp == T2_BEGCALL)
                    Sub_A10A();
                break;
        }

        tx2op3[tx2qp] = 0;
    }
    Sub_5795(0);
    boC1CC = boC1CD;
}


