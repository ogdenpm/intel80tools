
; MONITOR: DO;
	name MONITOR
; monitor: procedure public;
	public monitr
;  declare sp address;
;  declare wrd based sp address;
;  sp = stackptr;
	extrn @P0096		; hl = de - 0a
	dseg
?SP:	ds	2
	cseg
monitr:
        LXI     H,0
        DAD     SP
        SHLD    ?SP
;  wrd = 8;
        MVI     A,8H
        MOV     M,A
        INX     H
        MVI     M,0
;  stackptr = stackptr - 2;
        LXI     H,0
        DAD     SP
        ; DCX     H		; compiler puts optimised - 2
        ; DCX     H
	xchg			; source has older non optimised version
	mvi	a,2		; I could not get either compiler to generate
	call	@P0096		; this code hence I reverted to asm file
        SPHL
;  sp = stackptr;
        LXI     H,0
        DAD     SP
        SHLD    ?SP
;  wrd = 0;
        MVI     A,0H
        MOV     M,A
        INX     H
        MVI     M,0
;  end;
        RET
;  end;
	end

