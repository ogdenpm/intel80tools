	name	@P0045
	public	@P0045, @P0046, @P0047
	cseg

@P0045:			; hl = ~bc
	mov	e,c
	mov	d,b
@P0046:			; hl = ~de
	xchg
@P0047:			; hl = ~hl
	mov	a,l
	cma
	mov	l,a
	mov	a,h
	cma
	mov	h,a
	ret
	end

