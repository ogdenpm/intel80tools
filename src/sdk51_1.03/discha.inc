;******************************************************************
;
;	NAME: MEMORY_CMD
;
;	ABSTRACT: This routine saves the kind of memory operation
;	selected and checks for partitions and equal signs in order
;	to dicide whether a fill, load, display or block move is
;	requested.
;
;	INPUTS: TOKSTR
;
;	OUTPUTS: None
;
;	VARIABLES MODIFIED: A, TOKSAV, SELECT, PNTLOW, PNTHGH, B
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: IGETOKE, IGET_PART, BMOVE,
;	IEOL_CHECK, DISMEM, LODMEM, FILLMEM
;
;
;******************************************************************
MEMORY_CMD:	MOV	A,TOKSTR
	ANL	A,#07			;Last 3 bits of token determine selector
	MOV	TOKSAV,TOKSTR
	MOV	SELECT,A		;Load selector
	CALL	IGETOKE
	LCALL	IGET_PART		;Partition? Returns 1 bit (C)=true if part.
	MOV	PNTLOW,PARTIT_LO_LOW
	MOV	PNTHGH,PARTIT_LO_HIGH
	MOV	B.0,C
	CJNE	A,#EQUAL_TOKE,DIS_OR_ERR ;Check for equal sign from GET_PART
	JNB	B.0,LODMEM		;Single byte load (CBY addr = data)
	CALL	IGETOKE
	CJNE	A,#CBYTE_TOKE,FILLMEM	;Block move (CBY addr TO addr =CBY addr)
	LJMP	BMOVE			;Fill memo (CBY addr TO addr=data)
DIS_OR_ERR:
	LCALL	IEOL_CHECK
	LJMP	DISMEM			;Display memo (CBY addr TO addr-no equalsign)
$EJECT
;******************************************************************
;
;	NAME: LODMEM
;
;	ABSTRACT: The pointer will be set to memory address upon entry.
;	Parsing continues as long as new tokens are available on the
;	command line. Each new token either supplies a new value which
;	goes into memory or a <CR> which terminates the command. Commas
;	are expected between any two numbers and at the end of a line
;	when a continuation is desired. When entry of data has gone
;	beyond one line (a continuation line) the line buffer is filled
;	with the message and address which tells the user what address
;	is currently being modified.
;
;	INPUTS: SELECT, PNTHGH, PNTLOW
;
;	OUTPUTS: Memory which was supposed to be accessed by the command
;	typed in at the console.
;
;	VARIABLES MODIFIED: PARAM1, A, POINT0, LINE_START
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: IGETNUM, ISTORE, IGETOKE, INC_PNT,
;	ISAVE_AND_DISPLAY, IEOL_CHECK, IERROR
;
;
;******************************************************************
LODMEM:	LCALL	IGETNUM
LDLOOP:	MOV	PARAM1,VALLOW		;Load PARAM1 with data to be output
	LCALL	ISTORE			;Output data into memory
	LCALL	INC_PNT
	CALL	IGETOKE			;Get next token and character
	CJNE	A,#COMMA_TOKE,EOLMEM	;Jump to EOLMEM if token is not comma token
	CALL	IGETOKE			;Get next token and character after comma
	CJNE	A,#EOL_TOKE,NUMMEN	;Check if CR was entered
	MOV	POINT0,#LINBUF
	MOV	A,SELECT		;Choose first char, depending on type
	MOV	@POINT0,#'R'		;of memory access in progress
	CJNE	A,#(CBYTE_TOKE AND 007H),B_LAB_1
	MOV	@POINT0,#'C'
B_LAB_1:CJNE	A,#(DBYTE_TOKE AND 007H),B_LAB_2
	MOV	@POINT0,#'D'
B_LAB_2:CJNE	A,#(XBYTE_TOKE AND 007H),B_LAB_3
	MOV	@POINT0,#'X'
B_LAB_3:INC	POINT0
	MOV	@POINT0,#'B'
	INC	POINT0
	MOV	@POINT0,#'Y'
	CJNE	A,#(RBIT_TOKE AND 007H),T_LAB
	MOV	@POINT0,#'I'		;Choose third char for bit or byte type
