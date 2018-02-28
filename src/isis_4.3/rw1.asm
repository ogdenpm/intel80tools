	CSEG
	public	usrSys
	public	sysUsr
	extrn	datapp
	extrn	adpdif

; usrSys: PROCEDURE(buffer$p, buffer, datum$base)
usrSys:		
	push	d		; datum$base
	lhld	datapp
	mov	e, m		; e = a$data$ptr
	lda	adpdif
	mov	m, a		; a$data$ptr = adp$dif	- save final a$data$ptr value
	mvi	d, 0
	pop	h		; datum$base + e - stort of data to copy
	dad	d
	mov	d, a		; d = adp$dif

usloop:
	mov	a, d		; copy until we reach new end
	cmp	e
	jz	updbuf
	ldax	b		; buffer
	mov	m, a		; datum buffer
	inx	h
	inx	b
	inr	e		; advance data$ptr
	jmp	usloop


sysUsr:
	push	d		; as per usrSys
	lhld	datapp
	mov	e, m
	lda	adpdif
	mov	m, a
	mvi	d, 0
	pop	h
	dad	d
	mov	d, a

suloop:
	mov	a, d
	cmp	e
	jz	updbuf
	mov	a, m		; datum buffer
	stax	b		; buffer
	inx	h
	inx	b
	inr	e
	jmp	suloop

updbuf:
	pop	h	; return address
	xthl		; buffer$p
	mov	m, c	; save the updated buffer
	inx	h
	mov	m, b
	ret

	end;

