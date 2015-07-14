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
	b41C1(*) byte data(1Ah, 5, 80h, 0, 0C0h),
	b41C6(*) byte data(57h, 71h, 0F4h, 57h, 76h, 66h, 66h, 67h, 77h, 77h, 77h, 55h),
	b41D2(*) byte data(57h, 6, 2, 20h, 0, 0, 0, 0, 0, 0, 0, 22h),
	b41DE(*) byte data(3Ah, 0FFh, 80h, 0, 0, 0Fh, 0FEh, 0, 20h),
	precedence(*) byte data(0, 0, 0, 0, 8, 7, 1, 7, 7, 8, 7, 6, 6, 6, 6, 6, 6,
			   5, 4, 3, 3, 8, 8, 8, 9, 9, 1, 1, 1, 1, 1, 1, 1, 1,
			   1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
			   1, 1, 1
$IF OVL4
	       /* extra */ ,0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 0, 0Ah
$ENDIF
			   );


testBit: procedure(arg1b, arg2w) byte public;
	declare arg1b byte, arg2w address;
	declare ch based arg2w byte;

	if ch < arg1b then
		return 0;
	
	arg2w = arg2w + shr(arg1b, 3) + 1;
	return (ch and ROR(1, (arg1b and 7) + 1)) <> 0;
end;

sub$425B: procedure(arg1b) byte public;
	declare arg1b byte;

	return arg1b = 7 or arg1b = 8;
end;

sub$4274: procedure public;
	if testBit(b6B2B, .b41B7) then
		if sub$425B(b6858) then
			call operandError;
end;

sub$4291: procedure public;
	if sub$425B(b6858) then
		call operandError;
	if (b4181(b6B2B) and 2) = 0 then
		b6856 = 0;
	else if sub$425B(b6859) then
		call operandError;

	b6858 = 0Ch;
	bp6BE0(0) = (b6855 and 18h) <> 0;
	bp6BE0(1) = (b6856 and 18h) <> 0;
	if (b6855 and 7) <> 0 then
		if (b6856 and 7) <> 0 then
			if ((b6855 xor b6856) and 1Fh) <> 0 then
				call expressionError;
	if (ii := (b6855 and 40h) <> 0) or (b6BDC := (b6856 and 40h) <> 0) then
	do;
		if b6B2B = 5 then	/* +? (PAGE INPAGE)? */
			if not (ii or bp6BE0(0)) then
			do;
				w685A = w685C;
				b6855 = b6856;
				return;
			end;
		if b6BDC or bp6BE0(1) or not testBit(b6B2B, .b41C1) then
			goto L4394;
		else
			return;
	end;
	jj = shl(b6B2B - 4, 2) or (bp6BE0(0) and 2) or (bp6BE0(1) and 1);
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
	declare b6C17 byte;
	b6C17 = accum1$lb;
	accum1$lb = accum1$hb;
	accum1$hb = b6C17;
end;



sub$43D2: procedure public;
	b6B30 = 0FFh;
	b6B31 = 0;
end;



sub$43DD: procedure address public;
	declare s based curTokStart(1) byte,
		wrd$p address,
		wrd based wrd$p address;
$IF OVL4
	logError: procedure(arg1b);
		declare arg1b byte;

		if tokenType(tokenSP) <> 40h then
		do;
			call sourceError(arg1b);
			return;
		end;
		if tokenSize(0) = 0 then
			tokenType(tokenSP) = 41h;
	end;
$ENDIF

	b6855 = 0;
	accum1 = 0;
	b6858 = 9;
$IF OVL4
	if tokenType(0) = 40h then
		call pushToken(0Dh);
$ENDIF
	if tokenSP = 0 or tokenType(0) = 11 and not b6B36 then
$IF OVL4
		call logError('Q');
$ELSE
		call syntaxError;
$ENDIF
	else
	do;
		if tokenType(0) = 9 or tokenType(0) = 6 then
$IF OVL4
			call logError('U');
$ELSE
			call undefinedSymbolError;
$ENDIF
		else
		do;
			b6858 = tokenType(0);
			if testBit(b6858, .b41DE) then
			do;
				curTokStart = curTokenSym$p + 7;
				b6855 = s(0) and 0DFh;
				curTokStart, wrd$p = curTokenSym$p + 4;
				w685A = wrd;
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

			if tokenSize(0) > 0 then
				accum1$lb = s(0);
			if tokenSize(0) > 1 then
				accum1$hb = s(1) and tokenType(0) <> 7;
		end;	

		if has16bitOperand then
			if tokenSize(0) = 2 then
				if tokenType(0) = 10 then
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
		   or (not sub$425B(b6859) and b6B2B <> 2Eh) then    /* RST */
			call operandError;
		else if sub$425B(b6859) and b6B2B = 2Eh then	     /* RST */
			call operandError;
		if ror(arg1b, 2) then
			accum2$lb = rol(accum2$lb, 3);
		accum1$lb = accum1$lb or accum2$lb;
	end;
	else if b6B2B <> 2Dh then		/* single byte op */
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
	if b6B2B = 28h or b6B2B = 2Ch then	/* Imm8 or imm16 */
	do;
		b6855 = b6856;
		w685A = w685C;
	end;
	else
		b6855 = 0;

	if b6B2B <> 2Dh then		     /* single byte op */
		if accum1$lb = 76h then
			call operandError;
	if (b6B2B := shr(arg1b, 4) + 24h) = 24h then	/* LXI */
		b6B2D = 0Bh;
end;

sub$4646: procedure byte public;
	if b689A >= tokenSP then
		return 0;
	return (b689A := b689A + 1);
end;



sub$465B: procedure byte public;
	return ((not b6B20$9A77) and ctlLIST or b6A6F and b6B20$9A77)
$IF OVL4
	        and (not (b$905B > 1) or ctlGEN)
$ENDIF
		and (not(b6B32 or skipping(0)) or ctlCOND);
end;

sub$467F: procedure(arg1b, arg2w) public;
	declare arg1b byte, arg2w address;
	declare (b6C1F, b6C20) byte;

	if not isPhase1 or not ctlXREF or isSkipping and not b6881 then
		return;

	call outch(arg1b + '0');
	if arg1b <> 2 then
	do;
		call outStrN(arg2w, 6);
		b6881 = 0;
		b6C20 = bp6A4E(1);
		b6C1F = 0;
		do while b6C1F < 4;
			b6C1F = b6C1F + 1;
			if b6C1F then
			do;
				if b6C1F = 3 then
					b6C20 = bp6A4E(0);
				call outch(nibble2Ascii(shr(b6C20, 4)));
			end;
			else
				call outch(nibble2Ascii(b6C20));
		end;
	end;
	else
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