T_LAB:	INC	POINT0
	MOV	@POINT0,#'T'
	INC	POINT0
	MOV	@POINT0,#' '
	INC	POINT0
	MOV	PARAM1,PNTHGH
	LCALL	ISAVE_AND_DISPLAY
	MOV	PARAM1,PNTLOW
	LCALL	ISAVE_AND_DISPLAY
	MOV	@POINT0,#'='
	MOV	LINE_START,#00AH
	CALL	IGETOKE			;Get next token and character
NUMMEN:	CJNE	A,#NUMBER_TOKE,EOLMEM	;Check that a number was last char entered

	JMP	LDLOOP			;Loop until CR entered
EOLMEM:	LJMP	IEOL_CHECK
DISERR:	LJMP	IERROR
$EJECT
;******************************************************************
;
;	NAME: FILLMEM
;
;	ABSTRACT: This routine fills the memory selected with a single
;	value from PNTLOW and PNTHGH up to the high end of the
;	partition.
;
;	INPUTS: PNTLOW. PNTHGH. PARTIT_HI_LOW. PARTIT_HI_HIGH
;
;	OUTPUTS: Memory which was supposed to be accessed by the
;	command typed in at the console.
;
;	VARIABLES MODIFIED: ERRNUM. A. TEMP_LOW. VALLOW. PARAM1. C
;
;	ERROR EXITS: 1AH (TOKEN MUST BE A NUMBER)
;
;	SUBROUTINES ACCESSED DIRECTRLY: IGETEOL. ISTORE. INC PNT
;
;
;******************************************************************
FILLMEM:MOV	ERRNUM,#01AH		;Token must be a number
	CJNE	A,#NUMBER_TOKE,DISERR
	MOV	TEMP_LOW,VALLOW
	LCALL	IGETEOL
	MOV	VALLOW,TEMP_LOW
FILLOOP:MOV	PARAM1,VALLOW		;Load PARAM1 with single byte data
	LCALL	ISTORE			;Output data into memory
	CLR	C
	MOV	A,PNTLOW
	SUBB	A,PARTIT_HI_LOW		;Subtract pointer from ending address
	MOV	A,PNTHGH
	SUBB	A,PARTIT_HI_HIGH	;to see if partition is full yet
	JNC	FILL1			;If not. continue filling
	LCALL	INC_PNT
	JMP	FILLOOP
FILL1:	RET
$EJECT
;******************************************************************
;
;	NAME: DISMEM
;
;	ABSTRACT: This routine displays the data values of the selected
;	memory partition to the console.
;
;	INPUTS: PNTLOW, PNTHGH, PARTIT_HI_LOW, PARTIT_HI_HIGH
;
;	OUTPUTS: None
;
;	VARIABLES MODIFIED: COUNTR, A, DPTR, PARAM1, PARAM2
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: INEWLINE, IDISPLAY_TOKEN, SPACCO,
;	ILSTWRD, ICO, IFETCH, ILSTBYT, IWAIT_FOR_USER,ICONTINUATION_LINE
;
;
;*******************************************************************
DISMEM:	MOV	COUNTR,#1		;Load counter with 1
DISLOP:	DEC	COUNTR
	MOV	A,COUNTR
	JNZ	DISFET			;Jump to DISFET if counter is not zero
	LCALL	INEWLINE
	MOV	A,SELECT		;Move selector into ACC
	MOV	DPTR,#LAB23		;Load DPTR with base of table
	MOVC	A,@A+DPTR
	MOV	PARAM1,A		;Setup for DISPLAY_TOKEN
	CALL	IDISPLAY_TOKEN		;Output token
	LCALL	SPACCO			;Output space
	MOV	PARAM2,PNTLOW
	MOV	PARAM1,PNTHGH		;Set-up for ILSTWRD
	LCALL	ILSTWRD			;Output address
	MOV	PARAM1,#'='
	LCALL	ICO			;Output an equal sign
	MOV	COUNTR,#4		;Load counter with 4
