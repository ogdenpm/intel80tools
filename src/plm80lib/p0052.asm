	name	@P0052
	public	@P0052, @P0053, @P0054
	cseg

@P0052:			; hl = w[de] | w[bc]
	mov	l,c
	mov	h,b
@P0053:			; hl = w[de] | w[hl]
	mov	c,m
	inx	h
	mov	b,m
@P0054:			; hl = [de] | bc
	ldax	d
	ora	c
	mov	l,a
	inx	d
	ldax	d
	ora	b
	mov	h,a
	ret
	end

