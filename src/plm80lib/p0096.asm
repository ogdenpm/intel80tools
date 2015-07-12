	name	@P0096
	public	@P0096, @P0097
	cseg

@P0096:			; hl = de - a
	mov	c,a
	mvi	b,0
@P0097:			; hl = de - bc
	mov	a,e
	sub	c
	mov	l,a
	mov	a,d
	sbb	b
	mov	h,a
	ret
	end

