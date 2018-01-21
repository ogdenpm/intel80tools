#include "plm.h"

// lifted to file scope
static word wC267, wC269, wC26B, wC26D;
static byte bC26F;
static byte bC266;
static byte bC272, bC273;


static bool Sub_7254(byte arg1b, word arg2w)
{
    if (arg1b == 0) {
        if (tx2Aux2b[bC26F] != 4)
            return false;
    } else if (arg1b != 3) {
        if (tx2Aux2b[bC26F] != 8)
            return false;
        if (arg2w != 0) {
            if (arg1b == 2)
                return false;
            if (arg1b == 4) {
                if (wC269 != 0)
                   return false;
            } else if (arg1b == 5) {
                if (wC269 != wC26D)
                   return false;
            }
        }
    }
    else if (arg2w == 0x4000)
        return false;
    return true;
}


static word Sub_72F2()
{
    switch (bC266) {
    case 0: return wC267;
    case 1: return wC267 + wC26B;
    case 2: return wC267 - wC26B;
    case 3: return wC267 * wC26B;
    case 4: return wC267 / wC26B;
    case 5: return wC267 % wC26B;
    case 6: return wC267 & wC26B;
    case 7: return wC267 | wC26B;
    case 8: return wC267 ^ wC26B;
    case 9: return wC267 + wC26B;
    case 10: return wC267 + wC26B;
    case 11: return wC267 + wC26B + wC26B;
    case 12: return wC267 + wC26B;
    case 13: return -wC267;
    case 14: return ~wC267;
    case 15: return Low(wC267);
    case 16: return High(wC267);
    case 17: return wC267;
    }
	return wC267;	// added to prevent compiler warning C4715
}

static void Sub_73C5()
{
    if (curOp == T2_MEMBER && tx2Aux2b[tx2op1[tx2qp]] == 0xa) {
        bC272 = tx2Aux1b[tx2op2[tx2qp]];
        bC273 = 0xA;
    } else 
        switch (b5112[bC266] & 7) {
        case 0: 
            bC272 = 0;
            bC273 = 8;
            break;
        case 1: 
            if (tx2Aux1b[tx2op1[tx2qp]] == 0 && tx2Aux1b[tx2op2[tx2qp]] == 0)
               bC272 = 0;
            else
               bC272 = 1;
            bC273 = 8;
			break;
        case 2:
            bC272 = 1;
            bC273 = 8;
            break;
        case 3:
            bC272 = tx2Aux1b[tx2op1[tx2qp]];
            bC273 = tx2Aux2b[tx2op1[tx2qp]];
            break;
        case 4:
            bC272 = tx2Aux1b[tx2op2[tx2qp]];
            bC273 = tx2Aux2b[tx2op2[tx2qp]];
            break;
        case 5:
            bC272 = Sub_5748(tx2Aux1b[tx2op2[tx2qp]]);
            bC273 = 8;
            break;
    }
    if (wC269 == wC26D)
        curInfoP = 0;
    else if (wC26D == 0)
        curInfoP = tx2op1[tx2op1[tx2qp]];
    else
        curInfoP = tx2op1[tx2op2[tx2qp]];
}

void Sub_717B()
{
    word p;

    bC266 = curOp - 0x12;
    bC26F = (byte)tx2op1[tx2qp];
    Sub_5FBF(bC26F, &wC267, &wC269);
    if (Sub_7254(Shr(b5112[bC266], 6), wC269)) {
        bC26F = (byte)tx2op2[tx2qp];
        Sub_5FBF(bC26F, &wC26B, &wC26D);
        if (Sub_7254(Shr(b5112[bC266], 3) & 7, wC26D)) {
            Sub_611A();
            p = Sub_72F2();
            Sub_73C5();
            if (bC272 == 0 && bC273 == 8)
                p = p & 0xFF;
            Sub_5F4B(p, curInfoP, bC272, bC273);
            bC1D2 = b5124[curOp = tx2opc[tx2qp]];
        }
    }
}

static byte bC27A;
static word wC27B, wC27D;
static byte bC27F;
static word p;
static byte arg1b_765B, arg2b_765B, arg3b_765B;


static bool Sub_76E2(byte arg1b)
{
    if ((bC27F & 0x40) != 0) {
        if (tx2Aux1b[arg1b] == 1)
            return true;
        if (tx2Aux1b[arg1b] == 3)
            return true;
        return false;
    } else if (tx2Aux1b[arg1b] == 0)
        return true;
    else if (tx2Aux1b[arg1b] == 2)
        return true;
    else if (tx2Aux1b[arg1b] == 8)
        return true;
    else
        return false;
}

