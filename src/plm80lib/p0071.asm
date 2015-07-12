	name	@P0071
	public	@P0071, @P0072
	cseg

@P0071:			; a = ror([hl], c)
	mov	a,m
@P0072:			; a = ror(a, c)
	rrc
	dcr	c
	jnz	@P0072
	ret
	end