DISFET:	LCALL	IFETCH			;to get memory location
	MOV	PARAM1,A		;Set-up for ILSTBYT
	LCALL	ILSTBYT
	MOV	A,PNTLOW
	CJNE	A,PARTIT_HI_LOW,COUNT1	;See if PARTIT_LO_LOW=EALOW
	MOV	A,PNTHGH
	CJNE	A,PARTIT_HI_HIGH,COUNT1 ;See if PARTIT_LO_HlGH=EAHGH
	LJMP	IWAIT_FOR_USER
COUNT1:	MOV	A,COUNTR
	CJNE	A,#1,NTLAST		;See if counter = I,
	LCALL	ICONTINUATION_LINE
NOWAIT:	LCALL	INC_PNT
	JMP	DISLOP			;Loop until PNT is > EA
NTLAST:	MOV	PARAM1,#','
	LCALL	ICO			;To output a comma
	JMP	NOWAIT

LAB23:	DB	CBYTE_TOKE
	DB	RBYTE_TOKE
	DB	DBYTE_TOKE
	DB	00
	DB	RBIT_TOKE
	DB	00
	DB	XBYTE_TOKE
$EJECT
;******************************************************************
;
;	NAME: BMOVE
;
;	ABSTRACT: This routine will transfer CBYTE type memory from
;	a specific location to another location in blocks of contiguous
;	code. It does not relocate addresses and it is possible
;	to lose code by writing a block over the TOP address. The
;	pointer direction is changed depending on the direction of
;	the move so that no change to the data will occur if the
;	destination and source blocks overlap.
;
;	INPUTS: SELECT, PARTIT_HI_LOW, PARTIT_HI_HIGH, LENGTH_LOW,
;	LENGTH_HIGH, PARTIT_LO_LOW, PARTIT_LO_HIGH
;
;	OUTPUTS: Memory which was supposed to be accessed by the
;	command typed in at the console.
;
;	VARIABLES MODIFIED: A, ERRNUM, C, PCNTLO, PCNTHI, PNTLOW, C,
;	PARAM1, PNTHGH
;
;	ERROR EXITS: 18H (CBYTE TYPE ONLY)
;
;	SUBROUTINES ACCESSED DIRECTRLY: IGETNUM, SWAP_POINTERS, IFETCH,
;	DEC_PNT, ISTORE
;
;
;******************************************************************

BMOVE:	MOV	A,SELECT
	MOV	ERRNUM,#018H			;CBYTE type only
	CJNE	A,#(CBYTE_TOKE AND 007H),ERRMOD
	LCALL	IGETNUM
	MOV	PCNTLO,VALLOW
	MOV	PCNTHI,VALHGH
	CLR	C
	MOV	A,PNTLOW
	SUBB	A,PCNTLO
	MOV	A,PNTHGH
	SUBB	A,PCNTHI
	JC		DOWN_MOVE
	MOV	PNTLOW,PARTIT_HI_LOW
	MOV	PNTHGH,PARTIT_HI_HIGH
	MOV	A,PCNTLO
	ADD	A,LENGTH_LOW
	MOV	PCNTLO,A
	MOV	A,PCNTHI
	ADDC	A,LENGTH_HIGH
	MOV	PCNTHI,A
UP_MOVE:LCALL	SWAP_POINTERS
	LCALL	IFETCH
	MOV	PARAM1,A
	LCALL	DEC_PNT
	LCALL	SWAP_POINTERS
	LCALL	ISTORE
	CLR	C
	MOV	A,PARTIT_LO_LOW
	SUBB	A,PNTLOW
	MOV	A,PARTIT_LO_HIGH
	SUBB	A,PNTHGH
	JNC	BEND
	LCALL	DEC_PNT
	JMP	UP_MOVE
DOWN_MOVE:
	LCALL	SWAP_POINTERS
	LCALL	IFETCH
	MOV	PARAM1,A
	LCALL	INC_PNT
	LCALL	SWAP_POINTERS
	LCALL	ISTORE
	CLR	C
	MOV	A,PNTLOW
	SUBB	A,PARTIT_HI_LOW
	MOV	A,PNTHGH
	SUBB	A,PARTIT_HI_HIGH
	JNC	BEND
	LCALL	INC_PNT
	JMP	DOWN_MOVE
