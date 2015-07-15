global: do;
$IF BASE
$include(glb.ipx)
$ELSEIF OVL4
$include(glb4.ipx)
$ELSE
$include(glb5.ipx)
$ENDIF

$IF OVL4
declare	IN$BUF$SIZE lit '512',
	OUT$BUF$SIZE lit '512';
$ELSEIF OVL5
declare	IN$BUF$SIZE lit '2048',
	OUT$BUF$SIZE lit '2048';
$ELSE
declare	IN$BUF$SIZE lit '200',
	OUT$BUF$SIZE lit '128';
$ENDIF

$IF OVL4
declare	mem(1) byte public at(0),
	b$8FD5(127) byte public,
	w$9054 address public,
	off$9056 address public initial(.b$8FD5),
	b$9058 bool public initial(FALSE),
	b$9059 byte public,
	b$905A byte public,
	b$905B byte public,
	b$905C byte public,
	b$905D bool public initial(FALSE),
	b$905E byte public,
	b$905F byte,
	b$9060 byte public,
	b$9061 byte public,
	b$9062 byte public,
	b$9063 byte public,
	b$9064 byte public,
	b$9065 byte public,
	b$9066 byte public,
	argNestCnt byte public initial(0),
	w$9068 address public,
	w$906A address public,
	/*
		mtype has the following values
		1 -> IRP
		2 -> IRPC
		3 -> REPT
		4 -> ???
		5 -> ???
	*/
		
	macroStk(10) MACRO$T public,
	(tmac$macroCondSP, tmac$ifDepth,  tmac$mtype, tmac$b3) byte public at (.macroStk(0)),
	(tmac$w4, tmac$buf$p) address public at (.macroStk(0).w4),
	tmac$blk address public  at (.macroStk(0).blk) initial(0FFFFh),
	(tmac$w10, tmac$w12, tmac$w14) address public at (.macroStk(0).w10),
	w$910C address,
	curMacroBlk address public initial(0FFFFh),
	nxtMacroBlk address public initial(0),
	maxMacroBlk address public initial(0),
	w$9114 address public,
	macroBuf(129) byte public,
	w$9197 address public,
	w$9199 address public,
	w$919B address public,
	w$919D address public,
	w$919F address public,
	b$91A1 byte public initial(0),
	b$91A2 byte public initial(0),

	b$91A3 byte public initial(81h),
	b$91A4(*) byte public initial(3Fh, 3Fh, 0, 0, 0, 0, 80h);
$ENDIF
/* ov4 compat 2C8C */
declare	w651F address public,
	b6521 byte public,
	w6522 address public,
	b6524 byte public,
	b6525 byte public,
	fixIdxs(4) byte public initial(0, 0, 0, 0),
	(fix22Idx, fix24Idx, fix20Idx, fix6Idx) byte public at (.fixIdxs),
	extNamIdx byte public initial(0),
	b652B(4) bool public initial(TRUE, TRUE, TRUE, TRUE),
	b652F byte public initial(0FFh),
	r$eof EOF$T public initial(0Eh, 0),
	r$extnames1 EXTNAMES$T public initial(18h, 0, 0),
	moduleNameLen byte public initial(6),
	r$extnames2 EXTNAMES$T public initial(18h, 0, 0),
	r$content CONTENT$T public at(.r$extnames2),
	r$publics PUBLICS$T public initial (16h, 1, 0, 0, 0),
	r$reloc RELOC$T public at(.r$publics),
	r$interseg INTERSEG$T public,
	r$extref EXTREF$T public,
	r$modend MODEND$T public initial( 4, 4, 0),
	wZERO address public initial(0),
	pad6741 byte initial(0ah),
	b6742 byte public initial(0),
	b6743 byte public initial(0),
	b6744 byte public,
	startSeg byte public initial(1),
	padb6746 byte,
	activeSeg byte public,
	b6748 byte public initial(0),
	b6749 byte public initial(0),
	b674A(2) byte public,
	alignTypes(4) byte public initial(3, 3, 3, 3),
	externId address public,
	w6752 address public,
	b6754 byte public initial(0),
	startDefined byte public initial(0),
	startOffset address public initial(0),
	tokenSP byte public initial(0),
	lineBuf(128) byte public,
	tokStart(9) address public initial(.lineBuf),
	curTokStart address public at(.tokStart),
	tokenSym(9) address public initial(0),
	curTokenSym$p address public at(.tokenSym(0)),
	curTokenSym based curTokenSym$p TOKENSYM$T,
	tokenType(9) byte public,
	tokenSize(9) byte public initial(0),
	tokenAttr(9) byte public,
	tokenSymId(9) address public,
