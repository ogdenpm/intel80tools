;******************************************************************************
;
;	This is the include file called ASM.INC. It contains the
;	following subroutines in order:
;
;		START_DIVIDE
;		CALCULATE_INSTRUCTION_VALUE
;		UPDATE_OUR_CODE
;		GET_FIRST_OPERAND
;		CHECK_AND_SET_EXP_FLAG
;			SET_EXR_16_FLAG
;			SET_EXP_FLAG
;			CHECK_EXP_FLAG
;			SET_POUND_EXP_FLAG
;			CHECK_AND_SET_SECOND_EXP_FLAG
;			SET_SLASH_EXP_FLAG
;			SETREL_FLAG
;		GET_SECOND_EXP
;
;********************************************************************************
$EJECT
;********************************************************************************
; *										*
; * This module contains most procedures needed to implement the		*
; * assembler which processes the ASM command. The rest are contained		*
; * in the ASMA module.								*
; *										*
; *   INSTRUCTION_VALUE - Public variable used at parse time. The		*
; *	instruction is assembled into it.					*
; *										*
; *-----------------------------------------------------------------------------*
; *										*
; * The assembler consists of three pieces:					*
; * - Tables in the module ASM TBL code which contain the details of the	*
; *   8051 assembly language,							*
; * - Parse time procedures in this module use these tables to:			*
; *   -Set up flags and variables to control actual memory			*
; *    writing operations, search the tables for matched to the hashed		*
; *    command line.								*
; *   - Assemble the instruction as if any expression, immediate data, or	*
; *	jump addresses are zero (they are evaluated at run-time).		*
; * - Procedures selected by the above parse time procedures determine:		*
; *   - What the instruction format is,						*
; *   - How to combine the expressions, immediate data, or jump addresses	*
; *	(if any) after being calculated with the instruction value		*
; *	assembled at parse time to create the final result of the		*
; *	assembly in memory.							*
; *										*
; * The opcode is found by generating a hash value as the parser scans the	*
; * instruction. How the hash value is calculated is discussed in ASM_TBL.	*
; * All the hash values are stored in the table, #INSTRUCTION_CODE, and the	*
; * ordinal corresponding to a hash value is the opcode for that instruction.	*
; * Except for absolute instructions, in which case the opcode is further	*
; * calculated in CHANGE_TO_INSTRUCTION_OP, NUMBER_OF_BYTES contains either	*
; * the actual number of-bytes in the instruction or a code to enable		*
; * CHANGE_TO_INSTRUCTION_OP to write the correct number of bytes in the	*
; * correct order. See CHANGE_TO_INSTRUCTION_OP for more details.		*
; *										*
; * Parsing the command line leaves the opcode in INSTRUCTION_VALUE at run	*
; * time. CHANGE_TO_INSTRUCTION_OP is called after each command line		*
; * to process the type of instruction appropriately to write it out to		*
; * memory. Relative offsets and 2K jump or calls are generated here.		*
; *										*
; * Details on the use of the tables in the assembly can be found in the	*
; * documentation in the ASM_TBL module.					*
; *										*
; *-----------------------------------------------------------------------------*
; *										*
; * In the operand table the basicoperands(ex. C,A,R0-R7,etc.) have the		*
; * ordinal+1 values of 1-15 but the values 16-24 were used to represent	*
; * certain expressions as follows:						*
; *										*
; * 16 - BYTE EXP8			21 - EXP11				*
; * 17 - BIT EXP8			22 - RELATIVE OFFSET EXPRESSION		*
; * 18 - IMMEDIATE(H) EXP8		23 - @A+DPTR				*
; * 19 - COMPLEMENT(/) EXP8		24 - @A+PC				*
; * 20 - EXP16									*
; *										*
; *-----------------------------------------------------------------------------*
; *										*
; * A problem arose which made the software more involved: determining if	*
; * the eight bit expression was a bit or byte expression. Since disassembly	*
; * uses the same tables as assembly the hash values had to be precise.		*
; * The following instructions had bit expressions:				*
; *										*
; * JBC  BIT EXP,CODE EXP	ORL C,BIT EXP	MOV BIT EXP,C			*
; * JB   BIT EXP,CODE EXP	ANL C,BIT EXP					*
; * JNB  BIT EXP,CODE EXP	MOV C,BIT EXP					*
; * CLR  BIT EXP,CODE EXP							*
; * CPL  BIT EXP,CODE EXP							*
; * SETB BIT EXP,CODE EXP							*
; *										*
; * In the first group, if the mnemonic was one of those six mnemonics the   	*
; * BIT_EXP_FLAG was set and if an expression was found we know it was a bit 	*
; * expression. The second group was a little more difficult.  If the first  	*
; * operand of a two operand instruction was found to be a 'C' the BIT_EXP   	*
; * flag was set and then if the second operand was an expression we knew it 	*
; * was a bit expression. The third group was the real problem. If the		*
; * second operand of a two operand instruction was a 'c' and the first		*
; * operand had been an expression then the hash value was re-calculated to  	*
; * indicate a bit expression.							*
; *										*
;********************************************************************************
$EJECT
;********************************************************************************
;
;    NAME: START_DIVIDE
;
;    ABSTRACT: This is a software divide routine. Inputs are an 8-bit
;	divisor and a 16-bit dividend. The quotient is 16-bits and
;	the remainder is truncated to 8 bits.
;
;    INPUTS: DIVIDEND_HIGH, DIVIDEND_LOW, DIVISOR
;
;    OUTPUTS: QUOTIENT_HIGH, QUOTIENT_LOW
;
;    VARIABLES MODIFIED: A, PARAM6, DIVIDEND_LOW, QUOTIENT_HIGH,
;	PARAM5, PARAM4, C, DIVIDEND_HIGH, QUOTIENT_LOW
;
;    ERROR EXITS: None
;
;    SUBROUTINES ACCESSED DIRECTLY: None
;
;******************************************************************************
START_DIVIDE:
	MOV	A,DIVISOR
	MOV	PARAM6,#009H
	MOV	PARAM5,#000H
	MOV	PARAM4,#000H
