
	CSEG
	public	trap
trap:			; exit via mds monitor
	pop	h	; waste the return address
	jmp	0
	end

