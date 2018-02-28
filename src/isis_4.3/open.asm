; open:
; do;
; $include(open.ipx)
; declare a$access$p literally 'aAcesP';
; declare a$blk$count$p literally 'aBCntP';
; declare a$empty$p literally 'aEmtyP';
; declare a$eof$data$ptr$p literally 'aEoPP';
; declare Aft$Window literally 'AWindo';
; declare a$hdr$blk$p literally 'aHBlkP';
; declare Chk$Write$Protect literally 'CkWriP';
; declare Clear$Buf literally 'ClrBuf';
; declare direct$i$no literally 'dirINo';
; declare Get$Aft$Slot literally 'GASlot';
; declare Get$Block literally 'GetBlk';
; declare Map$Write literally 'MapWri';
; declare Write$Dir$Entry literally 'WDirEn';
; declare abswrite literally 'abs$wr';
; declare AFT$BOTTOM literally '10';
; declare AFT$LAST literally '17';
; declare AFT$T literally 'structure(AFT$T1, AFT$T2, AFT$T3, AFT$T4)';
; declare AFT$T1 literally 'empty BYTE, device BYTE, access BYTE, edit BYTE, lbuf ADDRESS';
; declare AFT$T2 literally 'dbuf ADDRESS, pbuf ADDRESS, data$ptr BYTE, i$no ADDRESS, ptr$ptr BYTE';
; declare AFT$T3 literally 'eofcnt BYTE, ptr$change BYTE, data$change BYTE, blk ADDRESS, blk$seq ADDRESS';
; declare AFT$T4 literally 'ptr$blk ADDRESS, hdr$blk ADDRESS, data$blk$count ADDRESS, dbuf$read BYTE';
; declare ALREADY$OPEN literally '12';
; declare BAD$ACCESS literally '22';
; declare BAD$ECHO literally '25';
; declare BBDEV literally '26';
; declare BOOLEAN literally 'BYTE';
; declare CI$AFT literally '1';
; declare CIDEV literally '27';
; declare CO$AFT literally '0';
; declare CODEV literally '28';
; declare DIRECTORY$FULL literally '9';
; declare DIRECT$T literally 'structure(DIRECT$T1, DIRECT$T2)';
; declare DIRECT$T1 literally 'empty BOOLEAN, file(6) BYTE, ext(3) BYTE, attrib BYTE';
; declare DIRECT$T2 literally 'eof$count BYTE, blk ADDRESS, hdr$blk ADDRESS';
; declare F9DEV literally '9';
; declare FALSE literally '0';
; declare LF literally '0ah';
; declare NO$SUCH$FILE literally '13';
; declare TRUE literally '0ffh';
; declare a$access$p address external;
; declare a$blk$count$p address external;
; declare a$dbuf$p address external;
; declare a$dev$p address external;
; declare a$edit$p address external;
; declare a$empty$p address external;
; declare a$eof$data$ptr$p address external;
; declare aft(18) AFT$T external;
; declare a$hdr$blk$p address external;
; declare a$i$no$p address external;
; declare a$lbuf$p address external;
; declare a$pbuf$p address external;
; declare direct DIRECT$T external;
; declare direct$i$no address external;
; declare pn(1) byte external;
; abs$wr: procedure(block, buffer$ptr) external; declare block address, buffer$ptr address; end;
; procedure(slot$number) external; declare slot$number byte; end;
; attrib: procedure(file$ptr, swid, value) external;
;           declare file$ptr address, swid address, value BOOLEAN; end;
; ChkWriteProtect: procedure external; end;
; ClearBuf: procedure(buf$address) external; declare buf$address address; end;
; del: procedure(disknum) external; declare disknum byte; end;
; dlook: procedure(fn) BOOLEAN external; declare fn address; end;
; Get$Aft$Slot: procedure byte external; end;
; get$block: procedure(diskNum) address external; declare diskNum byte; end;
; get$buf: procedure address external; end;
; MapWrite: procedure(diskNum) external; declare diskNum byte; end;
; rewind: procedure external; end;
; warn: procedure(errcode) external; declare errcode byte; end;
; write: procedure(aftn, buffer, count) external; declare aftn byte, buffer address, count address; end;
; WriteDirEntry: procedure external; end;
; xpath: procedure(p1, p2) external; declare p1 address, p2 address; end;
; declare a$access based a$access$p  byte;
; declare a$blk$count based a$blk$count$p  address;
; declare a$dbuf based a$dbuf$p  address;
; declare a$device based a$dev$p  byte;
; declare a$edit based a$edit$p  byte;
; declare a$empty based a$empty$p  byte;
; declare a$eof$data$ptr based a$eof$data$ptr$p  byte;
; declare a$hdr$blk based a$hdr$blk$p  address;
; declare a$i$no based a$i$no$p  address;
; declare a$lbuf based a$lbuf$p  address;
; declare a$pbuf based a$pbuf$p  address;
        