DIVIDE_1:
	CLR	C
DIVIDE_2:
	MOV	A,DIVIDEND_HIGH
	JC	SUBTRACT_WITH_C		;Carry occurs from rotate
	JZ	ROTATE			;Rotate quotient and dividend if zero
	SUBB	A,DIVISOR
	JC	ROTATE			;A carry means divisor is larger than dividend
	MOV	DIVIDEND_HIGH,A		;Replace DIVIDEND_HIGH with new number
	MOV	A,PARAM5		;PARAM5 holds lower byte of quotient
	ADD	A,#001H			;Increment quotient
	JNC	DIVIDE_3
	INC	PARAM4			;High counter incremented if carry occurs
DIVIDE_3:
	MOV	PARAM5,A		;Replace with new quotient
	JMP	DIVIDE_1		;Loop
SUBTRACT_WITH_C:
	MOV	A,PARAM5
	ADD	A,#001H
	JNC	DIVIDE_4
	INC	PARAM4
DIVIDE_4:
	MOV	PARAM5,A		;Quotient always incremented if carry set
	CLR	C
	MOV	A,DIVIDEND_HIGH
	SUBB	A,DIVISOR
	MOV	DIVIDEND_HIGH,A		;Subtract divisor from dividend
	JC	DIVIDE_1		;Jump to subtract with no carry if carry is set
	JMP	SUBTRACT_WITH_C		;Loop in s~btract with C if no carry
ROTATE:	DJNZ	PARAM6,ROTATE_CONTINUE	;PARAM6 counts number of rotates
	MOV	QUOTIENT_HIGH,PARAM4
	MOV	QUOTIENT_LOW,PARAM5
	RET				;Exit from divide routine
ROTATE_CONTINUE:
	CLR	C
	MOV	A,PARAM5
	RLC	A			;Rotate power byte of quotient first
	MOV	PARAM5,A		;Replace with new quotient low
	MOV	A,PARAM4
	RLC	A			;Rotate upper byte with MSB from lower
	MOV	PARAM4,A		;byte into LSB of upper byte
	CLR	C
	MOV	A,DIVIDEND_LOW		;Rotate dividend with every rotate of
					;quotient
	RLC	A
	MOV	DIVIDEND_LOW,A
	MOV	A,DIVIDEND_HIGH
	RLC	A
	MOV	DIVIDEND_HIGH,A
	SJMP	DIVIDE_2		;Loop
	RET				;End of divide routines
