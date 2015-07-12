	name	@P0036
	public	@P0036, @P0037, @P0038
	cseg

@P0036:			; hl = -w[bc]
	mov	e,c
	mov	d,b
@P0037:			; hl = -w[de]
	xchg
@P0038:			; hl = -w[hl]
	sub	a
	sub	m
	mov	e,a
	mvi	a,0
	inx	h
	sbb	m
	mov	d,a
	xchg
	ret
	end

