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
    7    -> collect reg / imm8 operand via acc1
    0Fh    -> binary topOp
    0Dh    -> unary topOp
    17h    -> collect IMM8 operand via acc2
    37h    -> collect imm16 operand via acc2
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
       0, 0x80,   0,   0, 0xF, 0xF,0x80, 0xF, 0xD, 0xF, 0xD, 0xF, 0xF, 0xF, 0xF, 0xF,
     0xF,  0xD, 0xF, 0xF, 0xF, 0xF, 0xF, 0xF, 0xD, 0xD,0x40,0x4D,   1,   1,   1,   1,
     0x80,   1,   0,   0,0x47,   7,   7,   7,0x17,0x47,   7,0x47,0x37,   5,   7,   0,
       0,    0,0x40,0x40,   0,   1,0x80,0x40,0x80,   0,0x40,0x80,0x80,0x40,0x81,0xC0,
     0x80, 0xD
};

byte noRegOperand[] = { 0x41, 0, 0, 0, 0x19, 0x40, 0, 0x1C, 0, 0 };
/* bit vector 66 -> 0 x 24 00011001 01000000 00000000 00011100 00000000 00 */
byte validRelocExprOp[] = { 0x1A, 5, 0x80, 0, 0xC0 };
/* bit vector 27 -> 00000101 10000000 00000000 110 */
/* +, -, unary+, K_HIGH, K_LOW*/
byte opIncompat[] = { 0x57, 0x71, 0xF4, 0x57, 0x76, 0x66, 0x66, 0x67, 0x77, 0x77, 0x77, 0x55 };
/* bit vector 88 -> 01110001 11110100 01010111 01110110
                    01100110 01100110 01100111 01110111
                    01110111 01110111 01010101 */
byte propagateFlags[] = { 0x57, 6, 2, 0x20, 0, 0, 0, 0, 0, 0, 0, 0x22 };
/* bit vector 88 -> 00000110 00000010 00100000 00000000
                00000000 00000000 00000000 00000000
                00000000 00000000 00100010 */
byte typeHasTokSym[] = { 0x3A, 0xFF, 0x80, 0, 0, 0xF, 0xFE, 0, 0x20 };
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
    1 - COMMA, DB - STKLEN, O_MACROPARAM, K_ENDM, K_EXITM, O_3D, K_REPT, K_LOCAL
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


bool TestBit(byte bitIdx, pointer bitVector) {
    // ch based bitVector byte;

    if (*bitVector < bitIdx)
        return false;

    bitVector = bitVector + (bitIdx >> 3) + 1;
    return (*bitVector & (1 << (7 - (bitIdx & 7)))) != 0;
}

bool IsReg(byte type) {
    return type == K_REGNAME || type == K_SP;
}

void ChkInvalidRegOperand() {
    if (TestBit(topOp, noRegOperand))
        if (IsReg(acc1ValType))
            OperandError();
}

void ResultType() {
    if (IsReg(acc1ValType))         // registers cannot be in full expressions
        OperandError();
    if (!(opFlags[topOp] & 2))      // only single operand
        acc2RelocFlags = 0;              // clear out flags
    else if (IsReg(acc2ValType))    // registers cannot be in full expressions
        OperandError();

    acc1ValType = O_NUMBER;
    aVar.lb = (acc1RelocFlags & UF_RBOTH) ? 0xff : 0;		// needs to be 0xff or 0 for correct constuction of kk below
    aVar.hb = (acc2RelocFlags & UF_RBOTH) ? 0xff : 0;
    if ((acc1RelocFlags & UF_SEGMASK) != SEG_ABS)
        if ((acc2RelocFlags & UF_SEGMASK) != SEG_ABS)
            if (((acc1RelocFlags ^ acc2RelocFlags) & (UF_SEGMASK | UF_RBOTH)) != 0)  /* must have same seg and relocation info */
                ExpressionError();
    if ((ii = (acc1RelocFlags & UF_EXTRN) != 0) | (jj = (acc2RelocFlags & UF_EXTRN) != 0)) { /* either extern ?*/
        if (topOp == T_PLUS)                            /* + op */
            if (!(ii || aVar.lb)) {                    /* if not (acc1 extern or relocatable) */
                acc1RelocVal = acc2RelocVal;            /* add relocation information from acc2*/
                acc1RelocFlags = acc2RelocFlags;
                return;
            }
        if (jj || aVar.hb || !TestBit(topOp, validRelocExprOp)) {	// unrolleded jump to inside if statement
            ExpressionError();
            acc1RelocFlags = 0;
        }
        return;
    }
    kk = ((topOp - 4) << 2) | (aVar.lb & 2) | (aVar.hb & 1);
    if (TestBit(kk, opIncompat)) {              // check operation is compatible with operands
        ExpressionError();
        acc1RelocFlags = 0;
        return;
    }
    if (TestBit(kk, propagateFlags)) {
        if (!aVar.lb)                          // only copy flags if not relocatable already
            acc1RelocFlags = acc2RelocFlags;
        return;
    }
    acc1RelocFlags = 0;     // is absolute value
}


void SwapAccBytes() {
    byte tmp = accum1Lb;
    accum1Lb = accum1Hb;
    accum1Hb = tmp;
}



void SetExpectOperands() {
    expectingOperands = true;
    expectingOpcode = false;
}


