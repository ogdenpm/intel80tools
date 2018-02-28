	name getatt
	public	getatt
	extrn	ckpndf	; chk$pn$disk$file
	extrn	dlokpn	; dlook$pn
	extrn	direct

	CSEG
getatt:	push	d
	call 	ckpndf	; chk$pn$disk$file
	call	dlokpn	; dlook$pn
	pop	h
	lda	direct + 10	; direct.attrib
	mov	m,a
	ret
	end;

