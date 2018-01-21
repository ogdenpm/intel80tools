#include "plm.h"

static byte bC2A5, bC2A6, bC2A7, bC2A8;
static word wC2A9, wC2AB, wC2AD;


static bool Sub_8861()
{
    byte i;

    for (wC1D6 = wC2AB; wC1D6 <= wC2AD; wC1D6++) {
        i = Sub_5679(bC2A5);
        if (0 <= i && i <= 3)
            return true;
        if (12 <= i && i <= 14)
            return true;
    }
    return false;
} /* Sub_8861() */


static void Sub_88C1()
{
    if (Sub_8861()) {
        for (bC2A8 = 0; bC2A8 <= 3; bC2A8++) {
            if (bC04E[bC2A8] == bC2A6) {
                if (bC045[bC2A8] == 0 || bC045[bC2A8] == 1 || bC045[bC2A8] == 6) {
                    bC0B3[bC2A5] = bC045[bC2A8];
                    bC0B5[bC2A5] = bC2A8;
                    if (bC0B5[1 - bC2A5] != bC2A8)
                        return;
                }
            }
        }
    }
} /* Sub_88C1() */

static void Sub_894A()
{
    if (bC0B5[bC2A5] > 3)
    {
        for (bC2A8 = 1; bC2A8 <= 3; bC2A8++) {
            if (bC04E[bC2A8] == bC2A6) {
                if (bC045[bC2A8] == 2 || bC045[bC2A8] == 3) {
                    bC0B3[bC2A5] = bC045[bC2A8];
                    bC0B5[bC2A5] = bC2A8;
                    if (bC0B5[1 - bC2A5] != bC2A8)
                        return;
                }
            }
        }
    }
} /* Sub_894A() */


static void Sub_89D1()
{
    byte i;
    word p;

    if (bC0B5[bC2A5] == 0xA)
        wC2A9 = tx2op2[bC2A6];
    else if (bC0B5[bC2A5] == 9) {
        wC2A9 = tx2op3[bC2A6];
        if (( ! boC069[0] && boC072[0]) || bC0B1 > 0 || wC2A9 != wC1C3) {
            i = bC0B1 + bC0B2;
            for (p = wC2A9; p <= wC1C3; p++) {
                if (bC140[p] != 0)
                    i = i + 1;  
            }
            if (i < 4)
                boC1D8 = true;
            else
                bC0B5[bC2A5] = 0xA;
        }
        wC2A9 = - Shl(wC2A9, 1);
    }
} /* Sub_89D1() */

static void Sub_8A9C()
{
    word p, q;
    byte i, j;
    word r;

    if (bC0B5[bC2A5] == 0xA) {
        p = wC2A9;
        q = 0x100;
        i = 4;
        j = Sub_5748(bC0B3[bC2A5]);
    } else if (bC0B5[bC2A5] == 8 && bC0B3[bC2A5] == 1) {
        Sub_5FBF(bC0B7[bC2A5], &p, &q);
        i = 2;
        j = 1;
    } else if (bC0B5[bC2A5] == 4 && (bC0B3[bC2A5] == 0 || bC0B3[bC2A5] == 8 || !Sub_8861())) {
        Sub_5FBF(bC0B7[bC2A5], &p, &q);
        i = 2;
        j = Sub_5748(bC0B3[bC2A5]);
    } else
        return;

    for (bC2A8 = 1; bC2A8 <= 3; bC2A8++) {
        if (boC069[bC2A8]) {
            if (bC0B7[0] == bC0B7[1] && curOp != T2_STORE)
                if (bC0B5[bC2A5] > 3)
                    bC0B5[bC2A5] = bC2A8;
        }
        else if (! boC072[bC2A8] && wC096[bC2A8] == q && boC057[bC2A8] && 1 <= bC045[bC2A8] && bC045[bC2A8] <= 6) {
            r = wC084[bC2A8] + bC0A8[bC2A8] - p;
            if (r > 0xff)
                r = -r;
            if (r < i) {
                bC0B5[bC2A5] = bC2A8;
                i = (byte)r;
            }
        }
    }
    if (bC0B5[bC2A5] <= 3) {
        bC2A8 = bC0B5[bC2A5];
        bC045[bC2A8], bC0B3[bC2A5] = j;
        bC04E[bC2A8] = bC0B7[bC2A5];
        bC0A8[bC2A8] = wC084[bC2A8] + bC0A8[bC2A8] - p;
        wC084[bC2A8] = p;
    }
} /* Sub_8A9C() */



