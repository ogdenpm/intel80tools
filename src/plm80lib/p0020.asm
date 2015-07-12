	name	@P0020
	public	@P0020, @P0021
	cseg

@P0020:			; hl = de & a
	mov	c,a
	mvi	b,0
@P0021:			; hl = de & bc
	mov	a,e
	ana	c
	mov	l,a
	mov	a,d
	ana	b
	mov	h,a
	ret
	end

