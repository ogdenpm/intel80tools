asm45$55$851:
do;
$IF OVL4
$include(asm45.ipx)
$ELSEIF OVL5
$include(asm55.ipx)
$ELSE
$include(asm851.ipx)
$ENDIF

$IF BASE
declare CHKOVL$1 lit	'call ovlMgr(1)',
	CHKOVL$2 lit	'call ovlMgr(2)';
$ELSE
declare CHKOVL$1 lit	' ',
	CHKOVL$2 lit	' ';
$ENDIF

declare pad1 address data(40h),
	pad2 address;


strUCequ: procedure(arg1w, arg2w) byte public;
	declare (arg1w, arg2w) address;
	declare ch1 based arg1w byte,
		ch2 based arg2w byte;

	do while ch1 <> 0;
		if ch1 <> ch2 and ch1 <> (ch2 and 5Fh) then
			return FALSE;
		arg1w = arg1w + 1;
		arg2w = arg2w + 1;
	end;
	return TRUE;
end;


isSkipping: procedure byte public;
	return 
$IF OVL4
	b905E or
$ENDIF
		 skipping(0);
end;

sub$546F: procedure public;
	spIdx = nxtTokI;
	if expectingOperands then
		call syntaxError;
	if haveTokens then
		if not(tokenType(spIdx) = O$DATA or b68AD) then
			call syntaxError;
	if inDB or inDW then
	do;
		if tokenIdx = 1 and not blankAsmErrCode and tokenSize(0) <> 1 then
			tokenSize(0) = 2;
	end;
	else if not blankAsmErrCode and haveTokens then
		if tokenSize(spIdx) > 3 then
			tokenSize(spIdx) = 3;
end;


finishLine: procedure public;
	declare lineno$p address,
		updating byte,
		ch based lineno$p byte;

	call sub$546F;
	if isPhase2Print then
	do;	/* update the ascii line number */
		lineno$p = .asciiLineNo(3);	/* point to last digit */
		updating = TRUE;

		do while updating;		/* adjust the line number */
			if ch = '9' then	/* if 9 then roll over to 0 */
				ch = '0';
			else
			do;
				if ch = ' ' then	/* new digit */
					ch = '1';
				else			/* just increment */
					ch = ch + 1;
				updating = FALSE;
			end;
			lineno$p = lineno$p - 1;
		end;

		if showLine or not blankAsmErrCode then
		do;
			CHKOVL$1;
			call printLine;
		end;
	end;

	if b6BD9 then
	do;
		out$p = out$p + 1;
		call flushout;
		call exit;
	end;

	if not isControlLine then
	do;
		ii = 2;
		if tokenIdx < 2 or inDB or inDW then
			ii = 0;

		w6BCE = tokStart(ii) + tokenSize(ii);
		if isSkipping or not b6B34 then
			w6BCE = .lineBuf;

		if chkGenObj then
		do;
			CHKOVL$2;
			call ovl8;
		end;
		b6B2C = 0FFh;
		segSize(activeSeg), effectiveAddr = segSize(activeSeg) + (w6BCE - .lineBuf);
	end;

	if ctlXREF and haveUserSymbol then
		if phase = 1 then
			call emitXref(1, .name);

$IF OVL4
	call sub$40B9;
$ENDIF

	do while tokenIdx > 0;
		call popToken;
	end;

	call initLine;
	if b6B33 then
	do;
		finished = 0FFh;
		if isPhase2Print and ctlSYMBOLS then
		do;
			CHKOVL$1;
			call sub7041$8447;
		end;

		call emitXref(2, .name);	/* finalise xref file */
		if chkGenObj then
		do;
			CHKOVL$2;
			call reinitFixupRecs;
		end;
	end;
end;

end;
