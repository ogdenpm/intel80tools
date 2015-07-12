	name	@P0082
	public	@P0082, @P0083
	cseg

@P0082:			; hl = a minus w[hl]
	mov	e,a
	mvi	d,0
@P0083:			; hl = de minus w[hl]
	mov	a,e
	sbb	m
	mov	e,a
	mov	a,d
	inx	h
	sbb	m
	mov	d,a
	xchg
	ret
	end

