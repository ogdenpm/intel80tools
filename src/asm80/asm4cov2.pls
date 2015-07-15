asm4c$ov2: do;
$IF OVL4
$include(asm4c.ipx)
$ELSE
$include(asmov2.ipx)
$ENDIF

declare	controlTable(*) byte data(1, 2, 1, 3),
	off$6D72(*) address data(.r$publics.len, .r$interseg.len, .r$extref.len, .r$content.len),
	b6D7A(*) byte data(7Bh, 3Ah, 39h, 7Ch),
	b6D7E(*) byte data(10, 12h, 40h); /* 11 bits 00010010010 index left to right */


declare	r$modhdr MODHDR$T initial(2), (dta$p, recSym$p) address;



writeRec: procedure(rec$p) public;
	declare rec$p address,
		len$p address, recLen address,
		i byte, crc byte;
	declare len based len$p address;
	declare ch based len$p byte;

	len$p = rec$p + 1;
	recLen = (len := len + 1) + 3;	/* include crc byte + type + len word */
	crc = 0;			/* crc */
	len$p = len$p - 1;
	do i = 2 to recLen;
		crc = crc - ch;
		len$p = len$p + 1;
	end;
	ch = crc;			/* insert crc byte */
	call write(objfd, rec$p, recLen, .statusIO);
	call ioErrChk;
end;


sub$6DF1: procedure byte;
	declare b7597 byte;
	if ((b7597 := tokenAttr(b6A56)) and 5Fh) = 0 then
		return 3;
	if (b7597 and 40h) <> 0 then
		return 2;
	if (b6521 := b7597 and 7) = 0 then
		return 3;
	return (b6521 <> activeSeg) and 1;
end;


sub$6E32: procedure public;
	declare i byte;
	declare wrd based dta$p address;
	do i = 0 to 3;
		ii = (i - 1) and 3;
		dta$p = off$6D72(ii);
		if wrd > controlTable(ii) then
			call writeRec(dta$p - 1);

		wrd = controlTable(ii);
		fixIdxs(ii) = 0;
		if b6525 <>  ii then
			b652B(ii) = TRUE;
	end;
	r$content.offset = w6752 + segSize(r$content.segid := activeSeg);
	r$publics.segid = b6524;
	r$interseg.segid = tokenAttr(b6A56) and 7;
	r$interseg.hilo, r$extref.hilo = b6524;
end;



sub$6EE1: procedure;
	declare w7599 address;

	declare wrd based dta$p address;

	dta$p = off$6D72(b6525 := sub$6DF1);
	if wrd > b6D7A(b6525) or r$content.len + tokenSize(b6A56) > 7Ch then
		call sub$6E32;

	if b652F then
	do;
		b652F = 0;
		r$content.offset = segSize(r$content.segid := activeSeg) + w6752;
	end;
	else if r$content.segid <> activeSeg or (w7599 := r$content.offset + fix6Idx) <> segSize(activeSeg) + w6752
	  or w7599 < r$content.offset then
		call sub$6E32;


	do case b6525;
/* 0 */		do;
			if b652B(0) then
			do;
				b652B(0) = FALSE;
				r$publics.segid = b6524;
			end;
			else if r$publics.segid <> b6524 then
				call sub$6E32;
		end;
/* 1 */		do;
			if b652B(1) then
			do;
				b652B(1) = FALSE;
				r$interseg.segid = tokenAttr(b6A56) and 7;
				r$interseg.hilo = b6524;
			end;
			else if r$interseg.hilo <> b6524 or (tokenAttr(b6A56) and 7) <> r$interseg.segid then
				call sub$6E32;
		end;
/* 2 */		do;
			if b652B(2) then
			do;
				b652B(2) = FALSE;
				r$extref.hilo = b6524;
			end;
			else if r$extref.hilo <> b6524 then
				call sub$6E32;

		end;
/* 4 */		;
	end;
end;


sub$704D: procedure;
	declare i byte;

	declare ch based w651F byte;
	do i = 1 to tokenSize(b6A56);
		r$content.dta(fix6Idx) = ch;
		fix6Idx = fix6Idx + 1;
		w651F = w651F + 1;
	end;
	r$content.len = r$content.len + tokenSize(b6A56);
end;



sub$709D: procedure;
	r$reloc.len = r$reloc.len + 2;
	r$reloc.dta(fix22Idx) = w6522;
	fix22Idx = fix22Idx + 1;
end;


sub$70C1: procedure;
	r$interseg.len = r$interseg.len + 2;
	r$interseg.dta(fix24Idx) = w6522;
	fix24Idx = fix24Idx + 1;
end;

sub$70E5: procedure;
	r$extref.dta(fix20Idx) = tokenSymId(b6A56);
	r$extref.dta(fix20Idx + 1) = w6522;
	r$extref.len = r$extref.len + 4;
	fix20Idx = fix20Idx + 2;
end;

sub$7131: procedure;
	b6524 = shr(tokenAttr(b6A56) and 18h, 3);
	w6522 = segSize(activeSeg) + w6752;
	if not (b6B23 or b6B24) and (tokenSize(b6A56) = 2 or tokenSize(b6A56) = 3) then
		w6522 = w6522 + 1;
	call sub$6EE1;
	w651F = w68A2;
	call sub$704D;
	do case sub$6DF1;
/* 0 */ 	call sub$709D;
/* 1 */		call sub$70C1;
/* 2 */		call sub$70E5;
/* 3 */		;
	end;
end;


