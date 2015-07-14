asm80$47$57$85: do;
$IF OVL4
$include(asm47.ipx)
$ELSEIF OVL5
$include(asm57.ipx)
$ELSE
$include(asm85.ipx)
$ENDIF

$IF BASE
declare	CHKOVL$2 lit	'call ovlMgr(2)';
$ELSE
declare	CHKOVL$2 lit	' ';
$ENDIF

declare b4A26(*) byte data(0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh,
			   0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh,
			   0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0,
			   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0FFh, 0FFh, 0FFh, 0FFh,
			   0FFh, 0FFh, 0FFh, 0FFh, 0, 0FFh, 0FFh, 0FFh),
	b4A68(*) byte data(0, 0FFh, 0FFh, 0FFh, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0FFh, 0FFh, 0,
			   0, 0, 0, 0, 0, 0, 0, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh,
			   0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh, 0, 0, 0, 0, 0,
			   0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0FFh, 0FFh);

declare pad byte;

declare	accum(4) byte at(.accum1);


sub$4AAA: procedure(arg1b) public;
	declare arg1b byte;

	if b674A(arg1b) then
	do;
		if haveTokens then
		do;
			accum1 = sub$43DD;
			if alignTypes(arg1b) <> accum1$lb then
				call expressionError;
		end;
		else if alignTypes(arg1b) <> 3 then
			call expressionError;
	end;
	else
	do;
		b674A(arg1b) = TRUE;
		if haveTokens then
		do;
			accum1 = sub$43DD;
			if accum1 = 1 or accum1 = 2 then
				alignTypes(arg1b) = low(accum1);
			else
				call expressionError;
		end;
	end;
end;


cond2Acc: procedure(cond) public;	/* convert conditional result to accum1 */
	declare cond byte;		/* 0 or 0ffffh */

	accum1 = cond;
	if accum1 then
		accum1 = 0FFFFh;
end;

sub$4B57: procedure(arg1b) public;
	declare arg1b byte;

	if (b6855 and 47h) <> 0 then
		b6855 = b6855 and 0E7h or arg1b;
end;

sub4B72$516F: procedure public;
	do case b6B2B;
/* 0 */		;
/* 1 */		call sub4C1E$54FD;		/* white space ? */
/* 2 */		goto case3;			/* ( */
/* 3 */ case3:	do;				/* : or ) */
			if not (b6B2B = 2 and opType = 3) then
				call balanceError;

			if tokenType(0) = 11 then
			do;
				tokenSize(0) = 1;
				tokenAttr(0) = 0;
				b6B36 = 0FFh;
			end;

			b6B35 = b6B2F;
			if opType = 3 then
				b6B2C = 0FFh;
		end;
/* 4 */		accum1 = accum1 * accum2;	/* * */
/* 5 */		accum1 = accum1 + accum2;	/* + */
/* 6 */		do;				/* , */
			call syntaxError;
			call popToken;
		end;
/* 7 */		accum1 = accum1 - accum2;	/* - */
/* 8 */		;				/* unary + */
/* 9 */		do;				/* / */
			if accum2 = 0 then
				call valueError;

			accum1 = accum1 / accum2;
		end;
/* 10 */	accum1 = -accum1;			/* unary - */
/* 11 */	call cond2Acc(accum1 = accum2);		/* EQ */
/* 12 */	call cond2Acc(accum1 < accum2);		/* LT */
/* 13 */	call cond2Acc(accum1 <= accum2);	/* LE */
/* 14 */	call cond2Acc(accum1 > accum2);		/* GT */
/* 15 */	call cond2Acc(accum1 >= accum2);	/* GE */
/* 16 */	call cond2Acc(accum1 <> accum2);	/* NE */
/* 17 */	accum1 = not accum1;			/* NOT */
/* 18 */	accum1 = accum1 and accum2;		/* AND */
/* 19 */	accum1 = accum1 or accum2;		/* OR */
/* 20 */	accum1 = accum1 xor accum2;		/* XOR */
/* 21 */	do;					/* MOD */
			if accum2 = 0 then
				call valueError;
			accum1 = accum1 mod accum2;
		end;
/* 22 */	do;					/* SHL */
			if accum2$lb <> 0 then
				accum1 = shl(accum1, accum2);
		end;
/* 23 */	do;					/* SHR */
			if accum2$lb <> 0 then
				accum1 = shr(accum1, accum2);
		end;
/* 24 */	do;					/* HIGH */
			accum1 = high(accum1);
			call sub$4B57(10h);
		end;
/* 25 */	do;					/* LOW */
			accum1 = low(accum1);
			call sub$4B57(8);
		end;
/* 26 */	do;					/* DB ? */
			if tokenType(0) <> 10 then
			do;
				accum1 = sub$43DD;
				if accum1$hb - 1 < 0FEh then
					call valueError;
				b6B28 = 22h;
				if (b6855 and 18h) = 18h then
				do;
					call valueError;
					b6855 = b6855 and 0E7h or 8;
				end;
			end;
			else
			do;
				b6855 = 0;
				tokenType(0) = 0Bh;
			end;

			if sub$425B(b6858) then
				call operandError;
			b6B2D = 0Bh;
			b6B23 = 0FFh;
		end;
