	name	@P0042
	public	@P0042, @P0043, @P0044
	cseg

@P0042:			; hl =  ~w[bc]
	mov	l,c
	mov	h,b
@P0043:			; hl = ~w[hl]
	xchg
@P0044:			; hl = ~w[de]
	ldax	d
	cma
	mov	l,a
	inx	d
	ldax	d
	cma
	mov	h,a
	ret
	end