/* end */
	endLineBuf address public initial(.tokStart),
	ifDepth byte public initial(0),
	skipping(9) byte public,
	inElse(9) byte public;
$IF OVL4
declare	macroCondSP byte public initial(0),
	macroCondStk(17) byte public,
	b$94DD(2) byte;
$ENDIF
declare	opSP byte public,
	opStack(17) byte public,
/* ov4 compat 2CA0 */
	(accum1, accum2) address public,
	(accum1$lb, accum1$hb, accum2$lb, accum2$hb) byte public at(.accum1),
	b6855 byte public,
	b6856 byte public,
	b6857 byte public,
	b6858 byte public,
	b6859 byte public,
	w685A address public,
	w685C address public,
	curChar byte public initial(0),
	reget byte public initial(0),
	lookAhead byte public,
	pad6861 byte initial(0),
	symTab(3) address public,
	endSymTab(3) address public,
	symHighMark address public,
	w6870 address public,
	b6872 byte public initial(0),
	b6873(6) byte public,
	b6879(6) byte public,
	b687F byte public,
	b6880 byte public,
	b6881 byte public initial(0),
	b6882 byte public initial(0),
	b6883 byte public initial(0),
	b6884 byte public initial(0),
	b6885 byte public initial(0),
	objfd address public,
	xreffd address public,
	infd address public,
	outfd address public,
	macrofd address public,
	statusIO address public,
	openStatus address public,
	pad6894 address initial(0FFFFh),
	asmErrCode byte public initial(' '),	
	b6897 byte public initial(0),
	primaryValid byte public initial(TRUE);
/* end */
$IF BASE
declare	pad6899 byte initial(0);
$ENDIF
/* ov4 compat 2C9F */
declare	b689A byte public,
	b689B byte public,
	b689C byte public,
	pad689D(2) byte,
	curCol byte public initial(1),
	w68A0 address public,
	w68A2 address public,
	lineCnt address public,
	w68A6 address public,
	pageCnt address public,
	pad68AA byte,
	b68AB byte public,
	pad68AC byte,
	b68AD byte public initial(0),
	b68AE byte public initial(FALSE),
	tokStr(7) byte public initial(0, 0, 0, 0, 0, 0, 0),
	w68B6 address public initial(IN$BUF$SIZE),
	b68B8(IN$BUF$SIZE) byte public,
	outbuf(OUT$BUF$SIZE) byte public,
/* end */
	b6A00 byte public,
