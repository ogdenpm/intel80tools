
physmem: procedure address public;
	declare top address at(4);

	return (top - 100h) and 0FF00h;
end;


nxtCmdCh: procedure byte public;
	declare cmdch based cmdch$p byte;
	declare ch byte;

	ch = cmdch;
	cmdch$p = cmdch$p + 1;
	if 'a' <= ch and ch <= 'z' then
		ch = ch and 5Fh;
	return ch;
end;	


ioErrChk: procedure public;
	if statusIO = 0 then
		return;
	call error(statusIO);
	call exit;
end;


flushout: procedure public;
	call write(outfd, .outbuf, out$p - .outbuf, .statusIO);
	out$p = .outbuf;
end;

outch: procedure(c) public;
	declare c byte;
	declare outc based out$p byte;

	if out$p = endOutBuf then
		call flushout;
	outc = c;
	out$p = out$p + 1;
end;

outStrN: procedure(s, n) public;
	declare s address, n byte;
	declare ch based s byte;

	do while n > 0;
		call outch(ch);
		s = s + 1;
		n = n - 1;
	end;
end;

$IF ASM801
ovlMgr: procedure(ovl) public;
	declare ovl byte;
	declare entry$p address;

	if ovl > 4 then
	do;
		statusIO = 0Dh;
		call ioErrChk;
	end;
	if ovl <> loadedOvl then
	do;
		ovlFile(12) = ovl + '0';
		call load(.ovlFile, 0, 0, .entry$p, .statusIO);
		call ioErrChk;
		loadedOvl = ovl;
	end;
end;
$ENDIF

closeF: procedure(arg1w) public;
	declare arg1w address;

	call close(arg1w, .statusIO);
end;

isSpace: procedure byte public;
	return curChar = ' ';
end;

isTab: procedure byte public;
	return curChar = TAB;
end;

isWhite: procedure byte public;
	return isSpace or isTab;
end;

isRParen: procedure byte public;
	return curChar = ')';
end;

isCR: procedure byte public;
	return curChar = CR;
end;

isComma: procedure byte public;
	return curChar = ',';
end;

$IF ASM42
isLT: procedure byte public;
	return curChar = '<';
end;


isGT: procedure byte public;
	return curChar = '>';
end;

$ENDIF

isPhase1: procedure byte public;
	return phase = 1;
end;

skip2EOL: procedure public;
	if not isCR then
		do while getCh <> 0Dh;
		end;
end;


chkGenObj: procedure byte public;
$IF ASM801
	return (phase > 2) and ctlOBJECT;
$ELSE
	return (phase = 2) and ctlOBJECT;
$ENDIF
end;


isPhase2Print: procedure byte public;
	return phase = 2 and ctlPRINT;
end;


wrConsole: procedure(buf$p, count) public;
	declare (buf$p, count) address;

	call write(0, buf$p, count, .statusIO);
	call ioErrChk;
end;


runtimeError: procedure(arg1b) public;
	declare arg1b byte;

	if b6BD9 then
		return;

	if isPhase1 and arg1b = 0 then
	do;
		b6B33 = TRUE;
		return;
	end;

	w6BE0 = .aError;		/* assume " ERROR\r\n" */
	if arg1b = 4 then		/* file error */
		w6BE0 = .aError$0;	/* replace with " ERROR, " */

	call wrConsole(errStrs(arg1b), errStrsLen(arg1b));	/* write the ERROR type */
	call wrConsole(w6BE0, 8);	/* write the ERROR string */
	if isPhase2Print then		/* repeat to the print file if required */
	do;
		call outStrN(errStrs(arg1b), errStrsLen(arg1b));
		call outStrN(w6BE0, 8);
	end;

	if arg1b = 4 or arg1b = 3 then	/* file or EOF error */
	do;
		if tokBufIdx = 0 then
		do;
			call wrConsole(.aBadSyntax, 12);
			if not scanCmdLine then
			do;
				call skip2NextLine;
				outfd = 0;
$IF ASM801
				call ovlMgr(1);
$ENDIF
				call printDecimal(w6A4E);	/* overlay 1 */
				call outch(LF);
			end;
		end;
		else
		do;
			call wrConsole(curFileName$p, tokBufIdx);
			call wrConsole(.ascCRLF, 2);
		end;
	end;

	if arg1b = 0 then	/* stack error */
	do;
		b6BD9 = TRUE;
		return;
	end;

	call exit;
end;

ioError: procedure(arg1w) public;
	declare arg1w address;
	declare ch based arg1w byte;

	tokBufIdx = 0;
	curFileName$p = arg1w;

	do while ch <> ' ' and ch <> 0dh and ch <> 9;
		tokBufIdx = tokBufIdx + 1;
		arg1w = arg1w + 1;
	end;
	if b6C27 then
		call runtimeError(3);	/* EOF error*/
	call runtimeError(4);		/* file error */