; supportedAccess(*) BYTE DATA(3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
;                      1, 2, 1, 2, 1, 2, 1, 1, 1, 1,
;                      2, 2, 2, 2, 2, 2, 3, 1, 2);
; NB. data follows code         


	name	OPEN
	public	open
; externals with aliased names
	extrn	aAcesP, aBCntP, aEmtyP, aEoPP, AWindo
	extrn	aHBlkP, CkWriP, ClrBuf, dirINo, GASlot
	extrn	GetBlk, MapWri, WDirEn, absWr
; externals with non aliased names
	extrn	attrib, del, dlook, getBuf, rewind
	extrn	warn, write, xpath
	extrn	aDbufP, aDevP, aEditP, aft, aINoP
	extrn	aLbufP, aPbufP, direct, pn
; plm80lib
	extrn @P0034, @P0097, @P0099
	extrn @P0100, @P0101, @P0103

ACCESS	equ	2	; offset of aft.access
DEVICE	equ	1	; offset of aft.device

	CSEG
; transfer$dir$aft: PROCEDURE;    /* copy data from directory to aft entry */
;            ; PROC  TRANSFERDIRAFT
;     a$eof$data$ptr = direct.eof$count;
dr2aft: LHLD    AEOPP
        LDA     DIRECT+0BH
        MOV     M,A
;     a$blk$count = direct.blk;
        LHLD    ABCNTP
;       PUSH    H	optimised
	xchg
        LHLD    DIRECT+0CH
        XCHG
;       POP     H	optimised
        MOV     M,E
        INX     H
        MOV     M,D
; end;
        RET
        
; open: PROCEDURE(pathname, access$mode, lined$aft) BYTE PUBLIC;
;            ; PROC  OPEN
open:   LXI     H,LINEDA+1H
        MOV     M,D
        DCX     H
        MOV     M,E
        DCX     H
        MOV     M,B
        DCX     H
        MOV     M,C
        DCX     H
        POP     D       
        POP     B       
        MOV     M,B
        DCX     H
        MOV     M,C
        PUSH    D       
;     DECLARE (pathname, access$mode, lined$aft) ADDRESS;
;     DECLARE (aftn, reqAccess, found, temp, i, ival) BYTE;
;     DECLARE temp$buf ADDRESS;
;     DECLARE bytes BASED temp$buf (128) BYTE;
;     DECLARE addr BASED temp$buf (64) ADDRESS;
        
        
        
        
;     if access$mode > 3 or (reqAccess := access$mode) = 0 then
        MVI     A,3H
        LXI     H,MODE
        CALL    @P0103 
        SBB     A
        XCHG
        DCX     H
        MOV     C,M
        INX     H
        MOV     B,M
        LXI     H,REQACC
        MOV     M,C
        LXI     D,0H
        PUSH    PSW
        CALL    @P0097 
        ORA     L
        SUI     1
        SBB     A
        POP     B
        MOV     C,B
        ORA     C
        RAR
        JNC     @1
;         call warn(BAD$ACCESS);
        MVI     C,16H
        CALL    WARN
@1:
;     call xpath(pathname, .pn);
        LHLD    PATHNM
        MOV     B,H
        MOV     C,L
        LXI     D,PN
        CALL    XPATH
;     if (i := supportedAccess(pn(0))) <> 3 then
        LHLD    PN
        MVI     H,0
        LXI     B,SUPACC
        DAD     B
        MOV     A,M
        STA     I
        CPI     3H
        JZ      @2
;         if i <> reqAccess then
        LXI     H,REQACC
;       LDA     I	optimised
        CMP     M
        JZ      @3
;             call warn(BAD$ACCESS);
        MVI     C,16H
        CALL    WARN
