	name	@P0077
	public	@P0077, @P0078, @P0079
	cseg

@P0077:			; hl = w[de] minus w[bc]
	mov	l,c
	mov	h,b
@P0078:			; hl = w[de] minus w[hl]
	mov	c,m
	inx	h
	mov	b,m
@P0079:			; hl = w[de] minus bc
	ldax	d
	sbb	c
	mov	l,a
	inx	d
	ldax	d
	sbb	b
	mov	h,a
	ret
	end

