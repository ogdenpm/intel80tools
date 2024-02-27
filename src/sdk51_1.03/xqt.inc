;******************************************************************
;
;	NAME: (I)BREAK
;
;	ABSTRACT: Control is transferred to this pOint when a break
;	interrupt occurs. The current user status is saved in the
;	page of external RAM starting at 'RAMOFF' and control then
;	passes to one of the return routines, STEP return and RUN
;	return.
;
;	INPUTS: BREAK_STATUS, MON_YLAGS
;
;	OUTPUTS: LINE_START, CAUSE_IMAGE, UPI_DATA_lMAGE, all the users
;	RAM and register image area.
;
;	VARIABLES MODIFIED: DPTR, SP, A, IE, POINT0, CAUSE_IMAGE,
;	ERRNUM, C, B, PARAM1, LINE_START, UPI_DATA_IMAGE
;
;	ERROR EXITS: 16H (EXECUTION OVER VECTOR AT LOCATION 3)
;
;	SUBROUTINES ACCESSED DIRECTRLY: ICSTS, UPI_IN, WRITE_PC, READ_PC,
;	INIT_IO, UPI_OUT, SET_BAUD, UPI_CMD, STGN_ON, STEP51_RETURN,
;	UNBREAK, RUN_USER_RETURN
;
;
;******************************************************************
IBREAK:	PUSH	DPL		;Save DPTR in the user stack.
	PUSH	DPH
	MOV	DPTR,#(RAMOFF + ACC)
	MOVX	@DPTR,A		;Save user ACC.
	MOV	DPL,#DPH
	POP	ACC
	MOVX	@DPTR,A		;Move user DPH from the stack to save area.
	DEC	DPL
	POP	ACC
	MOVX	@DPTR,A		;Move user DPL from the stack to save area.
	MOV	DPL,#IE		;Save the special function registers.
	MOV	A,IE
	MOVX	@DPTR,A
	MOV	IE,#000H
	MOV	DPL,#TCON
	MOV	A,TCON
	MOVX	@DPTR,A
	MOV	TCON,#0
	MOV	DPL,#B		;Start with 'B'.
	MOV	A,B
	MOVX	@DPTR,A
	MOV	DPL,#IP
	MOV	A,IP
	MOVX	@DPTR,A
	MOV	DPL,#P1
	MOV	A,P1
	MOVX	@DPTR,A
	MOV	DPL,#P3
	MOV	A,P3
	MOVX	@DPTR,A
	MOV	DPL,#PSW
	MOV	A,PSW
	MOVX	@DPTR,A
	MOV	DPL,#SCON
	MOV	A,SCON
	MOVX	@DPTR,A
	MOV	DPL,#SP
	MOV	A,SP
	DEC	A		;Compensate the SP for the break interrupt
	DEC	A
	MOVX	@DPTR,A
	MOV	DPL,#TH0
	MOV	A,TH0
	MOVX	@DPTR,A
	MOV	DPL,#TH1
	MOV	A,TH1
	MOVX	@DPTR,A
	MOV	DPL,#TL0
	MOV	A,TL0
	MOVX	@DPTR,A
	MOV	DPL,#TL1
	MOV	A,TL1
	MOVX	@DPTR,A
	MOV	DPL,#TMOD
	MOV	A,TMOD
	MOVX	@DPTR,A		;Save the user internal RAM.
	MOV	DPL,#0		;Set DPTR to start of save area.
	MOV	PSW,#0		;Select register bank O.
	MOV	A,R0		;Save users RO (our POINT0)
	MOVX	@DPTR,A
	MOV	POINT0,#001H	;Then save user RAM.
