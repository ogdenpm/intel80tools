	name	@P0086
	public	@P0086, @P0087, @P0088
	cseg

@P0086:			; hl = shl(w[hl], c)
	mov	e,m
	inx	h
	mov	d,m
@P0087:			; hl = shl(de, c)
	xchg
@P0088:			; hl = shl(hl, c)
	dad	h
	dcr	c
	jnz	@P0088
	ret
	end

