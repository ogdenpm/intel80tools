asm43$82:
do;
$IF OVL4
$include(asm43.ipx)
$ELSE
$include(asm82.ipx)
$ENDIF

			/* 0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F */
declare b3E5E(*) byte data(0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0,
			   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			   0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
			   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0,
			   0, 0),
	b3EA0(*) byte data(36h, 0, 0, 0, 6, 0, 0, 2),
		/* bit vector 55 -> 0 x 24 00000110 0 x 16 0000001 */
		/* 29, 30, 55 */
			/* 0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F */
	b3EA8(*) bool data(0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		 	   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0FFh,0,   0,   0FFh,
			   0,   0FFh,0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
			   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0FFh,0FFh,0FFh,0,   0FFh,0,
			   0,   0);
	/* true for DS, ORG, IF, 3A?, IRP, IRPC REPT */ 
$IF OVL4
declare	b$3F88(*) byte data(41h, 90h, 0, 0, 0, 0, 0, 0, 0, 40h);
	/* bit vector 66 -> 10010000 0 x 56 01 */

skipWhite$2: procedure public;
	do while getCh = ' ' or isTab;
	end;
end;


sub$3FA9: procedure byte public;
	declare w9B5A pointer,
		wrd based w9B5A address;

	w9B5A = curTokenSym$p - 6;
	return (wrd < 4679h) or ctlMACRODEBUG or (4682h < wrd);
end;



seekM: procedure(blk);
	declare blk address;

	if (w6BE0 := blk - nxtMacroBlk) <> 0 then
	do;
		kk = SEEKFWD;
		if blk < nxtMacroBlk then
		do;
			w6BE0 = - w6BE0;
			kk = SEEKBACK;
		end;

		call seek(macrofd, kk, .w6BE0, .w$3780, .statusIO);
		call ioErrChk;
	end;
	nxtMacroBlk = blk + 1;
end;



readM: procedure(blk) public;
	declare blk address;
	declare actual address;

	if blk >= maxMacroBlk then
		actual = 0;
	else if blk = curMacroBlk then
		return;
	else
	do;
		call seekM(blk);
		call read(macrofd, .macroBuf, 128, .actual, .statusIO);
		call ioErrChk;
	end;

	tmac$blk, curMacroBlk = blk;
	macroBuf(actual) = 0FEh;	/* flag end of macro buffer */
end;


writeM: procedure public;
	if phase = 1 then
	do;
		call seekM(maxMacroBlk);
		maxMacroBlk = maxMacroBlk + 1;
		call write(macrofd, symHighMark, 128, .statusIO);
		call ioErrChk;
	end;
	macroBlkCnt = macroBlkCnt + 1;
end;



sub$40B9: procedure public;
	declare w9B62 address;

	if b905E then
	do;
		do while (w9B62 := w906A - symHighMark) >= 128;
			call writeM;
			symHighMark = symHighMark + 128;
		end;
		if w9B62 <> 0 then
			call move(w9B62, symHighMark, endSymTab(2));
		w906A = (symHighMark := endSymTab(2)) + w9B62;
	end;
end;

$ENDIF

skipWhite: procedure public;
	do while isWhite;
		curChar = getCh;
	end;
end;


$IF BASE
skipWhite$2: procedure public;
	do while getCh = ' ' or isTab;
	end;
end;
$ENDIF

skip2NextLine: procedure public;
	call skip2EOL;
	call chkLF;
end;



tokeniseLine: procedure public;

	sub$416B: procedure;
		if opType = 0 then
			call expressionError;
		inExpression = 0;
		opType = 0;
	end;


    do while 1;
	if atStartLine then
	do;
		call parseControlLines;
		atStartLine = 0;
	end;

	do case getChClass;
        case0:	call illegalCharError;		/* CC$BAD */
		;				/* CC$WS */
		do;				/* CC$SEMI */
$IF OVL4
			if not b9058 then
$ENDIF
			do;
				b6742 = TRUE;
$IF OVL4
				if getChClass = CC$SEMI and b905E then
				do;
					b9059 = TRUE;
					w906A = w906A - 2;
				end;
$ENDIF
				call skip2NextLine;
				effectiveToken = T$CR;
				return;
			end;
		end;
		do;				/* CC$COLON */
			if not gotLabel then
			do;
				if skipping(0)
$IF OVL4
				   or b905E
$ENDIF
				then
					call popToken;
				else
				do;
					b6EC4$9C3A = 2;
					call sub5819$5CE8(segSize(activeSeg), 2);
				end;

				expectingOperands = FALSE;
				gotLabel, expectingOpcode = bTRUE;
			end;
			else
			do;
				call syntaxError;
				call popToken;
			end;

			call emitXref(0, .name);
			haveUserSymbol = FALSE;
			opType = O$LABEL;
		end;
		do;				/* CC$CR */
			call chkLF;
			effectiveToken = T$CR;