void LogError(byte ch) {
    if (tokenType[tokenIdx] != O_OPTVAL) {  /* ignore error if processing an optional value */
        SourceError(ch);
        return;
    }
    if (tokenSize[0] == 0)                  /* make into a NUL */
        tokenType[tokenIdx] = K_NUL;
}

word GetNumVal()        // load numeric value from top of stack
{
    wpointer valP;

    acc1RelocFlags = 0;         // initialise to absoulte zero value
    accum1 = 0;
    acc1ValType = O_NAME;       // with NAME type
    if (tokenType[0] == O_OPTVAL)
        PushToken(O_PARAM);
    if (tokenIdx == 0 || (tokenType[0] == O_DATA && !b6B36))
        LogError('Q');		// questionable syntax - possible missing opcode
    else {
        if (tokenType[0] == O_NAME || tokenType[0] == T_COMMA)      // can't handle undefined name or missing name
            LogError('U');	// undefined symbol - if here in pass2 then genuine error
        else {
            acc1ValType = tokenType[0];                             // update the value type
            if (TestBit(acc1ValType, typeHasTokSym)) {
                tokPtr = &tokenSym.curP->flags;    /* point to flags */
                acc1RelocFlags = *tokPtr & ~UF_PUBLIC; /* remove public attribute */
                valP = (wpointer)tokPtr = &tokenSym.curP->value;    /* point to value */
                acc1RelocVal = *valP;            /* pick up value */
                tokenSize[0] = 2;        /* word value */

            } else if (tokenSize[0] == 0)
                LogError('V');		// value illegal
            else {
                if (tokenSize[0] > 2)
                    LogError('V');
                acc1RelocFlags = tokenAttr[0] & ~UF_PUBLIC;    /* remove public attribute */
                acc1RelocVal = tokenSymId[0];        /* use the symbol Id() */
            }

            if (tokenSize[0] > 0)    /* get low byte */
                accum1Lb = *tokPtr;
            if (tokenSize[0] > 1)    /* and high byte if ! a register */
                accum1Hb = tokenType[0] != K_REGNAME ? tokPtr[1] : 0;
        }

        if (has16bitOperand)                    // For 16 bit string operand swap bytes
            if (tokenSize[0] == 2)
                if (tokenType[0] == O_STRING)
                    SwapAccBytes();

        if ((acc1RelocFlags & UF_EXTRN) != 0)
            if (tokenType[0] < 9)
                accum1 = 0;

        PopToken();
    }

    b6B36 = false;
    return accum1;
}


byte GetPrec(byte topOp) {
    return precedence[topOp];
}

/*
   control
   xxxxxxx1    acc2 -> 16 bit reg
   xxxxxx1x    acc1 = acc1 | acc2
   xxxxx1xx    acc2 = rol(acc2, 3)
   xxxx1xxx    acc2 -> 8 bit value
   nnnnxxxx    topOp = K_LXI + nnnn

*/
void MkCode(byte control) {
    if ((control & 3)) {   /* lxi, ldax, stax, regarith, mvi, mov, rst */
        if (accum2Hb != 0    /* reg or rst num <= 7 */
            || accum2Lb > 7
            || (control & accum2Lb & 1)    /* only B D H SP if lxi, ldax or stax */
            || ((control & 3) == 3 && accum2Lb > 2)    /* B or D if ldax or stax */
            || (!IsReg(acc2ValType) && topOp != K_RST))    /* reg unless rst */
            OperandError();
        else if (IsReg(acc2ValType) && topOp == K_RST)         /* cannot be reg for rst */
            OperandError();
        if (control & 4)
            accum2Lb = (accum2Lb << 3) + (accum2Lb >> 5);
        accum1Lb |= accum2Lb;
    } else if (topOp != K_SINGLE)        /* single byte topOp */
        if (IsReg(acc2ValType))
            OperandError();

    if (control & 8) {
        if ((acc2RelocFlags & UF_RBOTH) == UF_RBOTH) {       // can't support 16bit relocatable as 8 bit value
            ValueError();
            acc2RelocFlags = (acc2RelocFlags & ~UF_RBOTH) | UF_RLOW;  // assume low 8 bits
        }
        if ((byte)(accum2Hb + 1) > 1)    /* Error() if ! FF or 00 */
            ValueError();
    }
    if (topOp == K_IMM8 || topOp == K_IMM16) {   /* Imm8() or imm16 */
        acc1RelocFlags = acc2RelocFlags;                  // copy over relocation info for 8 or 16 bit imm
        acc1RelocVal = acc2RelocVal;
    } else
        acc1RelocFlags = 0;                          // else make abs non relocatable

    if (topOp != K_SINGLE)             /* single byte topOp */
        if (accum1Lb == 0x76)         /* mov m,m is actually Halt() */
            OperandError();
    if ((topOp = (control >> 4) + K_LXI) == K_LXI)
        nextTokType = O_DATA;
}

byte NxtTokI() {
    if (tokI >= tokenIdx)
        return 0;
    return ++tokI;
}



bool ShowLine() {
    return (((!isControlLine) && controls.list) || (ctlListChanged && isControlLine))
        && (expandingMacro <= 1 || controls.gen)
        && (!(condAsmSeen || skipIf[0]) || controls.cond);
}

/*
    xrefMode= 0 -> defined
        = 1 -> used
        = 2 -> finalise
*/
void EmitXref(byte xrefMode, pointer name) {
    byte i, byteval;

    if ((!IsPhase1() || !controls.xref || IsSkipping()) && !xRefPending)
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
            } else    /* emit low nibble */
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
