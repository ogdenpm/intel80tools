	name	@P0029
	public	@P0029, @P0030
	cseg

@P0029:			; de = de / hl, hl = de % hl
	mov	b,h
	mov	c,l
@P0030:			; de = de / bc, hl = de % bc
	lxi	h,0
	mvi	a,10H
L0007:
	push	psw
	dad	h
	xchg
	sub	a
	dad	h
	xchg
	adc	l
	sub	c
	mov	l,a
	mov	a,h
	sbb	b
	mov	h,a
	inx	d
	jnc	L0019
	dad	b
	dcx	d
L0019:
	pop	psw
	dcr	a
	jnz	L0007
	ret
	end

