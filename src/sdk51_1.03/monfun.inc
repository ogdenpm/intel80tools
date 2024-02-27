;******************************************************************
;
;	NAME: LIST_CMD
;
;	ABSTRACT: This routine gets the 'keyword =' message and sets
;	up the LSTFLG to display tokens to the console and an auxilary
;	terminal. Anytime display is called for. It will also terminate
;	any ISIS files with a control Z. List is on when LSTFLG = 1.
;
;	INPUTS: LSTFLG
;
;	OUTPUTS: LSTFLG
;
;	VARIABLES MODIFIED: LSTFLG, PARAM1. ERRNUM
;
;	ERROR EXITS: 08H (RESET OR ON REQUIRED)
;
;	SUBROUTINES ACCESSED DIRECTRLY: ISIT_DISPLAY, IGETOKE,
;	IDISPLAY_TOKEN, ICO, UPI_CMD, INEWLINE, IWAIT_FOR_USER
;
;
;******************************************************************
LIST_CMD:
	LCALL	ISIT_DISPLAY		;Sets up 'keyword =' msg
	JC	DISPLAY_LIST		;C=1 if display only
	LCALL	IGETOKE
	CJNE	A,#ON_TOKE,LIST_2	;List turned on. no display
	SETB	LSTFLG
	RET
LIST_2:	MOV	ERRNUM,#008H		;Reset or on required
	CJNE	A,#RESET_TOKE,STATE_ERR	;List turned off, no display
	MOV	PARAM1,#USART_MODE
	LCALL	UPI_CMD
	CLR	LSTFLG
	MOV	PARAM1,#01AH
	LCALL	ICO			;Send ctrl-z to close MDS file
	LJMP	INEWLINE		;Insure that ctrl-z get out before Usart Reset
DISPLAY_LIST:
	MOV	PARAM1,#ON_TOKE		;Display 'on' set up
	JB	LSTFLG,LIST_1
	MOV	PARAM1,#RESET_TOKE	;Display 'reset' set up
LIST_1:	LCALL	IDISPLAY_TOKEN
	JMP	IWAIT_FOR_USER
$EJECT
;******************************************************************
;
;	NAME: BAUD_CMD/ SET_BAUD
;
;	ABSTRACT: This routine will allow the user to display the
;	baud rate or change the baud rate to any legal value between
;	110 and 9600. Default on power up is 2400.
;
;	INPUTS: BAUD_HIGH, BAUD_LOW
;
;	OUTPUTS: BAUD_HIGH, BAUD_LOW, BAUOKEY
;
;	VARIABLES MODIFIED: DPTR, ERRNUM, A, B, BAUD_HIGH, BAUD_LOW, BAUDKEY
;
;	ERROR EXITS: 0AH (ILLEGAL BAUD VALUE)
;
;	SUBROUTINES ACCESSED DIRECTRLY: ISIT_DISPLAY, IGETNUM, IERROR,
;	ILSTWRD, IWAIT_FOR_USER
;
;
;******************************************************************
BAUD_CMD:
	LCALL	ISIT_DISPLAY
	JC	BAUD_DISPLAY
	LCALL	IGETNUM
	MOV	DPTR,#BAUD_RATE	;Check table for a valid baud rate request.
	MOV	POINT0,#000H
BS_LOOP:
	MOV	A,POINT0
	MOVC	A,@A+DPTR
	CJNE	A,VALHGH,BS_2
	MOV	A,VALLOW
	CJNE	POINT0,#000H,BM_1
					;If POINT0=O, the lower 2 digits better be
					;10 because the baud rate is 110.
	MOV	ERRNUM,#00AH		;Illegal baud value
	CJNE	A,#010H,STATE_ERR
PRE_SET_BAUD:
	MOV	DPTR,#(RAMOFF + BAUD_HIGH)
	MOV	A,VALHGH
	MOVX	@DPTR,A
	INC	DPTR
	MOV	A,VALLOW
	MOVX	@DPTR,A
	MOV	DPL,#BAUDKEY
	MOV	A,POINT0
	MOVX	@DPTR,A
