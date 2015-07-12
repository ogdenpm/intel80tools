	name	@P0089
	public	@P0089, @P0090
	cseg

@P0089:			; a = shr([hl], c)
	mov	a,m
@P0090:			; a = shr(a, c)
	ora	a
	rar
	dcr	c
	jnz	@P0090
	ret
	end

