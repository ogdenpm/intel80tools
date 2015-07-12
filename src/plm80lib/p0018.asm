	name	@P0018
	public	@P0018, @P0019
	cseg

@P0018:			; hl = hl & a
	mov	e,a
	mvi	d,0
@P0019:			; hl = hl & de
	mov	a,e
	ana	l
	mov	l,a
	mov	a,d
	ana	h
	mov	h,a
	ret
	end

