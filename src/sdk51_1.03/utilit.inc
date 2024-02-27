;******************************************************************
;
;	NAME: (I)ERROR
;
;	ABSTRACT: This routine handles all error messages for the SDK-51
;	except error O. These are not intended to be a standard format
;	for any other SDK product. After printing an error message, it
;	waits for any console entry and then starts fresh from START.
;	To find the routine which generates a particular error number,
;	check the cross reference listing (XREF) at the back of this
;	document for all uses of the variable name ERRNUM.
;
;	INPUTS: ERRNUM, LSTFLG
;
;	OUTPUTS: None
;
;	VARIABLES MODIFIED: PARAM1, PARAM2, C, A, TEMPI
;
;	ERROR EXITS: None
;
;	SUBROUTINES ACCESSED DIRECTRLY: ITIME, INIT_IO, UPI_CMD,
;	IPRINT_STRING, ILSTBYT, SPACCO, IWAIT_F0R_USER
;
;
;******************************************************************
IERROR:	MOV	PARAM1,#007H
	MOV	PARAM2,#000H
	LCALL	ITIME			;Wait for the completion of any
	CALL	INIT_IO			;list activity before emptying usart
	MOV	C,LSTFLG		;about 180ms
	CLR	A
	MOV	ACC.6,C
	MOV	PARAM1,A
	LCALL	UPI_CMD			;Select console with list status
	MOV	PARAM1,#HIGH(ERROR_MSG)
	MOV	PARAM2,#LOW(ERROR_MSG)
	LCALL	IPRINT_STRING
	MOV	PARAM1,ERRNUM
	LCALL	ILSTBYT
	LCALL	SPACCO
	MOV	TEMP1,#000H		;Table search counter
	MOV	DPTR,#ERROR_TABLE	;Table entry
ERROR_TEST:
	MOV	A,ERRNUM
	CJNE	A,TEMP1,ERROR_BEGIN	;Is it this entry?
	MOV	PARAM1,DPH
	MOV	PARAM2,DPL
	LCALL	IPRINT_STRING
	CALL	IWAIT_FOR_USER
	JMP	START			;Yes, print message
ERROR_BEGIN:
	CLR	A
	MOVC	A,@A+DPTR		;No, get num of letters to skip
ERROR_LOOP:
	INC	DPTR
	DJNZ	ACC ,ERROR_LOOP
	INC	DPTR			;Adjust addr of next tabl e entry
	INC	TEMP1			;Adjust table search counter
	JMP	ERROR_TEST
ERROR_MSG:
	DB	6,CR,LF,('ERR=')
ERROR_TABLE:
	DB	10,('PROM CKSUM')		;Error #00
	DB	12,('INVALID WORD')		;	01
	DB	15,('INVALID COMMAND')		;	02
	DB	10,('NUMBER REQ')		;	03
	DB	10,('RETURN REQ')		;	04
	DB	17,('EQUAL OR RTRN REQ')	;	05
	DB	09,('COMMA REQ')		;	06
	DB	13,('PARTITION ADR')		;	07
	DB	15,('RESET OR ON REQ')		;	08
	DB	15,('DECIMAL NUM REQ')		;	09
	DB	16,('ILLEGAL BAUD VAL')		;	0A
	DB	16,('BRK ENABL SYNTAX')		;	0B
	DB	16,('NUM OR RESET REQ')		;	0C
	DB	11,('TOP ) 7FFFH')		;	0D
	DB	12,('DISPLAY ONLY')		;	0E
	DB	16,('UNDEFINED OPCODE')		;	0F
	DB	15,('ASSEMBLY SYNTAX')		;	10
	DB	16,('ADR OUT OF RANGE')		;	11
	DB	16,('ADR OUT OF RANGE')		;	12
	DB	15,('ASM PC ) 0FFFFH')		;	13
	DB	13,('FILE RD OR WR')		;	14
	DB	12,('MEMORY WRITE')		;	15
	DB	16,('EX ACROSS ADR 03')		;	16
	DB	16,('NO RAM AT ADR 03')		;	17
	DB	14,('CBYTE TYPE REQ')		;	18
	DB	11,('CHANGE ONLY')		;	19
	DB	14,('CBY OR NUM REQ')		;	1A
