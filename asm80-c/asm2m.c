// vim:ts=4:expandtab:shiftwidth=4:
//
/* force the non standard code generation of outStrN and put2Hex */
#include "asm80.h"

//void OutStrN(pointer s, word n);
//void Put2Hex(word arg1w, word arg2w);
/* 
    0    -> ?
    1    -> start single word expression
    5    -> single byte opcode no operand
    7    -> reg operand
    0Fh    -> binary topOp
    0Dh    -> unary topOp
    17h    -> IMM8 operand
    37h    -> imm16 operand
    40h    -> list
    47h    -> 2 operand topOp
    4Dh    -> start word list
    80h    -> end expression
    81h    -> DoRept operand
    0C0h    -> DoLocal operand

    -------x    -> getnum to acc1 & copy to acc2
    ------x-    -> getnum to acc1
    -----x--    -> collect low(acc1)
    ----x---    -> collect high(acc1)
    ---x----    -> collect low(acc2)
    --x-----    -> collect high(acc2)
    -x------    -> list
*/

byte opFlags[] = {
       /* 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F */
          0, 0x80,   0,   0, 0xF, 0xF, 0x80, 0xF, 0xD, 0xF, 0xD, 0xF, 0xF, 0xF, 0xF, 0xF,
        0xF,  0xD, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xD, 0xD, 0x40, 0x4D,   1,   1,   1,   1,
        0x80,   1,   0,   0, 0x47,   7,   7,   7, 0x17, 0x47,   7, 0x47, 0x37,   5,   7,   0,
          0,    0, 0x40, 0x40,   0,   1, 0x80, 0x40, 0x80,   0, 0x40, 0x80, 0x80, 0x40, 0x81,0xC0,
        0x80, 0xD
	};

byte noRegOperand[] = {0x41, 0, 0, 0, 0x19, 0x40, 0, 0x1C, 0, 0};
    /* bit vector 66 -> 0 x 24 00011001 01000000 00000000 00011100 00000000 00 */
byte b41C1[] = {0x1A, 5, 0x80, 0, 0xC0};
    /* bit vector 27 -> 00000101 10000000 00000000 110 */
	/* K_SPECIAL, K_REGNAME, K_SP, K_HIGH, K_LOW*/
byte opCompat[] = {0x57, 0x71, 0xF4, 0x57, 0x76, 0x66, 0x66, 0x67, 0x77, 0x77, 0x77, 0x55};
    /* bit vector 88 -> 01110001 11110100 01010111 01110110
                        01100110 01100110 01100111 01110111
                        01110111 01110111 01010101 */
byte propagateFlags[] = { 0x57, 6, 2, 0x20, 0, 0, 0, 0, 0, 0, 0, 0x22 };
    /* bit vector 88 -> 00000110 00000010 00100000 00000000
                    00000000 00000000 00000000 00000000
                    00000000 00000000 00100010 */ 
byte typeHasValue[] = {0x3A, 0xFF, 0x80, 0, 0, 0xF, 0xFE, 0, 0x20};
    /* bit vector 59 -> 11111111 10000000 00000000 00000000
                        00001111 11111110 00000000 001 */
    /* T_BEGIN, T_CR, T_LPAREN, T_RPAREN/O_LABEL, T_STAR, T_PLUS/K_SPECIAL, T_COMMA, */
    /* T_MINUS/K_REGNAME, T_UPLUS/K_SP */
    /* K_LXI, K_REG16, K_LDSTAX, K_ARITH, K_IMM8, K_MVI, K_INRDCR. K_MOV, K_IMM16, K_SINGLE */
    /* K_RST */
/* precedence table */
/*
   10 - NULL
    9 - HIGH, LOW
    8 - *, /, MOD, SHL, SHR
    7 - +, -, UPLUS, UMINUS
    6 - =, <, <=, >, >=, !=
    5 - NOT
    4 - AND
    3 - OR, XOR,
    2 - ! used
    1 - COMMA, DB - STKLEN, O_MACROARG, DoEndm, DoExitm, O_3D, DoRept, DoLocal
    0 - T_BEGIN,T_CR,T_LPAREN,T_RPAREN,K_MACRO,T_MACRONAME,K_IRP,K_IRPC
*/
byte precedence[] = {
     /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
        0, 0, 0, 0, 8, 7, 1, 7, 7, 8, 7, 6, 6, 6, 6, 6,
        6, 5, 4, 3, 3, 8, 8, 8, 9, 9, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1,
        0, 10
     };


