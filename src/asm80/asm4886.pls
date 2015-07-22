asm48$86:
do;
$IF OVL4
$include(asm48.ipx)
$ELSE
$include(asm86.ipx)
$ENDIF
declare	b5666(*) byte data(9, 2Dh, 80h), /* bit vector 10 -> 00101101 10 */
	b5669(*) byte data(3Ah, 8, 80h, 0, 0, 0, 0, 0, 20h),
		/* bit vector 59 -> 00001000 1000000 00000000 0000000
				    00000000 0000000 00000000 001 */
		       /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
	op16(*) byte data(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1,
                          0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1),
			  /* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
	chClass(*) byte data(0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 4, 0, 0,	/* 00 */
$IF OVL4
			     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,0Bh,0, 0, 0, 0,	/* 10 - ESC maps to 0Bh */
$ELSE
			     0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 ,0, 0, 0, 0,	/* 10 */
$ENDIF
			     1, 0, 0, 0, 6, 0, 0, 7, 5, 5, 5, 5, 5, 5, 0, 5,	/* 20 */
			     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 3, 2, 0, 0, 0, 9,	/* 30 */
			     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,	/* 40 */
			     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0,	/* 50 */
			     0, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,	/* 60 */
			     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 0, 0, 0, 0, 0);	/* 70 */

unpackToken: procedure(src, dst) public;
	declare (src, dst) pointer;
	declare wrd based src address;
	declare ch based dst byte;
	declare packedword address;

	unpack1: procedure;
		ch = packedword mod 40;	
		if ch = 0 then
			ch = ' ';
		else if ch <= 10 then
			ch = ch + 2Fh;	/* digit */
		else 
			ch = ch + 34h;	/* ? @ and letters */
		packedword = packedword / 40;
		dst = dst - 1;
	end;


	src = src + 2;
	dst = dst + 5;
	packedword = wrd;
	call unpack1;
	call unpack1;
	call unpack1;
	src = src - 2;
	packedword = wrd;
	call unpack1;
	call unpack1;
	call unpack1;
end;


insertSym: procedure public;
	declare (q, p) pointer;
	declare ch1 based q byte;
	declare ch2 based p byte;

	/* move up the top block of the symbol tables to make room */
	symHighMark, q = (p := symHighMark) + 8;
	if w6870 < q then
		call runtimeError(1);	/* table error */

	do while p > curTokenSym$p;	/* byte copy */
		q = q - 1;
		p = p - 1;
		ch1 = ch2;
	end;
	/* insert the new symbol name */
	call move(4, tokPtr, curTokenSym$p);
$IF OVL4
	endSymTab(2) = endSymTab(2) + 8;	/* mark new top of macro table */
$ENDIF
	curTokenSym.type = 0;			/* clear the type */
end;

$IF OVL4
sub$5C73: procedure(arg1b) byte;
	declare arg1b byte;

	if endSymTab(arg1b) >= curTokenSym$p and curTokenSym$p >= symTab(arg1b) then
		return 0;

	call syntaxError;
	return 0FFh;
end;


sub$5CAD: procedure(arg1w, type) public;
	declare arg1w address, type byte;

	if sub$5C73(2) then
		return;
	call insertSym;
	curTokenSym.val = arg1w;		/* fill in the rest of the new entry */
	curTokenSym.type = type;
	curTokenSym.flags = 0;
	call popToken;
end;
$ENDIF

declare b6EC4$9C3A byte public;

sub5819$5CE8: procedure(arg1w, type) public;
	declare arg1w address, type byte;
	declare (b6CE8, b6CE9, b6CEA, isSetOrEqu, b6CEC) byte;

	sub$5B9A: procedure;
		tokenType(0) = type;
		if (valType = K$REGNAME or valType = K$SP) and isSetOrEqu then
			tokenType(0) = 12 - type;
	end;

	b6CEC = tokenType(0);
	isSetOrEqu = type = 5 or type = 4;
	b6CE9 = 0;
	b6CE8 = curTokenSym.flags;
	b6CEA = 0;
$IF OVL4
	if sub$5C73(1) then
	do;
$ELSE
	if curTokenSym$p > endSymTab(1) or curTokenSym$p < symTab(1) then
	do;
		call syntaxError;
$ENDIF
		return;
	end;

	if tokenIdx > 1 then
		call syntaxError;

	if isPhase1 then
		if tokenType(0) = 9 then
		do;
			if b6883 then
			do;
				if curTokenSym.type >= 80h
$IF OVL4
					 or type = O$3A and curTokenSym.val <> srcLineCnt
$ENDIF
				then
				do;
					call locationError;
					b6CE9 = 80h;
				end;
			end;
			else
			do;
				call insertSym;
$IF OVL4
				symTab(2) = symTab(2) + 8;		/* adjust the base of the macro table */
