	extrn	lchCnt
	extrn	lbufp
	extrn	lbufsz
	extrn	lfopen
	extrn	lstFil
	extrn	openf
	extrn	writef
	extrn	@P0095		;hl@deSUBhl
	public	wrcLst

	
TRUE	equ	0ffh

	CSEG

wrcLst:	lhld	lchCnt
	xchg
	lhld	lbufp
	dad	d
	mov	m, c
	lhld	lbufsz
	call	@P0095
	ora	l
	jnz	L7DBA
	lda	lfOpen
	rar
	jc	L7DA3
	mvi	e, 2
	lxi	b, lstFil
	call	openf
	lxi	h, lfOpen
	mvi	m, TRUE

L7DA3:	lxi	b, lstFil
	push	b
	lhld	lbufsz
	inx	h
	xchg
	lhld	lbufp
	mov	b, h
	mov	c, l
	call	writef
	lxi	h, 0
	jmp	L7DBE


L7DBA:	lhld	lchCnt
	inx	h

L7DBE:	shld	lchCnt
	ret

	end
