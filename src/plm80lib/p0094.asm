	name	@P0094
	public	@P0094, @P0095
	cseg

@P0094:			; hl = a - hl
	mov	e,a
	mvi	d,0
@P0095:			; hl = de - hl
	mov	a,e
	sub	l
	mov	l,a
	mov	a,d
	sbb	h
	mov	h,a
	ret
	end