/* 27 */	do;					/* DW ? */
			b6B2D = 0Bh;
			b6B24 = 0FFh;
		end;
/* 28 */	do;					/* DS ? */
			segSize(activeSeg) = segSize(activeSeg) + accum1;
			b68AB = 0FFh;
		end;
/* 29 */ case29:					/* EQU ? */
		do;
			b68AB = 0FFh;
			if (b6855 and 40h) = 40h then
			do;
				call expressionError;
				b6855 = 0;
			end;
			b6EC4$9C3A = 1;
			call sub5819$5CE8(accum1, 22h - b6B2B);	/* 4 for set, 5 for equ */
			b6B30 = 0;
		end;
/* 30 */	goto case29;				/* SET ? */
/* 31 */	do;					/* ORG ? */
			b68AB = 0FFh;
			if (b6855 and 40h) <> 40h then
			do;
				if (b6855 and 18h) <> 0 then
					if (b6855 and 7) <> activeSeg or (b6855 and 18h) <> 18h then
						call expressionError;
			end;
			else
				call expressionError;

			if ctlOBJECT then
				if segSize(activeSeg) > w6B41(activeSeg) then
					w6B41(activeSeg) = segSize(activeSeg);
			segSize(activeSeg) = accum1;
		end;
/* 32 */	do;					/* END ? */
			if tokenSP > 0 then
			do;
				startOffset = sub$43DD;
				startDefined = 1;
				startSeg = b6855 and 7;
				if (b6855 and 40h) = 40h then
					call expressionError;
				if sub$425B(b6858) then
					call operandError;

				b68AB = 0FFh;
			end;
$IF OVL4
			jj = b$905E;
			b$905E = 0;

			if macroCondSP > 0 or jj then
$ELSE
			if ifDepth > 0 then
$ENDIF
				call nestingError;
			if opType <> 1 then
				call syntaxError;
			if b6B35 then
				b6B33 = 0FFh;
			else
				call syntaxError;

		end;
/* 33 */	do;					/* IF ? */
			if b6B35 then
			do;
				b6B32 = 0FFh;
				call nestIF(2);
				b6881 = 0FFh;
				if skipping(0) = 0 then
					skipping(0) = not ((low(accum1) and 1) = 1);
				inElse(0) = 0;
			end;
		end;
/* 34 */	do;					/* ELSE ? */
			b6B32 = 0FFh;
$IF OVL4
			if macroCondStk(0) <> 2 then
$ELSE
			if ifDepth = 0 then
$ENDIF
				call nestingError;
			else if not inElse(0) then
			do;
				if not skipping(0) then
					skipping(0) = 0FFh;
				else
					skipping(0) = skipping(ifDepth);
				inElse(0) = 0FFh;
			end;
			else
				call nestingError;
		end;
/* 35 */	do;					/* ENDIF ? */
			if b6B35 then
			do;
				b6B32 = 0FFh;
				call unnestIF(2);
			end;
		end;
/* 36 */	do;					/* LXI ? */
			if b6744 = 1 then
				if b6873(0) = 4Dh then
					call syntaxError;
			call sub$450F(85h);
		end;
/* 37 */	do;				/* POP DAD PUSH INX DCX ? */
			if b6744 = 1 then
				if b6873(0) = 4Dh then
					call syntaxError;
			call sub$450F(5);
		end;
/* 38 */	call sub$450F(7);		/* LDAX STAX ? */
/* 39 */	call sub$450F(2);		/* ADC ADD SUB ORA SBB XRA ANA CMP ? */
/* 40 */	call sub$450F(8);		/* ADI OUT SBI ORI IN CPI SUI XRI ANI ACI ? */
/* 41 */	call sub$450F(46h);		/* MVI ? */
/* 42 */	call sub$450F(6);		/* INR DCR ? */
/* 43 */	call sub$450F(36h);		/* MOV */
/* 44 */	call sub$450F(0);		/* CZ CNZ JZ STA JNZ JNC LHLD */
						/* CP JC SHLD CPE CPO CM LDA JP JM JPE */
						/* CALL JPO CC CNC JMP */
/* 45 */	call sub$450F(0);		/* RNZ STC DAA DI SIM SPHL RLC */
						/* RP RAL HLT RM RAR RPE RET RIM */
						/* PCHL CMA CNC RPO EI XTHL NOP */
						/* RC RNX XCHG RZ RRC */
/* 46 */	call sub$450F(6);		/* RST */
/* 47 */	activeSeg = 0;			/* ASEG ? */
/* 48 */	do;				/* CSEG ? */
			activeSeg = 1;
			call sub$4AAA(0);
		end;
/* 49 */	do;				/* DSEG ? */
			activeSeg = 2;
			call sub$4AAA(1);
		end;

/* 50 */	do;				/* PUBLIC */
			b6748 = 0FFh;
			b6EC4$9C3A = 0;
			call sub5819$5CE8(0, 6);
		end;