BEND:	RET
$EJECT
;******************************************************************
;
;	NAME: BR_CMD
;
;	ABSTRACT: This routine checks a token to see if it is a
;	breakpoint display or change. If it is change, it sets the pamameters
;	of the range and clears or sets the breakpoints requested. (ABR is
;	a change only command). If it is a display command, each breakpoint
;	is output to the console. Reset is the default condition.
;	If the token is BR, the entire breakpoint RAM is cleared and then
;	breakpoints are added. If it is ABR, they are added without clearing
;	RAM first.
;
;	INPUTS: TOKSTR
;
;	OUTPUTS: Bits within the breakpoint hardware register.
;
;	VARIABLES MODIFIED: TOKSAV, A, ERRNUM, PARAM1, PARAM2, PARAM3, PARAM4,
;	LINE_START, POINT0, PNTLOW, PNTHGH, DPTR, VPC_lOW, VPC_HIGH,
;	ANY_BR_FlAG, FIRST_FLAG
;
;	ERROR EXITS: 19H (DISPLAY ONLY COMMAND)
;		05H (EQUAL OR RETURN EXPECTED)
;		OCH (NUMBER OR RESET REQUIRED)
;
;	SUBROUTINES ACCESSED DIRECTRLY: IGETOKE, IERROR, IGET_PART, IEOL_CHECK
;	IGETEOL, LSSEQL, IDISPlAY_TOKEN, IWAIT_FOR_USER, INC_PNT,
;	ICONTINUATION_lINE, ILSTWRD, SPACCO, INEWLINE, ICO, IERROR
;	BRK_LINE_HDR, SETBRK, CLRBRK
;
;
;******************************************************************
BR_CMD:	MOV	TOKSAV,TOKSTR		;Save last token for comparison
	CALL	IGETOKE			;Get next token
	CJNE	A,#EOL_TOKE,EQLMOD	;Check if token is end of line
	MOV	A,TOKSAV		;Move last token into ACC
	CJNE	A,#ABR_TOKE,LSTBRK	;Jump to list mod if not ABR token
	MOV	ERRNUM,#019H		;ABR is not a displayable command
ERRMOD:	LJMP	IERROR
EQLMOD:	MOV	ERRNUM,#005H		;Equal or return expected
	CJNE	A,#EQUAL_TOKE,ERRMOD	;Error if '=' not entered here
	CALL	IGETOKE
	CJNE	A,#NUMBER_TOKE,RSTMOD
	MOV	A,TOKSAV		;Recall last token entered
	CJNE	A,#BR_TOKE,NUMMOD	;Check if it was break token
	LCALL	CLRBRK		;Clear breakpoints
NUMMOD:	LCALL	IGET_PART
	LCALL	SETBRK
	MOV	A,TOKSTR		;Recall present token
	CJNE	A,#COMMA_TOKE,ENDMOD	;Check if comma was entered
	CALL	IGETOKE
	CJNE	A,#EOL_TOKE,NUMMOD	;Check for EOl
	MOV	LINE_START,#004H
	MOV	POINT0,#LINBUF
	MOV	@POINT0,#'A'
	INC	POINT0
	MOV	@POINT0,#'B'
	INC	POINT0
	MOV	@POINT0,#'R'
	INC	POINT0
	MOV	@POINT0,#'='
	CALL	IGETOKE
	CJNE	A,#EOL_TOKE,NUMMOD
ENDMOD:	LJMP	IEOL_CHECK
RSTMOD:	MOV	ERRNUM,#00CH		;Number or reset required
	CJNE	A,#RESET_TOKE,ERRMOD	;Check for reset entered
	LCALL	CLRBRK
	LJMP	IGETEOL
