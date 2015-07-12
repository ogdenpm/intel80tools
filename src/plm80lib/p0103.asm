	name	@P0103
	public	@P0103, @P0104
	cseg

@P0103:			; hl = a - w[hl] 
	mov	e,a
	mvi	d,0
@P0104:			; hl = de - w[hl]
	mov	a,e
	sub	m
	mov	e,a
	mov	a,d
	inx	h
	sbb	m
	mov	d,a
	xchg
	ret
	end