bool TestBit(byte bitIdx, pointer bitVector)
{
    // ch based bitVector byte;

    if (*bitVector < bitIdx)
        return false;
    
    bitVector = bitVector + (bitIdx >> 3) + 1;
    return (*bitVector & (1 << (7 - (bitIdx & 7)))) != 0;
}

bool IsReg(byte topOp)
{
    return topOp == K_REGNAME || topOp == K_SP;
}

void ChkInvalidRegOperand()
{
    if (TestBit(topOp, noRegOperand))
        if (IsReg(acc1ValType))
            OperandError();
}

void Sub4291()
{
    if (IsReg(acc1ValType))
        OperandError();
    if (!(opFlags[topOp] & 2))
        acc2Flags = 0;
    else if (IsReg(acc2ValType))
        OperandError();

    acc1ValType = O_NUMBER;
    aVar.lb = (acc1Flags & UF_BOTH) ? 0xff : 0;		// needs to be 0xff or 0 for correct constuction of kk below
    aVar.hb = (acc2Flags & UF_BOTH) ? 0xff : 0;
    if ((acc1Flags & UF_SEGMASK) != SEG_ABS)
        if ((acc2Flags & UF_SEGMASK) != SEG_ABS)
            if (((acc1Flags ^ acc2Flags) & 0x1F) != 0)
                ExpressionError();
    if ((ii = (acc1Flags & UF_EXTRN) != 0) | (jj = (acc2Flags & UF_EXTRN) != 0)) {
        if (topOp == K_SPECIAL)    /* +? (PAGE INPAGE)? */
            if (! (ii || aVar.lb)) {
                acc1NumVal = acc2NumVal;
                acc1Flags = acc2Flags;
                return;
            }
		if (jj || aVar.hb || !TestBit(topOp, b41C1)) {	// unrolled jump to inside if statement
			ExpressionError();
			acc1Flags = 0;
		}
        return;
    }
    kk = ((topOp - 4) << 2) | (aVar.lb & 2) | (aVar.hb & 1);
    if (TestBit(kk, opCompat)) {
        ExpressionError();
        acc1Flags = 0;
        return;
    }
    if (TestBit(kk, propagateFlags)) {
        if (! aVar.lb)
            acc1Flags = acc2Flags;
        return;
    }
    acc1Flags = 0;
}


void SwapAccBytes()
{
    byte tmp = accum1Lb;
    accum1Lb = accum1Hb;
    accum1Hb = tmp;
}



void SetExpectOperands()
{
    expectingOperands = true;
    expectingOpcode = false;
}


void LogError(byte ch)
{
	if (tokenType[tokenIdx] != O_OPTVAL) {
	    SourceError(ch);
	    return;
	}
	if (tokenSize[0] == 0)
	    tokenType[tokenIdx] = K_NUL;
}

word GetNumVal()
{
	wpointer valP;

    acc1Flags = 0;
    accum1 = 0;
    acc1ValType = O_NAME;
    if (tokenType[0] == O_OPTVAL)
        PushToken(CR);
    if (tokenIdx == 0 || (tokenType[0] == O_DATA && !b6B36))
        LogError('Q');
    else {
        if (tokenType[0] == O_NAME || tokenType[0] == T_COMMA)
            LogError('U');
        else {
            acc1ValType = tokenType[0];
            if (TestBit(acc1ValType, typeHasValue))
            {
                tokPtr = &tokenSym.curP->flags;    /* point to flags */
                acc1Flags = *tokPtr & ~UF_PUBLIC; /* remove public attribute */
                valP = (wpointer)tokPtr = &tokenSym.curP->value;    /* point to value */
                acc1NumVal = *valP;            /* pick up value */
                tokenSize[0] = 2;        /* word value */

            }
            else if (tokenSize[0] == 0)
                LogError('V');
            else {
                if (tokenSize[0] > 2)
                    LogError('V');
                acc1Flags = tokenAttr[0] & ~UF_PUBLIC;    /* remove public attribute */
                acc1NumVal = tokenSymId[0];        /* use the symbol Id() */
            }

            if (tokenSize[0] > 0)    /* get low byte */
                accum1Lb = *tokPtr;
            if (tokenSize[0] > 1)    /* and high byte if ! a register */
                accum1Hb = tokenType[0] != 7 ?  tokPtr[1] : 0;
        }    

        if (has16bitOperand)
            if (tokenSize[0] == 2)
                if (tokenType[0] == O_STRING)
                    SwapAccBytes();

        if ((acc1Flags & UF_EXTRN) != 0)
            if (tokenType[0] < 9)
                accum1 = 0;

        PopToken();
    }

    b6B36 = false;
    return accum1;
}