/* 51 */	do;				/* EXTRN ? */
			b6749 = 0FFh;
			if exernId = 0 and isPhase1 and ctlOBJECT then
			do;
				CHKOVL$2;
				call writeModhdr;
			end;
			b6EC4$9C3A = 0;
			call sub5819$5CE8(exernId, 2);
			if isPhase1 and ctlOBJECT and not b6754 then
			do;
				CHKOVL$2;
				call writeExtName;
			end;
			if not b6754 then
				exernId = exernId + 1;
			b6754 = 0;
		end;
/* 52 */	do;				/* NAME */
			if tokenSP <> 0 and b6743 then
			do;
				call move(6, .spaces24 + 18, .aModulePage);
				call move(moduleNameLen := b6744, .b6873, .aModulePage);
			end;
			else
				call sourceError('R');

			call popToken;
		end;
/* 53 */	segSize(3) = accum1;		/* STKLN ? */
$IF OVL4
/* 54 */	call sub$7517;			/* MACRO ? */
/* 55 */	call sub$753E;
/* 56 */	call sub$75FF;			/* ENDM */
/* 57 */	call sub$76CE;			/* EXITM */
/* 58 */	do;
			tmac$mtype = 4;
			call sub$7327;
		end;
/* 59 */	call sub$72A4(1);		/* IRP ? */
/* 60 */	call sub$72A4(2);		/* IRPC */
/* 61 */	call sub$770B;
/* 62 */	call sub$7844;			/* REPT ? */
/* 63 */	call sub$787A;			/* LOCAL */
/* 64 */	call sub$78CE;
/* 65 */	do;				/* NUL */
			call cond2Acc(tokenType(0) = 41h);
			call popToken;
			b6855 = 0;
		end;
$ENDIF
	end;

	if b6B2B <> 1 then
		b6743 = 0;
end;

sub$518F: procedure public;

	sub$53C0: procedure byte;
		if b6B29 > 3 then
			if b6B29 <> 6 then
				if b6B29 < 1Ah then
					return TRUE;
		return FALSE;
	end;

	sub$53DF: procedure(arg1b) byte;
		declare arg1b byte;
		return arg1b = 9 or arg1b = 64h;
	end;


	sub$53F8: procedure;
		if not b4A68(b6B2B) then
			b6B34 = 0;
	end;



	do while 1;
		if not (b6B29 = 1 or b6B29 >= 20h and b6B29 <= 23h) and skipping(0)
$IF OVL4
	           or (b4181(b6B29) < 128 or b$9058) and b$905E

$ENDIF
	        then
		do;
			b6885 = 0;
			call popToken;
			return;
		end;
		
		if phase <> 1 then
			if b6B25 then
				if sub$53C0 then
					if getPrec(b6B29) <= getPrec(opStack(opSP)) then
						call expressionError;

		if getPrec(opType := b6B29) > getPrec(b6B2B := opStack(opSP)) or opType = 2 then
		do;
			if opSP >= 10h then
			do;
				opSP = 0;
				call stackError;
			end;
			else
				opStack(opSP := opSP + 1) = opType;
			if opType = 2 then
			do;
				b6B2F = b6B35;
				b6B35 = 0FFh;
			end;
			if phase > 1 then
				b6B25 = sub$53C0;
			return;
		end;

		b6B25 = 0;
		if not b6B35 and b6B2B > 3 then
			call syntaxError;

		if b6B2B = 0 then
			b6B2B = opType;
		else
			opSP = opSP - 1;
		

		if (b6B28 := b4181(b6B2B)) then
		do;
			accum2 = sub$43DD;
			b6856 = b6855;
			w685C = w685A;
			b6859 = b6858;
		end;

		if (b6B28 := ror(b6B28, 1)) then
			accum1 = sub$43DD;

		if not b6857 then
			b6857 = sub$53DF(b6858) or sub$53DF(b6859);

		b6B2D = 0Ch;
		if b6B2B > 3 and b6B2B < 1Ah then	/* expression op */
			call sub$4291;
		else
		do;
			call sub$53F8;
			call sub$4274;
		end;

		call sub4B72$516F;
		if not b4A26(b6B2B) then
			b6B35 = 0;

		if b6B2C then
		do;
			b6B2C = 0;
			return;
		end;

		if b6B2B <> 1Ch and b68AB then		/* DS */
			w68A6 = accum1;

		if (b6B28 and 1Eh) <> 0 then
			call pushToken(b6B2D);

		do ii = 0 to 3;
			if (b6B28 := ror(b6B28, 1)) then
				call collectByte(accum(ii));
		end;

		tokenAttr(0) = b6855;
		tokenSymId(0) = w685A;
		if ror(b6B28, 1) then
			if opType = 6 then
			do;
				b6B29 = b6B2B;
				b6B35 = 0FFh;
			end;
	end;
end;



sub$540D: procedure public;
	do while finished = 0;
		call sub$3F19;
		call sub$518F;
	end;
end;

end;
