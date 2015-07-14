	name	ERROR
	public	ERROR
	extrn	ISIS

;	error: procedure (error$num) public;
;	    declare (error$num) address;
;	end error;

IERROR	equ	12

	dseg
status:	ds	2
arg:	ds	2	; the isis arg
statp:	dw	status	; and notional return status$p

	cseg
ERROR:	mvi	a,IERROR
	lxi	h,statp
	dcx	h
	mov	m,b	; save the error$num
	dcx	h
	mov	m,c
	mov	c,a
	xchg
	jmp	ISIS

	end

