asm4A$OV0: do;
$IF OVL4
$include(asm4a.ipx)
$ELSE
$include(asmov0.ipx)
$ENDIF

declare controlTable(*) byte data(35h, 'DEBUG', 3Ah, 'MACRODEBUG',
			   34h, 'XREF', 37h, 'SYMBOLS',
			   36h, 'PAGING', 33h, 'TTY',
			   25h, 'MOD85', 35h, 'PRINT',
			   36h, 'OBJECT', 39h, 'MACROFILE',
			   29h, 'PAGEWIDTH', 2Ah, 'PAGELENGTH',
			   7, 'INCLUDE', 5, 'TITLE',
			   4, 'SAVE', 7, 'RESTORE',
			   5, 'EJECT', 14h, 'LIST',
			   13h, 'GEN', 14h, 'COND');

declare (b7463, b7464, b7465, controlError) byte;


chkParen: procedure(arg1b) byte;
	declare arg1b byte;
	call skipWhite;
	reget = 0;
	return arg1b + '(' = curChar;
end;



getTok: procedure byte;
	tokBufLen = 0;
	tokType = O$NONE;
	if isCR then
		return curChar;

	call skipWhite$2;
	if curChar > 'A'-1 and 'Z'+1 > curChar or curChar > 'a'-1 and 'z'+1 > curChar then
	do;							/* letter */
		call getId(1);
		if blankAsmErrCode and tokenSize(0) < 14 then
			call move(tokBufLen := tokenSize(0), .lineBuf, .tokBuf);
	end;
	else if curChar > '0'-1  and  '9'+1 > curChar then	/* digit ? */
	do;
		call getNum;
		if blankAsmErrCode then
		do;
			tokNumVal = sub$43DD;
			tokType = TT$NUM;
			tokBuf(0) = ' ';
		end;
	end;
	else if curChar = '''' then	/* string ? */
	do;
		call getStr;
		if blankAsmErrCode then
		do;
			tokBufLen = 64;
			if tokenSize(0) < 64 then
				tokBufLen = tokenSize(0);
			tokType = TT$STR;
			if tokBufLen > 0 then
				call move(tokBufLen, .lineBuf, .tokBuf);
		end;
	end;
	else
	do;
		tokBufLen = 1;
		tokBuf(0) = curChar;
		return curChar;
	end;

	call popToken;
	return tokBuf(0);
end;



sub$6F07: procedure(arg1w) byte;
	declare arg1w address;
	declare pad address;

	if tokBufIdx = 0 then
		return 0;

	tokBuf(tokBufIdx) = 20h;
	tokBufLen = tokBufIdx;
	if isWhite then
		return chkParen(1);
	return 0FFh;
end;

sub$6F39: procedure;
	call skipWhite$2;

    do while 1;
	if isRParen or isWhite then
	do;
		if sub$6F07(.tokBuf(0)) then
			return;
		goto done;
	end;

	tokBuf(tokBufIdx) = curChar;
	tokBufIdx = tokBufIdx + 1;
	if tokBufIdx > 14 then
		goto done;
	curChar = getCh;
    end;
done:
	curFileName$p = .tokBuf;
	call fileError;
end;


getFileParam: procedure;
	tokBufIdx = 0;
	if not chkParen(0) then	/* ( */
		call fileError;
	else
	do;
		call sub$6F39;
		call move(tokBufIdx + 1, .tokBuf, curFileName$p);
		if not chkParen(1) then	/* ) */
			call fileError;
	end;
end;


getMacroFileDrive: procedure;
	call skipWhite$2;
	tokBufIdx = 0Dh;
	ii = 0;

	do while not isRParen and ii < 4;
		asmax$ref(ii) = curChar;
		ii = ii + 1;
		curChar = getCh;
	end;

	if isRParen or isWhite then
		if sub$6F07(.asmax$ref) then
			return;
	curFileName$p = .asmax$ref;
	tokBufIdx = 0;
	call fileError;
end;




getControlNumArg: procedure byte;
	if chkParen(0) then	/* ( */
	do;
		b7463 = getTok;
		if tokType = TT$NUM then
			return chkParen(1);	/* ) */
	end;
	return 0;
end;


lookupControl: procedure byte;
	declare (cmdIdx, cmdStartIdx, ctlVal, cmdLen, ctlFlags, noInvalid) byte,
		(control$p, nextControl$p, ctlSeen$p) address;
	declare twoch address at(.tokBuf);
	declare ch based control$p byte;
	declare ctlSeen based ctlSeen$p byte;

	setControl: procedure;
		if not noInvalid then
		do;
			controls(controlId) = ctlVal;
			if not ctlVal then
				controlId = 17;	/* no action needed */
		end;
	end;


	cmdLen = tokBufLen;
	cmdStartIdx = 0;
	ctlVal = 0FFh;
	if twoch = 'ON' then	/* NO stored with bytes swapped */
	do;
		cmdStartIdx = 2;	/* don't match the NO in the table */
		ctlVal = 0;		/* control will be set as false */
		cmdLen = tokBufLen - 2;	/* length of string to match excludes the NO */
	end;

	control$p = .controlTable;
	controlId = 0;
	cmdIdx = cmdStartIdx;

	do while controlId < 20;
		nextControl$p = control$p + (ch and 0Fh) + 1;
		if (ch and 0Fh) = cmdLen then
		do;
			ctlFlags = ch;
			do while cmdIdx < tokBufLen;
				control$p = control$p + 1;
				if ch <> tokBuf(cmdIdx) then
					cmdIdx = tokBufLen + 1;	/* cause early exit */
				else
					cmdIdx = cmdIdx + 1;	/* check next character */
			end;

			if cmdIdx = tokBufLen then		/* found it */
				goto found;
		end;
		control$p = nextControl$p;
		controlId = controlId + 1;
		cmdIdx = cmdStartIdx;
	end;
	return 255;						/* not found */

found:
$IF OVL0 
	if controlId = 1 or controlId = 12h then	/* MACRODEBUG or GEN */
	do;
		if scanCmdLine then			/* only valid on command line not $ line */
			MacroDebugOrGen = TRUE;
		else
			return 255;			/* error otherwise */
	end;
$ENDIF
	if (noInvalid := (ctlFlags and 10h) <> 10h) and not ctlVal then
		return 255;	/* NO not supported */

	if (ctlFlags and 20h) <> 20h then		/* GENERAL control */
	do;
		call setControl;
		return controlId;
	end;

	if not primaryValid then			/* is a PRIMARY control */
		return 255;

	ctlSeen$p = .controlSeen(controlId);		/* make sure we only see a primary once */
	if ctlSeen then
		return 255;
	ctlSeen = TRUE;
	call setControl;
	return controlId;
end;

processControl: procedure;
	if controlId >= 17 or controlId < 5 then
		return;

	do case controlId - 5;
/* 0 */		do;			/* TTY */
			ctlTTY = 0FFh;
			return;
		end;
/* 1 */		do;			/* MOD85 */
			ctlMOD85 = 0FFh;
			return;
		end;
/* 2 */		do;			/* PRINT */
			controlFileType = 2;
			curFileName$p = .lstFile;
			call getFileParam;
			return;
		end;
/* 3 */		do;			/* OBJECT */
			controlFileType = 3;
			curFileName$p = .objFile;
			call getFileParam;
			return;
		end;
/* 4 */		do;			/* MACROFILE */
			controlFileType = 3;
			if chkParen(0) then	/* optional drive for tmp file */
				call getMacroFileDrive;
			else
				reget = 1;
			ctlMACROFILE = 0FFh;
			return;
		end;
/* 5 */		do;			/* PAGEWIDTH */
			if getControlNumArg then
			do;
				ctlPAGEWIDTH = tokNumVal;
				if ctlPAGEWIDTH > 132 then
					ctlPAGEWIDTH = 132;
				if ctlPAGEWIDTH < 72 then
					ctlPAGEWIDTH = 72;
				return;
			end;
		end;
/* 6 */		do;			/* PAGELENGTH */
			if getControlNumArg then
			do;
				ctlPAGELENGTH = tokNumVal;
				if ctlPAGELENGTH < 15 then
					ctlPAGELENGTH = 15;
				return;
			end;
		end;
/* 7 */		do;			/* INCLUDE */
			if not needToOpenFile then
			do;
				controlFileType = 1;
				if fileIdx = 5 then
					call stackError;
				else
				do;
					fileIdx = fileIdx + 1;
					curFileName$p = .files(fileIdx);
					call getFileParam;
					needToOpenFile = TRUE;
					if scanCmdLine then
						includeOnCmdLine = TRUE;
					return;
				end;
			end;
		end;
/* 8 */		do;			/* TITLE */
			if chkParen(0) then
			do;
				b7463 = getTok;
				if tokType = TT$STR and tokBufLen <> 0 then
				do;
					if phase <> 1 or isPhase1 and primaryValid then
					do;
						call move(tokBufLen, .tokBuf, .ctlTITLESTR);
						ctlTITLESTR(titleLen := tokBufLen) = 0;
						if chkParen(1) then
						do;
							ctlTITLE = 0FFh;
							return;
						end;
					end;
				end;
			end;
		end;
/* 9 */		do;			/* SAVE */
			if saveIdx > 7 then
				call stackError;
			else
			do;
				call move(3, .ctlLIST, .saveStack + saveIdx * 3);
				saveIdx = saveIdx + 1;
				return;
			end;
		end;
/* 10 */	do;			/* RESTORE */
			if saveIdx > 0 then
			do;
				call move(3, .saveStack + (saveIdx := saveIdx - 1) * 3, .ctlLIST);
				return;
			end;
		end;
/* 11 */	do;			/* EJECT */
			ctlEJECT = ctlEJECT + 1;
			return;
		end;
	end;
	controlError = 0FFh;
end;

parseControls: procedure public;
	isControlLine = TRUE;
	b6A6F, b7464 = ctlLIST;
$IF OVL4
	b7465 = ctlGEN;
$ENDIF
	controlError = 0;

	do while getTok <> CR and not controlError;
		if tokBuf(0) = ';' then		/* skip comments */
			call skip2EOL;
		else if lookupControl = 255 then	/* error ? */
			controlError = TRUE;
		else
			call processControl;
	end;

	if controlError then
	do;
		if tokBuf(0) <> CR then
		do;
			reget = 0;
			call skip2EOL;
		end;

		if scanCmdLine then
			call runtimeError(2);	/* command error */
		else
			call commandError;
	end;

	call chkLF;			/* eat the LF */
	if ctlLIST <> b7464 then
		b6A6F = 0FFh;
$IF OVL4
	else if ctlGEN <> b7465 and expandingMacro then
		b6A6F = 0;
$ENDIF

	reget = 0;
end;

end;
