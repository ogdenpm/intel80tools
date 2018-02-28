; the original code in error.plm appears to have been hand optimised.
; the code below contains the plm code and the optimised asm

; declare ABORT literally '2';
; declare CMSK literally '0fch';
; declare CR literally '0dh';
; declare DISK$IO$ERROR literally '24';
; declare debug$Toggle literally 'dTogle';
; declare global$severity literally 'gblSev';
; declare LF literally '0ah';
; declare userStatus$p literally 'uStatP';
; declare WARNING literally '0';
; declare isis$E byte external;
; declare sector byte external;
; declare track byte external;
; declare usrSP address external;
; declare uStatP address external;
; CO: procedure(val) external; declare val byte; end;
; IOCHK: procedure byte external; end;
; IOSET: procedure(val) external; declare val byte; end;
; reboot: procedure external; end;
; trap: procedure external; end;
; declare userStatus based userStatus$p  address;

	name error
	public dTogle, gblSev, cdksat, err, warn, abortx, errDrv
	extrn isisE, sector, track, usrSP, uStatP
	extrn CO, IOCHK, IOSET, reboot, trap

ABORT	equ	2
WARNIN	equ	0
CR	equ	0dh
LF	equ	0ah        
        
; DECLARE pad ADDRESS,    /* padding to align to isis.bin */
;         debug$Toggle BYTE PUBLIC,
;         global$severity BYTE PUBLIC,
;         c$dk$sat ADDRESS PUBLIC;
        
; DECLARE msg1(7) BYTE INITIAL(CR, LF, 'ERROR'),
;         msg2(4) BYTE, /* error number goes here */
;         msg3(9) BYTE INITIAL(' USER PC '),
;         msg4(4) BYTE, /* user pc in hex goes here */
;         msg5(9) BYTE INITIAL(CR, LF, 'STATUS='),
;         msg6(4) BYTE, /* drive status goes here */
;         msg7(5) BYTE INITIAL(CR, LF, 'D= '),
;         errdrv  BYTE PUBLIC, /* drive goes here */
;         msg9(3) BYTE INITIAL(' T='),
;         msg10(1) BYTE,  /* track goes here */
;         msg11(3) BYTE INITIAL('   '),
;         msg12(3) BYTE INITIAL(' S='),
;         msg13(1) BYTE,  /* sector goes here */
;         msg14(5) BYTE INITIAL('   ', CR, LF);
; DECLARE initio BYTE AT (6);
initio	equ	6

	DSEG
pad:	ds 2
dTogle:	ds 1
gblSev:	ds 1
cdksat:	ds 2

msg1:	db CR, LF, 'ERROR'
msg2:	ds 4
msg3:	db ' USER PC '
msg4:	ds 4
msg5:	db CR, LF, 'STATUS='
msg6:	ds 4
msg7:	db CR, LF, 'D= '
errdrv:	ds 1
msg9:	db ' T='
msg10:	ds 1
msg11:	db '   '
msg12:	db ' S='
msg13:	ds 1
msg14:	db '   ', CR, LF
        


	extrn	@P0014, @P0029, @P0035, @P0094, @P0102

	CSEG
severe:	ds	1	; err arguments
errtyp:	ds	1
imax:	ds	1	; err var

value:	dw	0	; num arguments
base:	ds	1
bufp:	dw	0
div:	dw	0	; num vars
digit:	dw	0
j:			; shared var
i:	ds	1
offset:	ds	1		

; num: PROCEDURE(value, base, bufp);
num:	LXI     H,BUFP+1H
	MOV     M,D
	DCX     H
	MOV     M,E
	DCX     H
	MOV     M,C
	DCX     H
	POP     D       
	POP     B       
	MOV     M,B
	DCX     H
	MOV     M,C
	PUSH    D       
; DECLARE value ADDRESS, base BYTE, bufp ADDRESS;
; DECLARE buf based bufp (4) BYTE;
; DECLARE (div, digit) ADDRESS, (i, offset) BYTE;
; do i = 1 to 4;
	LXI     H,I
	MVI     M,1H
@11:
	MVI     A,4H
;	LXI     H,I	; optimised
	CMP     M
	JC      @12
;   offset = '0';
	LXI     H,OFFSET
	MVI     M,30H
; div = value / base;
	LHLD    VALUE
	XCHG
	LHLD    BASE
	MVI     H,0
	CALL    @P0029 
	XCHG
	SHLD    DIV
; digit = value - div * base;
	XCHG
	CALL    @P0035 
	LXI     D,VALUE
	CALL    @P0102 
	SHLD    DIGIT
;   if digit > 9 then
	MVI     A,9H
	CALL    @P0094 
	JNC     @1
;     offset = '7';
	LXI     H,OFFSET
	MVI     M,37H
@1:
;   buf(4 - i) = digit + offset;
	LDA     OFFSET
	LXI     D,DIGIT
	CALL    @P0014 
	XCHG
	MVI     A,4H
	INX     H
	SUB     M
	MOV     C,A
	MVI     B,0
	LHLD    BUFP
	DAD     B
	MOV     M,E
;   value = div;
	LHLD    DIV
	SHLD    VALUE
; end;
@13:
	LXI     H,I
	INR     M
	JNZ     @11
@12:
; do i = 0 to 2;
;	LXI     H,I	optimised
	MVI     M,0H
@14:
	MVI     A,2H
