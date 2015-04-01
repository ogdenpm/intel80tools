	public putLst
	extrn	wrcLst
	extrn	nlLead
	extrn	newPgl
	extrn	PWIDTH
	extrn	tWidth
	extrn	margin
	extrn	b3CFF
	extrn	linLft
	extrn	col
	extrn	@P0029	; dehl@deDIVhl
	extrn	@P0094	; hl@0aSUBhl

	DSEG
i:	ds	2

	CSEG
putLst:			
	push	b
	lda	col
	ora	a
	jnz	L76B8
	lda	linLft
	ora	a
	jnz	L7695
	call	newPgl
	jmp	L76BF

L7695:	mov	b, a
	lda	b3CFF
	cmp	b
	jc	L76A3
	call	newPgl
	jmp	L76BF

L76A3:	ora	a
	jz	L76BF
	mvi	c, 0Ah
	call	wrcLst
	lxi	h, linLft
	dcr	m
	lxi	h, b3CFF
	dcr	m
	mov	a, m
	jmp	L76A3

L76B8:	lxi	h, PWIDTH
	cmp	m
	cnc	nlLead

L76BF:	pop	b
	mvi	a, 9
	cmp	c
	jnz	L770C
	lxi	h, margin
	lda	col
	cmp	m
	rc
	lxi	h, margin
	lda	col
	sub	m
	mov	e, a
	mvi	d, 0
	lhld	tWidth
	mvi	h, 0
	call	@P0029		; de = de/hl; hl = de %	hl
	mov	a, c
	call	@P0094
	xchg
	lxi	h, i
	mov	m, e
	lda	col
	add	e
	lxi	h, PWIDTH
	cmp	m
	jc	L76F8
	call	nlLead
	ret

L76F8:	mov	a, e

L76F9:	ora	a
	rz
	mvi	c, 20h
	call	wrcLst
	lxi	h, col
	inr	m
	lxi	h, i
	dcr	m
	mov	a, m
	jmp	L76F9

L770C:	push	b
	call	wrcLst
	pop	b
	mvi	a, 0Dh
	cmp	c
	lxi	h, col
	jnz	L771D
	mvi	m, 0
	ret

L771D:	inr	m
	ret

	end