$EJECT
;******************************************************************************
;
;    NAME: CALCULATE_INSTRUCTION_VALUE
;
;    ABSTRACT: Parse-time action to assemble the instruction just parsed
;	into the public variable INSTRUCTION_VALUE. The values may be
;	calculated. and filled in at run-time.  Using the hash value,
;	the #INSTRUCTION_CODE table is searched for a corresponding match.
;	If one is found, the ordinal of the match (INSTRUCTION_VALUE) is
;	the opcode of the instruction. If one is not found, an error is issued
;	and processing stops.
;
;    INPUTS: OUR_CODE_LOW, OUR_CODE_HIGH
;
;    OUTPUTS: INSTRUCTION, OUR_CODE_LOW, OUR_CODE_HIGH
;
;    VARIABLES MODIFIED: DPTR, A, ERRNUM, C, INSTRUCTION
;
;    ERROR EXITS: 10H (ASSEMBLY SYNTAX)
;
;    SUBROUTINES ACCESSED DIRECTLY: ERROR
;
;******************************************************************************
CALCULATE_INSTRUCTION_VALUE:
	MOV	DPTR,#INSTRUCTION_CODE
	MOV	INSTRUCTION,#000H
INST_VALUE_LOOP:
	CLR	A
	MOVC	A,@A+DPTR
	INC	INSTRUCTION
	INC	DPTR
	CJNE	A,OUR_CODE_HIGH,CHECK_AND_INC_HASH_TAB
	CLR	A
	MOVC	A,@A+DPTR
	INC	DPTR
	CJNE	A,OUR_CODE_LOW,CHECK_HASH_TAB	;Second byte is high byte (CS)
	DEC	INSTRUCTION
	RET
CHECK_AND_INC_HASH_TAB:
	INC	DPTR
CHECK_HASH_TAB:
	MOV	A,DPH
	CJNE	A,#HIGH(INSTRUCTION_CODE + 200H),INST_VALUE_LOOP
	MOV	A,DPL
	CJNE	A,#LOW(INSTRUCTION_CODE + 200H),INST_VALUE_LOOP
	MOV	ERRNUM,#010H			;Assembly syntax
	LJMP	ERROR
$EJECT
;******************************************************************************
;
;    NAME: UPDATE_OUR_CODE
;
;    ABSTRACT: Local procedure used to determine whether to use
;	'MNEMONIC_FACTOR (first operand) or 'OPERAND FACTOR*'MNEMONIC FACTOR
;	(second operand) and then update the hash vaTue, OUR_CODE.
;
;    INPUTS: OUR_CODE_LOW, OUR_CODE_HIGH, ORDINAL
;
;    OUTPUTS: OUR_CODE_LOW, OUR_CODE_HIGH
;
;    VARIABLES MODIFIED: A, B, OUR_CODE_HIGH, OUR_CODE_LOW, PARAM6
;
;    ERROR EXITS: None
;
;    SUBROUTINES ACCESSED DIRECTLY: None
;
;******************************************************************************
UPDATE_OUR_CODE:
	MOV	A,OUR_CODE_HIGH
	JNZ	UL0
	MOV	A,#MNEMONIC_FACTOR		;Set-up for <=
	MOV	B,ORDINAL			;Set-up for MUL AB
	CJNE	A,OUR_CODE_LOW,UPDATE_LSSTHN	;Fall through ifn=n,or check
CONT_UPDATE_LSSTHN:				;for n<n
	MUL	AB				;ORDINAL * #MNEMONIC_FACTOR
	MOV	OUR_CODE_HIGH,B			;Save high order byte
	ADD	A,OUR_CODE_LOW			;Add old OUR_CODE
	JNC	UL1
	INC	OUR_CODE_HIGH
UL1:	MOV	OUR_CODE_LOW,A			;Replace with new code
	RET					;Exit
UPDATE_LSSTHN:
	JNC	CONT_UPDATE_LSSTHN
