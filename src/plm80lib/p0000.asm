	name	@P0000
	public	@P0000, @P0001
	cseg

@P0000:			; hl = a plus hl
	mov	e,a
	mvi	d,0
@P0001:			; hl = de plus hl
	mov	a,e
	adc	l
	mov	l,a
	mov	a,d
	adc	h
	mov	h,a
	ret
	end