static void Sub_7754()
{
    byte i, j;

    i = tx2qp + 1;
    while (tx2opc[i] == T2_OPTBACKREF && i < bC1BF) 
        i = i + 1;  
    j = tx2qp;
    if (tx2opc[i] == T2_STORE) {
        if (arg3b_765B == tx2op1[i]) {
            if (tx2op2[i] == j) {
                if (tx2Auxw[j] == 1) {
                    bC27A = 6;
                    tx2opc[i] = T2_SEMICOLON;
                    tx2Auxw[j] = 0;
                    tx2Auxw[arg3b_765B] = tx2Auxw[arg3b_765B] - 1;  
                }
            }
        }
    }
}

static bool Sub_7801()
{
    if (wC27D != 0 || (curOp < T2_BASED && tx2Aux2b[arg2b_765B] != 8))
        return false;
    if ((bC27F & 0x20) != 0)
        if (! Sub_76E2(arg3b_765B))
            return false;
    if ((bC27F & 1) != 0)
        if (Sub_76E2(arg2b_765B) != Sub_76E2(arg3b_765B))
            return false;
    if ((bC27F & 4) != 0) {
        if ((bC27F & 8) != 0)
            if (wC27B != 0)
                return true;
            else
                return false;
        p = w502A[Shr(b5048[arg1b_765B], 3)];
        if (p != wC27B)
            return false;
        if ((bC27F & 0x80) != 0)
            Sub_7754();
        return true;
    }
    if ((bC27F & 0x40) == 0) {
        p = w502A[Shr(b5048[arg1b_765B], 3)];
        bC27A = wC27B % p;
        return true;
    } else if (wC27B <= 4) {
        bC27A = (byte)wC27B;
        return true;
    } else
        return false;
}

static void Sub_7925()
{
    byte i, j, k;

    arg1b_765B = arg1b_765B - 1;
    i = b5221[arg1b_765B] + bC27A;
    j = b5048[arg1b_765B] & 3;
    if (i == 0xAD) {
        if (j == 0)
            wC27B = 0;
        Sub_611A();
        bC27F = 0;
        k = 1;
        if (curOp != T2_STAR)
            if (curOp != T2_SLASH)
                if (curOp != T2_MOD)
                    if (wC27B < 0x100)
                        if (Sub_76E2(arg2b_765B))
                            if (arg3b_765B == 0)
                                k = 0;
                            else if (Sub_76E2(arg2b_765B))
                                k = 0;
        Sub_5F4B(wC27B, 0, k, 8);
    } else {
        if (j == 1) {
            tx2op1[tx2qp] = arg3b_765B;
            tx2Auxw[arg2b_765B] = tx2Auxw[arg2b_765B] - 1;  
        } else {
            if (tx2op2[tx2qp] != 0)
                tx2Auxw[tx2op2[tx2qp]] = tx2Auxw[tx2op2[tx2qp]] - 1;  
            if ((bC1D2 & 4) != 0) {
                tx2op1[tx2qp] = tx2op2[1];
                if (boC20F) {
                    i = 0x43 - i;
                    boC20F = 0;
                }
            }
        }
        tx2opc[tx2qp] = i;
        tx2op2[tx2qp] = 0;
    }
    bC1D2 = b5124[curOp = i];
}

static bool Sub_765B(byte arg1b, byte arg2b, byte arg3b)
{
    bool i;

    // make visible to previously nested procedures
    arg1b_765B = arg1b;
    arg2b_765B = arg2b;
    arg3b_765B = arg3b;

    Sub_5FBF(arg2b_765B, &wC27B, &wC27D);
    bC27A = 0;
    i = true;

    while (i) {
        if ((bC27F = b50AD[arg1b_765B]) == 0xff)
            return false;
        if ((bC27F & 0x10) != 0) {
            i = ! Sub_76E2(arg2b_765B);
            if ((bC27F & 4) != 0 && !i)
                i = ! Sub_7801();
        } else 
            i = ! Sub_7801();
        arg1b_765B = arg1b_765B + 1;  
    }
    Sub_7925();
    return true;
} /* Sub_765B() */

void Sub_7550()
{
    if (curOp <= T2_GT) {
        if (tx2opc[tx2qp+1] == T2_JMPFALSE && tx2Auxw[tx2qp] == 1) {
            tx2opc[tx2qp + 1] = T2_JNZ;
            tx2op1[tx2qp + 1] = tx2op2[1];
            if (boC20F) {
                tx2op2[tx2qp + 1] = bC209[curOp];
                boC20F = 0;
            } else
                tx2op2[tx2qp + 1] = curOp;
            tx2Auxw[tx2qp] = 0;
        } else
            tx2opc[tx2qp] = curOp = curOp + 0x26;
    } else if (Sub_765B(b5202[curOp], (byte)tx2op2[tx2qp], (byte)tx2op1[tx2qp]))
        ;
    else if (Sub_765B(b51E3[curOp], (byte)tx2op1[tx2qp], (byte)tx2op2[tx2qp]))
        ;
}