;*******************************************************************
SET_BAUD:
	MOV	DPTR,#(RAMOFF + BAUDKEY)
	MOVX	A,@DPTR
	RL	A
	MOV	B,A
	MOV	DPTR,#TIMER_PRESET
	MOVC	A,@A+DPTR
	XCH	A,B
	INC	DPTR
	MOVC	A,@A+DPTR
	XCH	A,B			;Store the timer preset value.
	MOV	DPTR,#(RAMIO + TIMER_HIGH)
	ORL	A,#CONTINUOUS_MODE
	MOVX	@DPTR,A
	DEC	DPL
	MOV	A,B
	MOVX	@DPTR,A
	MOV	DPTR ,#RAMIO		;Start - load timer
	MOV	A,#START_16_TIMER
	MOVX	@DPTR,A
	RET
BM_1:	JZ	PRE_SET_BAUD		;Else the lower 2 digits better be 0
					;because all the other rates end in O.
BS_2:	INC	POINT0
	CJNE	A,#HIGH(9600H),BS_LOOP
STATE_ERR:
	JMP	IERROR
BAUD_RATE:
	DB	HIGH(110H)
	DB	HIGH(300H)
	DB	HIGH(600H)
	DB	HIGH(1200H)
	DB	HIGH(2400H)
	DB	HIGH(4800H)
	DB	HIGH(9600H)
TIMER_PRESET:
	DW	1136
	DW	0417
	DW	0208
	DW	0104
	DW	0052
	DW	0026
	DW	0013
;******************************************************************
BAUD_DISPLAY:
	MOV	DPTR,#(RAMOFF + BAUD_HIGH)
	MOVX	A,@DPTR
	MOV	PARAM1,A
	INC	DPTR
	MOVX	A,@DPTR
	MOV	PARAM2,A
	LCALL	ILSTWRD
	JMP	IWAIT_FOR_USER
$EJECT
;******************************************************************
;
;	NAME: TOP_CMD
;
;	ABSTRACT: This routine will set the top of memory to a value
;	requested by the user. It will error for values > 7FFFH.
;	It will also list the current TOP value to the console upon
;	request.
;
;	INPUTS: TOP_STORE
;
;	OUTPUTS: TOP_STORE
;
;	VARIABLES MODIFIED: DPTR, A, B, PARAM1, ERRNUM
;
;	ERROR EXITS: ODH (TOP VALUE > 7FFFH)
;
;	SUBROUTINES ACCESSED DIRECTRLY: IS_IT_DISPLAY, IGETNUM, ILSTBYT,
;	IWAIT_FOR_USER
;
;
;******************************************************************
TOP_CMD:LCALL	ISIT_DISPLAY
	MOV	DPTR,#(RAMOFF + TOP_STORE)
	JC		TOP_DISPLAY
	LCALL	IGETNUM
	MOV	A,VALHGH		;Do not allow top> 32k
	MOV	ERRNUM,#00DH		;Top value> 7FFFH
	JB		ACC.7,STATE_ERR
	MOV	B,A			;Check for the special case of 0000H
					;otherwise the display should end
	ORL	A,VALLOW		;with an FFH
	JZ	ST_1
	INC	B
ST_1:
	MOV	A,B
	MOV	DPTR,#(RAMOFF + TOP_STORE)
	MOVX	@DPTR,A
	RET
;******************************************************************
TOP_DISPLAY:
	MOVX	A,@DPTR		; Call 1istbyte(top).
	JZ		TOP_LIST_2
	DEC	A
TOP_LIST_2:
	MOV	PARAM1,A
	LCALL	ILSTBYT
	MOV	DPTR,#(RAMOFF + TOP_STORE)
	MOVX	A,@DPTR
	JZ		TOP_LIST_0
	MOV	PARAM1,#0FFH
	LCALL	ILSTBYT
	LJMP	TOP_LIST_1
TOP_LIST_0:
	MOV	PARAM1,#000H
	LCALL	ILSTBYT
TOP_LIST_1:
	LJMP	IWAIT_FOR_USER
