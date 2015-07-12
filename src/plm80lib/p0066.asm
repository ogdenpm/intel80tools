	name	@P0066
	public	@P0066, @P0067, @P0068
	cseg

@P0066:			; hl = scr(w[hl], c)
	mov	e,m
	inx	h
	mov	d,m
@P0067:			; hl = scr(de, c)
	xchg
@P0068:			; hl = scr(hl, c)
	mov	a,h
	rar
	mov	h,a
	mov	a,l
	rar
	mov	l,a
	dcr	c
	jnz	@P0068
	ret
	end

