	name	@P0105
	public	@P0105
	cseg

@P0105:			; delay
	mvi	b,0CH
L0002:
	mov	c,b
L0003:
	dcr	c
	jnz	L0003
	dcr	a
	jnz	L0002
	ret
	end

