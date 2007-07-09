	PUBLIC memmov
	CSEG
; mem$move(count, src$p, dst$p)
memmov: pop	h
	xthl
	push	b
	mov	b, h
	mov	c, l
	pop	h
	dcx	b
	mov	a, e
	sub	l
	mov	a, d
	jz	l410F
	sbb	h
	jmp	l4111

l410F:	sbb	h
	rz

l4111:	push	h
	lxi	h, l4167
	jc	l4121
	pop	h
	dad	b
	push	h
	xchg
	dad	b
	xchg
	lxi	h, l4190

l4121:	inx	b
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

l4147:	mov	a, m
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

l4167:	dcr	c
	jnz	l4147
	dcr	b
	jnz	l4147
	ret

l4170:	mov	a, m
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

l4190:	dcr	c
	jnz	l4170
	dcr	b
	jnz	l4170
	ret
	END
