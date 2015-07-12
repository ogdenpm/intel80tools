	name	@P0108
	public	@P0108, @P0109
	cseg

@P0108:			; hl = de ^ a
	mov	c,a
	mvi	b,0
@P0109:			; hl = de ^ bc
	mov	a,e
	xra	c
	mov	l,a
	mov	a,d
	xra	b
	mov	h,a
	ret
	end

