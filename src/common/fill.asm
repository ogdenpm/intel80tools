	public FILL

	CSEG

; FILL(cnt$w, dest$p, val$b)

FILL:	pop	h
	xthl
	mov	a, e
	inr	l
	inr	h
	jmp	L2

L1:	stax	b
	inx	b

L2:	dcr	l
	jnz	L1
	dcr	h
	jnz	L1
	ret

	END

