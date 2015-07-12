	name	@P0034
	public	@P0034, @P0035
	cseg

@P0034:			; hl = de * hl
	mov	b,h
	mov	c,l
@P0035:			; hl = de * bc
	lxi	h,0
	mvi	a,10H
L0007:
	dad	h
	xchg
	dad	h
	xchg
	jnc	L000F
	dad	b
L000F:
	dcr	a
	jnz	L0007
	ret
	end

