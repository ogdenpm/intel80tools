	CSEG
	public	memck
memck:		
	call	0F81BH
	mov	h, b
	mov	l, a
	ret

	end


