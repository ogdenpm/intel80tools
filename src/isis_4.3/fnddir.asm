
	CSEG
	public	fndDir
	public	dnum
	extrn	dirPtr
	extrn	fndFre
	extrn	inoPtr


; fndDir: PROCEDURE(fname, datum) PUBLIC BYTE; DECLARE fname ADDRESS, datum ADDRESS; END;

; direct.empty flag values
OPEN	equ	0
FREE	equ	0ffh
UNUSED	equ	07fh;

datum:	dw	0
dnum:	dw	0	

fndDir:
	xchg		; hl = datum
	mvi	e, 8	; 8 directory entries to match

dloop:
	shld	datum	; save passed in fn
	mov	a, m	; empty
	cpi	UNUSED	; never	used
	jz	skip
	ora	a
	jnz	skip	; 0 if open
	mvi	d, 9	; compare file names
	push	b	; save start of filename to match

cmpnam:
	inx	h
	ldax	b
	cmp	m
	jnz	nomat
	inx	b
	dcr	d
	jnz	cmpnam
	pop	b	; passed in filename
	mvi	d, 10h	; size of directory entry
	lhld	dirPtr	; copy the matched directory entry
	push	h
	pop	b
	lhld	datum

cpydir:	
	mov	a, m
	stax	b
	inx	h
	inx	b
	dcr	d
	jnz	cpydir
	call	setino
	mvi	a, FREE
	ret

skip:
	push	psw
	call	chk1st
	pop	psw
	rz			; return if never used,	shouldn't be any more after this
	push	b		; re-push filename to use common code

nomat:				; didn't match	
	pop	b		; restore filename
	lhld	dnum		; next dnum
	inx	h
	shld	dnum
	push	d		; save iteration count
	lxi	d, 10h		; next dir entry
	lhld	datum
	dad	d
	pop	d		; restore iteration count
	dcr	e		; we only have 8 directory entries in a buffer
	jnz	dloop
	xra	a		; not found in this dir	block
	ret

; end of fndDir

chk1st:
	lda	fndFre		; see if first free slot found
	ora	a
	rnz			; no then don't update
	cma			; mark as 1st now found
	sta	fndFre

setino:				; copy dnum of this slot either match or 1st free
	push	h
	push	d
	lhld	dnum
	xchg
	lhld	inoPtr
	mov	m, e
	inx	h
	mov	m, d
	pop	d
	pop	h
	ret

	end

