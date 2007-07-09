	public	fwrite
	extrn	movmem
	extrn	fflush
	extrn	@P0099	; hl@deiSUBhli
	extrn	@P0095	; hl@deSUBhl
	extrn	@P0012	; hl@deiADDhli
	extrn	@P0102	; hl@deiSUBhl

; file structure offsets
aftn	equ	0
sNam	equ	aftn + 2
fNam	equ	sNam + 6
bufp	equ	fNam + 16
bsize	equ	bufp + 2
actual	equ	bsize + 2
curoff	equ	actual + 2

DSEG
file:	ds 2
buf:	ds 2
len:	ds 2
wcnt:	ds 2

CSEG
; fwrite(file, buf, len)

fwrite:	xchg
	shld	len
	mov	h, b
	mov	l, c
	shld	buf
	pop	h
	xthl
	shld	file
	xchg
	lxi	h, bsize
	dad	d
	push	h
	xchg
	lxi	b, curoff
	dad	b
	push	h
	call	@P0099		; hl = m[de] - (bc = m[hl])
	xchg			; room in buffer

L1:	lhld	len
	call	@P0095
	lhld	len
	jnc	L2
	xchg

L2:	shld	wcnt
	lhld	file
	lxi	d, bufp
	dad	d
	pop	d		; current offset
	push	d		; current offset
	call	@P0012
	xchg
	lhld	buf		; copy first lot of bytes
	mov	b, h
	mov	c, l
	lhld	wcnt
	push	h		; wcnt
	call	movmem
	pop	h		; advance the offset
	push	h		; current offset
	lxi	d, wcnt
	call	@P0012
	xchg
	pop	h
	push	h		; current offset
	mov	m, e
	inx	h
	mov	m, d
	lhld	wcnt		; work out bytes left to copy
	mov	b, h
	mov	c, l
	lxi	d, len
	call	@P0102
	shld	len
	mov	a, h
	ora	l
	jz	L4		; all done
	lhld	buf		; advance the buf pointer
	dad	b
	shld	buf
	pop	d		; cur offset
	pop	h		; buf size
	push	d
	push	h
	call	@P0099		; hl = m[de] - (bc = m[hl])
	jc	L3		; ok still room	in buffer
	lhld	file		; write	what we	have
	mov	b, h
	mov	c, l
	call	fflush		; flushFile(arg1w)

L3:	pop	h		; set as bufSize byte space available
	mov	e, m
	inx	h
	mov	d, m
	dcx	h
	xthl
	push	h
	jmp	L1

L4:	pop	h
	pop	h
	ret

	end
