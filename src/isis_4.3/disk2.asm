	CSEG
	public	dkstat	; dk$stat
	public	rtype	; r$type
	public	rbyte	; r$byte
	public	strtio	; strt$io
	extrn	dkcont	; disk controller

IOCDR1	equ	0F821h
IOCCOM	equ	0F841h
IOCDR2	equ	0F844h

; controller
NOCONT	equ	0
CONT1	equ	1
CONT2	equ	2
CONTIN	equ	3	; integrated single density
CONTHD	equ	4

; and their IO port bases (ISD handled separately)
BASE1	equ	78h
BASE2	equ	88h
BASEHD	equ	68h


; IOC INTERFACE COMMANDS
IOCS	equ	0c1h		; IOC INPUT DBB STATUS PORT
IOCC	equ	0c1h		; IOC OUTPUTY CONTROL COMMAND PORT
IOCI	equ	0c0h		; IOC INPUT DATA (FROM DBB) PORT
IOCO	equ	0c0h		; IOC OUTPUT DATA (TO DBB) PORT
F0	equ	00000100B	; FLAG 0 - SLAVE IS BUSY, MASTER IS LOCKED OUT
IBF	equ	00000010B	; SLAVE INPUT BUFFER IS FULL
OBF	equ	00000001B	; SLAVE OUTPUT BUFFER IS FULL
WPBC	equ	15h		; write parameter block command
WPCC	equ	16h		; write parameter block command continuation
WDBC	equ	17h		; write data block command
WDCC	equ	18h		; reserved
RDBC	equ	19h		; read data block command
RDCC	equ	1ah		; reserved
RRSTS	equ	1bh		; read result status command
RDSTS	equ	1ch		; read device status command

; PSEUDO INTERRUPT INSTRUCTIONS
DISABL	equ	0dh		; disable interrupts
ENABL	equ	05h		; enable interrupts
CPUC	equ	0ffh		; port for these instructions

; DISK INSTRUCTIONS
SEEK	equ	1		; seek instruction
FORMAT	equ	2		; format instruction
RECAL	equ	3		; recalibrate instruction
READ	equ	4		; read data instruction
VERIFY	equ	5		; verify crc instruction
WRITE	equ	6		; write data instruction
WRITED	equ	7		; write deleted data instruction

; Layout of the I/O parameter block (IOPB)
;	Only the first five bytes of the IOPB are transmitted to the ISD
;	IOCW	BYTE	channel word
;	IOINS	BYTE	diskette instruction
;	NSEC	BYTE	number of sectors
;	TADR	BYTE	track address
;	SADR	BYTE 	sector address
;	BUF	ADDRESS	buffer address

dkstat:	lda	dkcont
	ora	a
	rz			; not present
	cpi	CONTHD
	jz	statHD		; hard disk
	cpi	CONTIN
	jnz	dkst1		; jump if not isd
	mvi	b, RDSTS	; use bios to get ISD status
	call	IOCDR1
	ret

dkst1:	cpi	CONT2
	jnz	dkst2
	in	BASE2		; controller 2
	ret

dkst2:	in	BASE1		; controller 1
	ret

statHD:	in	BASEHD
	ret




rtype:	lda	dkcont
	cpi	CONTHD		
	jz	typeHD		; hard disk
	cpi	CONTIN
	jnz	type1		; jump if not isd
	xra	a	; no result type for ISD
	ret

type1:	cpi	CONT2
	jnz	type2
	in	BASE2 + 1	; controller 2
	ret

type2:	in	BASE1 + 1	; controller 1
	ret

typeHD:	in	BASEHD + 1
	ret



rbyte:
	lda	dkcont
	cpi	CONTHD
	jz	byteHD		; hard disk
	cpi	CONTIN
	jnz	rbyte1		; jump if not isd
	mvi	b, RRSTS	; isd use bios
	call	IOCDR1
	ret

rbyte1:	cpi	CONT2
	jnz	rbyte2
	in	BASE2 + 3	; controller 2
	ret

rbyte2:	in	BASE1 + 3	; controller 1
	ret

byteHD:	in	BASEHD + 3
	ret



strtio:	lda	dkcont
	cpi	CONTHD
	jz	strtHD		; hard disk
	cpi	CONTIN
	jnz	strt1		; not isd
	call	isddr		; use standard code
	ret

strt1:	cpi	CONT2
	jnz	strt2
	mov	a, c		; controller 2
	out	BASE2 + 1
	mov	a, b
	out	BASE2 + 2
	jmp	wait

strt2:	mov	a, c		; controller 1
	out	BASE1 + 1
	mov	a, b
	out	BASE1 + 2
	jmp	wait

strtHD:	mov	a, c		; hard disk
	out	BASEHD + 1
	mov	a, b
	out	BASEHD + 2

