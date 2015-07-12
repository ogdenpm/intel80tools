	name	@P0048
	public	@P0048, @P0049
	cseg

@P0048:			; hl = hl | a
	mov	e,a
	mvi	d,0
@P0049:			; hl = hl | de
	mov	a,e
	ora	l
	mov	l,a
	mov	a,d
	ora	h
	mov	h,a
	ret
	end

