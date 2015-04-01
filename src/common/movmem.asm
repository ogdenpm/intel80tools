	public MOVMEM

	CSEG


; movmem(cnt, src, dest)

movmem:	pop	h		; hl = cnt arg
	xthl
	push	b		; src
	mov	b, h		; xchg hl,bc
	mov	c, l
	pop	h
	dcx	b
	mov	a, e
	sub	l
	mov	a, d
	jz	L1
	sbb	h
	jmp	L2

L1:	sbb	h
	rz

L2:	push	h
	lxi	h, L5
	jc	L3
	pop	h
	dad	b
	push	h
	xchg
	dad	b
	xchg
	lxi	h, L7

L3:	inx	b
	mov	a, b
	rrc
	rrc
	rrc
	mov	b, a
	mvi	a, 7
	ana	c
	push	psw
	xra	c
	rrc
	rrc
	rrc
	xra	b
	ani	1Fh
	xra	b
	mov	c, a
	mvi	a, 1Fh
	ana	b
	mov	b, a
	inr	b
	inr	c
	pop	psw
	rlc
	rlc
	push	b
	cma
	mov	c, a
	mvi	b, 0FFh
	inx	b
	dad	b
	pop	b
	xthl
	ret

L4:	mov	a, m
	stax	d
	inx	h
	inx	d
	mov	a, m
	stax	d
	inx	h
	inx	d
	mov	a, m
	stax	d
	inx	h
	inx	d
	mov	a, m
	stax	d
	inx	h
	inx	d
	mov	a, m
	stax	d
	inx	h
	inx	d
	mov	a, m
	stax	d
	inx	h
	inx	d
	mov	a, m
	stax	d
	inx	h
	inx	d
	mov	a, m
	stax	d
	inx	h
	inx	d

L5:	dcr	c
	jnz	L4
	dcr	b
	jnz	L4
	ret

L6:	mov	a, m
	stax	d
	dcx	h
	dcx	d
	mov	a, m
	stax	d
	dcx	h
	dcx	d
	mov	a, m
	stax	d
	dcx	h
	dcx	d
	mov	a, m
	stax	d
	dcx	h
	dcx	d
	mov	a, m
	stax	d
	dcx	h
	dcx	d
	mov	a, m
	stax	d
	dcx	h
	dcx	d
	mov	a, m
	stax	d
	dcx	h
	dcx	d
	mov	a, m
	stax	d
	dcx	h
	dcx	d

L7:	dcr	c
	jnz	L6
	dcr	b
	jnz	L6
	ret

	END