/* ov4 compat 2F57 */
	out$p address public initial(.outbuf),
	endOutBuf address public,
	pad6A05 address initial(.outbuf),
	pad6A07 byte initial(0),
	objFile(15) byte public initial('               '), /* 15 spaces */
	lstFile(15) byte public initial('               '), /* 15 spaces */
	asxref$tmp(*) byte public initial(':F0:ASXREF.TMP '),
	asxref(*) byte public initial(':F0:ASXREF '),
	asmax$ref(*) byte public initial(':F0:ASMAC.TMP '),
	w6A4E address public initial(1),
	bp6A4E(2) byte public at(.w6A4E),
	pad6A50(2) byte initial('  '),		/* protects for very big files */
	asciiLineNo(4) byte public initial('   0'),
	b6A56 byte public,
	b6A57(4) bool public initial('   0'),
	ctlDEBUG bool public initial(0),	/* DEBUG */
	controls(1) byte public at(.ctlDEBUG),
	ctlMACRODEBUG bool public initial(FALSE),	/* MACRODEBUG */
	ctlXREF bool public initial(FALSE),	/* XREF */
	ctlSYMBOLS bool public initial(TRUE),	/* SYMBOLS */
	ctlPAGING bool public initial(TRUE),	/* PAGING */
	ctlTTY bool public initial(FALSE),	/* TTY */
	ctlMOD85 bool public initial(FALSE),	/* MOD85 */
	ctlPRINT bool public initial(TRUE),	/* PRINT */
	ctlOBJECT bool public initial(TRUE),	/* OBJECT */
	ctlMACROFILE bool public initial(FALSE),	/* MACROFILE */
	ctlPAGEWIDTH byte public initial(120),	/* PAGEWIDTH */
	ctlPAGELENGTH byte public initial(66),	/* PAGELENGTH */
	ctlINLCUDE byte initial(0),		/* INCLUDE */
	ctlTITLE bool public initial(0),	/* TITLE */
	ctlSAVE byte,				/* SAVE */
	ctlRESTORE byte,			/* RESTORE */
	ctlEJECT bool public initial(FALSE),	/* EJECT */
	ctlLIST bool public initial(TRUE),	/* LIST */	/* SAVE/RESTORE act on next 3 opts */
	ctlGEN bool public initial(TRUE),	/* GEN */
	ctlCOND bool public initial(TRUE),	/* COND */
	b6A6F byte public initial(TRUE),
	titleLen byte public initial(0),
	b6A71 byte public,
	pad6A72(3) byte initial(120, TRUE),
	controlSeen(12) bool public initial(0,0,0,0,0,0,0,0,0,0,0,0),
	saveStack(24) byte public,
	saveIdx byte public initial(0),
	ctlTITLESTR(64) byte public,
	tokBufLen byte public,
	tokType byte public,
	controlId byte public,
	tokBuf(64) byte public,
	tokBufIdx byte public initial(0),
	w6B1E address public,
/* end */
	b6B20$9A77 byte public initial(FALSE);
$IF NOT OVL4
declare	MacroDebugOrGen byte public initial(0);
$ENDIF
/* ov4 compat 2F56 */
declare	scanCmdLine byte public,
	b6B23 byte public,
	b6B24 byte public,
	b6B25 byte public,
	has16bitOperand byte public,
	phase byte public,
	b6B28 byte public,
	b6B29 byte public,
	opType byte public,
	op byte public,
	b6B2C byte public,
	b6B2D byte public,
	finished byte public,
	b6B2F byte public,
	b6B30 byte public,
	b6B31 byte public,
	b6B32 byte public,
	b6B33 byte public,
	b6B34 byte public initial(TRUE),
	b6B35 byte public initial(TRUE),
	b6B36 byte public initial(FALSE),
	segSize(5) address public initial(0, 0, 0, 0, 0),	/* note treated as array */
	maxSegSize(3) address public initial(0, 0, 0),
	cmdLineBuf(129) byte public,
	actRead address public,
	errCnt address public,
	padw6BCB(2) byte,
	w6BCE address public,
	pad6BD0(3) byte initial(0),
	azero address public initial(0),
	cmdch$p address public initial(.cmdLineBuf),
	controls$p address public,
	b6BD9 byte public initial(FALSE),
	b6BDA byte public,
	ii byte public,
	b6BDC byte public,
	jj byte public;
/* end */
$IF OVL4
declare	b$9B34 byte initial(0);
$ENDIF
/* ov4 compat 2F57 */
declare	curFileName$p address public,
	w6BE0 address public,
	bp6BE0(2) byte public at(.w6BE0),
	w6BE2 address public;
/* end */
end;