static void Sub_8CF5()
{
    bC0B5[0] = 8;
    bC0B5[1] = 8;
    for (bC2A5 = 0; bC2A5 <= 1; bC2A5++) {
        if ((bC2A6 = bC0B7[bC2A5]) == 0)
            bC0B3[bC2A5] = 0xC;
        else if ((bC2A7 = tx2opc[bC2A6]) == T2_STACKPTR)
            bC0B3[bC2A5] = 0xA;
        else if (bC2A7 == T2_LOCALLABEL)
            bC0B3[bC2A5] = 9;
        else {
            bC0B3[bC2A5] = tx2Aux1b[bC2A6];
            bC0B5[bC2A5] = tx2Aux2b[bC2A6];
            Sub_88C1(); 
            Sub_894A();/*  checked */
        }
    }
    for (bC2A5 = 0; bC2A5 <= 1; bC2A5++) {
        bC2A6 = bC0B7[bC2A5];
        Sub_597E();
        Sub_89D1();
        Sub_8A9C();
        Sub_61A9(bC2A5);
    }
} /* Sub_8CF5() */




static byte Sub_8E7E(byte arg1b)
{
    word p;
    byte i;

    if (bC0B7[arg1b] == 0 || bC0B7[arg1b] == 1)
        return 1;
    i = Sub_5679(arg1b);
    return b4D23[p = bC0C1[arg1b] * 16 + i];
} /* Sub_8E7E() */

static void Sub_8ECD(byte arg1b, byte arg2b)
{
    bC0B9[arg1b] = b4C45[arg2b];
    bC0BB[arg1b] = b4CB4[arg2b];
    bC0BD[arg1b] = b4FA3[arg2b];
} /* Sub_8ECD() */

static void Sub_8DCD()
{
    byte h, i, j, k, m, n;

    j = 198;
    for (wC1D6 = wC2AB; wC1D6 <= wC2AD; wC1D6++) {
        k = Sub_8E7E(0);
        m = Sub_8E7E(1);
        n = b4C45[k] + b4C45[m] + (b43F8[b4A21[wC1D6]] & 0x1f);
        if (n < j) {
            j = n;
            h = k;
            i = m;
            cfrag1 = b4A21[wC1D6];
            bC1D9 = b46EB[wC1D6];
            bC0BF[0] = Sub_5679(0);
            bC0BF[1] = Sub_5679(1);
        }
    }
    Sub_8ECD(0, h);
    Sub_8ECD(1, i);
} /* Sub_8DCD() */



static void Sub_8F16()
{
    if (bC0B7[0] != 0)
        Sub_63AC(bC0B5[0]);

    if (bC0B7[1] != 0) 
        Sub_63AC(bC0B5[1]);

} /* Sub_8F16() */


static void Sub_8F35()
{
    word p;

    if (curOp == T2_STKARG || curOp == T2_STKBARG || curOp == T2_STKWARG) {
        Sub_5795(-Shl(wB53C[procCallDepth], 1));
        wB53C[procCallDepth] = wB53C[procCallDepth] + 1;  
        wC1C3 = wC1C3 + 1;  
    } else if (curOp == T2_CALL) {
        Sub_5795(-Shl(wB53C[procCallDepth], 1));
        curInfoP = tx2op3[tx2qp];
        if (TestInfoFlag(F_EXTERNAL))
            p = Shl(wB53C[procCallDepth] + 1, 1);
        else
            p = Shl(wB528[procCallDepth] + 1, 1) + GetBaseVal();
        if (p > wC1C5)
            wC1C5 = p;
    } else if (curOp == T2_CALLVAR) {
        Sub_5795(-Shl(wB53C[procCallDepth], 1));
        if (wC1C5 < Shl(wC1C3, 1))
            wC1C5 = Shl(wC1C3, 1);
    } else if (curOp == T2_RETURN || curOp == T2_RETURNBYTE || curOp == T2_RETURNWORD) {
        boC1CD = 0xff;
        Sub_5EE8();
    } else if (curOp == T2_JMPFALSE) {
        Sub_5795(0);
        if (boC20F) {
            cfrag1 = CF_JMPTRUE;
            boC20F = 0;
        }
    } else if (curOp == T2_63) 
        Sub_5795(0);
    else if (curOp == T2_MOVE) {
        if (wB53C[procCallDepth] != wC1C3) {
            Sub_5795(-Shl(wB53C[procCallDepth] + 1, 1));
            Sub_6416(3);
        }
        if (bC045[3] == 1)
            cfrag1 = CF_MOVE_HL;
    }
} /* Sub_8F35() */


