$eject
; *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
;
;	TITLE:	montor_186
;
;	INPUTS:	none
;
;	OUTPUTS: none, jumps to the beginning of the monitor code.
;
;	ABSTRACT:
;	This procedure sets up the chip select registers for
;	the 80186/8.  It must be executed before the execution leaves 
;	the top 1K of memory.
;
; *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*

EXTRN	reset_8086:FAR
NULL	EQU	0
sdm86_186_init	segment	at 0fff0h

	assume	cs:sdm86_186_init,ds:sdm86_186_init
	PUBLIC	montor_186

montor_186	proc	far
start_186:
	cli
	mov	bx,sdm86_186_init

	mov	es,bx			; the segment value is still in bx
	mov	bx,offset init186_table	; point to the init table

init_loop:
	mov	dx,es:[bx]		; set up the port
	cmp	dx,NULL			; a null port will terminate the loop.
	je	init_finished
	mov	ax,es:[bx+2]		; get the programming value
	out	dx,ax			; initialize the register
	add	bx,4			; point to the next table entry.
	jmp	init_loop		; do the next output

init_finished:
	jmp	reset_8086			; enter the monitor

montor_186	endp