$ENDIF
				endSymTab(1) = endSymTab(1) + 8;	/* adjust the end of the user symbol table */
				b6CE8 = 0;
			end;

			b6CE8 = (activeSeg <> 0 and 18h) or (inPublic and 20h) or (inExtrn and 58h);
			if b6EC4$9C3A = 1 then
				b6CE8 = b6855;

			if b6EC4$9C3A = 2 then
				b6CE8 = b6CE8 or activeSeg;

			if b6857 and isSetOrEqu then
				tokenType(0) = O$64;
			else
				call sub$5B9A;

			goto L5A9B$5F82;
		end;

	if b6882 = 2 then
		if tokenType(0) = O$ID then
			if valType <> O$ID then
				if isSetOrEqu then
				do;
					call sub$5B9A;
					if curTokenSym.type < 128 then
					do;
						curTokenSym.type = tokenType(0);
						curTokenSym.val = arg1w;
						b6CE8 = b6855;
						b6CEA = 0FFh;
					end;
					goto L5A9B$5F82;
				end;

	if isPhase1 then
		if tokenType(0) = 6 then
			if testBit(type, .b5666) then
			do;
				if inExtrn then
					tokenType(0) = 3;
				else
				do;
					tokenType(0) = type;
					b6CE8 = b6CE8 and 0E0h;
					if b6EC4$9C3A = 1 then
						b6CE8 = b6855 or 20h;

					if b6EC4$9C3A = 2 then
						if activeSeg <> 0 then
							b6CE8 = b6CE8 or activeSeg or 38h;
				end;
				goto L5A9B$5F82;
			end;

	if isPhase1 then
		if type = 6 then
			if testBit(tokenType(0), .b5666) then
			do;
				if (b6CE8 and 60h) <> 0 then
					tokenType(0) = 3;
				else
					b6CE8 = b6CE8 or 20h;
				goto L5A9B$5F82;
			end;

	if isPhase1 then
		if tokenType(0) <> type and tokenType(0) <> 8 or type = 5 then
			tokenType(0) = 3;

	if not inPublic and testBit(tokenType(0), .b5669) then
		b6CE8 = (b6CE8 and 20h) and tokenType(0) <> O$3A or b6855;
	else
	do;
		if isPhase1 then
			tokenType(0) = 3;

		if not (inPublic or inExtrn) then
			if curTokenSym.val <> arg1w then
				call phaseError;
	end;

L5A9B$5F82:
	b6CE9 = b6CE9 or (curTokenSym.type and 80h);

	if isPhase1 and (type = 9 or type = 6 or b6CEC <> tokenType(0)) then
		curTokenSym.type = tokenType(0) or b6CE9;

	jj = curTokenSym.type;
	if tokenType(0) = 3 or jj = 3 then
		call multipleDefError;

	if jj >= 80h then
		call locationError;

	if isPhase1 and (tokenType(0) = type or type = 5 and tokenType(0) = 7)
	   or type = 4 and blankAsmErrCode or b6CEA
$IF OVL4
	   or type = O$3A
$ENDIF
	then
		curTokenSym.val = arg1w;

	curTokenSym.flags = b6CE8;
	inPublic = 0;
	inExtrn = 0;
	if curTokenSym.type = 6 then
		call undefinedSymbolError;

	b6857 = 0;
	if b6883 then
		call popToken;

end;



lookup: procedure(tableId) byte public;
	declare tableId byte;
	declare (lowOffset, highOffset, midOffset, deltaToNext) address,
		(entryOffset, packedTok$p) pointer,
		(i, gt) byte;
	declare symEntry based entryOffset KEYWORD$T,
		packedTok based packedTok$p (2) address;
	declare addr based w6BE0 address;

	packedTok$p = tokPtr;
	if tableId = 0 then		/* hash chain look up key word */
	do;
		entryOffset = 0;	/* offset to current symbol to compare */
					/* offset of first to use - hashes packed symbol name */
		deltaToNext = symTab(0) + ((packedTok(0) + packedTok(1)) mod 151) * 8;

		do while deltaToNext <> 0;	/* while not end of chain */
			entryOffset = entryOffset + deltaToNext;	/* point to the next in chain */
			if symEntry.tok(0) = packedTok(0) then		/* check for exact match */
				if symEntry.tok(1) = packedTok(1) then
				do;
					curTokenSym$p = entryOffset;
					tokenType(0) = curTokenSym.type;
					if tokenType(0) < 2Dh then	/* instruction with arg */
						if op16(tokenType(0)) then
							has16bitOperand = TRUE;

					if curTokenSym.flags = 2 and not ctlMOD85 then	/* RIM/SIM only valid on 8085 */
						call sourceError('O');

					if tokenType(0) = 8 then		/* SP */
					do;
						if not(opType = 24h or opType = 25h) then /* lxi or push/pop/dad/inx/dcx */
							call sourceError('X');
						tokenType(0) = 7;		/* reg */
					end;
					return tokenType(0) and 7Fh;
				end;
			deltaToNext = symEntry.delta;
		end;
		return 9;
	end;

	lowOffset = symTab(tableId);
	highOffset, entryOffset = endSymTab(tableId);

	/* binary chop search for id */

	do while (midOffset := lowOffset + shr((highOffset - lowOffset) and 0FFF0h, 1)) <> entryOffset;
		entryOffset = midOffset;
		if packedTok(0) = symEntry.tok(0) then
		do;
			if packedTok(1) = symEntry.tok(1) then
			do;
				curTokenSym$p = entryOffset;
				tokenType(0) = curTokenSym.type;
				if tokenType(0) = O$64 then
					tokenType(0) = O$ID;

				if (b6884 := (jj := (tokenType(0) and 7Fh)) = O$ID) then
					if b6885 then
						curTokenSym.type = 89h;
				return jj;
			end;
			else
				gt = symEntry.tok(1) > packedTok(1);
		end;
		else
			gt = symEntry.tok(0) > packedTok(0);

		entryOffset = midOffset;
		if gt then
			highOffset = entryOffset;
		else
			lowOffset = entryOffset;
	end;

	curTokenSym$p = highOffset;
	if tableId = 1 and not isSkipping then
	do;
		b6883 = 0;
		b6EC4$9C3A = 0;
		call sub5819$5CE8(srcLineCnt, (b6885 and 80h) or 9);
		w6BE0 = .tokenSym;
		do i = 1 to tokenIdx;
			w6BE0 = w6BE0 + 2;
			if addr >= curTokenSym$p then
				addr = addr + 8;
		end;

		b6883 = 0FFh;
	end;
	return O$ID;