;******************************************************************
LSTBRK:	CLR	A
	MOV	PNTLOW,A		;Clear low byte of break pointer
	MOV	PNTHGH,A		;Clear high byte of break pointer
	CLR	ANY_BR_FLAG
	SETB	FIRST_FLAG
LAB2:	MOV	DPTR,#BRKOFF
	MOV	PARAM1,#MAXHGH
	MOV	PARAM2,#MAXLOW
	MOV	PARAM3,PNTHGH
	MOV	PARAM4,PNTLOW		;Set up for LSSEQL test
	LCALL	LSSEQL			;Check that P??? <= MAX???
	JC	LAB5B			;Exit if greater than
	JB	ANY_BR_FLAG,BRKEND	;If any breakpoints were displayed
					;don't display reset
	LCALL	BRK_LINE_HDR
	MOV	PARAM1,#RESET_TOKE
	CALL	IDISPLAY_TOKEN
BRKEND:	LJMP	IWAIT_FOR_USER
LAB5B:	MOV	A,PNTLOW		;Load ACC with break pointer low addr
	ADD	A,DPL			;Add low addr of break to pointer
	MOV	DPL,A			;Put new low addr back into DPL
	JNC	LAB5A
	INC	DPH			;Increment DPH if DPL had a carry
LAB5A:	MOV	A,PNTHGH
	ADD	A,DPH
	MOV	DPH,A
	MOVX	A,@DPTR			;Load ACC with external RAM memory
	JNB	ACC.0,LAB3		;Branch if break is set.
	LCALL	INC_PNT
	JMP	LAB2

LAB3:	MOV	VPC_LOW,PNTLOW		;Save break pointer low
	MOV	VPC_HIGH,PNTHGH		;Save break pointer high
	SETB	ANY_BR_FLAG
BK1LOP:	MOV	DPTR,#BRKOFF
	MOV	PARAM3,PNTHGH
	MOV	PARAM4,PNTLOW		;Set up for LSSEQL
	LCALL	LSSEQL			;Check that P??? <= MAX???
	JNC	LSTOUT			;Jump to LSTOUT if greater than
	MOV	A,PNTLOW		;Load ACC with low addr of break pointer
	ADD	A,DPL			;Add break RAM low addr offset to pointer low
	MOV	DPL,A			;Put new addr back into DPL
	JNC	LAB6A
	INC	DPH			;Increment DPH if DPL produced a carry
LAB6A:	MOV	A,PNTHGH
	ADD	A,DPH
	MOV	DPH,A
	MOVX	A,@DPTR			;Load ACC with data in break RAM
	JB	ACC.0,LSTOUT		;Branch if break is off.
	LCALL	INC_PNT
	JMP	BK1LOP			;Loop unti 1 0 occurs or end of break RAM
LSTOUT:	MOV	PARAM3,VPC_HIGH
	MOV	PARAM4,VPC_LOW		;Set up for LSSEQL
	LCALL	LSSEQL			;Check that SA??? <= MAX???
	JNC	LAB2			;Jump to BRKO and exit if true
	JB	FIRST_FLAG,LB_10
	LCALL	ICONTINUATION_LINE
LB_10:	LCALL	BRK_LINE_HDR
	CLR	FIRST_FLAG
	MOV	PARAM1,VPC_HIGH
	MOV	PARAM2,VPC_LOW		;Set up for ILSTWRD
	LCALL	ILSTWRD			;Output starting addr of one's in BRK RAM
	INC	VPC_LOW			;Increment starting address low
	MOV	A,VPC_LOW
	JNZ	LAB7			;Check for rollover
	INC	VPC_HIGH		; Increment' starting addr high if true
LAB7:	MOV	A,VPC_HIGH
	CJNE	A,PNTHGH,OUTOKE		;Jump to OUTOKE if VPC_HIGH+l <> PNTHGH
	MOV	A,VPC_LOW
	CJNE	A,PNTLOW,OUTOKE		;Jump to DUTOKE if VPC_LOW+l <> PNTLOW
	JMP	LAB2			;Go process the end of line.

