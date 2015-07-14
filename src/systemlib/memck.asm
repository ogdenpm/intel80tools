	name	MEMCK
	public	MEMCK
	cseg

MEMCK:	call	0F81BH
	mov	h,b
	mov	l,a
	ret

	end

