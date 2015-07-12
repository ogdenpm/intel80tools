	name	@P0055
	public	@P0055, @P0056, @P0057, @P0058
	cseg

@P0055:			; hl = w[de] | a
	xchg
@P0056:			; hl = w[hl] | a
	mov	e,a
	mvi	d,0
@P0057:			; hl = w[hl] | de
	xchg
@P0058:			; hl = w[de] | hl
	ldax	d
	ora	l
	mov	l,a
	inx	d
	ldax	d
	ora	h
	mov	h,a
	ret
	end