wait:	call	dkstat		; wait till i/o done
	ani	4
	jz	wait
	ret

; code is a direct lift from the Intellec series II interface document

;***************************************************************
;* PROCEDURE NAME = ISDDR (IHTEGRATED SINGLE DENSITY DISK DRIVER)
;* PROCESS: TRANSMIT THE IOPB; ONE BYTE AT A TIME, TO THE ISD
;* 	    IF THE INSTRUCTION TO THE OISK IS A OATA TRAHSFER
;*	    (I E READ DATA, FORMAT, WRITE DATA, WRITE DELETED
;*	    DATA) THEN TRAHSFER THE DATA. ONE BYTE AT A TIME
;*	    TO/FROH THE ISD
;* IHPUT:   B-REG COHTAIHS MSB OF IOPB
;*	    C-REG COHTAIHS LSB OF IOPB
;* OUTPUT:  THE IOPB IS TRANSMITTED TO THE ISD DATA IS TRANSFERED
;*	    TO/FROM THE ISD AS REQUIRED
;***************************************************************

isddr:
	push	b		; save the iopb
	inx	b
	ldax	b		; test ioins
	cpi	READ
	jnz	isd1		; jump if not read
	pop	h		; get the iobp
	push	h		; save again
	inx	h
	inx	h
	mov	d, m		; d = number of sectors
	pop	h
	push	d		; save the number of sectors
	call	triopb		; issue the command
	inx	h		; get the buffer address into hl
	mov	e, m
	inx	h
	mov	d, m
	xchg
	mvi	b, RDBC		; issue the read data block command
	call	IOCCOM
	pop	d		; recover the number of sectors	

rdlp1:
	mvi	e, 128		; read a sector (128 bytes)

rdlp2:
	in	IOCS		; wait for byte available
	ani	F0 or IBF or OBF
	cpi	OBF		; test for slave done; somethign for the master
	jnz	rdlp2		; loop until slave is ready
	in	IOCI		; get the byte from the DBB
	mov	m, a		; save to buffer
	inx	h
	dcr	e		; loop for one sector
	jnz	rdlp2
	dcr	d		; loop for number of sectors
	jnz	rdlp1
	mvi	a, ENABL		; enable interrupts
	out	CPUC
	ret
	;----------------------------------------------------------------------
isd1:				; here if not a read so test for
	cpi	FORMAT		; FORMAT, WRITE pr WRITED 
	jz	isd2
	cpi	WRITE
	jz	isd2
	cpi	WRITED
	jnz	isd3		; must be a SEEK, RECALB or VERIFY

isd2:
	pop	h		; recover iopb
	push	h		; save again
	inx	h
	mov	a, m
	cpi	FORMAT
	inx	h
	jz	isd2a		; don't use sector count for format
	mov	c, m		; pick up sector count
	jmp	isd2b

isd2a:
	mvi	c, 1		; format has only single sector

isd2b:
	inx	h
	inx	h
	inx	h
	mov	e, m
	inx	h
	mov	d, m
	xchg			; hl = buffer
	mvi	b, WDBC		; load wite data block command
	call	IOCCOM		; output the command
	call	iocrdy		; wait till slave is idle
	mov	a, c		; number of sectors to be written
	out	IOCO

wrlp1:
	push	psw
	mvi	d, 128		; sector length

wrlp2:
	call	iocrdy		; wait will slave is idle
	mov	a, m		; write the data byte
	out	IOCO
	inx	h		; advance buffer
	dcr	d
	jnz	wrlp2
	pop	psw		; number of sectors
	dcr	a
	jnz	wrlp1		; loop till all written
	mvi	a, ENABL	; enable interrupts
	out	CPUC

isd3:
	pop	h		; hl = iopb
	call	triopb		; send the iopb
	ret

;------
iocrdy:	in	IOCS		; get the status
	ani	F0 or OBF or IBF; look till ready
	jnz	iocrdy
	ret

;***************************************************************
;* Procedure name: TRIOPB (transmit IOPB to ISD)
;* Process: Transmit the IOPB to the 8271 integrated single density
;*	    controller this procedure is called only bu procedure ISDDR
;* Input: HL contains address of the IOPB
;* Output: Transmit the IOPB
;*	   HL points to SADR of IOPB
;* Modified: A, FLAGS, B, C, D, HL
; *
;***************************************************************
triopb:				; transfer$iopb$to$isd
	mov	c, m		; the iocw 
	mvi	b, WPBC		; issue first byte
	call	IOCDR2
	mvi	d, 4		; send the remaining 4 bytes

trloop:
	inx	h		
	mov	c, m
	mvi	b, WPCC
	call	IOCDR2
	dcr	d
	jnz	trloop

trwait:	call	dkstat		; see if the bit indicating operation
	ani	4		; compete is set
	jz	trwait		; loop until done
	ret

	end;

