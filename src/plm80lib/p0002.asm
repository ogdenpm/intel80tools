	name	@P0002
	public	@P0002, @P0003
	cseg

@P0002:			; hl = de plus a
	mov	c,a
	mvi	b,0
@P0003:			; hl = de plus bc
	mov	a,e
	adc	c
	mov	l,a
	mov	a,d
	adc	b
	mov	h,a
	ret
	end

