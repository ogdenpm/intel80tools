as4483: do;

/* force the non standard code generation of outStrN and put2Hex */

outStrN: procedure(s, n) external; declare s address, n address; end;
$IF OVL4
put2Hex: procedure(arg1w, arg2w) external; declare arg1w address, arg2w address; end;
$include(asm44.ipx)
$ELSE
$include(asm83.ipx)
$ENDIF
declare b4181(*) byte public data(0, 80h, 0, 0, 0Fh, 0Fh, 80h, 0Fh, 0Dh, 0Fh,
			   0Dh, 0Fh, 0Fh, 0Fh, 0Fh, 0Fh, 0Fh, 0Dh, 0Fh, 0Fh,
			   0Fh, 0Fh, 0Fh, 0Fh, 0Dh, 0Dh, 40h, 4Dh, 1, 1,
			   1, 1, 80h, 1, 0, 0, 47h, 7, 7, 7,
			   17h, 47h, 7, 47h, 37h, 5, 7, 0, 0, 0,
			   40h, 40h, 0, 1
$IF OVL4
			   ,80h, 40h, 80h, 0, 40h, 80h, 80h, 40h, 81h, 0C0h, 80h, 0Dh
$ENDIF
			   ),

	b41B7(*) byte data(41h, 0, 0, 0, 19h, 40h, 0, 1Ch, 0, 0),
		/* bit vector 66 -> 0 x 24 00011001 01000000 00000000 00011100 00000000 00 */
	b41C1(*) byte data(1Ah, 5, 80h, 0, 0C0h),
		/* bit vector 27 -> 00000101 10000000 00000000 110 */
	b41C6(*) byte data(57h, 71h, 0F4h, 57h, 76h, 66h, 66h, 67h, 77h, 77h, 77h, 55h),
		/* bit vector 88 -> 01110001 11110100 01010111 01110110
                                    01100110 01100110 01100111 01110111
				    01110111 01110111 01010101 */
	b41D2(*) byte data(57h, 6, 2, 20h, 0, 0, 0, 0, 0, 0, 0, 22h),
		/* bit vector 88 -> 00000110 00000010 00100000 00000000
				    00000000 00000000 00000000 00000000
				    00000000 00000000 00100010 */ 
	b41DE(*) byte data(3Ah, 0FFh, 80h, 0, 0, 0Fh, 0FEh, 0, 20h),
		/* bit vector 59 -> 11111111 10000000 00000000 00000000
				    00001111 11111110 00000000 001 */
	precedence(*) byte data(0, 0, 0, 0, 8, 7, 1, 7, 7, 8, 7, 6, 6, 6, 6, 6, 6,
			   5, 4, 3, 3, 8, 8, 8, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1,
			   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			   1, 1, 1
$IF OVL4
	       /* extra */ ,0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0Ah
$ENDIF
			   );


testBit: procedure(bitIdx, bitVector) byte public;
	declare bitIdx byte, bitVector address;
	declare ch based bitVector byte;

	if ch < bitIdx then
		return 0;
	
	bitVector = bitVector + shr(bitIdx, 3) + 1;
	return (ch and ROR(1, (bitIdx and 7) + 1)) <> 0;
end;

sub$425B: procedure(arg1b) byte public;
	declare arg1b byte;

	return arg1b = 7 or arg1b = 8;
end;

sub$4274: procedure public;
	if testBit(op, .b41B7) then
		if sub$425B(valType) then
			call operandError;
end;

sub$4291: procedure public;
	if sub$425B(valType) then
		call operandError;
	if (b4181(op) and 2) = 0 then
		b6856 = 0;
	else if sub$425B(b6859) then
		call operandError;

	valType = 0Ch;
	bp6BE0(0) = (b6855 and 18h) <> 0;
	bp6BE0(1) = (b6856 and 18h) <> 0;
	if (b6855 and 7) <> 0 then
		if (b6856 and 7) <> 0 then
			if ((b6855 xor b6856) and 1Fh) <> 0 then
				call expressionError;
	if (ii := (b6855 and 40h) <> 0) or (b6BDC := (b6856 and 40h) <> 0) then
	do;
		if op = 5 then	/* +? (PAGE INPAGE)? */
			if not (ii or bp6BE0(0)) then
			do;
				w685A = w685C;
				b6855 = b6856;
				return;
			end;
		if b6BDC or bp6BE0(1) or not testBit(op, .b41C1) then
			goto L4394;
		else
			return;
	end;
	jj = shl(op - 4, 2) or (bp6BE0(0) and 2) or (bp6BE0(1) and 1);
	if testBit(jj, .b41C6) then
L4394:	do;
		call expressionError;
		b6855 = 0;
		return;
	end;
	if testBit(jj, .b41D2) then
	do;
		if not bp6BE0(0) then
			b6855 = b6856;
		return;
	end;
	b6855 = 0;
end;


swapAccBytes: procedure public;
	declare tmp byte;
	tmp = accum1$lb;
	accum1$lb = accum1$hb;
	accum1$hb = tmp;
end;



setExpectOperands: procedure public;
	expectingOperands = 0FFh;
	expectingOpcode = 0;
end;



sub$43DD: procedure address public;
	declare tokByte based tokPtr (1) byte,	/* and high byte if not a register */
		val$p address,
		val based val$p address;