@3:
@2:
;     if pn(0) = CIDEV then
        LDA     PN
        CPI     1BH
        JNZ     @4
;         return CI$AFT;
        MVI     A,1H
        RET
@4:
;     if pn(0) = CODEV then
;       LDA     PN	optimised
        CPI     1CH
        JNZ     @5
;         return CO$AFT;
        MVI     A,0H
        RET
@5:
;     call aft$window(aftn := get$aft$slot);
        CALL    GASLOT
        STA     AFTN
        MOV     C,A
        CALL    AWINDO
;     a$access = reqAccess;
        LHLD    AACESP
        LDA     REQACC
        MOV     M,A
;     a$edit = 0;
        LHLD    AEDITP
        mvi	a,0
;       MVI     M,0H	optimised
        mov	m,a
;     if lined$aft <> 0 then
;       MVI     A,0H	optimised
        LXI     D,LINEDA
        CALL    @P0101 
        ORA     L
        JZ      @6
;     do;
;         a$edit = AFT$BOTTOM + low(lined$aft);
        LHLD    LINEDA
        MOV     A,L
        ADI     0AH
        LHLD    AEDITP
        MOV     M,A
;         if aft(a$edit).access <> 2 OR (reqAccess AND 1) = 0 then
;       LHLD    AEDITP	optimised
        MOV     L,M
        MVI     H,0
        LXI     D,1CH
        CALL    @P0034 
        LXI     B,AFT
        DAD     B
        LXI     B,ACCESS
        DAD     B
        MOV     A,M
        SUI     2H
        ADI     255
        SBB     A
        PUSH    PSW     ; 1
        LDA     REQACC
        ANI     1H
        SUI     0H
        SUI     1
        SBB     A
        POP     B       ; 1
        MOV     C,B
        ORA     C
        RAR
        JNC     @7
;             call warn(BAD$ECHO);
        MVI     C,19H
        CALL    WARN
@7:
;     end;
@6:
;     if (a$device := pn(0)) <= F9DEV then
        LHLD    ADEVP
        LDA     PN
        MOV     M,A
        MOV     C,A
        MVI     A,9H
        CMP     C
        JC      @8
;     do;
;         found = dlook(.PN);
        LXI     B,PN
        CALL    DLOOK
        STA     FOUND
;         if found then
;       LDA     FOUND	optimised
        RAR
        JNC     @9
;             do i = AFT$BOTTOM TO AFT$LAST;
        LXI     H,I
        MVI     M,0AH
@27:
        MVI     A,11H
;       LXI     H,I	optimised
        CMP     M
        JC      @28
;                 call aft$window(i);
;       LHLD    I	optimised
;       MOV     C,L
	mov	c,m
        CALL    AWINDO
;                 if not a$empty then
        LHLD    AEMTYP
        MOV     A,M
        RAR
        JC      @10
;                     if a$i$no = direct$i$no and a$device = pn(0) then
        LHLD    AINOP
        LXI     D,DIRINO
        CALL    @P0099 
        ORA     L
        SUI     1
        SBB     A
        LHLD    ADEVP
        PUSH    PSW     ; 1
        LDA     PN
        SUB     M
        SUI     1
        SBB     A
        POP     B       ; 1
        MOV     C,B
        ANA     C
        RAR
        JNC     @11
;                         call warn(ALREADY$OPEN);
        MVI     C,0CH
        CALL    WARN
@11:
@10:
;             end;
@29:
        LXI     H,I
        INR     M
        JNZ     @27
@28:
@9:
;         call aft$window(aftn);
        LHLD    AFTN
        MOV     C,L
        CALL    AWINDO
;         temp = 0;
        LXI     H,TEMP
        MVI     M,0H
;         a$i$no = direct$i$no;
        LHLD    AINOP
;       PUSH    H	optimised
	xchg
        LHLD    DIRINO
        XCHG
;       POP     H	optimised
        MOV     M,E
        INX     H
        MOV     M,D
;         if reqAccess = 1 then
        LDA     REQACC
        CPI     1H
        JNZ     @12
;         do;
;             if not found then
        LDA     FOUND
        RAR
        JC      @13
;                 call warn(NO$SUCH$FILE);
        MVI     C,0DH
        CALL    WARN
@13:
;         end;
        JMP     @14