OUTOKE:	LCALL	SPACCO
	MOV	PARAM1,#TO_TOKE
	CALL	IDISPLAY_TOKEN		;Call display_token(to_toke).
	LCALL	SPACCO
	MOV	A,PNTLOW		;Load ACC with break poi nter low
	DEC	A			;Oecrement break pointer low
	MOV	PARAM2,A		;Set up for ILSTWRO
	CPL	A
	MOV	PARAM1,PNTHGH		;Set up for ILSTWRD
	JNZ	LAB8			;Check for roll over
	DEC	PARAM1			;Decrement break pointer high if true
LAB8:	LCALL	ILSTWRD
;
	JMP	LAB2			;Continue the display of break RAM.
					;as soon as the user hits a character
BRK_LINE_HDR:
	LCALL	INEWLINE
	MOV	PARAM1,#BR_TOKE
	CALL	IDISPLAY_TOKEN
	MOV	PARAM1,#'='
	LJMP	ICO
$EJECT

;*****END OF LSTBRK*****

CLRBRK:	MOV	PARAM1,#MAXLOW		;Load PARAM1 with size of break RAM,low 8 bits
	MOV	PARAM2,#(MAXHGH + 1)	;Load PARAM2 with size of break RAM+1,high bits
	MOV	DPTR,#BRKOFF		;Load DPTR with break RAM offset
	MOV	A,#001H			;To clear the break condition.
CLRLOP:	MOVX	@DPTR,A			;Fill break RAM
	INC	DPTR			;Increment pointer at break RAM
	DJNZ	PARAM1,CLRLOP		;Repeat loop until PARAM1=0
	MOVX	@DPTR,A			;Once more for PARAM1=0
	DJNZ	PARAM2,CLRLOP		;Continue loop until PARAM2=0
	RET				;Exit from CLRBRK
;*****END OF CLRBRK*****

SETBRK:	CLR	C
	MOV	A,PARTIT_HI_LOW		;Load ACC with ending addr low
	SUBB	A,PARTIT_LO_LOW		;To obtain number of locations to set
	MOV	DPL,A			;Save low number in PARAM4
	MOV	A,PARTIT_HI_HIGH	;Load ACC with ending addr high
	JB	ACC.7,BRKERR
	SUBB	A,PARTIT_LO_HIGH	;Subtract starting addr high from ending addr
	MOV	DPH,A			;Save high break count in PARAM3
	INC	DPTR
	INC	DPH
	MOV	PARAM1,DPH
	MOV	PARAM2,DPL
	MOV	DPTR,#BRKOFF
	MOV	A,PARTIT_LO_HIGH
	ANL	A,#MAXHGH
	MOV	TEMP,A
	MOV	A,PARTIT_LO_LOW		;Put starting addr low in ACC
	ADD	A,DPL			;Add break offset low
	MOV	DPL,A			;Put back into data pointer
	MOV	A,TEMP			;Load ACC with starting addr high
	ADDC	A,DPH			;Add break offset high
	MOV	DPH,A			;Load DPH with starting addr high + offset
OUT1BK:	CLR	A			;To output O'S
	MOVX	@DPTR,A			;Load break RAM
	INC	DPTR			;Increment break RAM pointer
	DJNZ	PARAM2,OUT1BK		;Loop until count low=O
	DJNZ	PARAM1,OUT1BK		;Loop until PARAM3=0
	RET				;Exit from SETBRK
;*****END OF SETBRK*****

BRKERR:	MOV	ERRNUM,#00DH		;7 is the error number for
					;break range low> range high
	LJMP	IERROR			;Exit from break routine on error


$EJECT
;******************************************************************
;
;	NAME: ACC_CMD/ PSW_CMD/ SP_CMD/ B_CMD
;
;	ABSTRACT: Displays or modifies the byte which is referenced
;	by the user register images passed to it.
;
;	INPUTS: None
;
;	OUTPUTS: Users version of the PC, DPTR, TMO, TM1
;
;	VARIABLES MODIFIED: PNTLOW, PNTHGH, SELECT, PARAM1
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: ISIT_DISPLAY, IFETCH, ILSTBYT,
;	IWAIT_FOR_USER, ISTORE, KEY_BYTES
;
;
;******************************************************************
ACC_CMD:
	MOV	PNTLOW,#ACC
	LJMP	KEY_BYTE
