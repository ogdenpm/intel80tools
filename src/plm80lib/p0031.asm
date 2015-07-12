	name	@P0031
	public	@P0031, @P0032, @P0033
	cseg

@P0031:			; hl = [hl] * 10
	mov	e,m
	inx	h
	mov	d,m
@P0032:			; hl = de * 10
	xchg
@P0033:			; hl = hl * 10
	dad	h
	push	h
	dad	h
	dad	h
	pop	b
	dad	b
	ret
	end