$IF OVL4
			b9058 = 0;
$ENDIF
			return;
		end;
		do;				/* CC$PUNCT */
			if curChar = '+' or curChar = '-' then
$IF OVL4
				if not testBit(opType, .b$3F88) then /* not 0, 3 or 41h */
$ELSE
				if opType <> O$NONE and opType <> T$RPAREN then
$ENDIF
					curChar = curChar + (T$UPLUS - T$PLUS);	/* make unary versions */
			effectiveToken = curChar - '(' + T$LPAREN;
			return;
		end;
		do;				/* CC$DOLLAR */
			call pushToken(O$NUMBER);
			call collectByte(low(segSize(activeSeg)));
			call collectByte(high(segSize(activeSeg)));
			if activeSeg <> SEG$ABS then
				tokenAttr(0) = tokenAttr(0) or activeSeg or 18h;
			call sub$416B;
		end;
		do;				/* CC$QUOTE */
$IF OVL4
			if effectiveToken = 37h then
			do;
				call illegalCharError;
				return;
			end;
			if b905E then
				b9058 = not b9058;
			else
$ENDIF
			do;
				call getStr;
				if expectingOpcode then
					call setExpectOperands;
				call sub$416B;
			end;
		end;
		do;				/* CC$DIGIT */
			call atoi;
			if expectingOpcode then
				call setExpectOperands;
			call sub$416B;
		end;
		do;				/* CC$LET */
$IF OVL4
			w919F = w906A - 1;
$ENDIF
			call getId(O$ID);	/* assume it's an id */
			if tokenSize(0) > 6 then	/* cap length */
				tokenSize(0) = 6;

			if ctlXREF then
			do;
				call move(6, .name, .savName);
				call move(6, .spaces6, .name);
			end;
			/* copy the token to name */
			call move(tokenSize(0), tokPtr, .name);
			nameLen = tokenSize(0);
			call packToken;		/* make into 4 byte name */
			if haveUserSymbol then
			do;
				b687F = TRUE;
				haveUserSymbol = FALSE;
			end;


$IF OVL4
			if lookup(2) <> O$ID and b905E then
			do;
				if not b9058 or (kk := tokenType(0) = 0) and (curChar = '&' or byteAt(w919F-1) = '&') then
				do;
					w906A = w919F;
					call sub$3D55(kk + 81h);
					call sub$3D34(getNumVal);
					call sub$3D55(curChar);
					effectiveToken = O$ID;
				end;
			end;
			else if effectiveToken <> O$37 and not b905E = 2 then
$ENDIF
			do;
				if lookup(0) = O$ID then		/* not a key word */
				do;
					tokenType(0) = lookup(1);	/* look up in symbol space */
					haveUserSymbol = TRUE;		/* not a key word */
				end;

				effectiveToken = tokenType(0);
				b6885 = b3EA8(tokenType(0));		/* DS, ORG, IF, O$3A, IRP, IRPC REPT */
				if not b3E5E(tokenType(0)) then		/* i.e. not instruction, reg or O$37 or 1,2,3,4,6,A */
					call popToken;

				if b687F then
				do;			   /* EQU, SET or O$37 */
					call emitXref((not testBit(effectiveToken, .b3EA0)) and 1, .savName);
					b687F = FALSE;
				end;
			end;
$IF OVL4
			if b905E = 1 then
			do;
				if effectiveToken = K$LOCAL then
				do;
					b905E = 2;
					if b6897 then
						call syntaxError;
					b6897 = FALSE;
				end;
				else
				do;
					b6897 = FALSE;
					b905E = 0FFh;
				end;
			end;

			if effectiveToken = K$NUL then
				call pushToken(40h);
$ENDIF
			if effectiveToken < 10 or effectiveToken = 9 or 80h then /* !! only first term contributes */
			do;
				call sub$416B;
				if expectingOpcode then
					call setExpectOperands;
			end;
			else
			do;
				expectingOpcode = FALSE;
				return;
			end;
		end;
$IF OVL4
		do;				/* 10? */
			b6BDA = FALSE;
			call sub$73AD;
			if b6BDA then
				return;
		end;
		do;				/* CC$ESC */
			if expandingMacro then
			do;
				skipping(0) = FALSE;
				effectiveToken = 40h;
				return;
			end;
			else
				goto case0;
		end;
$ENDIF
	end;
    end;
end;

end;
