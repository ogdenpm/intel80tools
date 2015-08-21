as4483: do;

/* force the non standard code generation of outStrN and put2Hex */

OutStrN: procedure(s, n) external; declare s address, n address; end;
$IF OVL4
Put2Hex: procedure(arg1w, arg2w) external; declare arg1w address, arg2w address; end;
$include(asm44.ipx)
$ELSE
$include(asm83.ipx)
$ENDIF
/* 
	0	-> ?
	1	-> start single word expression
	5	-> single byte opcode no operand
	7	-> reg operand
	0Fh	-> binary leftOp
	0Dh	-> unary leftOp
	17h	-> IMM8 operand
	37h	-> imm16 operand
	40h	-> ? start byte list
	47h	-> 2 operand leftOp
	4Dh	-> start word list
	80h	-> end expression
	81h	-> rept operand
	0C0h	-> local operand

	-------x	-> getnum to acc1 & copy to acc2
	------x-	-> getnum to acc1
	-----x--	-> collect low(acc1)
	----x---	-> collect high(acc1)
	---x----	-> collect low(acc2)
	--x-----	-> collect high(acc2)
	-x------	-> list
*/

				/* 0   1    2    3    4    5    6    7    8    9    A    B    C    D    E    F */
declare b4181(*) byte public data(0, 80h,   0,   0, 0Fh, 0Fh, 80h, 0Fh, 0Dh, 0Fh, 0Dh, 0Fh, 0Fh, 0Fh, 0Fh, 0Fh,
				0Fh, 0Dh, 0Fh, 0Fh, 0Fh, 0Fh, 0Fh, 0Fh, 0Dh, 0Dh, 40h, 4Dh,   1,   1,   1,   1,
				80h,   1,   0,   0, 47h,   7,   7,   7, 17h, 47h,   7, 47h, 37h,   5,   7,   0,
				  0,   0, 40h, 40h,   0,   1
$IF OVL4
							    , 80h, 40h, 80h,   0, 40h, 80h, 80h, 40h, 81h,0C0h,
				80h, 0Dh
$ENDIF
			   ),

	b41B7(*) byte data(41h, 0, 0, 0, 19h, 40h, 0, 1Ch, 0, 0),
		/* bit vector 66 -> 0 x 24 00011001 01000000 00000000 00011100 00000000 00 */
	b41C1(*) byte data(1Ah, 5, 80h, 0, 0C0h),
		/* bit vector 27 -> 00000101 10000000 00000000 110 */
	opCompat(*) byte data(57h, 71h, 0F4h, 57h, 76h, 66h, 66h, 67h, 77h, 77h, 77h, 55h),
		/* bit vector 88 -> 01110001 11110100 01010111 01110110
                                    01100110 01100110 01100111 01110111
				    01110111 01110111 01010101 */
	propagateFlags(*) byte data(57h, 6, 2, 20h, 0, 0, 0, 0, 0, 0, 0, 22h),
		/* bit vector 88 -> 00000110 00000010 00100000 00000000
				    00000000 00000000 00000000 00000000
				    00000000 00000000 00100010 */ 
	b41DE(*) byte data(3Ah, 0FFh, 80h, 0, 0, 0Fh, 0FEh, 0, 20h),
		/* bit vector 59 -> 11111111 10000000 00000000 00000000
				    00001111 11111110 00000000 001 */
		/* O$NONE, T$CR, T$LPAREN, T$RPAREN/O$LABEL, T$STAR, T$PLUS/K$SPECIAL, T$COMMA, */
		/* T$MINUS/K$REGNAME, T$UPLUS/K$SP */
		/* K$LXI, K$REG16, K$LDSTAX, K$ARITH, K$IMM8, K$MVI, K$INRDCR. K$MOV, K$IMM16, K$SINGLE */
		/* K$RST
/* precedence table */
/*
   10 - NULL
    9 - HIGH, LOW
    8 - *, /, MOD, SHL, SHR
    7 - +, -, UPLUS, UMINUS
    6 - =, <, <=, >, >=, <>
    5 - NOT
    4 - AND
    3 - OR, XOR,
    2 - not used
    1 - COMMA, DB - STKLEN, O$37, ENDM, EXITM, O$3D, REPT, LOCAL
    0 - all others
*/
			     /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
	precedence(*) byte data(0, 0, 0, 0, 8, 7, 1, 7, 7, 8, 7, 6, 6, 6, 6, 6,
				6, 5, 4, 3, 3, 8, 8, 8, 9, 9, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
				1, 1, 1, 1, 1, 1
$IF OVL4
	       /* for macro ver */ 		, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1,
				0, 0Ah
$ENDIF
			   );