BRK_LOOP:
	INC	DPTR
	MOV	A,@POINT0
	MOVX	@DPTR,A
	INC	POINT0
	CJNE	POINT0,#128,BRK_LOOP
	MOV	DPTR,#(RAMOFF + UPC + 1)
	POP	ACC
	MOVX	@DPTR,A		; Get first byte of PC into 8155
	DEC	DPL
	POP	ACC
	MOVX	@DPTR,A		; Save second byte of PC off stack
	MOV	SP,#STACK
	MOV	DPL,#MON_FLAGS
	MOVX	A,@DPTR
	MOV	020H,A		;Move the monitor flags storage area to the
				;first eight bit locations.
	MOV	DPL,#BREAK_STATUS
	MOVX	A,@DPTR		;See if break was invoked by the power
	JZ	BREAK_CONTINUE	;on and skip further checks if it was.
				;If not continue.
	MOV	DPTR,#BRKOFF	;Find the cause of the break
	MOVX	A,@DPTR
	MOV	CAUSE_IMAGE,A
	ANL	A,#03CH
	JNZ	BREAK_CONTINUE
	LCALL	ICSTS		;No break set up-was it a keyboard entry?
	JC	BRKMORE
	MOV	ERRNUM,#016H	;Execution over vector at loc 3
	MOV	CAUSE_IMAGE,#4	;Cause is guarded access.
	LJMP	IERROR
BRKMORE:
	LCALL	UPI_IN		;Else get the character
	ANL	A,#07FH
	CJNE	A,#ESC,PRE_UNBREAK ;Return to the user unless char is an ESCAPE.
	MOV	CAUSE_IMAGE, #2	;Cause is user abort.
BREAK_CONTINUE:			;The interrupt is due to a valid break.
				;Determine which one and reenter the
				;monitor at the appropriate point.
	MOV	IE,#0		;Shut down all the interrupts while in the
	MOV	SP,#STACK	;Set up the monitor stack pointer
	MOV	A,CAUSE_IMAGE
	JB	ACC.4,BRK3	;Always adjust for data break
	ANL	A,#028H
	JZ	BRK4		;Bypass adjusting PC for any break
	MOV	A,CAUSE_IMAGE	;except PROG or STEP
	JNB	ACC.6,BRK4	;Check to see if NOP was forced on break.
				;(i.e. PC is too big)
BRK3:	LCALL	READ_PC
	CLR	C
	SUBB	A,#1
	JNC	BRK5
	DEC	B
BRK5:	MOV	PARAM2,A
	MOV	PARAM1,B
	LCALL	WRITE_PC
BRK4:	LCALL	INIT_IO
	MOV	PARAM1,#TOP_PORT
	LCALL	UPI_CMD
	MOV	PARAM1,#0
	LCALL	UPI_OUT
	LCALL	UPI_IN		;Clear UPIOBF
	LCALL	INIT_IO
	LCALL	SET_BAUD
	MOV	C,LSTFLG
	MOV	LINE_START,#0
	CLR	A
	MOV	ACC.6,C
	MOV	PARAM1,A
	LCALL	UPI_CMD
	MOV	DPTR,#(RAMOFF + BREAK_STATUS)
	MOVX	A,@DPTR
	JNZ	BRK1
	LJMP	SIGN_ON
BRK1:	MOV	A,CAUSE_IMAGE
	ANL	A,#01EH
	JZ	BRK2		;Check for cause other than singlestep
	LJMP	RUN_USER_RETURN
BRK2:	MOV	A,CAUSE_IMAGE
	JNB	ACC.5,PRE_UNBREAK ;Reenter execution if not singlestep
	LJMP	STEP51_RETURN	;Return to the step command.
PRE_UNBREAK:
	MOV	DPTR,#(RAMOFF + UPI_DATA_IMAGE)
	SETB	ACC.7
	MOVX	@DPTR,A		;escape
$EJECT
;******************************************************************
;
;	NAME: UNBREAK
;
;	ABSTRACT: Restores the user status and starts execution of the
;	user program. CAUTION:	This routine is position sensitive.
;	It is entered from BREAK as "in line" code.
;
;	INPUTS: All of the users registers and RAM images wil be used.,
;	TOP_STORE
;
;	OUTPUTS: MON_FLAGS
;
;	VARIABLES MODIFIED: A, DPTR, RO, B, PSW, SCON, SP, IP, TH0,
;	TH1, TMOD, TCON, IE, IE0, IT0, PX0
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: UPI_CMD, UPI_OUT
;
;******************************************************************
UNBREAK:MOV	PARAM1,#USART_MODE
	LCALL	UPI_CMD
	MOV	PARAM1,#0FFH
	LCALL	UPI_OUT
	LCALL	UPI_OUT		;Output nulls to clr usart b/f reset in break
	MOV	PARAM1,#TOP_PORT
	LCALL	UPI_CMD
	MOV	DPTR,#(RAMOFF + TOP_STORE)
	MOVX	A,@DPTR
	MOV	PARAM1,A
	LCALL	UPI_OUT
	LCALL	UPI_IN			;Clear UPIOBF
	MOV	PARAM1,#SELECT_CON	;Re-enable the console for I/O
	LCALL	UPI_CMD			;then return
	MOV	A,020H			;Save the MON~LAGS during execution.
	MOV	DPL,#MON_FLAGS
	MOVX	@DPTR,A
	MOV	R0,#127			;First restore the internal RAM.
	MOV	DPL,#127