$EJECT
;******************************************************************
;
;	NAME: CAUSE_CMD
;
;	ABSTRACT: This routine will display the reason detected
;	for a break execution. It is a display-only function.
;	The cause is determined and stored during BREAK.
;
;	INPUTS: CAUSE_IMAGE
;
;	OUTPUTS: None
;
;	VARIABLES MODIFIED: A, DPTR, COUNT, PARAM1, PARAM2, ERRNUM
;
;	ERROR EXITS: 0EH (DISPLAY ONLY)
;
;	SUBROUTINES ACCESSED DIRECTRLY: ISIT_DISPLAY, IPRINT_STRING,
;	IWAIT_FOR_USER
;
;
;******************************************************************
CAUSE_CMD:
	LCALL	ISIT_DISPLAY
	MOV	ERRNUM,#00EH		;Display only
	JNC	STATE_ERR
	MOV	A,CAUSE_IMAGE
	MOV	DPTR,#CAUSE_TAB
	MOV	COUNT,#5		;Output the appropriate message.
CL_LOOP:
	RRC	A			;Isolate the bit which indicates the
					;cause of the break.
	JB	ACC.0,CL_0
	INC	DPTR
	INC	DPTR
	DJNZ	COUNT,CL_LOOP
CL_0:
	CLR	A
	MOVC	A,@A+DPTR
	MOV	PARAM1,A
	CLR	A
	INC	DPTR
	MOVC	A,@A+DPTR
	MOV	PARAM2,A
	LCALL	IPRINT_STRING
	JMP	IWAIT_FOR_USER
CAUSE_TAB:
	DW	USER_MSG
	DW	GUARD_MSG
	DW	PROG_MSG
	DW	DATA_MSG
	DW	SINGLE_STEP_MSG
	DW	NOBRK_MSG
USER_MSG:
	DB	10,('USER ABORT')
GUARD_MSG:
	DB	14,('GUARDED ACCESS')
PROG_MSG:
	DB	13, ('PROGRAM BREAK')
DATA_MSG:
	DB	10,('DATA BREAK')
SINGLE_STEP_MSG:
	DB	11,('SINGLE STEP')
NOBRK_MSG:
	DB	11,('WHAT BREAK?')
$EJECT
;******************************************************************
;
;	NAME: SEND_BYTE
;
;	ABSTRACT: This routine outputs one byte, in either hex or
;	binary depending on the setting of the binary flag, to
;	the USART. A new checksum is calculated and returned.
;
;	INPUTS: CHECKSUM, A
;
;	OUTPUTS: CHECKSUM
;
;	VARIABLES MODIFIED: A, PARAM1
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: ICO, ILSTBYT
;
;
;******************************************************************
SEND_BYTE:
	XCH	A,CHECKSUM
	ADD	A,CHECKSUM
	XCH	A,CHECKSUM
	MOV	PARAM1,A
	JB	BINARY_FLG,SEND_BINARY
	JMP	ILSTBYT
SEND_BINARY:
	JMP	ICO
$EJECT
;******************************************************************
;
;	NAME: HEXBIN
;
;	ABSTRACT: Reads two characters from the input device and
;	converts them to binary. If the binary flag is set, then
;	one binary character is input. This value is added to the
;	checksum byte and also returned to the calling routine.
;
;	INPUTS: BINARY_FLG, CHECKSUM
;
;	OUTPUTS: CHECKSUM
;
;	VARIABLES MODIFIED: PARAM1, A, TEMP
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: UPI_IN, IASCII_TO_HEX, ICI
;
;
;******************************************************************
HEXBIN:	LCALL	UPI_IN
	JB	BINARY_FLG,BINARY_LOAD
	MOV	PARAM1,A
	LCALL	IASCII_TO_HEX
	SWAP	A			;Move the digit to the upper nibble.
	MOV	TEMP,A			;Then save it in a temporary location.
	LCALL	ICI
	MOV	PARAM1,A
	LCALL	IASCII_TO_HEX
	ORL	A,TEMP			;Then combine with previous digit.
BINARY_LOAD:
	XCH	A,CHECKSUM		;Before returning the binary value
	ADD	A,CHECKSUM		;include it in checksum calculation.
	XCH	A,CHECKSUM
	RET
