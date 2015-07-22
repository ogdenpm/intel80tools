start: do;
$IF BASE
$include(asm801.ipx)
$ELSEIF OVL4
$include(asm41.ipx)
$ELSE
$include(asm51.ipx)
$ENDIF

$IF BASE
declare CHKOVL$0 lit	'call ovlMgr(0)',
	CHKOVL$1 lit	'call ovlMgr(1)',
	CHKOVL$2 lit	'call ovlMgr(2)',
	CHKOVL$3 lit	'call ovlMgr(3)';
$ELSE
declare CHKOVL$0 lit	' ',
	CHKOVL$1 lit	' ',
	CHKOVL$2 lit	' ',
	CHKOVL$3 lit	' ';
$ENDIF

$IF OVL4
declare w$3780 address public data(0),
	b$3782 byte public data(80h),
	b$3783 byte public data(81h);
$ENDIF

declare	spaces24(*) byte public data('         '),
	spaces15(*) byte public data('         '),
	spaces6(*) byte public data(' '),
	spaces5(*) byte public data(' '),
	spaces4(*) byte public data('  '),
	spaces2(*) byte public data('  ', 0),
	ascCRLF(*) byte public data(CR, LF, 0),
	signonMsg(*) byte public data(CR, LF),
	asmHeader(*) byte public data('ISIS-II 8080/8085 MACRO ASSEMBLER, V4.1', TAB, TAB),
	aModulePage(*) byte public data('MODULE ', TAB, ' PAGE ', 0),
	bZERO byte public data(0),
	bTRUE byte public data(TRUE),
	copyright(*) byte data('(C) 1976,1977,1979,1980 INTEL CORP'),
	aStack(*) byte public data(CR, LF, 'STACK', 0),
	aTable(*) byte public data(CR, LF, 'TABLE', 0),
	aCommand(*) byte public data(CR, LF, 'COMMAND', 0),
	aEof(*) byte public data(CR, LF, 'EOF', 0),
	aFile(*) byte public data(CR, LF, 'FILE', 0),
	aMemory(*) byte public data(CR, LF, 'MEMORY', 0),
	aError(*) byte public data(' ERROR', CR, LF, 0),
	aError$0(*) byte public data(' ERROR, ', CR,LF, 0),
	errStrs(*) address public data(.aStack, .aTable, .aCommand, .aEof, .aFile, .aMemory),
	errStrsLen(*) byte public data(7, 7, 9, 5, 6, 8),
	aBadSyntax(*) byte public data('BAD SYNTAX', CR, LF),
	aCo(*) byte public data(':CO:', 0);

$IF BASE
declare	loadedOvl byte initial(4),
	ovlFile(*) byte public initial(':F0:ASM80.OV0 ');
$ENDIF

physmem: procedure address public;
	declare top address at(4);

	return (top - 100h) and 0FF00h;
end;


getCmdCh: procedure byte public;
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
	declare s pointer, n byte;
	declare ch based s byte;

	do while n > 0;
		call outch(ch);
		s = s + 1;
		n = n - 1;
	end;
end;

$IF BASE
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

$IF OVL4
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
$IF BASE
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
				CHKOVL$1;
				call printDecimal(srcLineCnt);	/* overlay 1 */
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

ioError: procedure(s) public;
	declare s pointer;
	declare ch based s byte;

	tokBufIdx = 0;
	curFileName$p = s;

	do while ch <> ' ' and ch <> 0dh and ch <> 9;
		tokBufIdx = tokBufIdx + 1;
		s = s + 1;
	end;
	if missingEnd then
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
	declare bp pointer, idx byte;
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

	if not isSkipping or op = 22h then	/* ELSE */
	do;
		if inExtrn then
			b6754 = TRUE;
		if blankAsmErrCode then
			errCnt = errCnt + 1;

		if blankMorPAsmErrCode or arg1b = 'L' or arg1b = 'U' then	/* no error or M, P L or U */
			if asmErrCode <> 'L' then	/* override unless already location counter error */
				asmErrCode = arg1b;

	end;
end;

$IF OVL4

sub$3D34: procedure(c) public;
	declare c byte;
	declare ch based w906A byte;

	ch = c;
	if (w906A := w906A + 1) > w6870 then
		call runtimeError(1);	/* table error */
end;


sub$3D55: procedure(c) public;
	declare c byte;

	call sub$3D34(c);
	if c = CR then
		call sub$3D34(LF);
end;

$ENDIF


parseControlLines: procedure public;

	do while getCh = '$';
		if isSkipping then
		do;
			call skip2NextLine;
			isControlLine = TRUE;
$IF OVL4
			if b905E = 1 then
				b6897 = TRUE;
