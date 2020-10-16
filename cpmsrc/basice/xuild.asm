	name	@P1295
	public	@P0012, @P0019, @P0088,@P0095
	cseg
; extracted plm80.lib files for bespoke linkage to build.obj

@P0012:			; hl = w[de] + w[hl]
	mov	c,m
	inx	h
	mov	b,m
	ldax	d
	add	c
	mov	l,a
	inx	d
	ldax	d
	adc	b
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

@P0088:			; hl = shl(hl, c)
	dad	h
	dcr	c
	jnz	@P0088
	ret

@P0095:			; hl = de - hl
	mov	a,e
	sub	l
	mov	l,a
	mov	a,d
	sbb	h
	mov	h,a
	ret
	end
