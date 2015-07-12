	name	@P0011
	public	@P0011, @P0012, @P0013
	cseg

@P0011:			; hl = w[de] + w[bc]
	mov	l,c
	mov	h,b
@P0012:			; hl = w[de] + w[hl]
	mov	c,m
	inx	h
	mov	b,m
@P0013:			; hl = w[de] + bc
	ldax	d
	add	c
	mov	l,a
	inx	d
	ldax	d
	adc	b
	mov	h,a
	ret
	end

