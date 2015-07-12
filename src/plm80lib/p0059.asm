	name	@P0059
	public	@P0059, @P0060
	cseg

@P0059:			; a = scl([hl], c) 
	mov	a,m
@P0060:			; a = scl(a, c)
	ral
	dcr	c
	jnz	@P0060
	ret
	end

