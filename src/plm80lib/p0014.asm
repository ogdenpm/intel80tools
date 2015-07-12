	name	@P0014
	public	@P0014, @P0015, @P0016, @P0017
	cseg

@P0014:			; hl = w[de] + a
	xchg
@P0015:			; hl = w[hl] + a
	mov	e,a	
	mvi	d,0
@P0016:			; hl = w[hl] + de
	xchg
@P0017:			; hl = w[de] + hl
	ldax	d
	add	l
	mov	l,a
	inx	d
	ldax	d
	adc	h
	mov	h,a
	ret
	end

