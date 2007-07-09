	public pckTkn
	public ISIS
	extrn toksiz
	extrn tokst;

	aseg
ISIS	equ	40h

	CSEG
pckTkn: lhld	tokst
	xchg
	lxi	h, toksiz
	mov	c, m
	mvi	m, 4
	call pack3
	push	h
	call pack3
	pop	b
	xchg
	dcx	h
	dcx	h
	dcx	h
	mov	m, d
	dcx	h
	mov	m, e
	dcx	h
	mov	m, b
	dcx	h
	mov	m, c
	ret
pack3:
	lxi	h, 0	
	mvi	b, 3

L6291:
	push	d
	mov	d, h	; x 40
	mov	e, l
	dad	h
	dad	h
	dad	d
	dad	h
	dad	h
	dad	h
	pop	d
	call pack1
	add	l
	mov	l, a
	mvi	a, 0
	adc	h
	mov	h, a
	dcr	b
	jnz	L6291
	ret

pack1:	ldax	d
	inx	d
	dcr	c
	jm	L62B7
	sui	2Fh		;  012345678  (0-9)
				; 9?@ABCDEFG  (10-19)
				; HIJKLMNOPQ  (20-29)
				; RSTUVWXYZ[  (30-39)
	cpi	10h
	rc
	sui	5
	ret

L62B7:
	sub	a
	ret

	end
