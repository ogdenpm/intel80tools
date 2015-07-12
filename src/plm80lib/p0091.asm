	name	@P0091
	public	@P0091, @P0092, @P0093
	cseg

@P0091:			; hl = shr(w[hl], c)
	mov	e,m
	inx	h
	mov	d,m
@P0092:			; hl = shr(de, c)
	xchg
@P0093:			; hl = shr(hl, c)
	mov	a,h
	ora	a
	rar
	mov	h,a
	mov	a,l
	rar
	mov	l,a
	dcr	c
	jnz	@P0093
	ret
	end

