	name	@P0064
	public	@P0064, @P0065
	cseg

@P0064:			; a = scr([hl], c) 
	mov	a,m
@P0065:			; a = scr(a, c)
	rar
	dcr	c
	jnz	@P0065
	ret
	end