$EJECT
;*******************************************************************
;
;	NAME: GET_BYTE
;
;	ABSTRACT: This routine looks for a colon from the cassette or
;	auxilary termimal input, gets the byte count, address and
;	file-type tnformation contained in the header and does a checksum.
;
;	INPUTS: None
;
;	OUTPUTS: TYPE, PNTLOW, PNTHGH, COUNT, CHECKSUM
;
;	VARIABLES MODIFIED: A, CHECKSUM, COUNT, PNTHGH, PNTLOW, TYPE
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: ICI, HEXBIN
;
;
;******************************************************************
GET_TYPE:
	LCALL	UPI_IN		;Scan for a colon.
	ANL	A,#07FH
	CJNE	A,#':',GET_TYPE
	CLR	A
	MOV	CHECKSUM,A
	CALL	HEXBIN			;Load the byte count from
	MOV	COUNT,A			;the next two characters of the record.
	CALL	HEXBIN			;Load the load address
	MOV	PNTHGH,A
	CALL	HEXBIN
	MOV	PNTLOW,A
	CALL	HEXBIN			;Load the record type.
	MOV	TYPE,A
	RET
$EJECT
;******************************************************************
;
;	NAME: LOAD_HEX
;
;	ABSTRACT: Loads audio cassette data files (type 0) until EOF
;	is encountered. Calculates a checksum, passes label (addr), writes
;	user PC, converts hex data to binary and returns.
;
;	INPUTS: None
;
;	OUTPUTS: Code memory locations addressed in the file being loaded.
;
;	VARIABLES MODIFIED: A, PARAM1, SELECT, PNTLOW, PNTHGH, ERRNUM
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: GET_TYPE, HEXBIN, INIT_IO,
;	ISTORE, WRITE_PC, ITIME
;
;
;******************************************************************
LOAD_HEX:
	CALL	GET_TYPE
	JNZ	LH_7			;If type is not zero (data record)
					;then quit loading records.
LH_4:	MOV	A,COUNT			;Load memory until the count gets
	JZ	LH_6			;to zero, COUNT=length read from file
	CALL	HEXBIN
	MOV	PARAM1,A
	MOV	SELECT,#0
	LCALL	ISTORE			;Increment the load address.
	LCALL	INC_PNT			;Repeat the load loop until zero.
	DJNZ	COUNT,LH_4
LH_6:	CALL	HEXBIN			;The end of the record has been reached
	MOV	A,CHECKSUM		;so check the checksum field.
	JZ	LOAD_HEX		;Recall CHECKSUM from HEXBIN
LH_8:	LJMP	LH_ERROR
LH_7:	CJNE	A,#1,LH_8		;Look for EOF (type 1)
	CALL	HEXBIN
	MOV	A, CHECKSUM
	JNZ	LH_8
	LCALL	INIT_IO
	MOV	PARAM2,PNTLOW		;Write addr (label) to user PC
	MOV	PARAM1,PNTHGH
	LCALL	WRITE_PC
	MOV	PARAM1,#007H
	MOV	PARAM2,#000H
	LCALL	ITIME			;Wait for 2 char times at 110 baud
	MOV	DPTR,#UPI_DATA		;So no other chars get into the
	MOVX	A,@DPTR			;Command buffer. Flush output
	RET				;buffer flag.
$EJECT
;******************************************************************
;
;	NAME: STORE_HEX
;
;	ABSTRACT: This routine writes hex bytes on the cassette or to
;	the USART from memory. It outputs all record marks and header
;	information and calculates a checksum.
;
;	INPUTS: BINARY_FLG, PARTIT_LO_LOW, PARTIT_LO_HIGH, PARTIT_HI_LOW,
;	PARTIT_HI_HIGH, Memory contents within the partition bounds.
;
;	OUTPUTS: None
;
;	VARIABLES MODIFIED: PARAM1, PARAM1, C, A, COUNT, TEMP, CHECKSUM,
;	SELECT, PNTHGH, PNTLOW, PARTIT_LO_LOW, PARTIT_LO_HIGH,
;	ERRNUM
;
;	ERROR EXITS: 14H (FILE READ/WRITE ERROR)
;
;	SUBROUTINES ACCESSED DIRECTRLY: INEWLINE, ITIME, SEND_BYTE,
;	IFETCH, READ_PC, UPI_CMD, ICO, IERROR
;
;
;******************************************************************
STORE_HEX:
	JB	BINARY_FLG,SH_6
	MOV	PARAM1,#001H		;Delay 40 milliseconds.
	MOV	PARAM2,#090H
	LCALL	ITIME
	LCALL	INEWLINE		;Start sending record.
	MOV	PARAM1,#013H
	MOV	PARAM2,#088H		;Delay 1/2 sec.
	LCALL	ITIME
