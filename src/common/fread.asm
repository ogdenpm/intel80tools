	PUBLIC	fread
	extrn	movmem
	extrn	readf
	extrn	fatlIO
	extrn	@P0099	; hl@deiSUBhli
	extrn	@P0095	; hl@deSUBhl
	extrn	@P0012	; hl@deiADDhli
	extrn	@P0102	; hl@deiSUBhl

DSEG
file:	ds 2
buf:	ds 2
cnt:	ds 2
avail:	ds 2

; file structure offsets
aftn	equ 0
snam	equ 2
fnam	equ 8
bufptr	equ 24
size	equ 26
actual	equ 28
curoff	equ 30

CSEG

; fread: procedure(file, buf, cnt) public; declare (file, buf, cnt) address; end;

fread:	xchg
	shld	cnt
	mov	h, b
	mov	l, c
	shld	buf
	pop	h
	xthl
	shld	file
	xchg
	lxi	h, actual
	dad	d
	push	h
	xchg
	lxi	b, curoff
	dad	b
	push	h
	call	@P0099
	xchg

L1:	lhld	cnt
	call	@P0095
	lhld	cnt
	jnc	L2
	xchg

L2:	shld	avail
	lhld	file
	mov	b, h
	mov	c, l
	lxi	h, actual
	dad	b
	mov	a, m
	inx	h
	mov	e, m
	ora	e
	jnz	L3
	lxi	d, 0FEh
	call	fatlIO		; fatalIO(arg1w, arg2w)

L3:	lhld	file
	lxi	d, bufptr
	dad	d
	pop	d
	push	d
	call	@P0012
	mov	b, h
	mov	c, l
	lhld	avail
	push	h
	lhld	buf
	xchg
	call	movmem
	pop	h
	push	h
	lxi	d, avail
	call	@P0012
	xchg
	pop	h
	push	h
	mov	m, e
	inx	h
	mov	m, d
	lhld	avail
	mov	b, h
	mov	c, l
	lxi	d, cnt
	call	@P0102
	shld	cnt
	mov	a, h
	ora	l
	jz	L5
	lhld	buf
	dad	b
	shld	buf
	pop	d
	pop	h
	push	d
	push	h
	call	@P0099
	jc	L4
	lhld	file
	push	h
	xchg
	lxi	h, bufptr
	dad	d
	mov	c, m
	inx	h
	mov	b, m
	push	b
	inx	h
	mov	c, m
	inx	h
	mov	b, m
	inx	h
	xchg
	call	readf
	lxi	b, curoff
	lhld	file
	dad	b
	mvi	m, 0
	inx	h
	mvi	m, 0

L4:	pop	h
	mov	e, m
	inx	h
	mov	d, m
	dcx	h
	xthl
	push	h
	jmp	L1

L5:	pop	h
	pop	h
	ret

	end