static void Sub_940D()
{
    for (bC2A8 = 0; bC2A8 <= 3; bC2A8++) {
        if (bC04E[bC2A8] == bC0B7[0])
            if (bC045[bC2A8] < 2 || 5 < bC045[bC2A8])
                bC04E[bC2A8] = 0;
    }
}

static void Sub_90EB()
{
    word p, q;
    byte i, j, k;

    p = w48DF[bC1D9] * 16;
    q = w493D[bC1D9];
    k = 0;
    if (curOp == T2_STORE) {
        Sub_940D();
        if (tx2Auxw[bC0B7[1]] == 0)
            if (tx2Auxw[bC0B7[0]] > 0) {
                if (cfrag1 == CF_MOVMLR || cfrag1 == CF_STA) {
                    bC045[bC0B5[1]] = 0;
                    bC04E[bC0B5[1]] = bC0B7[0];
                } else if (cfrag1 == CF_SHLD || cfrag1 == CF_MOVMRP) {
                    bC045[bC0B5[1]] = 1;
                    bC04E[bC0B5[1]] = bC0B7[0];
                }
            }
    }
    else if (T2_51 <= curOp && curOp <= T2_56)
        Sub_940D();
    for (bC2A8 = 5; bC2A8 <= 8; bC2A8++) {
        i = Shr(p, 13);
        j = Shr(q, 12);
        p = Shl(p, 3);
        q = Shl(q, 4);
        if (j <= 3) {
            Sub_5B96(j, bC2A8);
            if (i == 1)
                bC0A8[bC2A8] = bC0A8[bC2A8] + 1;  
            else if (i == 2) {
                if (bC045[bC2A8] == 0) {
                    bC045[bC2A8] = 6;
                } else {
                    bC045[bC2A8] = 1;
                    boC057[bC2A8] = 0;
                }
            }
        }
        else if (j == 4) {
            boC057[k = bC2A8] = 0;
            if (0 < tx2Auxw[tx2qp]) {
                bC04E[bC2A8] = tx2qp;
                bC045[bC2A8] = tx2Aux1b[tx2qp] = Shr(b43F8[cfrag1], 5);
                bC0A8[bC2A8] = 0;
            } else
                bC04E[bC2A8] = 0;
        } else if (j == 5) {
            bC04E[bC2A8] = 0;
            wC096[bC2A8] = 0;
            bC0A8[bC2A8] = 0;
            boC057[bC2A8] = 0xFF;
            bC045[bC2A8] = 0;
            wC084[bC2A8] = i;
        } else {
            bC04E[bC2A8] = 0;
            boC057[bC2A8] = 0;
        }
    }
    if (k == 0 && tx2Auxw[tx2qp] > 0) {
        for (bC2A8 = 5; bC2A8 <= 8; bC2A8++) {
            if (bC04E[bC2A8] == 0) 
                if (! boC057[k = bC2A8])
                    break;
        }
        if (k != 0) {
            bC04E[k] = tx2qp;
            boC057[k] = 0;
            bC045[k] = 0;
            tx2Aux1b[tx2qp] = 0;
            bC0A8[k] = 0;
        }
    }
    for (bC2A8 = 0; bC2A8 <= 3; bC2A8++)
        Sub_5B96(bC2A8 + 5, bC2A8);
} /* Sub_90EB() */

void Sub_87CB()
{
	bC0B7[0] = (byte)tx2op1[tx2qp];
	bC0B7[1] = (byte)tx2op2[tx2qp];
	wC2AB = wAF54[curOp];
	wC2AD = wC2AB + b499B[curOp] - 1;
	Sub_8CF5();

	while (1) {
		Sub_8DCD();	/*  OK */
		if (bC0B9[0] == 0)
			if (bC0B9[1] == 0)
				break;
		if (boC1D8)
			Sub_7A85();
		else
			Sub_7DA9();
	}
	Sub_8F16();
	Sub_611A();
	Sub_5E66(Shr(w48DF[bC1D9], 12));
	Sub_8F35();
	Sub_84ED();
	Sub_90EB();
} /* Sub_87CB() */


void Sub_9457()
{
	if (EnterBlk()) {
		wB488[procChainId] = pc;
		wB4B0[procChainId] = wC1C3;
		wB4D8[procChainId] = wC1C5;
		extProcId[procChainId] = curExtProcId;
		procChainNext[blkSP] = procChainId;
		procChainId = blkSP;
		curInfoP = blkCurInfo[blkSP] = tx2op1[tx2qp] + botInfo;
		curExtProcId = GetProcId();
		pc = 0;
		EmitTopItem();
		Sub_981C();
	}
}
