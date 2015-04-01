	public MEMCHK

	CSEG

MEMCHK:		
	call	0F81Bh
	mov	h, b
	mov	l, a
	ret

	end;
