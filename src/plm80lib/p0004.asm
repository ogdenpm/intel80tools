	name	@P0004
	public	@P0004, @P0005, @P0006
	cseg

@P0004:			; hl = w[de] plus w[bc]
	mov	l,c
	mov	h,b
@P0005:			; hl = w[de] plus w[hl]
	mov	c,m
	inx	h
	mov	b,m
@P0006:			; hl = w[de] plus hl
	ldax	d
	adc	c
	mov	l,a
	inx	d
	ldax	d
	adc	b
	mov	h,a
	ret
	end

