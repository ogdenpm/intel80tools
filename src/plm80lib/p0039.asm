	name	@P0039
	public	@P0039, @P0040, @P0041
	cseg

@P0039:			; hl = -bc
	mov	e,c
	mov	d,b
@P0040:			; hl = -de
	xchg
@P0041:			; hl = -hl
	sub	a
	sub	l
	mov	l,a
	mvi	a,0
	sbb	h
	mov	h,a
	ret
	end