writeExtName: procedure public;
	declare i byte;

	if r$extnames1.len + 9 > 125 then	/* check room for extName */
	do;
		call writeRec(.r$extnames1);	/* flush existing extNam Record */
		r$extnames1.type = 18h;
		r$extnames1.len = 0;
		extNamIdx = 0;
	end;
	r$extnames1.len = r$extnames1.len + b6744 + 2;	/* update length for this ref */
	r$extnames1.dta(extNamIdx) = b6744;		/* write len */
	extNamIdx = extNamIdx + 1;
	do i = 0 to b6744;			/* and name */
		r$extnames1.dta(extNamIdx + i) = b6873(i);
	end;

	r$extnames1.dta(extNamIdx + b6744) = 0;	/* and terminating 0 */
	extNamIdx = extNamIdx + b6744 + 1;	/* update where next ref writes */
end;

writeSymbols: procedure(isPublic);			/* isPublic= TRUE -> PUBLICs else LOCALs */
    declare isPublic byte;
    declare segId byte;
    declare symb based curTokenSym$p (1) byte;

    addSymbol: procedure;
        declare offsetInSeg$p address;
        declare symNam based dta$p (1) byte;
        declare len based recSym$p byte;
        declare symOffset based recSym$p address;
        declare offsetInSeg based offsetInSeg$p address;

        if (symb(1) and 40h) <> 0 then
            return;
        offsetInSeg$p = curTokenSym$p - 2;
        symOffset = offsetInSeg; 
        call unpackToken(curTokenSym$p - 6, (dta$p := (recSym$p := recSym$p + 2) + 1));
        symNam(6) = ' ';	/* trailing space to ensure end */
        len = 0;

        do while symNam(0) <> ' ';	/* find length of name */
            len = len + 1;
            dta$p = dta$p + 1;
        end;
        symNam(0) = 0;			/* terminate name with 0 */
        recSym$p = dta$p + 1;
    end;

    flushSymRec: procedure;
        if (r$publics.len := recSym$p - .r$publics.segid) > 1 then	/* something to write */
            call writeRec(.r$publics);
        r$publics.type = (isPublic and 4) or 12h;			/* PUBLIC or LOCAL */
        r$publics.segid = segId;
        recSym$p = .r$publics.dta;
    end;

    recSym$p = .r$publics.dta;
    do segId = 0 to 4;
        call flushSymRec;
        curTokenSym$p = symTab(1) - 2;		/* point to type byte of user symbol (-1) */

        do while (curTokenSym$p := curTokenSym$p + 8) < endSymTab(1);
        	if recSym$p > .r$publics.dta(114) then		/* make sure there is room */
        		call flushSymRec;

            if (symb(1) and 7) = segId
$IF OVL4
               and symb(0) <> 3Ah and sub$3FA9
$ENDIF
               and not testBit(symb(0), .b6D7E) and
               (not isPublic or (symb(1) and 20h) <> 0) then
       	        call addSymbol;
        end;
        call flushSymRec;
    end;
end;



writeModhdr: procedure public;
	declare w based dta$p address;
	declare b based dta$p byte;
	declare i byte;

	/* fill the module name */
	call move((r$modhdr.dta(0) := moduleNameLen), .aModulePage, .r$modhdr.dta(1));
	dta$p = .r$modhdr + moduleNameLen + 4;
	w = 0;	/* the two xx bytes */
	dta$p = dta$p + 1;	/* past first x byte */

	if segSize(SEG$CODE) < maxSegSize(SEG$CODE) then	/* code segment */
		segSize(SEG$CODE) = maxSegSize(SEG$CODE);
	if segSize(SEG$DATA) < maxSegSize(SEG$DATA) then	/* data segment */
		segSize(SEG$DATA) = maxSegSize(SEG$DATA);

	do i = 1 to 4;
		dta$p = dta$p + 1;
		b = i;		/* seg id */
		dta$p = dta$p + 1;
		w = segSize(i);	/* seg size */
		dta$p = dta$p + 2;
		b = alignTypes(i - 1);	/* aln typ */
	end;
	r$modhdr.len = moduleNameLen + 19;	/* set record length */
	call writeRec(.r$modhdr);
end;

writeModend: procedure public;
	declare lenb byte at (.r$eof.len);
	r$modend.modtyp = startDefined;
	r$modend.segid = startSeg;
	r$modend.offset = startOffset;
	call writeRec(.r$modend);
	lenb = 0;
	call writeRec(.r$eof);
end;

ovl8: procedure public;
	w6752 = 0;
	b689A = 1;
	b6A56 = 1;
	if b6B33 then
		;
	else
	do while b6A56 <> 0;
		b6A56 = sub$4646;
		w68A0 = tokStart(b6A56) + tokenSize(b6A56);
		w68A2 = tokStart(b6A56);
		if isSkipping or not b6B34 then
			w68A0 = w68A2;
		if w68A0 > w68A2 then
		do;
			call sub$7131;
			w6752 = w6752 + tokenSize(b6A56);
		end;
		if not(b6B23 or b6B24) then
			b6A56 = 0;
	end;
end;


ovl11: procedure public;
	if externId <> 0 then
	do;
		call seek(objfd, SEEKABS, .azero, .azero, .statusIO);	/* rewind */
		call writeModhdr;
		call seek(objfd, SEEKEND, .azero, .azero, .statusIO);
	end;
	r$publics.type = 16h;		  /* public declarations record */
	r$publics.len = 1;
	r$publics.segid = SEG$ABS;
	r$publics.dta(0) = 0;
	call writeSymbols(TRUE);	  /* EMIT PUBLICS */
	if ctlDEBUG then
		call writeSymbols(FALSE); /* EMIT LOCALS */
end;
end;