TestBit: procedure(bitIdx, bitVector) bool public;
	declare bitIdx byte, bitVector pointer;
	declare ch based bitVector byte;

	if ch < bitIdx then
		return FALSE;
	
	bitVector = bitVector + shr(bitIdx, 3) + 1;
	return (ch and ROR(1, (bitIdx and 7) + 1)) <> 0;
end;

IsReg: procedure(leftOp) bool public;
	declare leftOp byte;

	return leftOp = K$REGNAME or leftOp = K$SP;
end;

Sub4274: procedure public;
	if TestBit(leftOp, .b41B7) then
		if IsReg(acc1ValType) then
			call OperandError;
end;

Sub4291: procedure public;
	if IsReg(acc1ValType) then
		call OperandError;
	if (b4181(leftOp) and 2) = 0 then
		acc2Flags = 0;
	else if IsReg(acc2ValType) then
		call OperandError;

	acc1ValType = O$NUMBER;
	accFixFlags(0) = (acc1Flags and UF$BOTH) <> 0;
	accFixFlags(1) = (acc2Flags and UF$BOTH) <> 0;
	if (acc1Flags and UF$SEGMASK) <> SEG$ABS then
		if (acc2Flags and UF$SEGMASK) <> SEG$ABS then
			if ((acc1Flags xor acc2Flags) and 1Fh) <> 0 then
				call ExpressionError;
	if (ii := (acc1Flags and UF$EXTRN) <> 0) or (jj := (acc2Flags and UF$EXTRN) <> 0) then
	do;
		if leftOp = K$SPECIAL then	/* +? (PAGE INPAGE)? */
			if not (ii or accFixFlags(0)) then
			do;
				acc1NumVal = acc2NumVal;
				acc1Flags = acc2Flags;
				return;
			end;
		if jj or accFixFlags(1) or not TestBit(leftOp, .b41C1) then
			goto L4394;
		else
			return;
	end;
	kk = shl(leftOp - 4, 2) or (accFixFlags(0) and 2) or (accFixFlags(1) and 1);
	if TestBit(kk, .opCompat) then
L4394:	do;
		call ExpressionError;
		acc1Flags = 0;
		return;
	end;
	if TestBit(kk, .propagateFlags) then
	do;
		if not accFixFlags(0) then
			acc1Flags = acc2Flags;
		return;
	end;
	acc1Flags = 0;
end;


SwapAccBytes: procedure public;
	declare tmp byte;
	tmp = accum1$lb;
	accum1$lb = accum1$hb;
	accum1$hb = tmp;
end;



SetExpectOperands: procedure public;
	expectingOperands = TRUE;
	expectingOpcode = FALSE;
end;



GetNumVal: procedure address public;
	declare tokByte based tokPtr (1) byte,	/* and high byte if not a register */
		val$p pointer,
		val based val$p address;
$IF OVL4
	LogError: procedure(arg1b);
		declare arg1b byte;

		if tokenType(tokenIdx) <> 40h then
		do;
			call SourceError(arg1b);
			return;
		end;
		if tokenSize(0) = 0 then
			tokenType(tokenIdx) = 41h;
	end;
$ENDIF

	acc1Flags = 0;
	accum1 = 0;
	acc1ValType = O$ID;
$IF OVL4
	if tokenType(0) = 40h then
		call PushToken(0Dh);
$ENDIF
	if tokenIdx = 0 or tokenType(0) = O$DATA and not b6B36 then
$IF OVL4
		call LogError('Q');
$ELSE
		call SyntaxError;
$ENDIF
	else
	do;
		if tokenType(0) = O$ID or tokenType(0) = T$COMMA then
$IF OVL4
			call LogError('U');
$ELSE
			call UndefinedSymbolError;
$ENDIF
		else
		do;
			acc1ValType = tokenType(0);
			if TestBit(acc1ValType, .b41DE) then
			do;
				tokPtr = curTokenSym$p + 7;	/* point to flags */
				acc1Flags = tokByte(0) and 0DFh;
				tokPtr, val$p = curTokenSym$p + 4;
				acc1NumVal = val;			/* pick up value */
				tokenSize(0) = 2;

			end;
			else if tokenSize(0) = 0 then
$IF OVL4
				call LogError('V');
$ELSE
				call ValueError;
$ENDIF
			else
			do;
				if tokenSize(0) > 2 then
$IF OVL4
					call LogError('V');
$ELSE
					call ValueError;
