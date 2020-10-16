	name	libbas
	public	@P0014,@P0019,@P0057
    public  @P0099, @P0101, @P0102, @P0104
	cseg
; extracted plm80.lib files for bespoke linkage for bassyn.obj
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

@P0019:			; hl = hl & de
	mov	a,e
	ana	l
	mov	l,a
	mov	a,d
	ana	h
	mov	h,a
	ret

@P0057:			; hl = w[hl] | de
	xchg
@P0058:			; hl = w[de] | hl
	ldax	d
	ora	l
	mov	l,a
	inx	d
	ldax	d
	ora	h
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