UL0:	MOV	A,#MNEMONIC_FACTOR
	MOV	B,ORDINAL
	MUL	AB				;ORDINAL * #MNEMONIC_FACTOR
	MOV	PARAM6,B
	MOV	B,#OPERAND_FACTOR
	MUL	AB
	ADD	A,OUR_CODE_LOW			;Add old OUR CODE
	MOV	OUR_CODE_LOW,A			;Replace with new OUR_CODE
	MOV	A,B
	ADDC	A,OUR_CODE_HIGH			;Add high byte of OUR_CODE
	MOV	OUR_CODE_HIGH,A
	MOV	A,PARAM6			;Recall upper byte of first multo
	MOV	B,#OPERAND_FACTOR
	MUL	AB
	ADD	A,OUR_CODE_HIGH			;Add upper byte if second multiply
						;to upper byte of first multiply
						;Multiplied by OPER_FACTOR
	MOV	OUR_CODE_HIGH,A
	RET					;Exit
$EJECT
;******************************************************************************
;
;    NAME: GET_FIRST_OPERAND
;
;    ABSTRACT: (ORDINAL + 1)*MNEMONIC_FACTOR is added to OUR_CODE
;	(the hash value). If the operand was a 'C', then BIT_EXP is
;	set to 1 (true).
;
;    INPUTS: TOKSTR, OUR_CODE_LOW, OUR_CODE_HIGH
;
;    OUTPUTS: BIT_EXP, OUR_CODE_LOW, OUR_CODE_HIGH
;
;    VARIABLES MODIFIED: B, A, C, OUR_CODE_LOW, OUR_CODE_HIGH, PARAM6,
;	BIT_EXP
;
;    ERROR EXITS: None
;
;    SUBROUTINES ACCESSED DIRECTLY: None
;
;******************************************************************************
GET_FIRST_OPERAND:
	MOV	B,#MNEMONIC_FACTOR
	MOV	A,TOKSTR
	CLR	C
	SUBB	A,#090H
	JC	FIRST_NOT_REGISTER
	SUBB	A,#008H
	JNC	FIRST_NOT_REGISTER	;Check if TOKSTR=REGISTER token(O-7)
	MOV	A,TOKSTR
	CLR	C
	SUBB	A,#08CH
	MUL	AB
	ADD	A,OUR_CODE_LOW
	MOV	OUR_CODE_LOW,A
	JNC	GE_FI_OP_1
	INC	OUR_CODE_HIGH
GE_FI_OP_1:
	MOV	A,B
	ADD	A,OUR_CODE_HIGH
	MOV	OUR_CODE_HIGH,A
	LJMP	SET_BIT_EXP
FIRST_NOT_REGISTER:
	MOV	A,#OFST
	ADD	A,#REG
	MOV	PARAM6,A
	MOV	A,TOKSTR
	CLR	C
	SUBB	A,PARAM6
	MUL	AB
	ADD	A,OUR_CODE_LOW
	MOV	OUR_CODE_LOW,A
	JNC	GE_FI_OP_2
	INC	OUR_CODE_HIGH
GE_FI_OP_2:
	MOV	A,B
	ADD	A,OUR_CODE_HIGH
	MOV	OUR_CODE_HIGH,A
SET_BIT_EXP:
	MOV	A,TOKSTR
	CJNE	A,#C_TOKE,END_FIRST_OPERAND
	SETB	BIT_EXP
END_FIRST_OPERAND:
	RET				;Exit