SH_6:	MOV	PARAM1,#':'		;Output the record mark.
	LCALL	ICO
	CLR	C			;Output hex records while sa<=ea.
	MOV	A,PARTIT_HI_LOW
	SUBB	A,PARTIT_LO_LOW
	MOV	COUNT,A			;(Save difference for later use).
	MOV	A,PARTIT_HI_HIGH
	SUBB	A,PARTIT_LO_HIGH
	MOV	TEMP,A			;Set count to 16 or the number of bytes
	JC	SH_5			;left-whichever is less.
	MOV	A,TEMP
	JZ	SH_1
	MOV	COUNT,#00FH
SH_1:	MOV	A,COUNT
	ANL	A,#0F0H
	JZ	SH_2
	MOV	COUNT,#00FH
SH_2:	INC	COUNT
	CLR	A
	MOV	CHECKSUM,A
	MOV	A,COUNT
	CALL	SEND_BYTE
	MOV	A,PARTIT_LO_HIGH
	CALL	SEND_BYTE
	MOV	A,PARTIT_LO_LOW
	CALL	SEND_BYTE
	CLR	A
	CALL	SEND_BYTE
SH_3:					;Now go into a loop to output the data.
	MOV	SELECT,#000H		;Call fetch(O,sa).
	MOV	PNTHGH,PARTIT_LO_HIGH
	MOV	PNTLOW,PARTIT_LO_LOW
	LCALL	IFETCH
	CALL	SEND_BYTE
	MOV	A,PARTIT_LO_LOW		;Increment the address
	ADD	A,#001H
	MOV	PARTIT_LO_LOW,A
	JNC	SH_4
	INC	PARTIT_LO_HIGH
SH_4:	DJNZ	COUNT,SH_3		;Decrement count and loop till zero.
	MOV	A, CHECKSUM		;Once done output the negation of the
	CPL	A			; checksum.
	INC	A
	CALL	SEND_BYTE		;Then go output another record
	JMP	STORE_HEX
SH_5:	CLR	A
	MOV	CHECKSUM,A
	CALL	SEND_BYTE
	LCALL	READ_PC
	XCH	A,B
	CALL	SEND_BYTE
	MOV	A,B
	CALL	SEND_BYTE
	CLR	A
	INC	A
	CALL	SEND_BYTE
	MOV	A,CHECKSUM
	CPL	A
	INC	A
	CALL	SEND_BYTE
	MOV	PARAM1,#1
	MOV	PARAM2,#090H
	LCALL	ITIME
	LCALL	INEWLINE
	JB	BINARY_FLG,SH_7		;Skip control-Z if cassette operation.
	MOV	PARAM1,#USART_MODE	;Select USART mode.
	LCALL	UPI_CMD
	MOV	PARAM1,#01AH		;Insert control Z to close MDS file
	LCALL	ICO
SH_7:	MOV	PARAM1,#013H
	MOV	PARAM2,#088H
	JMP	ITIME			;Del ay 1/2 sec to catch cntrl Z in 1ist mode
LH_ERROR:
	MOV	ERRNUM,#014H		;File read/write error
	LCALL	IERROR
