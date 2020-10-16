	name	libbas
	public	@P0094, @P0098, @P0101, @P0103
	cseg
; extracted plm80.lib files for bespoke linkage for baspar.obj
@P0094:			; hl = a - hl
	mov	e,a
	mvi	d,0
@P0095:			; hl = de - hl
	mov	a,e
	sub	l
	mov	l,a
	mov	a,d
	sbb	h
	mov	h,a
	ret

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

@P0101:			; hl = w[de] - a
	mov	l,a
	mvi	h,0
@P0102:			; hl = w[de] - hl
	ldax	d
	sub	l
	mov	l,a
	inx	d
	ldax	d
	sbb	h
	mov	h,a
	ret

@P0103:			; hl = a - w[hl] 
	mov	e,a
	mvi	d,0
@P0104:			; hl = de - w[hl]
	mov	a,e
	sub	m
	mov	e,a
	mov	a,d
	inx	h
	sbb	m
	mov	d,a
	xchg
	ret

	end