$ENDIF
		end;
		else
		do;
			CHKOVL$0;
			call parseControls;
		end;
		call finishLine;
	end;
	reget = 1;
end;


initialControls: procedure public;
	cmdch$p = controls$p;
	scanCmdLine = TRUE;
	CHKOVL$0;
	call parseControls;
	if isPhase2Print then
	do;
		CHKOVL$1;
		call printCmdLine;
	end;
	if needToOpenFile then
		call openSrc;

	needToOpenFile, isControlLine, scanCmdLine = bZERO;
	call parseControlLines;			/* initial control lines allow primary controls */
	primaryValid = FALSE;			/* not allowed from now on */
	ctlDEBUG = ctlDEBUG and ctlOBJECT;	/* debug doesn't make sense if no object code */
	ctlXREF = ctlXREF and ctlPRINT;		/* disable controls that require printing */
	ctlSYMBOLS = ctlSYMBOLS and ctlPRINT;
	ctlPAGING = ctlPAGING and ctlPRINT;
end;


initLine: procedure public;
	startLine$p = inCh$p + 1;	
	lineChCnt = 0;
	if needToOpenFile then
		call openSrc;

	b68AD, has16bitOperand, isControlLine, b689B, b687F, inExpression, expectingOperands, b6881, gotLabel, haveUserSymbol,
	inDB, inDW, b6B32, showAddr, b6884,
$IF OVL4
	b9059, b9060, 
$ENDIF
	b6885 = bZERO;

	atStartLine, expectingOpcode, b6B34, b6B35 = bTRUE;
	ctlEJECT, b6857, tokenIdx,
$IF OVL4
	b9058, argNestCnt,
$ENDIF
	tokenSize(0), tokenType(0), valType, b6859, b6742, b6855 = bZERO;

	asmErrCode = ' ';
$IF OVL4
	macro$p = .macroLine;
	w919D = w906A;
	expandingMacro = expandingMacro > 0;
$ENDIF
	tokI = 1;
	srcLineCnt = srcLineCnt + 1;
$IF OVL4
	macro$p = .macroLine;
$ENDIF
	skipping(0) = skipping(0) > 0;
end;


start:
	call getAsmFile;
	phase = 1;
	call resetData;
	call initialControls;
$IF BASE
	if ctlMACROFILE then
	do;
		if physmem < 8001h then
			call runtimeError(5);	 /* memory error */
		if srcfd <> rootfd then
			call closeF(srcfd);
		call closeF(infd);
		ovlFile(12) = '4';		/* use macro asm version */
		call load(.ovlFile, 0, 1, 0, .statusIO);
		call ioErrChk;
	end;
	if physmem > 8001h then
	do;
		if srcfd <> rootfd then
			call closeF(srcfd);
		call closeF(infd);
		ovlFile(12) = '5';		/* use big memory asm version */
		call load(.ovlFile, 0, 1, 0, .statusIO);
		call ioErrChk;
	end;

	if MacroDebugOrGen then			/* attempt to use macro features */
		call runtimeError(2);		/* command error */
$ELSEIF OVL4
	macrofd = inOpen(.asmax$ref, 3);
$ENDIF

	if ctlOBJECT then
	do;
		call delete(.objFile, .statusIO);
		objfd = inOpen(.objFile, 3);
	end;

	if ctlXREF then
	do;
		xreffd = inOpen(.asxref$tmp, 2);
		outfd = xreffd;
	end;

	call doPass;
	phase = 2;
	if ctlOBJECT then
	do;
		CHKOVL$2;	/* for small version load in overlay 2 for writeRec & writeModhdr */
		if r$extnames1.len > 0 then
			call writeRec(.r$extnames1);	/* in overlay 2 */

		if externId = 0 then
			call writeModhdr;		/* in overlay 2 */
$IF NOT BASE
		call initRecTypes;
$ENDIF
	end;
$IF BASE
	if not ctlOBJECT or ctlPRINT then
$ENDIF
	do;
		if ctlPRINT then
			outfd = inOpen(.lstFile, 2);
		CHKOVL$3;
		call resetData;
		call initialControls;
		call doPass;
	end;
	if ctlPRINT then
	do;
		CHKOVL$1;
		call asmComplete;
		call flushout;
	end;

	if ctlOBJECT then
	do;
$IF BASE
		phase = 3;
		CHKOVL3;
		call resetData;
		call initRecTypes;
		call initialControls;
		call doPass;
		CHKOVL2;
$ENDIF
		call ovl11;
		call writeModend;
	end;

	if not strUCequ(.aCo, .lstFile) then
	do;
		CHKOVL$1;
		call ovl9;
	end;
	CHKOVL$1;
	call ovl10;
end;