UNBRK_LOOP:
	MOVX	A,@DPTR
	MOV	@R0,A
	DEC	DPL
	DJNZ	R0,UNBRK_LOOP
	MOVX	A,@DPTR
	MOV	@R0,A
	MOV	DPL,#B
	MOVX	A,@DPTR
	MOV	B,A
	MOV	DPL,#P1
	MOVX	A,@DPTR
	MOV	P1,A
	MOV	DPL,#P3
	MOVX	A,@DPTR
	ORL	A,#0C4H
	MOV	P3,A
	MOV	DPL,#PSW
	MOVX	A,@DPTR
	MOV	PSW,A
	MOV	DPL,#SCON
	MOVX	A,@DPTR
	MOV	SCON,A
	MOV	DPL,#SP
	MOVX	A,@DPTR
	MOV	SP,A
	MOV	DPL,#UPC
	MOVX	A,@DPTR
	PUSH	ACC			; Restore first byte of PC to top of stack
	INC	DPTR
	MOVX	A,@DPTR
	PUSH	ACC			; Second by of CP
	MOV	DPL,#IP
	MOVX	A,@DPTR
	MOV	IP,A
	MOV	DPL,#TH0
	MOVX	A,@DPTR
	MOV	TH0,A
	MOV	DPL,#TH1
	MOVX	A,@DPTR
	MOV	TH1,A
	MOV	DPL,#TL0
	MOVX	A,@DPTR
	MOV	TL0,A
	MOV	DPL,#TL1
	MOVX	A,@DPTR
	MOV	TL1,A
	MOV	DPL,#TMOD
	MOVX	A,@DPTR
	MOV	TMOD,A
	MOV	DPL,#TCON
	MOVX	A,@DPTR
	MOV	TCON,A
	MOV	DPL,#IE
	MOVX	A,@DPTR
	ANL	A,#07EH		;Leave overall enable and external o off unti 1
					;interrupt mode is established.
	MOV	IE,A			;Set up IE.
	MOV	DPL,#DPL
	MOVX	A,@DPTR
	PUSH	ACC			;Push user data pointer into the user stack.
	INC	DPL
	MOVX	A,@DPTR
	PUSH	ACC
	MOV	DPL,#ACC		;Restore the user A register.
	MOVX	A,@DPTR
	POP	DPH
	POP	DPL			;Restore user data pOinter.
	CLR	IE0			;Set up the break logic interrupts.
	SETB	IT0
	SETB	PX0
	ORL	IE,#081H		;Edge mode, highest priority.
	RETI				; 'Return' to the user.
$EJECT
;******************************************************************
;
;	NAME: READ_PC/WRITE_PC
;
;	ABSTRACT:
;	READ PC:	This routine returns a copy of the user program
;	counter in A and B from the page of external RAM devoted to
;	saving the user status.
;
;		WRITE PC:	this routine loads the user program counter
;		with the parameter passed to it.
;
;	INPUTS: PARAM1 (high byte), PARAM2 (low byte)
;
;	OUTPUTS: ACC (low byte), B (high byte), users version of PC
;
;	VARIABLES MODIFIED: DPTR, A, B
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: None
;
;
;******************************************************************
READ_PC:				;Set DPTR to point at the user PC in the
					;user stack.
	MOV	DPTR,#(RAMOFF + UPC)
	MOVX	A,@DPTR
	MOV	B,A			;Load the user pc into Band A.
	INC	DPTR
	MOVX	A,@DPTR
	XCH	A,B
	RET
