	public MEMCK
	public ISIS
	ASEG
	ORG	40H
ISIS:

	CSEG

MEMCK:		
	call	0F81Bh
	mov	h, b
	mov	l, a
	ret

	end;

