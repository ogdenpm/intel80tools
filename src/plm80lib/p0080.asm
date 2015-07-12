	name	@P0080
	public	@P0080, @P0081
	cseg

@P0080:			; hl = w[de] minus a 
	mov	l,a
	mvi	h,0
@P0081:			; hl = w[de] minus hl
	ldax	d
	sbb	l
	mov	l,a
	inx	d
	ldax	d
	sbb	h
	mov	h,a
	ret
	end

