	name	@P0101
	public	@P0101, @P0102
	cseg

@P0101:			; hl = w[de] - a
	mov	l,a
	mvi	h,0
@P0102:			; hl = w[de] - hl
	ldax	d
	sub	l
	mov	l,a
	inx	d
	ldax	d
	sbb	h
	mov	h,a
	ret
	end

