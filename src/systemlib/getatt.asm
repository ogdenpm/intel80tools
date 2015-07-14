	name	GETATT
	public	GETATT
	extrn	ISIS

;	declare I$GETATT literally '17';
;	getatt(arg1, arg2, arg3) public;
;	    declare (arg1, arg2, arg3) address;
;	    call ISIS(I$GETATT, .arg1);
;	end;


IGTATT	equ	17
	dseg
D0000:	ds	6
D0006:
	cseg
GETATT:	mvi	a,IGTATT
	lxi	h,D0006
	dcx	h
	mov	m,d
	dcx	h
	mov	m,e
	dcx	h
	mov	m,b
	dcx	h
	mov	m,c
	pop	b
	pop	d
	dcx	h
	mov	m,d
	dcx	h
	mov	m,e
	push	b
	mov	c,a
	xchg
	jmp	ISIS

	end;
