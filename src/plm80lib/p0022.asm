	name	@P0022
	public	@P0022, @P0023, @P0024
	cseg

@P0022:			; hl = w[de] & w[bc]
	mov	l,c
	mov	h,b
@P0023:			; hl = w[de] & w[hl]
	mov	c,m
	inx	h
	mov	b,m
@P0024:			; hl = w[de] & bc
	ldax	d
	ana	c
	mov	l,a
	inx	d
	ldax	d
	ana	b
	mov	h,a
	ret
	end

