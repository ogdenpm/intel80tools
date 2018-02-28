	name reboot
	public	trap0B
	public	vec0E
	public	reboot
	public	jmpto
	public	isisE		; isis$E
	public	usrSP
	public	uStatP		; userStatus$p

	public	IOCHK		; bios entry points
	public	IOSET
	public	CI
	public	RI
	public	CO
	public	PO
	public	LO
	public	CSTS

	extrn	abortx
	extrn	open, close, delete, read, lined, write
	extrn	seek, load, rename, consol, exit, attrib
	extrn	rescan, err, whocon, path, getatt
	extrn	@P0101,@P0034
	extrn	@P0018		; not used but need to force load to align binaries
	extrn	clsAll
	extrn	diskio, warn

	extrn	cli
	extrn	gblSev		; global$severity
	extrn	aft, dkCfTb

CI	equ	0F803H
RI	equ	0F806H
CO	equ	0F809H
PO	equ	0F80CH
LO	equ	0F80FH
CSTS	equ	0F812H
IOCHK	equ	0F815H
IOSET	equ	0F818H

	DSEG
	db	'(C)1982INTELCORP'
	ds	40		; system stack	
sysstk:
	ds	16		; user stack
usrstk:
	dw	0
param:	dw	0,0,0,0,0	; up to 5 parameters
pTypes:	db	10011b, 10b, 11b, 11010b
	db	1010b, 11100b, 10001b, 111b
	db	111b, 1, 1001b, 10b, 10b
	db	110b, 111b, 101b, 111b
b2AB1:	db	0ffh, 0, 0, 0
	db	0, 0, 0, 0
	db	0, 0, 0, 0
	db	0, 0, 0, 0
	db	0, 0, 0, 0

	CSEG			; locate sets code base to 0
	org 8h
rst1:	jmp	restrt		; restart 1
trap0B:	ret			; trap routine ?
	db	0
	ds	1
vec0E:	dw	0

	org	3Eh
	db	4,2		; isis version?
isis40:	jmp	isis

	pop	b		; ?
usrSP:	dw	0
uStatP: dw 0		; userStatus$p
isisE:	db	0

bootld:	db	80h		; iocw
	db	4		; ioins - READ
	db	26		; nsec - size of t0boot.bin
	db	0		; tadr - track of t0boot.bin
	db	1		; sadr - start sector of t0boot.bin
	dw	MEMORY		; buf - 3000, where t0boot.bin loads

restrt:	mvi	a,20h
	out	0FDh


;	reboot: PROCEDURE PUBLIC;
;		output(0FCh) = 0FEh;
;		ENABLE;
;		ISIS$E = 0;
;		trap0B = 0c9h;	/* make sure trap is a null sub routine */
;		STACKPTR = sysstk;
;		call close$all(AFT$BOTTOM);	/* aliased to clsall */
;		call diskio(cli(2) and 0fh, .bootLd);	/* cli(2) = boot drive */
;		call jmpto(.MEMORY, STACKPTR);
;	end;
;	
;	jmpto: PROCEDURE(entry, sysstk) PROCEDURE PUBLIC;
;		STACKPTR = sysstk;
;		goto entry;
;	end;

AFTBTM	equ	10

reboot:
	mvi	a, 0FEh
	out	0FCh
	ei
	lxi	h, isisE
	mvi	m, 0
	lxi	h, trap0B
	mvi	m, 0C9h		; a return instuction
	lxi	h, sysstk
	sphl
	mvi	c, AFTBTM
	call	clsAll
	lda	cli+2		; pick up boot device
	ani	0Fh
	mov	c, a
	lxi	d, bootLd
	call	diskio
	lxi	h, 0
	dad	sp
	xchg			; de = current stack
	lxi	b, MEMORY	; goto to isis.t0

jmpto:				; bc = entry point, de = stack
	xchg
	sphl			; set stack
	mov	h, b		; goto entry point
	mov	l, c
	pchl


; in plm this is isis(command, param$ptr)

isis:	mov	a, c		; check for 'E' command
	cpi	'E'
	jnz	isis1
	ldax	d		; stuff low byte of 1st parameter into isis$E
	sta	isisE
	ret

