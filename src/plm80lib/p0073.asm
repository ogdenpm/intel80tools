	name	@P0073
	public	@P0073, @P0074
	cseg

@P0073:			; hl = a minus hl
	mov	e,a
	mvi	d,0
@P0074:			; hl = de minus hl
	mov	a,e
	sbb	l
	mov	l,a
	mov	a,d
	sbb	h
	mov	h,a
	ret
	end