$EJECT
;******************************************************************
;
;	NAME: LOAD_CMD
;
;	ABSTRACT: This routine calls the routine LOAD HEX which
;	reads data files from the audio cassette in binary. It sets
;	up the user messages and does checksums.
;
;	INPUTS: None
;
;	OUTPUTS: Code memory locations referenced by the file being loaded.
;
;	VARIABLES MODIFIED: PCNHTI, PCNTLO, BINARY_FLG, PARAM1, A,
;	PARAM2
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: IGETOKE, IPRINT_STRING,
;	ICI, UPI CMD, GET_TYPE. HEXBIN, LOAD_HEX, INIT_10, ILSTWRD,
;	IWAIT_FOR_USER
;
;
;******************************************************************
LOAD_CMD:
	LCALL	IGETOKE				;Have a valid LOAD cmd
	MOV	PCNTHI,VALHGH			;Save addr (label) field
	MOV	PCNTLO,VALLOW
	MOV	PARAM1,#HIGH(CASS_MSG)		;Set up "start cassette" msg
	MOV	PARAM2,#LOW(CASS_MSG)
	LCALL	IPRINT_STRING
	LCALL	ICI				;Holds msg on display long enough to be seen
	SETB	BINARY_FLG			;Indicates a binary file
	MOV	PARAM1,#CASSETTE_READ
	LCALL	UPI_CMD				;Select cassette mode
	MOV	A,TOKSTR			;Restore original token
	CJNE	A,#NUMBER_TOKE,FILE_DISPLAY	;If not a number, need to get next
LOAD_LOOP:			;Get number off cass and display it (directory)
	CALL	GET_TYPE	;0=data file. 1=EOF, 2=file label record
	CJNE	A,#2,LOAD_LOOP			;Is it the beginning of a file?
	MOV	A,PCNTHI			;Yes. get the label (addr)
	CJNE	A,PNTHGH,LOAD_LOOP
	MOV	A,PCNTLO
	CJNE	A,PNTLOW,LOAD_LOOP
	CALL	HEXBIN			;Convert to hex, calculate checksum
	MOV	A,CHECKSUM
	JNZ	LH_ERROR			;Checksum error
	CALL	LOAD_HEX			;Read the data file from cassette
	LCALL	INIT_IO
	MOV	DPTR,#UPI_DATA
	MOVX	A,@DPTR		;Go back to console mode. clear OBF status bit
	MOV	PARAM1,#HIGH(FILE_FOUND)	;Set up "File loaded" msg
	MOV	PARAM2,#LOW(FILE_FOUND)
	LCALL	IPRINT_STRING
	MOV	PARAM1,PCNTHI			;Set up file number for display
	MOV	PARAM2,PCNTLO
	LCALL	ILSTWRD
	JMP	IWAIT_FOR_USER			;Holds msg on display a short time
FILE_DISPLAY:
	CALL	GET_TYPE		;Get here by saying LOAD <CR>
	CJNE	A,#2,FILE_DISPLAY	;Ask for directory, cant load w/o file #
	LCALL	INIT_IO
	MOV	DPTR,#UPI_DATA
	MOVX	A,@DPTR		;Go back to console mode, clr OBF status bit
	MOV	PARAM1,#HIGH(NUM_FOUND)	;Sets up "first file found" msg
	MOV	PARAM2,#LOW(NUM_FOUND)
	LCALL	IPRINT_STRING
	MOV	PARAM1,PNTHGH		;Set up file number (addr) for display
	MOV	PARAM2,PNTLOW
	LCALL	ILSTWRD
	JMP	IWAIT_FOR_USER		;Holds msg on display a short time