isis1:	lxi	h, 0		; save user's own stack and use internal stack
	dad	sp
	shld	usrSP
	lxi	h, usrstk
	sphl
	cpi	'D'		; 'D' command?
	jnz	isis2
	xchg
	mov	c, m		; call diskio(param(0), param(1)
	inx	h
	inx	h
	mov	e, m
	inx	h
	mov	d, m
	call	diskio
	jmp	retusr

isis2:	cpi	17		; map op 17 to op 15	getatt
	jnz	isis3
	mvi	a, 15

isis3:	cpi	28		; map op 28 to op 16
	jnz	isis4
	mvi	a, 16

isis4:	mov	c, a		; c is the command
	mvi	b, 0
	lxi	h, pTypes	; get the parameter type info
	dad	b
	mov	a, m
	xchg			; hl is param$ptr
	lxi	d, param	; de is address local param copy
	jmp	isis5

pcheck:	rar			; carry indicates this is an address param
	push	psw
	mov	a, m		; copy the next parameter to local copy
	stax	d
	inx	h
	inx	d
	mov	a, m
	stax	d
	inx	h
	inx	d
	jnc	pok		; not address param
	sui	30h		; check	if address < 3000h
	jnc	pok		; call abortx(33)
	mvi	c, 21h
	call	abortx

pok:	pop	psw		; restore type info

isis5:	ana	a	; check for any more params - note last will be status return address
	jnz	pcheck
	dcx	h	; get the status return address
	mov	d, m
	dcx	h
	mov	e, m
	xchg
	shld	uStatP		; userStatus$p
	mov	m, a		; userStatus = 0
	inx	h
	mov	m, a
	lxi	h, gblSev	; global$severity = 0
	mov	m, a
	lhld	param		; push parameters 0, 1, 2 onto the stack read for call in handler
	push	h
	lhld	param+2
	push	h
	lhld	param+4
	push	h
	lxi	h, cmdJmp	; get the command handler
	dad	b
	dad	b
	mov	e, m
	inx	h
	mov	d, m
	xchg
	mov	a, c
	cpi	17		; if command >= 17 then call abortx(18)
	mvi	c, 12h
	cnc	abortx
	pchl			; else goto command handler

; the command handlers
;###############################
cOpen:	lhld	param		; users conn = 255
	mvi	m, 0FFh
	inx	h
	mvi	m, 0
	pop	b		; access parameter
	pop	d		; path$p parameter
	pop	h		; conn$p parameter
	push	d		; the file parameter
	lhld	param+6		; echo parameter
	xchg
	call	open		; call open(path$p, access, echo)
	lhld	param		; update the users conn
	mov	m, a
	inx	h
	mvi	m, 0
	jmp	retusr

;###############################
cClose:	call	aftChk		; check conn is valid
	mvi	a, 12
	lxi	d, param
	call	@P0101		; conn - 12
	jc	close1		; jmp if conn < 12
	pop	h		; non existant parameter
	pop	h		; status$p
	pop	h		; conn pre aftChk adjustment
	lhld	param		; conn
	mov	b, h
	mov	c, l
	call	close		; call close(conn)

close1:	jmp	retusr

;###############################
cDelet:	pop	h		; non existant parameter
	pop	h		; status$p
	pop	b		; path$p
	call	delete		; call delete(path$p)
	jmp	retusr

;###############################
cRead:	call	aftChk
	lxi	d, 28		; size of aft entry
	lhld	param
	call	@P0034
	lxi	b, aft + 3	; hl = .aft(conn).edit
	dad	b
	mov	a, m
	cpi	0		; edit = 0 ?
	pop	b		; count
	pop	d		; buff$p
	pop	h		; conn pre aftChk adjustment
	lhld	param		; conn
	push	h
	push	d
	lhld	param+6		; actual$p
	xchg
	jnz	read1		; edit set so do line edit
	call	read		; call read(conn, buff$p, count, actual$p)
	jmp	read2

read1:	call	lined		; do line editing

read2:	jmp	retusr		; all done

;###############################
cWrite:	call	aftChk
	pop	d		; count
	pop	b		; buff$p
	pop	h		; conn pre aftChk adjustment
	lhld	param		; conn
	push	h
	call	write		; call write(conn, buff$p, count)
	jmp	retusr

;###############################
cSeek:	call	aftChk
	pop	b		; block$p
	pop	d		; mode
	pop	h		; conn  pre aftChk adjustment
	lhld	param		; conn
	push	h
	push	d
	lhld	param+6		; byte$p
	xchg
	call	seek		; call seek(conn, mode, block$p, byte$p)
	jmp	retusr

;###############################
cLoad:	pop	b		; switch
	lhld	param+6		; entry$p
	xchg
	call	load		; call load(path$p, load$offset, switch, entry$p)
	jmp	retusr

;###############################
cRenam:	pop	h		; status$p
	pop	d		; new$p
	pop	b		; old$p
	call	rename		; call rename(old$p, new$p)
	jmp	retusr

;###############################
cConso:	pop	h		; status$p
	pop	d		; co$p
	pop	b		; ci$p
	call	consol		; call console(ci$p, co$p)
	jmp	retusr

;###############################
cExit:	pop	h		; non existant parameter
	pop	h		; non existant parameter
	pop	h		; non existant parameter
	call	exit		; call exit
	jmp	retusr

;###############################
cAttri:	pop	d		; on$off
	pop	b		; attrib
	call	attrib		; call attrib(path$p, attrib, on$off)
	jmp	retusr

;###############################
cResca:	call	aftChk
	pop	h		; non exisitant parameter
	pop	h		; status$p
	pop	h		; conn pre aftChk adjustment
	lhld	param		; conn
	mov	b, h
	mov	c, l
	call	rescan		; call rescan(conn)
	jmp	retusr

;###############################
cError:	pop	h		; non existant parameter
	pop	h		; non existant parameter
	pop	d		; error$num
	mvi	c, 1
	call	err		; call err(1, error$num)
	jmp	retusr

;###############################
cWhoco:	pop	h		; non existant parameter
	pop	d		; buff$p
	pop	b		; conn
	call	whocon		; call whocon(conn, buff$p)
	jmp	retusr

;###############################
cSpath:	pop	h		; status$p
	pop	d		; info$p
	pop	b		; path$p
	call	path		; userStatus = path(path$p, info$p)
	lhld	uStatP
	mov	m, a
	inx	h
	mvi	m, 0
	lhld	param+2		; info.deviceNo
	mov	a, m		; if deviceNo >= 10 then 
	cpi	0Ah
	jc	spath1
	sui	4		;	info.deviceNo = info.deviceNo - 4
	mov	m, a
	jmp	spath3

spath1:	cpi	6		; else if 6 <= deviceNo and deviceNo <= 9 then
	jc	spath3
	mvi	a, 9
	cmp	m
	jc	spath3
	lda	dkCfTb	; if dk$cf$tb(0) = 4 then /* hard disk */
	cpi	4
	jnz	spath2
	lhld	param+2		;	info.deviceNo = info.deviceNo + 19
	mvi	a, 19
	add	m
	mov	m, a
	jmp	spath3

spath2: lhld	uStatP		; else
	mvi	m, 4		;	userStatus = 4

spath3:	jmp	retusr

;###############################
cGetat:	pop	h		; status$p
	pop	d		; attrib$p
	pop	b		; file$p
	call	getatt		; call getatt(file$p, attrib$p)
	jmp	retusr

;###############################
cUnkwn:	lhld	aft + 4 + (28 * 11)	; console line in buffer aft(11).lbuf
	xchg
	lhld	param		; return using ptr in param0
	mov	m, e
	inx	h
	mov	m, d
	lxi	d, b2AB1	; local buffer unknown use
	lhld	param+2		; return using ptr in param1
	mov	m, e
	inx	h
	mov	m, d
	jmp	retusr


cmdJmp:	dw cOpen		; command dispatch table
	dw cClose
	dw cDelet
	dw cRead
	dw cWrite
	dw cSeek
	dw cLoad
	dw cRenam
	dw cConso
	dw cExit
	dw cAttri
	dw cResca
	dw cError
	dw cWhoco
	dw cSpath
	dw cGetat
	dw cUnkwn


retusr:	lhld	usrSP		; restore user's own stack
	sphl
	ret			; all done


aftChk: lhld	param		; pick up conn parameter
	lxi	d, 10
	mov	a, l
	sui	8
	mov	a, h
	sbb	d
	mvi	c, 2
	cnc	warn		; if conn >= 8 then call warn(BAD$AFT$NO)
	dad	d
	shld	param		; conn = conn + 10
	mvi	e, 1Ch
	call	@P0034
	lxi	b, aft
	dad	b
	mov	a, m		; if aft(conn).empty then call warn(BAD$AFT$NO)
	rar
	mvi	c, 2
	cc	warn
	ret

	end

