	name	@P0069
	public	@P0069, @P0070
	cseg

@P0069:			; a = rol([hl], c)
	mov	a,m
@P0070:			; a = rol(a, c)
	rlc
	dcr	c
	jnz	@P0070
	ret
	end

