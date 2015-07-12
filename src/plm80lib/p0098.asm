	name	@P0098
	public	@P0098, @P0099, @P0100
	cseg

@P0098:			; hl = w[de] - w[bc]
	mov	l,c
	mov	h,b
@P0099:			; hl = w[de] - w[hl]
	mov	c,m
	inx	h
	mov	b,m
@P0100:			; hl = w[de] - bc
	ldax	d
	sub	c
	mov	l,a
	inx	d
	ldax	d
	sbb	b
	mov	h,a
	ret
	end

