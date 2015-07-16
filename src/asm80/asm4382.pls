asm43$82:
do;
$IF OVL4
$include(asm43.ipx)
$ELSE
$include(asm82.ipx)
$ENDIF

declare b3E5E(*) byte data(0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0,
			   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			   0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0,
			   0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0),
	b3EA0(*) byte data(36h, 0, 0, 0, 6, 0, 0, 2),
		/* bit vector 55 -> 0 x 24 00000110 0 x 16 0000001 */
		/* 29, 30, 55 */
	b3EA8(*) byte data(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0FFh, 0, 0, 0FFh,
			   0, 0FFh, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0FFh, 0FFh, 0FFh, 0,
			   0FFh, 0, 0, 0);
$IF OVL4
declare	b$3F88(*) byte data(41h, 90h, 0, 0, 0, 0, 0, 0, 0, 40h);
	/* bit vector 66 -> 10010000 0 x 56 01 */

skipWhite$2: procedure public;
	do while getCh = ' ' or isTab;
	end;
end;


sub$3FA9: procedure byte public;
	declare w9B5A address,
		wrd based w9B5A address;

	w9B5A = curTokenSym$p - 6;
	return (wrd < 4679h) or ctlMACRODEBUG or (4682h < wrd);
end;



seekM: procedure(arg1w);
	declare arg1w address;

	if (w6BE0 := arg1w - nxtMacroBlk) <> 0 then
	do;
		jj = SEEKFWD;
		if arg1w < nxtMacroBlk then
		do;
			w6BE0 = - w6BE0;
			jj = SEEKBACK;
		end;

		call seek(macrofd, jj, .w6BE0, .w$3780, .statusIO);
		call ioErrChk;
	end;
	nxtMacroBlk = arg1w + 1;
end;



readM: procedure(arg1w) public;
	declare arg1w address;
	declare w9B60 address;

	if arg1w >= maxMacroBlk then
		w9B60 = 0;
	else if arg1w = curMacroBlk then
		return;
	else
	do;
		call seekM(arg1w);
		call read(macrofd, .macroBuf, 128, .w9B60, .statusIO);
		call ioErrChk;
	end;

	tmac$blk, curMacroBlk = arg1w;
	macroBuf(w9B60) = 0FEh;	/* flag end of macro buffer */
end;


writeM: procedure public;
	if phase = 1 then
	do;
		call seekM(maxMacroBlk);
		maxMacroBlk = maxMacroBlk + 1;
		call write(macrofd, symHighMark, 128, .statusIO);
		call ioErrChk;
	end;
	w9114 = w9114 + 1;
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



sub$3F19: procedure public;

	sub$416B: procedure;
		if opType = 0 then
			call expressionError;
		b6B25 = 0;
		opType = 0;
	end;


    do while 1;
	if b689C then
	do;
		call preStatementControls;
		b689C = 0;
	end;

	do case getChClass;
        case0:	call illegalCharError;		/* CC$BAD */
		;				/* CC$WS */
		do;				/* CC$SEMI */
$IF OVL4
			if not b9058 then
$ENDIF
			do;
				b6742 = 0FFh;
$IF OVL4
				if getChClass = CC$SEMI and b905E then
				do;
					b9059 = 0FFh;
					w906A = w906A - 2;
				end;
$ENDIF
				call skip2NextLine;
				b6B29 = 1;
				return;
			end;
		end;
		do;				/* CC$COLON */
			if not b6872 then
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

				b6B30 = 0;
				b6872, b6B31 = bTRUE;
			end;
			else
			do;
				call syntaxError;
				call popToken;
			end;

			call sub$467F(0, .extName);
			haveUserSymbol = FALSE;
			opType = 3;
		end;
		do;				/* CC$CR */
			call chkLF;
			b6B29 = 1;
$IF OVL4
			b9058 = 0;
$ENDIF
			return;
		end;
		do;				/* CC$PUNCT */
			if curChar = '+' or curChar = '-' then
$IF OVL4
				if not testBit(opType, .b$3F88) then /* not 0, 3 or 65 */
$ELSE
				if opType <> 0 and opType <> 3 then
$ENDIF
					curChar = curChar + 3;
			b6B29 = curChar - '(' + 2;
			return;
		end;
		do;				/* CC$DOLLAR */
			call pushToken(0Ch);
			call collectByte(low(segSize(activeSeg)));
			call collectByte(high(segSize(activeSeg)));
			if activeSeg <> 0 then
				tokenAttr(0) = tokenAttr(0) or activeSeg or 18h;
			call sub$416B;
		end;
		do;				/* CC$QUOTE */
$IF OVL4
			if b6B29 = 37h then
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
				if b6B31 then
					call sub$43D2;
				call sub$416B;
			end;
		end;
		do;				/* CC$DIGIT */
			call getNum;
			if b6B31 then
				call sub$43D2;
			call sub$416B;
		end;
		do;				/* CC$LET */
$IF OVL4
			w919F = w906A - 1;
$ENDIF
			call getId(9);
			if tokenSize(0) > 6 then
				tokenSize(0) = 6;

			if ctlXREF then
			do;
				call move(6, .extName, .b6879);
				call move(6, .spaces6, .extName);
			end;

			call move(tokenSize(0), curTokStart, .extName);
			nameLen = tokenSize(0);
			call packToken;
			if haveUserSymbol then
			do;
				b687F = 0FFh;
				haveUserSymbol = FALSE;
			end;


$IF OVL4
			if lookup(2) <> 9 and b905E then
			do;
				if not b9058 or (jj := tokenType(0) = 0) and (curChar = 26h or mem(w919F-1) = 26h) then
				do;
					w906A = w919F;
					call sub$3D55(jj + 81h);
					call sub$3D34(sub$43DD);
					call sub$3D55(curChar);
					b6B29 = 9;
				end;
			end;
			else if b6B29 <> 37h and not b905E = 2 then
$ENDIF
			do;
				if lookup(0) = 9 then		/* not a key word */
				do;
					tokenType(0) = lookup(1);	/* look up in symbol space */
					haveUserSymbol = TRUE;		/* not a key word */
				end;

				b6B29 = tokenType(0);
				b6885 = b3EA8(tokenType(0));
				if not b3E5E(tokenType(0)) then
					call popToken;

				if b687F then
				do;
					call sub$467F((not testBit(b6B29, .b3EA0)) and 1, .b6879);
					b687F = 0;
				end;
			end;
$IF OVL4
			if b905E = 1 then
			do;
				if b6B29 = 3Fh then
				do;
					b905E = 2;
					if b6897 then
						call syntaxError;
					b6897 = 0;
				end;
				else
				do;
					b6897 = 0;
					b905E = 0FFh;
				end;
			end;

			if b6B29 = 41h then
				call pushToken(40h);
$ENDIF
			if b6B29 < 0Ah or b6B29 = 9 or 80h then
			do;
				call sub$416B;
				if b6B31 then
					call sub$43D2;
			end;
			else
			do;
				b6B31 = 0;
				return;
			end;
		end;
$IF OVL4
		do;				/* 10? */
			b6BDA = 0;
			call sub$73AD;
			if b6BDA then
				return;
		end;
		do;				/* CC$ESC */
			if expandingMacro then
			do;
				skipping(0) = 0;
				b6B29 = 40h;
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
