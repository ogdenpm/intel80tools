	name	@P0075
	public	@P0075, @P0076
	cseg

@P0075:			; hl = de minus a
	mov	c,a
	mvi	b,0
@P0076:			; hl = de minus bc
	mov	a,e
	sbb	c
	mov	l,a
	mov	a,d
	sbb	b
	mov	h,a
	ret
	end