$IF OVL4
	logError: procedure(arg1b);
		declare arg1b byte;

		if tokenType(tokenIdx) <> 40h then
		do;
			call sourceError(arg1b);
			return;
		end;
		if tokenSize(0) = 0 then
			tokenType(tokenIdx) = 41h;
	end;
$ENDIF

	b6855 = 0;
	accum1 = 0;
	valType = O$ID;
$IF OVL4
	if tokenType(0) = 40h then
		call pushToken(0Dh);
$ENDIF
	if tokenIdx = 0 or tokenType(0) = O$DATA and not b6B36 then
$IF OVL4
		call logError('Q');
$ELSE
		call syntaxError;
$ENDIF
	else
	do;
		if tokenType(0) = O$ID or tokenType(0) = T$COMMA then
$IF OVL4
			call logError('U');
$ELSE
			call undefinedSymbolError;
$ENDIF
		else
		do;
			valType = tokenType(0);
			if testBit(valType, .b41DE) then
			do;
				tokPtr = curTokenSym$p + 7;	/* point to flags */
				b6855 = tokByte(0) and 0DFh;
				tokPtr, val$p = curTokenSym$p + 4;
				w685A = val;			/* pick up value */
				tokenSize(0) = 2;

			end;
			else if tokenSize(0) = 0 then
$IF OVL4
				call logError('V');
$ELSE
				call valueError;
$ENDIF
			else
			do;
				if tokenSize(0) > 2 then
$IF OVL4
					call logError('V');
$ELSE
					call valueError;
$ENDIF
				b6855 = tokenAttr(0) and 0DFh;
				w685A = tokenSymId(0);
			end;

			if tokenSize(0) > 0 then	/* get low byte */
				accum1$lb = tokByte(0);
			if tokenSize(0) > 1 then	/* and high byte if not a register */
				accum1$hb = tokByte(1) and tokenType(0) <> 7;
		end;	

		if has16bitOperand then
			if tokenSize(0) = 2 then
				if tokenType(0) = O$STRING then
					call swapAccBytes;

		if (b6855 and 40h) <> 0 then
			if tokenType(0) < 9 then
				accum1 = 0;

		call popToken;
	end;

	b6B36 = 0;
	return accum1;
end;


getPrec: procedure(arg1b) byte public;
	declare arg1b byte;
	return precedence(arg1b);
end;

sub$450F: procedure(arg1b) public;
	declare arg1b byte;

	if (arg1b and 3) <> 0 then
	do;
		if accum2$hb <> 0
		   or accum2$lb > 7
		   or arg1b and accum2$lb
		   or (arg1b and 3) = 3 and accum2$lb > 2
		   or (not sub$425B(b6859) and op <> K$RST) then    /* RST */
			call operandError;
		else if sub$425B(b6859) and op = K$RST then	     /* RST */
			call operandError;
		if ror(arg1b, 2) then
			accum2$lb = rol(accum2$lb, 3);
		accum1$lb = accum1$lb or accum2$lb;
	end;
	else if op <> K$SINGLE then		/* single byte op */
		if sub$425B(b6859) then
			call operandError;

	if shr(arg1b, 3) then
	do;
		if (b6856 and 18h) = 18h then
		do;
			call valueError;
			b6856 = b6856 and 0E7h or 8;
		end;
		if accum2$hb + 1 > 1 then
			call valueError;
	end;
	if op = K$IMM8 or op = K$IMM16 then	/* Imm8 or imm16 */
	do;
		b6855 = b6856;
		w685A = w685C;
	end;
	else
		b6855 = 0;

	if op <> K$SINGLE then		     /* single byte op */
		if accum1$lb = 76h then	     /* mov m,m is actually Halt */
			call operandError;
	if (op := shr(arg1b, 4) + 24h) = 24h then	/* LXI */
		b6B2D = 0Bh;
end;

nxtTokI: procedure byte public;
	if tokI >= tokenIdx then
		return 0;
	return (tokI := tokI + 1);
end;



showLine: procedure byte public;
	return ((not isControlLine) and ctlLIST or b6A6F and isControlLine)
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
emitXref: procedure(xrefMode, name) public;
	declare xrefMode byte, name address;
	declare (i, byteval) byte;
	declare (srcLineLow, srcLineHigh) byte at(.srcLineCnt);

	if not isPhase1 or not ctlXREF or isSkipping and not b6881 then
		return;

	call outch(xrefMode + '0');
	if xrefMode <> 2 then	/* not finalise */
	do;
		call outStrN(name, 6);
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
				call outch(nibble2Ascii(shr(byteval, 4)));
			end;
			else	/* emit low nibble */
				call outch(nibble2Ascii(byteval));
		end;
	end;
	else	/* finalise */
	do;
		call outStrN(.lstFile, 15);
		if ctlPAGING then
			call outch('1');
		else
			call outch('0');

		call outch(nibble2Ascii(ror(ctlPAGELENGTH, 4)));
		call outch(nibble2Ascii(ctlPAGELENGTH));
		call outch(nibble2Ascii(ror(ctlPAGEWIDTH, 4)));
		call outch(nibble2Ascii(ctlPAGEWIDTH));
		call outch('3');
		call flushout;
		call closeF(xreffd);
	end;
end;
end;