$EJECT
;******************************************************************************
;
;    NAME: CHECK_AND_SET EXP_FLAG, SET_EXP_16_FLAG, SET_EXP_FLAG,
;	CHECK_EXP_FLAG, SET_POUND_EXP_FLAG, CHECK_AND_SET_SECOND_EXP_FLAG,
;	SET_SLASH_EXP_FLAG,SET_REL_FLAG
;
;    ABSTRACT:
;	CHECK_AND_SET_EXP_FLAG: Parse-time action to check to see if
;	BIT_EXP is set(1).  If so, the EXP8 is a bit EXP8 (eight-bit
;	expression), otherwise is ti a byte EXP8.  The ordinal is set
;	appropriately and UPDATE_OUR_CODE is called to update the
;	hash value, OUR_CODE.  The FIRST_EXP flag is set(1) to signify
;	that the first operand was an expression of some sort.
;	NUMBER_OF_BYTES is set to 2 to signify that it is a two byte
;	instruction so far.
;
;	SET_EXP_16_FLAG: Parse-time action to set the ordinal to 20 to
;	show that the operand has an EXP16 ad then cal UPDATE_OUR_CODE to
;	update the hash value, OUR_CODE. SET_NUMBER_OF_BYTES equal to
;	7 to signify that the instruction was-a long-jump or call or
;	MOV DPTR,EXP16.
;
;	SET_EXP_FLAG:  Parse-time prodecure to set the  ordinal equal to
;	16 to show that the operand was a byte EXP8 expression ad call
;	UPDATE_OUR_CODE to update the hash value, OUR_CODE. Set the
;	FIRST_EXP flag to show that the first operand-was an expression
;	of some sort.
;
;	CHECK_EXP_FLAG: Parse-time action that checks the FIRST_EXP
;	flag and the SECOND_EXP flag. by determining which are set
;	and which are not, NUMBER_OF_BYTES is set according to the
;	number of bytes in the instruction.
;		FIRST_EXP	SECOND_EXP	NUMBER_OF_BYTES
;		0		0		1
;		0		1		2
;		1		0		2
;		1		1		3
;
;	SET_POUND_EXP_FLAG: Parse-time action to set the ordinal equal
;	to I8 to show-that the operand was an immediate(#) expression.
;	update the hash value, OUR_CODE, by calling UPDATE_OUR_CODE.
;	SECOND_EXP flag is set to signify that the second operand was an
;	expression of some sort.
;
;	CHECK_AND_SET_SECOND_EXP_FLAG: Parse-time action to set the
;	SECOND_EXP flag to signify that the second operand was an expression
;	of some sort. The BIT EXP flag is checked. If set, the ordinal
;	is set equal to 17 to show that the operand was a bit EXP8. If
;	it was not set, the ordinal is set to 16 to show that the operand
;	was a byte EXP8. The hash value is updated by calling UPDATE_OUR_CODE.
;
;	SET_SLASH_EXP_FLAG: Parse-time action to set the ordinal equal to 19
;	to show that the operand was the complement(/) of a bit expression.
;	update the hash value, OUR_CODE, by calling UPDATE_OUR_CODE.
;	SECOND_EXP is set to signifY that the second operand was an expression
;	of some sort.
;
;	SET_REL_FLAG: Parse-time action to set the ordinal equal to 22 to
;	show that the operand was a relative offset(EXP8). The hash value,
;	OUR_CODE, is updated by calling UPDATE_OUR_CODE. Set NUMBER_OF_BYTES
;	equal to 4 to signify that it was a jump instruction with a relative
;	operand.
;
;    INPUTS: BIT_EXP, OUR_CQDE_LOW, OUR_CODE_HIGH, FIRST_EXP, SECOND_EXP
;
;    OUTPUTS: NUMBER_OF_BYTES, ORDINAL, FIRST_EXP, SECOND_EXP, OUR_CODE_LOW,
;	OUR_CODE_HIGH
;
;    VARIABLES MODIFIED: ORDINAL, FIRST_EXP, NUMBER_OF_BYTES, SECOND_EXP,
;	A, C, B, DPTR
;
;    ERROR EXITS: None
;
;    SUBROUTINES ACCESSED DIRECTLY: UPDATE_OUR_CODE
;
;******************************************************************************
CHECK_AND_SET_EXP_FLAG:
	MOV	ORDINAL,#010H		;In case no bit 8
	JNB	BIT_EXP,NO_BIT_8
	INC	ORDINAL			;Bit 8 occurance
NO_BIT_8:
	CALL	UPDATE_OUR_CODE
	SETB	FIRST_EXP
	MOV	NUMBER_OF_BYTES,#002H	;Two bytes so far
	RET				;Exit
;******************************************************************************
SET_EXP_16_FLAG:
	MOV	ORDINAL,#014H
	CALL	UPDATE_OUR_CODE
	MOV	NUMBER_OF_BYTES,#007H	;To signify an EXP16 instruction
	RET				;Exit
;******************************************************************************
SET_EXP_FLAG:
	MOV	ORDINAL,#010H
	CALL	UPDATE_OUR_CODE
	SETB	FIRST_EXP		;First operand of an exrpession
	RET
;******************************************************************************
CHECK_EXP_FLAG:
	CLR	A
	MOV	C,FIRST_EXP
	RLC	A
	MOV	C,SECOND_EXP
	RLC	A
	MOV	B,#004H
	MOV	DPTR,#EXP_FLAG_TABLE
	MUL	AB
	JMP	@A+DPTR
EXP_FLAG_TABLE:
	MOV	NUMBER_OF_BYTES,#001H
	RET
	MOV	NUMBER_OF_BYTES,#002H
	RET
	MOV	NUMBER_OF_BYTES,#002H
	RET
	MOV	NUMBER_OF_BYTES,#003H
	RET				;Exit
;******************************************************************************
SET_POUND_EXP_FLAG:
	MOV	ORDINAL,#012H
	CALL	UPDATE_OUR_CODE
	SETB	SECOND_EXP
	RET				;Exit
;******************************************************************************
CHECK_AND_SET_SECOND_EXP_FLAG:
	SETB	SECOND_EXP
	MOV	A,#010H
	JNB	BIT_EXP,SECOND_NO_BIT_8
	INC	A
SECOND_NO_BIT_8:
	MOV	ORDINAL,A
	JMP	UPDATE_OUR_CODE
;******************************************************************************
SET_SLASH_EXP_FLAG:
	MOV	ORDINAL,#13H		;Complement of a bit expression
	CALL	UPDATE_OUR_CODE
	SETB	SECOND_EXP
	RET				;Exit
;******************************************************************************
SET_REL_FLAG:
	MOV	ORDINAL,#16H	;Re1ative offset
	CALL	UPDATE_OUR_CODE
	MOV	NUMBER_OF_BYTES,#04H	;Jump instruction with relative operand
	RET				;Exit
$EJECT
;******************************************************************************
;
;    NAME: GET_SECOND_EXP
;
;    ABSTRACT: (#MNEMONIC_FACTOR* #OPERAND FACTOR) is added to the
;	hash value, OUR_CODE. If the operand was a 'C', then OUR_CODE
;	must be re-calculated to allow for a bit EXP8 instead of a byte
;	EXP8.
;
;    INPUTS: OUR_CODE_LOW, OUR_CODE_HIGH, TOKSTR
;
;    OUTPUTS: OUR_CODE_LOW, OUR_CODE_HIGH
;
;    VARIABLES MODIFIED: B, A, C, PARAM6, OUR_CODE_LOW, OUR_CODE_HIGH
;
;    ERROR EXITS: None
;
;    SUBROUTINES ACCESSED DIRECTLY: None
;
;******************************************************************************
GET_SECOND_OPERAND:
	MOV	B,#MNEMONIC_FACTOR
	MOV	A,TOKSTR
	CLR	C
	SUBB	A,#090H
	JC	SECOND_NOT_REGISTER
	SUBB	A,#08H
	JNC	SECOND_NOT_REGISTER	;Check if TOKSTR=REGISTER token(0-7)
	MOV	A,TOKSTR
	CLR	C
	SUBB	A,#08CH
	MUL	AB
	MOV	PARAM6,B
	MOV	B,#OPERAND_FACTOR
	MUL	AB
	ADD	A,OUR_CODE_LOW
	MOV	OUR_CODE_LOW,A
	MOV	A,B
	ADDC	A,OUR_CODE_HIGH
	MOV	OUR_CODE_HIGH,A
	MOV	A,PARAM6
	MOV	B,#OPERAND_FACTOR
	MUL	AB
	ADD	A,OUR_CODE_HIGH
	MOV	OUR_CODE_HIGH,A
	SJMP	OPERAND_C
SECOND_NOT_REGISTER:
	MOV	A,#OFST
	ADD	A,#REG
	MOV	PARAM6,A
	MOV	A,TOKSTR
	CLR	C
	SUBB	A,PARAM6
	MUL	AB
	MOV	PARAM6,B
	MOV	B,#OPERAND_FACTOR
	MUL	AB
	ADD	A,OUR_CODE_LOW
	MOV	OUR_CODE_LOW,A
	MOV	A,B
	ADDC	A,OUR_CODE_HIGH
	MOV	OUR_CODE_HIGH,A
	MOV	A,PARAM6
	MOV	B,#OPERAND_FACTOR
	MUL	AB
	ADD	A,OUR_CODE_HIGH
	MOV	OUR_CODE_HIGH,A
OPERAND_C:
	MOV	A,OUR_CODE_HIGH
	CJNE	A,#03CH,END_SECOND_OPERAND
	MOV	A,OUR_CODE_LOW
	CJNE	A,#08FH,END_SECOND_OPERAND
	MOV	OUR_CODE_LOW,#0BBH
END_SECOND_OPERAND:
	RET				;EXIT