;******************************************************************
PSW_CMD:
	MOV	PNTLOW,#PSW
	LJMP	KEY_BYTE
;******************************************************************
SP_CMD:
	MOV	PNTLOW,#SP
	LJMP	KEY_BYTE
;******************************************************************
B_CMD:
	MOV	PNTLOW,#B
	LJMP	KEY_BYTE
;******************************************************************
KEY_BYTE:
	LCALL	ISIT_DISPLAY
	MOV	PNTHGH,#000H
	MOV	SELECT,#(RBYTE_TOKE AND 007H)	;Set-up for fetch
	JNC	CHANGE
	LCALL	IFETCH
	MOV	PARAM1,A			;Call ILSTBYT (result) to display it
	LCALL	ILSTBYT
	LJMP	IWAIT_FOR_USER
CHANGE:
	LCALL	IGETNUM				;Get the numeric parameter
	MOV	PARAM1,VALLOW
	LJMP	ISTORE
$EJECT
;******************************************************************
;
;	NAME: PC_CMD/ DPTR_CMD/ TM0_CMD/ TM1_CMD
;
;	ABSTRACT: Decodes and exeutes those conmands which display or alter
;	sixteen hit variables which have unique keywords to identify
;	them.
;
;	INPUTS: None
;
;	OUTPUTS: Users version of the PC, DPTR, TM0 and TM1
;
;	VARIABLES MODIFIED: PARAM1, PARAM2, PNTlOW, TEMP_LOW, PNTHGH, A
;
;	ERROR EX ITS:	None
;
;	SUBROUTINES ACCESSED DIRECTRLY: ISH_DISPLAY, READ_PC, ILSTWRD,
;	WRITE_PC, IFETCH, I_STORE , IGETEOL, IGETNUM, IWAIT_FOR_USER,
;	KEYWORD_DISPLAY
;
;
;******************************************************************
PC_CMD:
	LCALL	ISIT_DISPLAY
	JNC	PC_CHA
	LCALL	READ_PC			;Get the user program counter.
	MOV	PARAM2,A		;And set up parameters to display it.
	MOV	PARAM1,B
	LCALL	ILSTWRD
	LJMP	IWAIT_FOR_USER
PC_CHA:
	LCALL	IGETNUM
	MOV	PARAM1,VALHGH
	MOV	PARAM2,VALLOW
	LCALL	WRITE_PC
	LJMP	IGETEOL
;*******************************************************************
DPTR_CMD:
	MOV	PNTLOW,#DPH
	MOV	TEMP_LOW,#DPL
	LJMP	KEYWORD_DISPLAY
;*******************************************************************
TM0_CMD:
	MOV	PNTLOW,#TH0
	MOV	TEMP_LOW,#TL0
	LJMP	KEYWORD_DISPLAY
;*******************************************************************
TM1_CMD:
	MOV	PNTLOW,#TH1
	MOV	TEMP_LOW,#TL1
;*******************************************************************
KEYWORD_DISPLAY:
	CALL	ISIT_DISPLAY
	MOV	SELECT,#(RBYTE_TOKE AND 007H)
	MOV	PNTHGH,#0
	JNC	WCHANGE
	LCALL	IFETCH
	XCH	A,TEMP_LOW
	MOV	PNTLOW,A
	LCALL	IFETCH
	MOV	PARAM2,A
	MOV	PARAM1,TEMP_LOW
	LCALL	ILSTWRD
	LJMP	IWAIT_FOR_USER	;Wait for CR then start the monitor.
WCHANGE:
	LCALL	IGETNUM		;If it is, get the data to be loaded.
	MOV	PARAM1,VALHGH
	LCALL	ISTORE
	MOV	PNTLOW,TEMP_LOW
	MOV	PARAM1,VALLOW
	LCALL	ISTORE
	LJMP	IGETEOL		;Process end of line and return to the
;******************************************************************
