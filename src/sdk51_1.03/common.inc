BASE	EQU	0E000H
;***** CONSTANTS USED BY ALL MODULES *****

NUMBER_TOKE		EQU	001H	    ;Constant (GETOKE,number token)
BAR_TOKE		EQU	003H	    ;Constant (GETOKE,slash (/) token)
POUND_TOKE		EQU	006H	    ;Constant
PLUS_TOKE		EQU	005H
EOL_TOKE		EQU	007H	    ;Constant (GETOKE,end of line token)
ATA_TOKE		EQU	00AH
C_TOKE			EQU	05EH
CBYTE_TOKE		EQU	080H
DPTR_TOKE		EQU	0A1H
ORG_TOKE		EQU	0D4H
PC_TOKE			EQU	0A0H
REG			EQU	040H
OFST			EQU	010H
LINMAX			EQU	24
TOKSIZ			EQU	4
BLINK			EQU	080H	    ;Set the blink bit in bytes to go to the UPI
SELECT_CON		EQU	000H	    ;Set up UPI for on-board console

;***** GLOBAL VARIABLES USED BY MORE THAN ONE MAIN MODULE *****
	DSEG
	ORG		024H
;***** ARRAYS *****

LINBUF:			DS	LINMAX	;Input line buffer(24 chars)
STRGBF:			DS	TOKSIZ	;Buffer for string
WORKING_SPACE:		DS	3	;Buffer for ASM/DASM

;***** VARIABLES *****

ERRNUM:			DS	1
PNTHGH:			DS	1
PNTLOW:			DS	1
SELECT:			DS	1
TEMP_LOW:		DS	1
TOKSTR:			DS	1
VALHGH:			DS	1
VALLOW:			DS	1
ASM_PC_HIGH:		DS	1
ASM_PC_LOW:		DS	1
NUMBER_OF_BYTES:	DS	1
OUR_CODE_HIGH:		DS	1
OUR_CODE_LOW:		DS	1
CHARIN:			DS	1
CHRCNT:			DS	1
LINE_START:		DS	1
LINCNT:			DS	1
LNLGTH:			DS	1
STRGCT:			DS	1
TEMP1:			DS	1
PARTIT_LO_HIGH:		DS	1
PARTIT_LO_LOW:		DS	1
PARTIT_HI_HIGH:		DS	1
PARTIT_HI_LOW:		DS	1

;***** FLAGS *****
	BSEG
	ORG		0

B_O_T:			DBIT	1
LSTFLG:			DBIT	1
	CSEG
;***** REGISTERS *****
POINT0			EQU	R0	;Register (addr pointer)
POINT1			EQU	R1	;Register (addr pointer)
PARAM1			EQU	R2	;Register (parameter passing media #1)
PARAM2			EQU	R3	;REGISTER (Parameter passing media #2)
PARAM3			EQU	R4	;REGISTER (Parameter passing media #3)
PARAM4			EQU	R5
PARAM5			EQU	R6
PARAM6			EQU	R7
COUNT			EQU	R7
CHECKSUM		EQU	R6
TEMP			EQU	R5
;***** END OF VARIABLE EQUATES *****
;******************************************************************
;		JUMP TABLE ENTRY ADDRESSES FOR ALL MODULES
;******************************************************************
CO			EQU	6 + BASE
CI			EQU	9 + BASE
CSTS			EQU	00CH + BASE
NEWLINE			EQU	00FH + BASE
TIME			EQU	012H + BASE
LSTBYT			EQU	015H + BASE
LSTWRD			EQU	018H + BASE
PRINT_STRING		EQU	01EH + BASE

FETCH			EQU	04AH + BASE
STORE			EQU	04DH + BASE
GETNUM			EQU	050H + BASE
GETEOL			EQU	053H + BASE
GETOKE			EQU	056H + BASE
DISPLAY_TOKEN		EQU	059H + BASE
SAVE_AND_DISPLAY	EQU	05CH + BASE
ERROR			EQU	05FH + BASE
WAIT_FOR_USER		EQU	062H + BASE
GET_PART		EQU	065H + BASE
CONTINUATION_LINE	EQU	068H + BASE
GET_COMMA		EQU	06BH + BASE
EOL_CHECK		EQU	06EH + BASE