	name	@P0061
	public	@P0061, @P0062, @P0063
	cseg

@P0061:			; hl = scl(w[hl], c)
	mov	e,m
	inx	h
	mov	d,m
@P0062:			; hl = scl(de, c)
	xchg
@P0063:			; hl = scl(hl, c)
	mov	a,l
	ral
	mov	l,a
	mov	a,h
	ral
	mov	h,a
	dcr	c
	jnz	@P0063
	ret
	end