end;



getCh: procedure byte public;
	declare (curCH, prevCH) byte;
$IF OVL4
	declare ch based tmac$buf$p byte;
	declare ch1 based macro$p byte;
$ENDIF

L6339:
	if not reget then
	do;
		prevCH = curCH;

	L6347:
		curCH = lookAhead;
$IF OVL4
		if expandingMacro then
		do;
			do while (lookAhead := ch) = 0FEh;
				call readM(curMacroBlk + 1);
				tmac$buf$p = .macroBuf;
			end;

			tmac$buf$p = tmac$buf$p + 1;
		end;
		else
$ENDIF
	        if scanCmdLine then
			lookAhead = getCmdCh;
		else
			lookAhead = getSrcCh;

		if chClass(curCH) = CC$BAD then
			if curCH = 0 or curCH = 7Fh or curCH = FF then
				goto L6347;
$IF OVL4
		if expandingMacro then
		do;
			if curCH = 1Bh then
			do;
				goto L65B2;
			end;
			else if curCH = '&' then
			do;
				if not prevCH < 80h or lookAhead = 80h then
					goto L6339;
			end;
			else if curCH = '!' and prevCH <> 0 then
			do;
				if not (b905D or b905E) and b905C then
				do;
					curCH = 0;
					goto L6339;
				end;
			end;
			else if curCH >= 128 then
			do;
				if not (b905C := not b905C) then
					tmac$buf$p = w9197;
				else
				do;
					w9197 = tmac$buf$p;
					if curCH = 80h then
					do;
						tmac$buf$p = tmac$w12;
						if b9062 = 2 then
						do;
							b91A2 = ch;
							tmac$buf$p = .b91A2;
							if ch = '!' then
							do;
								b91A1 = 21h;
								b91A2 = byteAt(tmac$w12 + 1);
								tmac$buf$p = tmac$buf$p - 1;
							end;
						end;
						else
						do;
							do while (lookAhead := lookAhead - 1) <> 0FFh;
								tmac$buf$p = tmac$buf$p - (ch and 7Fh);
							end;

							tmac$buf$p = tmac$buf$p + 1;
						end;
					end;
					else
					do;
						tmac$buf$p = .b91A4;
						w6BE0 = lookAhead + tmac$w4;
						do ii = 1 to 4;
							b91A4(6 - ii) = w6BE0 mod 10 + '0';
							w6BE0 = w6BE0 / 10;
						end;
					end;
				end;

				lookAhead = 0;
				goto L6339;
			end;
		end;

		if expandingMacro > 1 then
			if isPhase2Print then
				if macro$p < .endMacroLine then	/* append character */
				do;
					ch1 = curCH;	
					macro$p = macro$p + 1;
				end;

		if b905E then
			if w919D <> w906A and curCH = 0Dh or not b9059 then
			call sub$3D55(curCH);

		if not(prevCH = '!' or b6742) then
		do;
			if curCH = '>' then
				argNestCnt = argNestCnt - 1;

			if curCH = '<' then
				argNestCnt = argNestCnt + 1;
		end;
$ENDIF
	end;
L65B2:
	reget = 0;
	return (curChar := curCH);
end;

getChClass: procedure byte public;
	curChar = getCh;
$IF OVL4
	if b905D then
		return 0Ah;
$ENDIF

	return chClass(curChar);
end;



chkLF: procedure public;
	if lookAhead = LF then
		lookAhead = 0;
	else
	do;
$IF OVL4 
		b905E = b905E and 0FEh;
$ENDIF
		call illegalCharError;
$IF OVL4
		b905E = b905E > 0;
$ENDIF
	end;
end;

end;

