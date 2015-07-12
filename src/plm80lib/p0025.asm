	name	@P0025
	public	@P0025, @P0026, @P0027, @P0028
	cseg

@P0025:			; hl = w[de] & a
	xchg
@P0026:			; hl = w[hl] & a
	mov	e,a
	mvi	d,0
@P0027:			; hl = w[hl] & de
	xchg
@P0028:			; hl = w[de] & hl
	ldax	d
	ana	l
	mov	l,a
	inx	d
	ldax	d
	ana	h
	mov	h,a
	ret
	end