WRITE_PC:				;Set the DPTR to point at the user PC in the
					;user stack.
	MOV	DPTR,#(RAMOFF + UPC)
	MOV	A,PARAM2		;Write into the user PC.
	MOVX	@DPTR,A
	INC	DPTR
	MOV	A,PARAM1
	MOVX	@DPTR,A
	RET
$EJECT
;******************************************************************
;
;	NAME: CHECK FROM
;
;	ABSTRACT: This routine gets a token and if it is a 'from', it
;	will get th~ number and send it to the users PC. It always
;	leaves this routine with a 'fresh' token whether it finds a
;	'from' or not.
;
;	INPUTS: None
;
;	OUTPUTS: TOKSTR
;
;	VARIABLES MODIFIED: PARAM1, PARAM2
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: IGETOKE, IGETNUM, WRITE_PC
;
;
;******************************************************************
CHECK_FROM:
	CALL	IGETOKE
	CJNE	A,#FROM_TOKE,NOTFRM
	LCALL	IGETNUM
	MOV	PARAM1,VALHGH
	MOV	PARAM2,VALLOW
	CALL	WRITE_PC
	CALL	IGETOKE
NOTFRM:	RET
$EJECT
;******************************************************************
;
;	NAME: BREAK_VECTOR
;
;	ABSTRACT: This routine writes location 03 as a break
;	vector, and verifies that it was able to write. This vector
;	does a long call to a service routine for all level zero
;	interrupts. Level zero interrupts include:
;		UPI interrupts (keyboard closures, USART buffer
;			empty or full, cassette characters rec'd)
;		Hardware breakpoints (PROG, DATA, GUARDED ACCESS,
;			SINGLESTEP)
;
;	INPUTS: None
;
;	OUTPUTS: Code memory locations 3, 4 and 5
;
;	VARIABLES MODIFIED: DPTR, A, ERRNUM
;
;	ERROR EXITS: 17H (NO RAM AT LOCATION 3)
;
;	SUBROUTINES ACCESSED DIRECTRLY: IERROR
;
;
;******************************************************************
BREAK_VECTOR:
	MOV	DPTR,#0003H		;Point to INT0 vector address again
	MOV	A,#002H			;Store a "LCALL" instruction
	MOVX	@DPTR,A
	MOV	A,#HIGH(BREAK)	;Store the high byte of address for "break"
	INC	DPTR
	MOVX	@DPTR,A
	INC	DPTR
	MOV	A,#LOW(BREAK)	;Store low byte of "break" address
	MOVX	@DPTR,A
	CLR	A
	MOVC	A,@A+DPTR		; Verify that the write did go into RAM
	CJNE	A,#LOW(BREAK),B_V_ERR	;if not the same, go to error
	RET
;*********************************************************************
B_V_ERR:
	MOV	ERRNUM,#017H		;No RAM at location 3
	LJMP	IERROR
$EJECT
;******************************************************************
;
;	NAME: STEP CMD
;
;	ABSTRACT: STEP executes one or more instructions at a user
;	selectable rate, breaking after each instruction.
;	The monitor displays the contents of the PC, ACC,
;	DPTR, SP and, optionally, a specified bit or byte.
;
;	INPUTS: None
;
;	OUTPUTS: BREAK_STATUS
;
;	VARIABLES MODIFIED: A, TOKSAV, DPTR, ERRNUM, PARAM1, BREAK_STATUS
;
;	ERROR EXITS: 03H (NUMBER EXPECTED)
;		09H (DECIMAL NUMBER EXPECTED)
;
;	SUROUTINES ACCESSED DIRECTRLY: CHECK_FROM, IGETOKE, IGETEOL,
;	BREAK_VECTOR, UPI_CMD, UPI_OUT, UNBREAK, IEOL_CHECK, IERROR
;
;******************************************************************
STEP_CMD:
	CALL	CHECK_FROM
	MOV	DPTR,#(RAMOFF + SAVE_SEL)
	CLR	A
	MOVX	@DPTR,A			;Clear SAVE_SEL to avoid unwanted display..
	MOV	A,TOKSTR
	CJNE	A,#COMMA_TOKE,STPEOL
	CALL	IGETOKE
	ANL	A,#0F8H			;Strip out the lower 3 bits
	CJNE	A,#080H,DCLAUSE		;and skip to process the delay clause if
					;not a display memory token.
	MOV	TOKSAV,TOKSTR		;Else proceed with display clause.
	LCALL	IGETNUM		;Save the address to be displayed in external
					;RAM.
	MOV	DPTR,#(RAMOFF + ADDR_SAVE_HIGH)
	MOV	A,VALHGH
	MOVX	@DPTR,A
	INC	DPTR
	MOV	A,VALLOW
	MOVX	@DPTR,A
	MOV	DPL,#SAVE_SEL
	MOV	A,TOKSAV
	MOVX	@DPTR,A			;Save token to be displayed after STEP
	CALL	IGETOKE
	CJNE	A,#COMMA_TOKE,STPEOL
	CALL	IGETOKE
DCLAUSE:MOV	A,TOKSTR
	MOV	ERRNUM,#003H		;Number expected
	CJNE	A,#NUMBER_TOKE,EXERRO
	MOV	A,#9
	CJNE	A,VALLOW,LAB18
LAB18:	MOV	ERRNUM,#009H		;Decimal number expected
	JC	EXERRO			;Error unless number is less than 9.
	MOV	A,VALHGH
	JNZ	EXERRO			;Upper bits must be zero also.
	MOV	DPTR,#(RAMOFF + DELAY)
	MOV	A,VALLOW
	MOVX	@DPTR,A
	LCALL	IGETEOL			;Check that next entry is CR
STPLOP:
	MOV	A,#MULTISTEP
STEP51:
	MOV	DPTR,#(RAMOFF + BREAK_STATUS)
	MOVX	@DPTR,A
	CALL	BREAK_VECTOR
	MOV	PARAM1,#GR_PORT
	LCALL	UPI_CMD
	MOV	PARAM1,#CLR_BRK_LATCHES ;Clear all break latches
	LCALL	UPI_OUT
	MOV	PARAM1,#SINGLE_BREAK
	LCALL	UPI_OUT			;Send it to the UPl data channel
	LJMP	UNBREAK
STPEOL:	LCALL	IEOL_CHECK
	MOV	A,#SINGLESTEP
	JMP	STEP51
EXERRO:	LJMP	IERROR
$EJECT
;******************************************************************
;
;	NAME: STEP51_RETURN
;
;	ABSTRACT: After the branch to UNBREAK in STEP_CMD, the user
;	execution has begun. Exit from execution with the STEP_FLAG
;	set will result in a branch to STEP51_RETURN.
;
;	INPUTS: SAVE_SEL, BREAK_STATUS, DELAY, USER_SP, ACC, DPTR,
;	ADDR_SAVE_IGH, ADDR_SAVE_LOW
;
;	OUTPUTS: None
;
;	VARIABLES MODIFIED: PARAM1, PARAM2, ERRNUM, CAUSE_IMAGE, DPTR,
;
;	ERROR EXITS: 16H (EXECUTION ACROSS LOCATION 3)
;
;	SUBROUTINES ACCESSED DIRECTRLY: INEWLINE, READ_PC, ICO, ILSTWRD,
;	SPACCO, ILSTBYT, IFETCH, ITIME, ICSTS, UPI_IN, ICI,
;	IWAIT_FOR_USER, IERROR
;
;
;******************************************************************
STEP51_RETURN:
	LCALL	INEWLINE		;Output a CR-LF.
	LCALL	READ_PC			;Output the contents of the user PC to the
	MOV	PARAM1,B		;console.
	MOV	PARAM2,A
	CJNE	PARAM1,#0E0H,NOT_STEP_THREE
	CJNE	PARAM2,#3,NOT_STEP_THREE
	MOV	ERRNUM,#016H		;Adr 3 executed
	MOV	CAUSE_IMAGE,#4		;Cause is guarded access to loc 3
	JMP	IERROR
NOT_STEP_THREE:
	MOV	PARAM1,#'P'		;Output PC label
	LCALL	ICO
	MOV	PARAM1,B		;Restore PC value to register for display.
	LCALL	ILSTWRD			;Output address
	LCALL	SPACCO			;Output space
	MOV	PARAM1,#'A'		;Output user accumulator label
	LCALL	ICO
	MOV	DPTR,#(RAMOFF + ACC)
	MOVX	A,@DPTR
	MOV	PARAM1,A		;Call ILSTBYT(user ACC).
	LCALL	ILSTBYT
	LCALL	SPACCO
	MOV	PARAM1,#'D'
	LCALL	ICO			;Output DPTR label
	MOV	DPTR,#(RAMOFF + DPL)
	MOVX	A,@DPTR			;Displays the low and high byte of DPTR
	MOV	PARAM2,A
	INC	DPTR
	MOVX	A,@DPTR
	MOV	PARAM1,A
	LCALL	ILSTWRD
	LCALL	SPACCO
	MOV	PARAM1,#'S'		;Output the SP label
	LCALL	ICO
	MOV	DPTR,#(RAMOFF + SP)
	MOVX	A,@DPTR
	MOV	PARAM1,A
	LCALL	ILSTBYT			;Output the value of SP
	MOV	DPTR,#(RAMOFF + SAVE_SEL)
	MOVX	A,@DPTR			;Get the select code saved in memory.
	MOV	TOKSAV,A
	JZ	STEP51_EXIT		;Exit if no optional display.
	LCALL	SPACCO			;Output space
	MOV	PARAM1,#'('
	LCALL	ICO			;Output left parentheses
	MOV	A,TOKSAV		;Move saved token into ACC
	ANL	A,#007H			;Mask lower 3 bits
	MOV	SELECT,A		;Move lower 3 bits into selector for FETCH
	INC	DPTR			;Fetch the saved address.
	MOVX	A,@DPTR
	MOV	PNTHGH,A
	INC	DPTR
	MOVX	A,@DPTR
	MOV	PNTLOW,A		;Fetch the memory byte the user wants
					; disp1ayed.
	LCALL	IFETCH
	MOV	PARAM1,A		;And display it.
	LCALL	ILSTBYT
	MOV	PARAM1,#')'		;Output right parentheses
	LCALL	ICO
STEP51_EXIT:
	MOV	DPTR,#(RAMOFF + BREAK_STATUS)
	MOVX	A,@DPTR
	CJNE	A,#MULTISTEP,SSRET
	MOV	DPTR,#(RAMOFF + DELAY)
	MOVX	A,@DPTR			;Execute multiple Single steps
	MOV	DLYCNT,A
STPDLY:	MOV	A,DLYCNT
	JZ	DLY_THRU
	DEC	DLYCNT
	MOV	PARAM1,#013H
	MOV	PARAM2,#088H
	LCALL	ITIME			;Delay for about 1/2 second per DLYCNT
	JMP	STPDLY			;Loop until delay count = 0
DLY_THRU:
	MOV	PARAM1,#000H
	MOV	PARAM2,#0A5H
	LCALL	ITIME			;Delays 16ms
	LCALL	ICSTS
	JC	STEP_STOP		;No carry means no input pending
STPLOP_REACH:
	JMP	STPLOP
STEP_STOP:
	LCALL	UPI_IN
	CJNE	A,#ESC,STPLOP_REACH
	LCALL	ICI			;First esc stops step,2nd will exit.
	JMP	STPLOP_REACH		;Any key after 1st esc resumes step
;******************************************************************
SSRET:	LCALL	IWAIT_FOR_USER
	JMP	START
EXERR1:	JMP	IERROR

$EJECT
;******************************************************************
;
;	NAME: GO CMD
;
;	ABSTRACT: This routine sets up conditions for entering user execution.
;	It looks for partition information and breakpoints and saves
;	an image of break enable hardware in software.
;
;	INPUTS: GR
;
;	OUTPUTS: GR, BREAK_STATUS
;
;	VARIABLES MODIFIED: A, ERRNUM, DPTR, PARAM1, PARAM2, GR
;
;	ERROR EXITS: 0BH (BREAK ENABLE SYNTAX)
;
;	SUBROUTINES ACCESSED DIRECTRLY: CHECK_FROM, IGETEOL, IGETOKE,
;	IEOL_CHECK, BREAK_VECTOR, UPI_CMD, UPI_OUT, UNBREAK, IPRINT_STRING,
;	READ_PC, ILSTWRD, IWAIT_FOR_USER
;
;******************************************************************
GO_CMD:
	LCALL	CHECK_FROM
	XRL	A,#EOL_TOKE
	JZ	RUN_USER		;If have the end of line token go to user
					;emulation.
					;If not then find out what kind of emulation
					;is required.

	MOV	A,TOKSTR		; First restore the token.
	CJNE	A,#FOREVER_TOKE,NOTFOR
					;See if token is FOREVER token
	LCALL	IGETEOL			;Wait for CR after FOREVER
	MOV	DPTR,#(RAMOFF + GR)	;Copy break enable image into hrdwr
	MOV	A,#NO_BREAK
	MOVX	@DPTR,A
	LJMP	RUN_USER		;Jump to GO loop
NOTFOR:	MOV	ERRNUM,#00BH		;BRK enable syntax
	CJNE	A,#TILL_TOKE,EXERR1	;Jump to error routine if not TIL token
	LCALL	IGETOKE
	CJNE	A,#DATA_TOKE,NOTDAT	;See if next input was data break
	LCALL	IGETEOL			;Make sure next input is CR
	MOV	DPTR,#(RAMOFF + GR)	;Copy break enable image into hrdwr
	MOV	A,#DATA_BREAK
	MOVX	@DPTR,A
	LJMP	RUN_USER		;Jump to GO loop
NOTDAT:	MOV	ERRNUM, #00BH		;BRK enable syntax
	CJNE	A,#PROGRAM_TOKE,EXERR1 ;If program break not entered by now,
					;Then error
	LCALL	IGETOKE
	CJNE	A,#OR_TOKE,PGMBRK	;See if OR was entered in break sequence
	LCALL	IGETOKE
	MOV	ERRNUM,#00BH		;BRK enable syntax
	CJNE	A,#DATA_TOKE,EXERR1	;Make sure data token was entered next
	LCALL	IGETEOL			;Make sure CR was entered last
	MOV	DPTR,#(RAMOFF + GR)	;Copy break enable image into sftwr
	MOV	A,#(DATA_BREAK OR PROGRAM_BREAK)
	MOVX	@DPTR,A
	LJMP	RUN_USER
PGMBRK:	LCALL	IEOL_CHECK
	MOV	DPTR,#(RAMOFF + GR)	;Copy break enable image into sftwr
	MOV	A,#PROGRAM_BREAK
	MOVX	@DPTR,A
RUN_USER:
	MOV	DPTR,#(RAMOFF + BREAK_STATUS)
	MOV	A,#NOT_STEP
	MOVX	@DPTR,A		;Clear the step flag to show a 'run' condition
	LCALL	BREAK_VECTOR
	MOV	PARAM1,#HIGH(XEQT_MSG)
	MOV	PARAM2,#LOW(XEQT_MSG)
	LCALL	IPRINT_STRING
	MOV	PARAM1,#GR_PORT
	LCALL	UPI_CMD
	MOV	PARAM1,#CLR_BRK_LATCHES ;Clear all break latches
	LCALL	UPI_OUT
	MOV	DPTR,#(RAMOFF + GR)	;Copy break enable image into hrdwr
	MOVX	A,@DPTR
	MOV	PARAM1,A
	LCALL	UPI_OUT			;Send it to the UPI data channel
	LJMP	UNBREAK
;******************************************************************
RUN_USER_RETURN:
	MOV	PARAM1,#HIGH(BREAK_MSG)
	MOV	PARAM2,#LOW(BREAK_MSG)
	LCALL	IPRINT_STRING
	LCALL	READ_PC
	MOV	PARAM1,B			;Display the user PC
	MOV	PARAM2,A
	LCALL	ILSTWRD
	LCALL	IWAIT_FOR_USER			;And goto the monitor.
	JMP	START
;**********************************************************************
XEQT_MSG:
	DB	17,CR,LF,('EXECUTION BEGUN')
BREAK_MSG:
	DB	22,CR,LF,('EXECUTION HALTED PC=')
