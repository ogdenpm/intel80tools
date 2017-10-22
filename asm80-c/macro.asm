$MACROFILE

move	macro	x,y,z
	irpc	param,z
	lhld	x&&param
	shld	y&&param
	endm
	endm

inc	macro	f1,f2
$save gen
f1&f2:	db	0	;generate labels & db's
$restore
	endm

block	macro	numb,prefix
$save nogen
count	set	0
	rept	numb
count	set	count+1
	inc	prefix,%count	;nested macro call
	endm
$restore
	endm


tjump	macro		;jump to a-th addr in table
tjcode:	add	a	;multipy a by 2
	mvi	d,0	;clear d reg
	mov	e,a	;get table offset into d&e
	dad	d	;add offset to table addr in h&l
	mov	e,m	;get 1st address byte
	inx	h
	mov	d,m	;get 2nd address byte
	xchg
	pchl		;jmp to address
tjump	macro		;redefine tjump to save code
	jmp	tjcode	;next call jumps to above code
	endm
	endm

goto	macro	index,list
	local	jtable
	lda	index		;load accum with index
	lxi	h,jtable	;load h&l with table address
	tjump
jtable:	irp	formal,<list>
	dw	formal		;set up table
	endm
	endm

	cseg
	move	src,dst,123
	block	3,lab

	goto	case,<counts,time,date,ptdrvr>


case:	db 3
counts:	jmp	100h
time:	jmp	200h
date:	jmp	300h
ptdrvr:	jmp	400h
src1:	ds	2
src2:	ds	2
src3:	ds	2
dst1:	ds	2
dst2:	ds	2
dst3:	ds	2

	end;