byte GetPrec(byte topOp)
{
    return precedence[topOp];
}

/*
   arg1b
   xxxxxxx1    acc2 -> 16 bit reg
   xxxxxx1x    acc1 = acc1 | acc2
   xxxxx1xx    acc2 <<= 3
   xxxx1xxx    acc2 -> 8 bit value
   nnnnxxxx    topOp = 24h + nnnn
   
*/   
void MkCode(byte arg1b)
{
    if ((arg1b & 3))    /* lxi, ldax, stax, regarith, mvi, mov, rst */
    {
        if (accum2Hb != 0    /* reg or rst num <= 7 */
           || accum2Lb > 7     
           || (arg1b & accum2Lb & 1)    /* only B D H SP if lxi, ldax or stax */
           || ((arg1b & 3) == 3 && accum2Lb > 2)    /* B or D if ldax or stax */
           || (! IsReg(acc2ValType) && topOp != K_RST))    /* reg unless rst */
            OperandError();
        else if (IsReg(acc2ValType) && topOp == K_RST)         /* cannot be reg for rst */
            OperandError();
        if (arg1b & 4)
            accum2Lb = (accum2Lb << 3) + (accum2Lb >> 5);
        accum1Lb |= accum2Lb;
    }
    else if (topOp != K_SINGLE)        /* single byte topOp */
        if (IsReg(acc2ValType))
            OperandError();

    if (arg1b & 8)
    {
        if ((acc2Flags & UF_BOTH) == UF_BOTH)
        {
            ValueError();
            acc2Flags = (acc2Flags & ~UF_BOTH) | UF_LOW;
        }
        if ((byte)(accum2Hb + 1) > 1)    /* Error() if ! FF or 00 */
            ValueError();
    }
    if (topOp == K_IMM8 || topOp == K_IMM16)    /* Imm8() or imm16 */
    {
        acc1Flags = acc2Flags;
        acc1NumVal = acc2NumVal;
    }
    else
        acc1Flags = 0;

    if (topOp != K_SINGLE)             /* single byte topOp */
        if (accum1Lb == 0x76)         /* mov m,m is actually Halt() */
            OperandError();
    if ((topOp = (arg1b >> 4) + 0x24) == 0x24)
        nextTokType = O_DATA;
}

byte NxtTokI()
{
    if (tokI >= tokenIdx)
        return 0;
    return ++tokI;
}



bool ShowLine()
{
    return (((!isControlLine) && controls.list) || (ctlListChanged && isControlLine))
            && (expandingMacro <= 1 || controls.gen)
        && (!(condAsmSeen || skipIf[0]) || controls.cond);
}

/*
    xrefMode= 0 -> defined
        = 1 -> used
        = 2 -> finalise
*/
void EmitXref(byte xrefMode, pointer name)
{
    byte i, byteval;

    if ((! IsPhase1() || ! controls.xref || IsSkipping()) && ! xRefPending)
        return;

    Outch(xrefMode + '0');    /* convert to hex char */
    if (xrefMode != XREF_FIN) {    /* ! finalise */
        OutStrN(name, 6);        /* label ref */
        xRefPending = false;
        byteval = srcLineCnt >> 8;    /* line number in hex - high byte first */
        i = 0;
        while (i < 4) {
            if (++i & 1)    /* high nibble ? */
            {
                if (i == 3)    /* get low byte */
                    byteval = srcLineCnt & 0xff;
                /* emit high nibble */
                Outch(Nibble2Ascii(byteval >> 4));
            }
            else    /* emit low nibble */
                Outch(Nibble2Ascii(byteval));
        }
    } else {    /* finalise */
        OutStrN(lstFile, 15);    /* listing file name */
        if (controls.paging)        /* whether paging '1' or '0' */
            Outch('1');
        else
            Outch('0');
        /* page length and page width as 2 hex chars */
        Outch(Nibble2Ascii(controls.pageLength >> 4));
        Outch(Nibble2Ascii(controls.pageLength));
        Outch(Nibble2Ascii(controls.pageWidth >> 4));
        Outch(Nibble2Ascii(controls.pageWidth));
        Outch('3');    /* end of file */
        Flushout();
        CloseF(xreffd);
    }
}
