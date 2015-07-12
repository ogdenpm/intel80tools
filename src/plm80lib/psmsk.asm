$mod85
	name	@PSMSK
	public	SMASK
	cseg

SMASK:			; smask: procedure(msk) public; declare msk byte; end;
	mov	a,c
	sim
	ret
	end