;	LXI     H,I	optimised
	CMP     M
	JC      @15
;   if buf(i) = '0' then
;	LHLD    I	optimised
	mov	l,m
	MVI     H,0
	XCHG
	LHLD    BUFP
	DAD     D
	MOV     A,M
	CPI     30H
	JNZ     @2
;     buf(i) = ' ';
;	LHLD    I	optimised
;	MVI     H,0
;	XCHG
;	LHLD    BUFP
;	DAD     D
	MVI     M,20H
	JMP     @3
@2:
;   else
;     return;
	RET
@3:
; end;
@16:
	LXI     H,I
	INR     M
	JNZ     @14
@15:
; end;
	RET
	
	
; err: PROCEDURE(severity, error$type) PUBLIC;
err:	LXI     H,ERRTYP
	MOV     M,E
	DCX     H
	MOV     M,C
; DECLARE (severity, error$type, j, imax) BYTE;
; DECLARE pc BASED usrSP ADDRESS;
	
; if (severity := severity or global$severity) <> WARNING then 
	LDA     GBLSEV
	LXI     H,SEVERE
	ORA     M
	MOV     M,A
	CPI     0H
	JZ      @4
; do;
;   call num(error$type, 10, .msg2);
	LHLD    ERRTYP
	MVI     H,0
	PUSH    H 
	LXI     D,MSG2
	MVI     C,0AH
	CALL    NUM
;   call num(pc, 16, .msg4);    
	LHLD    USRSP
	MOV     C,M
	INX     H
	MOV     B,M
	PUSH    B 
	LXI     D,MSG4
	MVI     C,10H
	CALL    NUM
;   call ioset((iochk AND CMSK) OR (initio AND 3));
	CALL    IOCHK
	ANI     0FCH
	PUSH    PSW 
	LDA     INITIO
	ANI     3H
	POP     B 
	MOV     C,B
	ORA     C
	MOV     C,A
	CALL    IOSET
;   imax = 25;  /* number of characters in normal error message */
	LXI     H,IMAX
	MVI     M,19H
;   if error$type = DISK$IO$ERROR then
	LDA     ERRTYP
	CPI     18H
	JNZ     @5
;   do;
;     if high(c$dk$sat) <> 0 then
	LHLD    CDKSAT
	MOV     A,H
	CPI     0H
	JZ      @6
;     c$dk$sat = 128;
	LXI     H,80H
	SHLD    CDKSAT
@6:
	push	h	; optimised LHLD CDKSAT below
;     imax = 58;
	LXI     H,IMAX
	MVI     M,3AH
;     call num(c$dk$sat, 16, .msg6);
;	LHLD    CDKSAT	; optimised see above
;	PUSH    H
	LXI     D,MSG6
	MVI     C,10H
	CALL    NUM
;     errdrv = errdrv or '0'; /* convert drive to ascii number */
	LDA     ERRDRV
	ORI     30H
	STA     ERRDRV
;     call num(track, 10, .msg10);
	LHLD    TRACK
	MVI     H,0
	PUSH    H       ; 1
	LXI     D,MSG10
	MVI     C,0AH
	CALL    NUM
;     call num(sector, 10, .msg13);
	LHLD    SECTOR
	MVI     H,0
	PUSH    H       ; 1
	LXI     D,MSG13
	MVI     C,0AH
	CALL    NUM
; end;
@5:
;   do j = 0 to imax;
	LXI     H,J
	MVI     M,0H
@17:
	LDA     IMAX
;	LXI     H,J	optimised
	CMP     M
	JC      @18
;     call co(msg1(j));
;	LHLD    J	optimised
	mov	l,m
	MVI     H,0
	LXI     B,MSG1
	DAD     B
	MOV     C,M
	CALL    CO
;   end;
@19:
	LXI     H,J
	INR     M
	JNZ     @17
@18:
; end;
	JMP     @7
@4:
; else
; do;
;   userStatus = error$type;
	LHLD    USTATP
	LDA     ERRTYP
	MOV     M,A
	INX     H
	MVI     M,0
;   STACKPTR = usrSP;
	LHLD    USRSP
	SPHL
; end;
@7:
; if severity >= ABORT then
	LDA     SEVERE
	CPI     2H
	JC      @8
; do;
;   if debug$toggle then
	LDA     DTOGLE
	RAR
	JNC     @9
;     call trap;  /* trap to monitor */
	CALL    TRAP
@9:
;   if ISIS$E then      /* Parameter 1 of the ISIS 'E' command */
	LDA     ISISE
	RAR
	JNC     @10
;   do;
;     userStatus = error$type;
	LHLD    USTATP
	LDA     ERRTYP
	MOV     M,A
	INX     H
	MVI     M,0
;     STACKPTR = usrSP;
	LHLD    USRSP
	SPHL
;     return;
	RET
; end;
@10:
; call reboot;
	CALL    REBOOT
; end;
@8:
; end;
	RET
        
        
; abortx: PROCEDURE(errcode) PUBLIC;
; 	DECLARE errcode BYTE;
; 	call err(ABORT, errcode);
; end;
abortx:	mov	e,c
	mvi	c,ABORT
	jmp	err
        
; warn: PROCEDURE(errcode) PUBLIC;
; 	DECLARE errcode BYTE;
;	call err(WARNING, errcode);
; end;
warn:	mov	e,c
	mvi	c,WARNIN
	jmp	err
        
	end
