init: do;
$IF OVL4
$include(asm4e.ipx)
$ELSEIF OVL5
$include(asm5d.ipx)
$ELSE
$include(asmov3.ipx)
$ENDIF

declare aExtents(*) byte public initial(' LSTOBJ'),
    aDebug(*) byte public initial('DEBUG');


cmdSkipWhite: procedure public;
    declare cmdch based cmdch$p byte;
    
    do while (cmdch = ' ' or cmdch = TAB) and cmdch$p <> actRead;
        cmdch$p = cmdch$p + 1;
    end;
end;

getDrive: procedure byte public;
    declare cmdch based cmdch$p byte;

    if cmdch = ':' then
    do;
        cmdch$p = cmdch$p + 2;
        return cmdch;
    end;
    else
        do ii = 0 to 4;		/* case insensitive compare to DEBUG */
        if cmdch <> aDebug(ii) and aDebug(ii) + 20h <> cmdch then
                return '0';	/* must be a file name so drive 0 */
            cmdch$p = cmdch$p + 1;
        end;
    call cmdSkipWhite;
    if cmdch <> ':' then
        return '0';
    cmdch$p = cmdch$p + 2;
    return cmdch;
end;

addExtents: procedure public;
    do ii = 1 to 3;
        lstFile(kk + ii) = aExtents(ii);
        objFile(kk + ii) = aExtents(ii+3);
    end;
end;


/* asmov3 usage include overlay file initiatisation */

getAsmFile: procedure public;
	declare cmdch based cmdch$p byte;

	cmdIsWhite: procedure byte;
		declare cmdch based cmdch$p byte;
		return cmdch = ' ' or cmdch = TAB or cmdch = CR;
	end;

$IF OVL4
	symTab(0) = .extKeywords;	/* extended key words */
$ELSE
	symTab(0) = .stdKeywords;	/* no extended key words */
$ENDIF
	symHighMark, endSymTab(0), symTab(1), endSymTab(1) =
$IF NOT OVL3
						 .MEMORY;
$ELSE
						 .EDATA;
$ENDIF
	call rescan(1, .statusIO);	/* get the command line */
	call ioErrChk;
	call read(1, .cmdLineBuf, 128, .actRead, .statusIO);
	call ioErrChk;
	actRead = actRead + .cmdLineBuf;	/* convert to pointer */
	scanCmdLine = TRUE;		/* scanning command line */
$IF OVL3
	call write(0, .signonMsg, 29h, .statusIO);
	call write(0, .signonMsg, 2, .statusIO);
	call ioErrChk;
$ENDIF
	call cmdSkipWhite;
$IF OVL3
	ovlFile(2),
$ENDIF
	asxref(2) = getDrive; 	/* tem defaults to current drive */

	do while not cmdIsWhite;
		cmdch$p = cmdch$p + 1;
	end;

	call cmdSkipWhite;
	if cmdch = CR then		/* no name !! */
		call runtimeError(4);

	infd = inOpen(cmdch$p, 1);	/* open file for reading */
	rootfd, srcfd = infd;
	ii = TRUE;
	kk = 0;

	do while not cmdIsWhite;	/* copy file name over to the files list */
		files(0).name(kk) = cmdch;
		if ii then		/* and the name for the lst and obj files */
			lstFile(kk), objFile(kk) = cmdch;
		if cmdch = '.' then
		do;
			ii = FALSE;
			call addExtents;	/* add lst and obj file extents */
		end;
		kk = kk + 1;
		cmdch$p = cmdch$p + 1;
	end;
	controls$p = cmdch$p;		/* controls start after file name */
	if ii then			/* no extent in source file */
	do;
		lstFile(kk) = '.';	/* add the . and the extents */
		objFile(kk) = '.';
		call addExtents;
	end;

	files(0).name(kk) = ' ';	/* override current drive for tmp if explict in source file */
	if lstFile(0) = ':' and lstFile(2) <> '0' then
$IF OVL4
		asmax$ref(2),
$ENDIF
		asxref$tmp(2) = lstFile(2);
end;


resetData: procedure public;	/* extended initialisation */

    call initLine;

    b6B33, scanCmdLine, skipping(0), b6B2C, inElse(0), finished, segHasData(0), segHasData(1), b6742,
$IF OVL4
    expandingMacro, b905C, b905E,
$ENDIF
    b6857, needToOpenFile = bZERO;
    b6743, primaryValid, ctlLIST, b6A6F,
$IF OVL4
    ctlGEN,
$ENDIF
    ctlCOND = bTRUE;
$IF OVL4
    macroDepth, b9064, macroCondStk(0), macroCondSP, 
$ENDIF
    saveIdx, lookAhead, activeSeg, ifDepth, opSP, opStack(0) = bZERO;
$IF OVL4
    macroBlkCnt,
$ENDIF
    segSize(SEG$ABS), segSize(SEG$CODE), segSize(SEG$DATA),
    maxSegSize(SEG$ABS), maxSegSize(SEG$CODE), maxSegSize(SEG$DATA), effectiveAddr,
$IF OVL4
    w919B,
$ENDIF
    externId, errCnt = wZERO;
    b6882 = b6882 + 1;
    srcLineCnt, opType, pageCnt, pageLineCnt = 1;
    b68AE = FALSE;
    curChar = ' ';
    do ii = 0 to 11;		/* reset all the control seen flags */
        controlSeen(ii) = 0;
    end;
$IF OVL4
    curMacroBlk = 0FFFFh;
$ENDIF
    if not isPhase1 then	/* close any open include file */
    do;
        if fileIdx <> 0 then
        do;
            call closeF(srcfd);
            call ioErrChk;
            srcfd = rootfd;
        end;

        fileIdx = bZERO;	/* reset files for another pass */
        endInBuf$p = .inBuf;
        inCh$p = endInBuf$p - 1;
        startLine$p = .inBuf;
        call seek(infd, SEEKABS, .azero, .azero, .statusIO);	/* rewind */
        call ioErrChk;
    end;

    w6870 = physmem + 0BFh;
    endOutBuf = .b6A00;
end;

initRecTypes: procedure public;
    r$content.type = OMF$CONTENT;
    r$content.len = 3;
    r$publics.type = OMF$RELOC;
    r$publics.len = 1;
    r$interseg.type = OMF$INTERSEG;
    r$interseg.len = 2;
    r$extref.type = OMF$EXTREF;
    r$extref.len = 1;
end;
end;