@12:
;         else
;         do;
;             if not found then
        LDA     FOUND
        RAR
        JC      @15
;             do;
;                 if direct$i$no = 0ffffh then
        LXI     B,0FFFFH
        LXI     D,DIRINO
        CALL    @P0100 
        ORA     L
        JNZ     @16
;                     call warn(DIRECTORY$FULL);
        MVI     C,9H
        CALL    WARN
@16:
;                 direct.blk = 0;
        LXI     H,0H
        SHLD    DIRECT+0CH
;                 direct.eof$count = 128;
        LXI     H,DIRECT+0BH
        MVI     M,80H
;                 direct.attrib = 0;
        DCX     H
        MVI     M,0H
;             end;
@15:
;             call chk$write$protect;
        CALL    CKWRIP
;             call transfer$dir$aft;
        CALL    DR2AFT
;         end;
@14:
;         a$hdr$blk = direct.hdr$blk;
        LHLD    AHBLKP
;       PUSH    H	optimised
	xchg
        LHLD    DIRECT+0EH
        XCHG
;       POP     H	optimised
        MOV     M,E
        INX     H
        MOV     M,D
;         a$dbuf = get$buf;
        CALL    GETBUF
        PUSH    H       ; 1
        LHLD    ADBUFP
        POP     B       ; 1
        MOV     M,C
        INX     H
        MOV     M,B
;         a$pbuf = get$buf;
        CALL    GETBUF
        PUSH    H       ; 1
        LHLD    APBUFP
        POP     B       ; 1
        MOV     M,C
        INX     H
        MOV     M,B
;         if reqAccess = 2 and found then
        LDA     REQACC
        SUI     2H
        SUI     1
        SBB     A
        LXI     H,FOUND
        ANA     M
        RAR
        JNC     @17
;         do;
;             call del(a$device);
        LHLD    ADEVP
        MOV     C,M
        CALL    DEL
;             call aft$window(aftn);
        LHLD    AFTN
        MOV     C,L
        CALL    AWINDO
;             direct.blk = 0;
        LXI     H,0H
        SHLD    DIRECT+0CH
;             direct.eof$count = 128;
        LXI     H,DIRECT+0BH
        MVI     M,80H
;         end;
@17:
;         call transfer$dir$aft;
        CALL    DR2AFT
;         ival = input(0fch);
        IN      0FCH
        STA     IVAL
;         output(0fch) = ival OR 2;
        ORI     2H
        OUT     0FCH
;         if reqAccess <> 1 then  /* not read only */
        LDA     REQACC
        CPI     1H
        JZ      @18
;         do;
;             if (temp := not (reqAccess = 3 and found)) then
        LDA     REQACC
        SUI     3H
        SUI     1
        SBB     A
        LXI     H,FOUND
        ANA     M
        CMA
        INX     H
        MOV     M,A
        RAR
        JNC     @19
;             do;
;                 direct.hdr$blk = get$block(a$device);
        LHLD    ADEVP
        MOV     C,M
        CALL    GETBLK
        SHLD    DIRECT+0EH
;                 a$hdr$blk = direct.hdr$blk;
	xchg		; optimisation for code below
        LHLD    AHBLKP
;       PUSH    H	optimised by xchg above
;       LHLD    DIRECT+0EH
;       XCHG
;       POP     H
        MOV     M,E
        INX     H
        MOV     M,D
;             end;
@19:
;             direct.empty = 0;
        LXI     H,DIRECT
        MVI     M,0H
;             call write$dir$entry;
        CALL    WDIREN
;         end;
@18:
;         call aft$window(aftn);
        LHLD    AFTN
; LER                                                                                                PAGE  11


        MOV     C,L
        CALL    AWINDO
;         call rewind;
        CALL    REWIND
;         if temp then    /* not update + new file */
        LDA     TEMP
        RAR
        JNC     @20
;         do;
;             call clear$buf(a$pbuf);
        LHLD    APBUFP
        MOV     C,M
        INX     H
        MOV     B,M
        CALL    CLRBUF
;             call map$write(a$device);
        LHLD    ADEVP
        MOV     C,M
        CALL    MAPWRI
