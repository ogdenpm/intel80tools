asm49$861:
do;
$IF OVL4
$include(asm49.ipx)
$ELSE
$include(asm861.ipx)
$ENDIF

syntaxError: procedure public;
	call sourceError('Q');
end;

doubleDefError: procedure public;
	call sourceError('D');
end;

expressionError: procedure public;
	call sourceError('E');
end;

commandError: procedure public;
	call sourceError('C');
end;

OpcodeOperandError: procedure public;
	call sourceError('O');
end;

nameError: procedure public;
	call sourceError('R');
end;

multipleDefError: procedure public;
	call sourceError('M');
end;

valueError: procedure public;
	call sourceError('V');
end;

nestingError: procedure public;
	call sourceError('N');
end;

phaseError: procedure public;
	call sourceError('P');
end;

stackError: procedure public;
	call runtimeError(0);
end;

fileError: procedure public;
	call runtimeError(4);
end;

illegalCharError: procedure public;
	call sourceError('I');
end;

balanceError: procedure public;
	call sourceError('B');
end;

undefinedSymbolError: procedure public;
	call sourceError('U');
end;

locationError: procedure public;
	call sourceError('L');
end;

operandError: procedure public;
	call sourceError('X');
end;

haveTokens: procedure byte public;
	return tokenSP <> 0;
end;


popToken: procedure public;
	tokStart(0) = tokStart(tokenSP);
	tokenSym(0) = tokenSym(tokenSP);
	tokenType(0) = tokenType(tokenSP);
	tokenAttr(0) = tokenAttr(tokenSP);
	tokenSize(0) = tokenSize(tokenSP);
	tokenSymId(0) = tokenSymId(tokenSP);
	if haveTokens then
		tokenSP = tokenSP - 1;
end;



nestIF: procedure(arg1b) public;
	declare arg1b byte;
$IF OVL4
	macroCondStk(macroCondSP := macroCondSP + 1) = macroCondStk(0);
	if (macroCondStk(0) := arg1b) = 1 then
	do;
		if (b9063 := b9063 + 1) > 9 then
		do;
			call stackError;
			b9063 = 0;
		end;
		else
		do;
			call move(16, .macroStk(0), .macroStk(b9063));
			tmac$macroCondSP = macroCondSP;
			tmac$ifDepth = ifDepth;
			b9061 = 0FFh;
		end;
	end;
	else
$ENDIF
	do;
		if (ifDepth := ifDepth + 1) > 8 then
		do;
			call stackError;
			ifDepth = 0;
		end;
		else
		do;
			skipping(ifDepth) = skipping(0);
			inElse(ifDepth) = inElse(0);
		end;
	end;
end;


unnestIF: procedure(arg1b) public;
	declare arg1b byte;

$IF OVL4
	if arg1b <> macroCondStk(0) then
	do;
		call nestingError;
		if arg1b = 2 then
			return;
		macroCondSP = tmac$macroCondSP;
		ifDepth = tmac$ifDepth;
	end;

	macroCondStk(0) = macroCondStk(macroCondSP);
	macroCondSP = macroCondSP - 1;
	if arg1b = 1 then
	do;
		call move(16, .macroStk(b9063), .macroStk(0));
		call readM(tmac$blk);
		b9062 = tmac$mtype;
		if (b9063 := b9063 - 1) = 0 then
		do;
			expandingMacro = 0;
			w6870 = physmem + 0BFh;
		end;
	end;
$ELSE
	if ifDepth = 0 then
		call nestingError;
$ENDIF
	else
	do;
		skipping(0) = skipping(ifDepth);
		inElse(0) = inElse(ifDepth);
		ifDepth = ifDepth - 1;
	end;
end;

pushToken: procedure(type) public;
	declare type byte;

	if tokenSP >= 8 then
		call stackError;
	else
	do;
		tokenSP = tokenSP + 1;
		tokStart(tokenSP) = tokStart(0);
		tokenSym(tokenSP) = tokenSym(0);
		tokenType(tokenSP) = tokenType(0);
		tokenAttr(tokenSP) = tokenAttr(0);
		tokenSize(tokenSP) = tokenSize(0);
		tokenSymId(tokenSP) = tokenSymId(0);	
		tokStart(0) = tokStart(0) + tokenSize(0);	/* advance for next token */
		tokenType(0) = type;
		tokenAttr(0), tokenSize(0) = bZERO;
		tokenSym(0) = wZERO;
$IF OVL4
		tokenSymId(0) = wZERO;
$ELSE
		tokenSymId(0) = tokenSym(0);
$ENDIF
	end;
end;

collectByte: procedure(c) public;
	declare c byte;
	declare s address;
	declare ch based s byte;


	if (s := curTokStart + tokenSize(0)) < endLineBuf then	/* check for lineBuf overrun */
	do;
		ch = c;
		tokenSize(0) = tokenSize(0) + 1;
	end;
	else
		call stackError;
end;

getId: procedure(type) public;
	declare type byte;

	call pushToken(type);
	reget = 1;

	do while (type := getChClass) = CC$DIGIT or type = CC$LET;	/* digit or letter */
		if curChar > 60h then	/* make sure upper case */
			curChar = curChar and 0DFh;
		call collectByte(curChar);
	end;
	reget = 1;
end;


getNum: procedure public;
	declare accum address,
		(radix, digit, i) byte;
	declare chrs based curTokStart (1) byte;

	call getId(12);
	radix = chrs(tokenSize(0):= tokenSize(0) - 1);
	if radix = 'H' then
		radix = 16;

	if radix = 'D' then
		radix = 10;

	if radix = 'O' or radix = 'Q' then
		radix = 8;

	if radix = 'B' then
		radix = 2;

	if radix > 16 then
		radix = 10;
	else
		tokenSize(0)= tokenSize(0) - 1;

	accum = 0;
	do i = 0 to tokenSize(0);
		if chrs(i) = '?' or chrs(i) = '@' then
		do;
			call illegalCharError;
			digit = 0;
		end;
		else
		do;
			if (digit := chrs(i) - '0') > 9 then
				digit = digit - 7;
			if digit >= radix then
				if not (tokenType(2) = 40h) then /* risk that may be uninitialised */
				do;
					call illegalCharError;
					digit = 0;
				end;
		end;

		accum = accum * radix + digit;
	end;
	/* replace with packed number */
	tokenSize(0) = 0;
	call collectByte(low(accum));
	call collectByte(high(accum));
end;

getStr: procedure public;
	call pushToken(0Ah);

	do while getCh <> CR;
		if curChar = '''' then
			if getCh <> '''' then
				goto L6268;
		call collectByte(curChar);
	end;

	call balanceError;

L6268:
	reget = 1;
end;

end;