$EJECT
;*******************************************************************
;
;	NAME: SAVE_CMD
;
;	ABSTRACT: This routine writes data in a user specified partition
;	to the audio cassette in binary using STORE_HEX which provides
;	address, type and checksum for each record.  This procedure
;	takes care of all UPI set up.
;
;	INPUTS: Code memory within the partition
;
;	OUTPUTS: None
;
;	VARIABLES MODIFIED: PCNTHI, PCNTLO, PARAM1, PARAM2, BINARY_FLG
;	A, CHECKSUM
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: IGETNUM, IGETOKE, IGET_PART, IPRINT_STRING,
;	ICI, UPI_CMD, ICO, SEND_BYTE, IGET_COMMA, IEOL_CHECK, STORE_HEX
;
;
;******************************************************************
SAVE_CMD:
	LCALL	IGETNUM
	MOV	PCNTHI,VALHGH
	MOV	PCNTLO,VALLOW
	LCALL	IGET_COMMA
	LCALL	IGETOKE
	LCALL	IGET_PART
	LCALL	IEOL_CHECK
	MOV	PARAM1,#HIGH(CASS_MSG)
	MOV	PARAM2,#LOW(CASS_MSG)
	LCALL	IPRINT_STRING
	LCALL	ICI
	SETB	BINARY_FLG
	MOV	PARAM1,#CASSETTE_WRITE
	LCALL	UPI_CMD			;Select cassette mode
	MOV	PARAM1,#':'
	LCALL	ICO
	CLR	A
	MOV	CHECKSUM, A
	CALL	SEND_BYTE
	MOV	A,PCNTHI
	CALL	SEND_BYTE
	MOV	A,PCNTLO
	CALL	SEND_BYTE
	MOV	A,#2
	CALL	SEND_BYTE
	MOV	A,CHECKSUM
	CPL	A
	INC	A
	CALL	SEND_BYTE
	JMP	STORE_HEX
$EJECT
;******************************************************************
;
;	NAME: DOWNLOAD_CMD
;
;	ABSTRACT: This routine temporarily turns off the list mode,
;	selects the console, configures the UPI and loads hex files
;	from the auxiliary terminal into memory.
;
;	INPUTS: None
;
;	OUTPUTS: Code memory location specified in the file being loaded.
;
;	VARIABLES MODIFIED: PARAM1, PARAM2, BINARY_FLG
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: IPRINT_STRING, UPI_CMD,
;	LOAD_HEX
;
;
;******************************************************************
DOWNLOAD_CMD:
	CLR	BINARY_FLG		;Set "LIST = RESET"
	MOV	PARAM1,#SELECT_CON
	LCALL	UPI_CMD
	MOV	PARAM1,#HIGH(LOAD_MSG)
	MOV	PARAM2,#LOW(LOAD_MSG)
	LCALL	IPRINT_STRING		;Print loading msg
	MOV	PARAM1,#USART_MODE
	LCALL	UPI_CMD			;Select USART mode
	CALL	LOAD_HEX
	RET
$EJECT
;******************************************************************
;
;	NAME: UPLOAD_CMD
;
;	ABSTRACT: This routine gets a token and partition, turns off
;	list mode and outputs hex files to the console through the
;	UPI.
;
;	INPUTS: Code memory locations specified by the partition typed
;	by the user.
;
;	OUTPUTS: None
;
;	VARIABLES MODIFIED: PARAM1, BINARY_FLG, LSTFLG
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: IGET_PART, IGETOKE,
;	UPI_CMD, STORE_HEX, IEOL_CHECK
;
;
;******************************************************************
UPLOAD_CMD:
	LCALL	IGETOKE
	LCALL	IGET_PART
	LCALL	IEOL_CHECK
	CLR	BINARY_FLG
	CLR	LSTFLG			;Set 'LIST = RESET'
	MOV	PARAM1,#040H		;Select Keybd/Oisply with list on.
	LCALL	UPI_CMD
	JMP	STORE_HEX
;******************************************************************
LOAD_MSG:	DB	9,CR,LF,('LOADING')
CASS_MSG:	DB	16,CR,LF,('START CASSETTE')
FILE_FOUND:	DB	15,CR,LF,('LOADED FILE  ')
NUM_FOUND:	DB	19,('FIRST FILE FOUND = ')
;******************************************************************
VERIFY_CMD:
	MOV	DPTR,#06009H
	SJMP	JMP_TAB_CHECKER
TRANSFER_CMD:
	MOV	DPTR,#6006H
	SJMP	JMP_TAB_CHECKER
PROGRAM_CMD:
	MOV	DPTR,#6003H
	SJMP	JMP_TAB_CHECKER
MODE_CMD:
	MOV	DPTR,#06000H
JMP_TAB_CHECKER:
	CLR	A
	MOVC	A,@A+DPTR
	CJNE	A,#2,FAKE_BAD_CMD_ERR	;Check for first byte of LJMP opcode
	CLR	A
	JMP	@A+DPTR
FAKE_BAD_CMD_ERR:
	MOV	ERRNUM,#002H
	JMP	IERROR
