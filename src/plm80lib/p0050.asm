	name	@P0050
	public	@P0050, @P0051
	cseg

@P0050:			; hl = de | a
	mov	c,a
	mvi	b,0
@P0051:			; hl = de | bc
	mov	a,e
	ora	c
	mov	l,a
	mov	a,d
	ora	b
	mov	h,a
	ret
	end