end;

inOpen: procedure(path$p, access) address public;
	declare (path$p, access) address;
	declare open$infd address;

	call open(.open$infd, path$p, access, 0, .openStatus);
	if openStatus <> 0 then
		call ioError(path$p);
	return open$infd;
end;


nibble2Ascii: procedure(n) byte public;
	declare n byte;
	n = (n and 0Fh) + '0';
	if n > '9' then
		n = n + 7;
	return n;
end;

put2Hex: procedure(arg1w, arg2b) public;
	declare arg1w address, arg2b byte;

	call arg1w(nibble2Ascii(ror(arg2b, 4)));
	call arg1w(nibble2Ascii(arg2b));
end;

blankAsmErrCode: procedure byte public;
	return asmErrCode = ' ';
end;

blankMorPAsmErrCode: procedure byte public;
	return blankAsmErrCode or asmErrCode = 'M' or asmErrCode = 'P';
end;


getNibble: procedure(bp, idx) byte public;
	declare bp address, idx byte;
	declare b based bp byte;
	declare n byte;

	bp = bp + shr(idx, 1);	/* index into buffer the number of nibbles */
	n = b;			/* pick up the byte there */
	if not idx then		/* pick up the right nibble */
		n = ror(n, 4);
	return n and 0Fh;	/* mask to leave just the nibble */
end;

sourceError: procedure(arg1b) public;
	declare arg1b byte;

	if not isSkipping or b6B2B = 22h then	/* ELSE */
	do;
		if b6749 then
			b6754 = TRUE;
		if blankAsmErrCode then
			errCnt = errCnt + 1;

		if blankMorPAsmErrCode or arg1b = 'L' or arg1b = 'U' then	/* no error or M, P L or U */
			if asmErrCode <> 'L' then	/* override unless already location counter error */
				asmErrCode = arg1b;

	end;
end;

$IF ASM42

sub$3D34: procedure(arg1b) public;
	declare arg1b byte;
	declare ch based w$906A byte;

	ch = arg1b;
	if (w$906A := w$906A + 1) > w6870 then
		call runtimeError(1);	/* table error */
end;


sub$3D55: procedure(arg1b) public;
	declare arg1b byte;

	call sub$3D34(arg1b);
	if arg1b = 0Dh then
		call sub$3D34(0Ah);
end;

$ENDIF


preStatementControls: procedure public;

	do while getCh = '$';
		if isSkipping then
		do;
			call skip2NextLine;
			b6B20$9A77 = 0FFh;
$IF ASM42
			if b$905E = 1 then
				b6897 = 0FFh;
$ENDIF
		end;
		else
		do;
$IF ASM801
			call ovlMgr(0);
$ENDIF
			call parseControls;
		end;
		call sub4C1E$54FD;
	end;
	reget = 1;
end;


initialControls: procedure public;
	cmdch$p = controls$p;
	scanCmdLine = TRUE;
$IF ASM801
	call ovlMgr(0);
$ENDIF
	call parseControls;
	if isPhase2Print then
	do;
$IF ASM801
		call ovlMgr(1);
$ENDIF
		call printCmdLine;
	end;
	if b6C21 then
		call sub$4904;

	b6C21, b6B20$9A77, scanCmdLine = bZERO;
	call preStatementControls;
	primaryValid = 0;
	ctlDEBUG = ctlDEBUG and ctlOBJECT;
	ctlXREF = ctlXREF and ctlPRINT;
	ctlSYMBOLS = ctlSYMBOLS and ctlPRINT;
	ctlPAGING = ctlPAGING and ctlPRINT;
end;


sub3DCE$3DFB: procedure public;
	off$6C2E = off$6C2C + 1;
	b6C30 = 0;
	if b6C21 then
		call sub$4904;

	b68AD, has16bitOperand, b6B20$9A77, b689B, b687F, b6B25, b6B30, b6881, b6872, b6880,
	b6B23, b6B24, b6B32, b68AB, b6884,
$IF ASM42
	b$9059, b$9060, 
$ENDIF
	b6885 = bZERO;

	b689C, b6B31, b6B34, b6B35 = bTRUE;
	ctlEJECT, b6857, tokenSP,
$IF ASM42
	b$9058, argNestCnt,
$ENDIF
	tokenSize(0), tokenType(0), b6858, b6859, b6742, b6855 = bZERO;
	asmErrCode = 20h;
$IF ASM42
	off$9056 = .b$8FD5;
	w$919D = w$906A;
	b$905B = b$905B > 0;
$ENDIF
	b689A = 1;
	w6A4E = w6A4E + 1;
$IF ASM42
	off$9056 = .b$8FD5;
$ENDIF
	skipping(0) = skipping(0) > 0;
end;

/* pckToken code follows */

