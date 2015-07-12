	name	@P0084
	public	@P0084, @P0085
	cseg

@P0084:			; a = shl([hl], c)
	mov	a,m
@P0085:			; a = shl(a, c)
	add	a
	dcr	c
	jnz	@P0085
	ret
	end