$ENDIF
				acc1Flags = tokenAttr(0) and 0DFh;
				acc1NumVal = tokenSymId(0);
			end;

			if tokenSize(0) > 0 then	/* get low byte */
				accum1$lb = tokByte(0);
			if tokenSize(0) > 1 then	/* and high byte if not a register */
				accum1$hb = tokByte(1) and tokenType(0) <> 7;
		end;	

		if has16bitOperand then
			if tokenSize(0) = 2 then
				if tokenType(0) = O$STRING then
					call SwapAccBytes;

		if (acc1Flags and 40h) <> 0 then
			if tokenType(0) < 9 then
				accum1 = 0;

		call PopToken;
	end;

	b6B36 = FALSE;
	return accum1;
end;


GetPrec: procedure(leftOp) byte public;
	declare leftOp byte;
	return precedence(leftOp);
end;

/*
   arg1b
   xxxx1xxx	single byte arg
   xxxxx11x	acc1 = acc1 | (acc2 << 3) 
   xxxxx01x	acc1 = acc1 | acc2
   
*/   
MkCode: procedure(arg1b) public;
	declare arg1b byte;

	if (arg1b and 3) <> 0 then
	do;
		if accum2$hb <> 0
		   or accum2$lb > 7
		   or arg1b and accum2$lb
		   or (arg1b and 3) = 3 and accum2$lb > 2
		   or (not IsReg(acc2ValType) and leftOp <> K$RST) then    /* RST */
			call OperandError;
		else if IsReg(acc2ValType) and leftOp = K$RST then	     /* RST */
			call OperandError;
		if ror(arg1b, 2) then
			accum2$lb = rol(accum2$lb, 3);
		accum1$lb = accum1$lb or accum2$lb;
	end;
	else if leftOp <> K$SINGLE then		/* single byte leftOp */
		if IsReg(acc2ValType) then
			call OperandError;

	if shr(arg1b, 3) then
	do;
		if (acc2Flags and UF$BOTH) = UF$BOTH then
		do;
			call ValueError;
			acc2Flags = acc2Flags and 0E7h or UF$LOW;
		end;
		if accum2$hb + 1 > 1 then	/* Error if not FF or 00 */
			call ValueError;
	end;
	if leftOp = K$IMM8 or leftOp = K$IMM16 then	/* Imm8 or imm16 */
	do;
		acc1Flags = acc2Flags;
		acc1NumVal = acc2NumVal;
	end;
	else
		acc1Flags = 0;

	if leftOp <> K$SINGLE then		     /* single byte leftOp */
		if accum1$lb = 76h then	     /* mov m,m is actually Halt */
			call OperandError;
	if (leftOp := shr(arg1b, 4) + 24h) = 24h then
		b6B2D = O$DATA;
end;

NxtTokI: procedure byte public;
	if tokI >= tokenIdx then
		return 0;
	return (tokI := tokI + 1);
end;



ShowLine: procedure byte public;
	return ((not isControlLine) and ctlLIST or ctlLISTChanged and isControlLine)
$IF OVL4
	        and (not (expandingMacro > 1) or ctlGEN)
$ENDIF
		and (not(b6B32 or skipping(0)) or ctlCOND);
end;

/*
	xrefMode= 0 -> defined
		= 1 -> used
		= 2 -> finalise
*/
EmitXref: procedure(xrefMode, name) public;
	declare xrefMode byte, name address;
	declare (i, byteval) byte;
	declare (srcLineLow, srcLineHigh) byte at(.srcLineCnt);

	if not IsPhase1 or not ctlXREF or IsSkipping and not b6881 then
		return;

	call Outch(xrefMode + '0');
	if xrefMode <> 2 then	/* not finalise */
	do;
		call OutStrN(name, 6);
		b6881 = FALSE;
		byteval = srcLineHigh;	/* high byte */
		i = 0;
		do while i < 4;
			i = i + 1;
			if i then	/* high nibble ? */
			do;
				if i = 3 then	/* get low byte */
					byteval = srcLineLow;
				/* emit high nibble */
				call Outch(Nibble2Ascii(shr(byteval, 4)));
			end;
			else	/* emit low nibble */
				call Outch(Nibble2Ascii(byteval));
		end;
	end;
	else	/* finalise */
	do;
		call OutStrN(.lstFile, 15);
		if ctlPAGING then
			call Outch('1');
		else
			call Outch('0');

		call Outch(Nibble2Ascii(ror(ctlPAGELENGTH, 4)));
		call Outch(Nibble2Ascii(ctlPAGELENGTH));
		call Outch(Nibble2Ascii(ror(ctlPAGEWIDTH, 4)));
		call Outch(Nibble2Ascii(ctlPAGEWIDTH));
		call Outch('3');
		call Flushout;
		call CloseF(xreffd);
	end;
end;
end;
