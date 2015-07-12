	name	@P0106
	public	@P0106, @P0107
	cseg

@P0106:			; hl = a ^ hl 
	mov	e,a
	mvi	d,0
@P0107:			; hl = de ^ hl
	mov	a,e
	xra	l
	mov	l,a
	mov	a,d
	xra	h
	mov	h,a
	ret
	end

