	name	@P0007
	public	@P0007, @P0008, @P0009, @P0010
	cseg

@P0007:			; hl = w[de] plus a
	xchg
@P0008:			; hl = w[hl] plus a
	mov	e,a
	mvi	d,0
@P0009:			; hl = w[hl] plus de
	xchg
@P0010:			; hl = w[de] plus hl	
	ldax	d
	adc	l
	mov	l,a
	inx	d
	ldax	d
	adc	h
	mov	h,a
	ret
	end