;             call abswrite(a$hdr$blk, a$pbuf);
        LHLD    AHBLKP
        PUSH    H
        LHLD    APBUFP
        MOV     E,M
        INX     H
        MOV     D,M
        POP     H
        MOV     C,M
        INX     H
        MOV     B,M
        CALL    ABSWR
;         end;
@20:
;         output(0FCh) = ival;
        LDA     IVAL
        OUT     0FCH
;     end;
        JMP     @21
@8:
;     else if a$device <> BBDEV then
        LHLD    ADEVP
        MOV     A,M
        CPI     1AH
        JZ      @22
;     do;
;         do i = AFT$BOTTOM to AFT$LAST;
        LXI     H,I
        MVI     M,0AH
@30:
        MVI     A,11H
;       LXI     H,I	optimisation
        CMP     M
        JC      @31
;             if not aft(i).empty then
	mov	l,m	; optimisation
	mvi	h,0	; reordering
        LXI     D,1CH
;       LHLD    I
;       MVI     H,0	optmisation and minor reorder
   	CALL    @P0034 
        LXI     B,AFT
        DAD     B
        MOV     A,M
        RAR
        JC      @23
;                 if a$device = aft(i).device then
        LHLD    ADEVP
        LXI     D,1CH
        PUSH    H
        LHLD    I
        MVI     H,0
        CALL    @P0034 
        LXI     B,AFT
        DAD     B
        LXI     B,DEVICE
        DAD     B
        POP     B
        LDAX    B
        CMP     M
        JNZ     @24
;                     call warn(ALREADY$OPEN);
        MVI     C,0CH
        CALL    WARN
@24:
@23:
;         end;
@32:
        LXI     H,I
        INR     M
        JNZ     @30
@31:
;         if a$device >= 14h and a$device <= 17h then
        LHLD    ADEVP
        MOV     A,M
        SUI     14H
        SBB     A
        CMA
        PUSH    PSW
        MVI     A,17H
        SUB     M
        SBB     A
        CMA
        POP     B
        MOV     C,B
        ANA     C
        RAR
        JNC     @25
;             do i = 1 to 120;
        LXI     H,I
        MVI     M,1H
@33:
        MVI     A,78H
;       LXI     H,I	optimisation
        CMP     M
        JC      @34
;                 a$empty = FALSE;
        LHLD    AEMTYP
        MVI     M,0H
;                 call write(aftn, .(0), 1);
        LHLD    AFTN
        PUSH    H
        LXI     D,1H
        LXI     B,zero
        CALL    WRITE
;             end;
@35:
        LXI     H,I
        INR     M
        JNZ     @33
@34:
@25:
;     end;
@22:
@21:
;     if lined$aft <> 0 then
        MVI     A,0H
        LXI     D,LINEDA
        CALL    @P0101 
        ORA     L
        JZ      @26
;     do;
;         temp$buf, a$lbuf = get$buf;
        CALL    GETBUF
        SHLD    TMPBUF
        PUSH    H 
        LHLD    ALBUFP
        POP     B
        MOV     M,C
        INX     H
        MOV     M,B
;         bytes(0) = LF;
        LHLD    TMPBUF
        MVI     M,0AH
;         addr(63) = 0ff00h;
        LXI     B,7EH
        LHLD    TMPBUF
        DAD     B
        LXI     B,0FF00H
        MOV     M,C
        INX     H
        MOV     M,B
;         bytes(125) = TRUE;
        LXI     B,7DH
        LHLD    TMPBUF
        DAD     B
        MVI     M,0FFH
;     end;
@26:
;     a$empty = FALSE;
        LHLD    AEMTYP
        MVI     M,0H
;     return aftn - AFT$BOTTOM;
        LDA     AFTN
        SUI     0AH
        RET
; end;
; end;
supAcc:	db 3, 3, 3, 3, 3, 3, 3, 3	; supportedAccess
	db 3, 3, 1, 2, 1, 2, 1, 2
	db 1, 1, 1, 1, 2, 2, 2, 2
	db 2, 2, 3, 1, 2
pathNm:	dw 0			; open arguments pathname
mode:	dw 0			; access$mode
linedA:	dw 0			; lined$aft
aftn:	ds 1			; open local vars
reqAcc:	ds 1
found:	ds 1
temp:	ds 1
i:	ds 1
ival:	ds 1
tmpBuf: dw 0
zero:	dw 0			; 0 used in call to write

	end;
