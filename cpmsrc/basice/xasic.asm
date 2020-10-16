	name	libbas
	public	@P0014,@P0099
	cseg
; extracted plm80.lib files for bespoke linkage for basic.obj
@P0014:			; hl = w[de] + a
	xchg
@P0015:			; hl = w[hl] + a
	mov	e,a	
	mvi	d,0
@P0016:			; hl = w[hl] + de
	xchg
@P0017:			; hl = w[de] + hl
	ldax	d
	add	l
	mov	l,a
	inx	d
	ldax	d
	adc	h
	mov	h,a
	ret

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
