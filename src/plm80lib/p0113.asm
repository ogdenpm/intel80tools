	name	@P0113
	public	@P0113, @P0114, @P0115, @P0116
	cseg

@P0113:			; hl = w[de] ^ a
	xchg
@P0114:			; hl = w[hl] ^ a
	mov	e,a
	mvi	d,0
@P0115:			; hl = w[hl] ^ de
	xchg
@P0116:			; hl = w[de] ^ hl
	ldax	d
	xra	l
	mov	l,a
	inx	d
	ldax	d
	xra	h
	mov	h,a
	ret
	end

