	name	@P0110
	public	@P0110, @P0111, @P0112
	cseg

@P0110:			; hl = w[de] ^ w[bc]
	mov	l,c
	mov	h,b
@P0111:			; hl = w[de] ^ w[hl]
	mov	c,m
	inx	h
	mov	b,m
@P0112:			; hl = w[de] ^ bc
	ldax	d
	xra	c
	mov	l,a
	inx	d
	ldax	d
	xra	b
	mov	h,a
	ret
	end

