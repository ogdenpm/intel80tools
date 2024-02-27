;******************************************************************************
;
;	This is the include file called ASMA.INC. It contains the
;	following subroutines in order:
;
;		CHECK_AND_CHANGE_ASM_PC
;		CHANGE_TO_INSTRUCTION_OP
;
;******************************************************************************
$EJECT
;******************************************************************************
;
;    NAME: CHECK_AND_CHANGE_ASM_PC
;
;    ABSTRACT: Change the ASM_PC according to NUMBER_OF_BYTES
;	and check to make sure it does not wrap around.
;
;    INPUTS: NUMBER_OF_BYTES, ASM_PC_LOW, ASM_PC_HIGH
;
;    OUTPUTS: ASM_PC_LOW, ASM_PC_HIGH
;
;    VARIABLES MODIFIED: A, PARAM1, ASM_FC_HIGH, ASM_PC_LOW, ERRNUM
;
;    ERROR EXITS: 13H (ASM PC>OFFFH)
;
;    SUBROUTINES ACCESSED DIRECTLY: ERROR
;
;******************************************************************************

CHECK_AND_CHANGE_ASM_PC:
	MOV	A,NUMBER_OF_BYTES
	ADD	A,ASM_PC_LOW
	MOV	PARAM1,A		;Save to put in ASM_PC_LOW
	CLR	A
	ADDC	A,ASM_PC_HIGH		;Add 1 to ASM_PC_HIGH if carry set
	JNC	CHANGE_ASM_PC_1		;Error if carry set after add
	MOV	ERRNUM,#013H		;ASM PC > OFFFH
	LJMP	ERROR
CHANGE_ASM_PC_1:
	MOV	ASM_PC_HIGH,A
	MOV	ASM_PC_LOW,PARAM1		;Replace ASM_PC with new value
	RET
$EJECT
;******************************************************************************
;
;    NAME: CHANGE_TO_INSTRUCTION_OP
;
;    ABSTRACT: Run time action used to process the one, two or three bytes of
;	the assembled instruction and write it out to memory. The assembly
;	program counter (ASM_PC) is updated according to the number of bytes
;	in the instruction. A case statement will take care of all the
;	different types of instructions. The byte(s) of the instruction are
;	stored in the appropriate order in a working area, WORKING_SPACE (3).
;	The opcode is always put in the fist byte. If the instruction is
;	other than a one byte instruction, the other bytes are obtained from
;	VALLOW, VALHGH or TEMP_SEC as necessary. NUMBER_OF_BYTES is updated
;	to reflect the number of bytes in the instruction to be written out
;	to memory and the ASM_PC is updated. The individual cases are as
;	follows:
;
;	Case 1: One byte instructions (ex. NOP)
;
;	Case 2: Two byte instructions (ex. MOV R7,#DATA)
;		Put expression in second byte.
;
;	Case 3: Three byte instructions (ex. MOV EXP8,#EXP)
;		Put the first expression in the second byte.
;		put the second expression in the third byte.
;
;	Case 4: Jump instruction with one relative operand (ex. JC REL. OPER.)
;		Calculate the relative offset and put it in the second byte.
;
;	Case 5: Jump instruction with an expression as the first operand
;		and a relative operand as the second operand
;		(ex. JNB EXP8,REL. OPER.)
;		Put the expression in the second byte, calculate the relative
;		offset and put it in the third byte.
;
;	Case 6: Absolute call or jump instruction (ex. ACALL EXP11).
;		Calculate the 2K jump or call and incorporate it into the
;		opcode. Put the lower 8 bits of EXP11 in the second byte.
;
;	Case 7: Long jump or call instruction or MOV DPTR,EXP16
;		(ex. LJMP EXP16).
;		The high byte of EXP16 is put in the second byte.   The low
;		byte of EXP16 is put in the third byte.
;
;    INPUTS: VALHGH, VALLOW, TEMP_SEC, INSTRUCTION_VALUE
;
;    OUTPUTS: Memory at address of ASM_PC
;
;    VARIABLES MODIFIED: NUMBER_OF_BYTES, REL_OFFSET_LOW, REL_OFFSET_HIGH,
;	A, ERRNUM, OLD_ASM_PC_HIGH, OLD_ASM_PC_LOW, POINT0, TEMP_SEC, C,
;	TEMP_LOW, SELECT, PNTLOW, PNTHGH, ASM_PC_HIGH, ASM_PC_LOW
;
;    ERROR EXITS: 10H (ASSEMBLY SYNTAX)
;		11H (ADDRESS OUT OF RANGE-11 BIT ABSOLUTE OFFSET)
;		12H (ADDRESS OUT OF RANGE-8 BIT RELATIVE OFFSET)
;
;    SUBROUTINES ACCESSED DIRECTLY: CHECK_AND_CHANGE_ASM_PC, ERROR
;
;******************************************************************************
CHANGE_TO_INSTRUCTION_OP:
	MOV	OLD_ASM_PC_HIGH,ASM_PC_HIGH
	MOV	OLD_ASM_PC_LOW,ASM_PC_LOW
	MOV	A,NUMBER_OF_BYTES
	CJNE	A,#001H,CHANGE_CASE_2		;Change case 1
	CALL	CHECK_AND_CHANGE_ASM_PC		;Update ASM PC
	MOV	POINT0,#WORKING_SPACE		;Get opcode
	MOV	@POINT0,INSTRUCTION
	LJMP	CHANGE_END
CHANGE_CASE_2:
	CJNE	A,#02H,CHANGE_CASE_3
	CALL	CHECK_AND_CHANGE_ASM_PC
	MOV	POINT0,#WORKING_SPACE
	MOV	@POINT0,INSTRUCTION		;Put opcode in 1st byte
	INC	POINT0
	MOV	@POINT0,VALLOW
	LJMP	CHANGE_END
CHANGE_CASE_3:
	CJNE	A,#03H,CHANGE_CASE_4
	CALL	CHECK_AND_CHANGE_ASM_PC
	MOV	POINT0,#WORKING_SPACE
	MOV	@POINT0,INSTRUCTION		;Put opcode in 1st byte
	MOV	A,INSTRUCTION
	CJNE	A,#085H,CASE_3_MORE
	MOV	A,TEMP_SEC
	XCH	A,VALLOW
	MOV	TEMP_SEC,A
CASE_3_MORE:
	MOV	POINT0,#(WORKING_SPACE + 1)
	MOV	@POINT0,TEMP_SEC
	INC	POINT0
	MOV	@POINT0,VALLOW
	LJMP	CHANGE_END
CHANGE_CASE_4:
	CJNE	A,#004H,CHANGE_CASE_5
	MOV	ERRNUM,#012H			;Adr out of range-8 bit
	MOV	NUMBER_OF_BYTES,#002H		;2 byte instruction
	CALL	CHECK_AND_CHANGE_ASM_PC
	MOV	REL_OFFSET_LOW,VALLOW
	MOV	REL_OFFSET_HIGH,VALHGH		;Move value into relative offset
	MOV	A,REL_OFFSET_HIGH
	CJNE	A,ASM_PC_HIGH,CHANGE_CASE_4A
	LJMP	CHANGE_CASE_4AA
CHANGE_CASE_4A:
	JC	BACKWARD_JUMP_CASE_4
	LJMP	FORWARD_JUMP_CASE_4
CHANGE_CASE_4AA:
	MOV	A,REL_OFFSET_LOW
	CJNE	A,ASM_PC_LOW,CHANGE_CASE_4C
CHANGE_CASE_4C:
	JC	BACKWARD_JUMP_CASE_4		;Jump if re 1. offset if < ASM_PC
FORWARD_JUMP_CASE_4:
	CLR	C
	MOV	A,REL_OFFSET_LOW
	SUBB	A,ASM_PC_LOW
	MOV	REL_OFFSET_LOW,A
	MOV	A,REL_OFFSET_HIGH
	SUBB	A,ASM_PC_HIGH			;Subtract ASM PC from relative offset
	JNZ	CHANGE_ERROR			;Error if relative offset> OFFH
	MOV	A,#07FH
	CJNE	A,REL_OFFSET_LOW,CHANGE_CASE_4D
CHANGE_CASE_4D:
	JC	CHANGE_ERROR			;Error if relative offset> 7FH
	MOV	POINT0,#(WORKING_SPACE + 1)
	MOV	@POINT0,REL_OFFSET_LOW		;Move offset into WORKING_SPACE (1)
	SJMP	CHANGE_CASE_4_END

BACKWARD_JUMP_CASE_4:
	CLR	C
	MOV	A,ASM_PC_LOW
	SUBB	A,REL_OFFSET_LOW
	MOV	REL_OFFSET_LOW,A
	MOV	A,ASM_PC_HIGH
	SUBB	A,REL_OFFSET_HIGH		;Subtract rel. offset from ASM_PC
	MOV	REL_OFFSET_HIGH,A
	JNZ	CHANGE_ERROR			;Error if relative offset> OFFH
	MOV	A,#080H
	CJNE	A,REL_OFFSET_LOW,CHANGE_CASE_4F
CHANGE_CASE_4F:
	JC	CHANGE_ERROR			;Error if relative offset is > 80H
	MOV	POINT0,#(WORKING_SPACE + 1)
	MOV	A,REL_OFFSET_LOW
	CPL	A
	INC	A
	MOV	@POINT0,A

CHANGE_CASE_4_END:
	MOV	POINT0,#WORKING_SPACE
	MOV	@POINT0,INSTRUCTION		;Move 8-bit inst into WORKING_SPACE (1)
	LJMP	CHANGE_END
CHANGE_CASE_5:
	CJNE	A,#005H,CHANGE_CASE_6
	MOV	ERRNUM,#012H			;Adr out of range - 8 bit
	MOV	NUMBER_OF_BYTES,#003H		;3 byte instruction
	CALL	CHECK_AND_CHANGE_ASM_PC		;Update ASM PC
	MOV	POINT0,#WORKING_SPACE
	MOV	@POINT0,INSTRUCTION		;Move instruction into WORKING_SPACE (0)
	MOV	REL_OFFSET_LOW,VALLOW
	MOV	REL_OFFSET_HIGH,VALHGH		;Move value into relative offset
	MOV	A,REL_OFFSET_HIGH
	CJNE	A,ASM_PC_HIGH,CHANGE_CASE_5A
	LJMP	CHANGE_CASE_5AA
CHANGE_CASE_5A:
	JC	BACKWARD_JUMP_CASE_5
	LJMP	FORWARD_JUMP_CASE_5
CHANGE_CASE_5AA:
	MOV	A,REL_OFFSET_LOW
	CJNE	A,ASM_PC_LOW,CHANGE_CASE_5C
CHANGE_CASE_5C:
	JC	BACKWARD_JUMP_CASE_5
FORWARD_JUMP_CASE_5:
	CLR	C
	MOV	A,REL_OFFSET_LOW
	SUBB	A,ASM_PC_LOW
	MOV	REL_OFFSET_LOW,A
	MOV	A,REL_OFFSET_HIGH
	SUBB	A,ASM_PC_HIGH			;Subtract ASM PC from dest. addr
						;and place in-relative offset
	JZ	FJC_5_CONTINUE			;Error if relative offset < OFFH
CHANGE_ERROR:
	MOV	ASM_PC_HIGH,OLD_ASM_PC_HIGH
	MOV	ASM_PC_LOW,OLD_ASM_PC_LOW
	LJMP	ERROR
FJC_5_CONTINUE:
	MOV	A,#07FH
	CJNE	A,REL_OFFSET_LOW,CHANGE_CASE_5D
CHANGE_CASE_5D:
	JC	CHANGE_ERROR			;Error if relative offset < 07FH
	MOV	POINT0,#(WORKING_SPACE + 2)
	MOV	@POINT0,REL_OFFSET_LOW		;Move REL_OFFSET_LOW into WORKING_SPACE
	SJMP	CHANGE_CASE_5_END

BACKWARD_JUMP_CASE_5:
	CLR	C
	MOV	A,ASM_PC_LOW
	SUBB	A,REL_OFFSET_LOW
	MOV	REL_OFFSET_LOW,A
	MOV	A,ASM_PC_HIGH
	SUBB	A,REL_OFFSET_HIGH		;Subtract relative offset from ASM PC
	MOV	REL_OFFSET_HIGH,A		;and store in relative offset
	JNZ	CHANGE_ERROR			;Error if relative offset> OFFH
	MOV	A,#080H
	CJNE	A,REL_OFFSET_LOW,CHANGE_CASE_5F
CHANGE_CASE_5F:
	JC	CHANGE_ERROR			;Error if relative offset> 080H
	MOV	POINT0,#(WORKING_SPACE + 2)
	MOV	A,REL_OFFSET_LOW
	CPL	A
	INC	A
	MOV	@POINT0,A			;Move REL_OFFSET_LOW i,nto WORKING_SPACE
CHANGE_CASE_5_END:
	MOV	POINT0,#(WORKING_SPACE + 1)
	MOV	@POINT0,TEMP_SEC		;Move TEMP_LOW into WORKING_SPACE (1 )
	LJMP	CHANGE_END
CHANGE_CASE_6:
	CJNE	A,#006H,CHANGE_CASE_7
	MOV	NUMBER_OF_BYTES,#002H		;2 byte instruction
	CALL	CHECK_AND_CHANGE_ASM_PC
	MOV	A,VALHGH			;Move value into TEMP
	ANL	A,#0F8H				;Use 3 top bits of 11 to determine
	MOV	TEMP_LOW,A			;which 2k page JMP or CALL it is
	MOV	A,#0F8H
	ANL	A,ASM_PC_HIGH
	MOV	ERRNUM,#011H			;Adr out of range (11 bit)
	CJNE	A,TEMP_LOW,CHANGE_ERROR
	MOV	POINT0,#WORKING_SPACE
	MOV	A,VALHGH			;TEMP HIGH <= 07
	ANL	A,#007H
	SWAP	A
	RL	A				;TEMP_HIGH now rotated right 3X
	ADD	A,INSTRUCTION
	MOV	@POINT0,A			;Put result in WORKING_SPACE (0)
	INC	POINT0
	MOV	@POINT0,VALLOW			;TEMP_LOW stored in WORKING_SPACE (1)
	LJMP	CHANGE_END			;truncates to 8 bits
CHANGE_CASE_7:
	MOV	ERRNUM,#010H			;Assembly syntax
	CJNE	A,#007H,CHANGE_ERROR		;Error if orig NUMBER_OF_BYTES > 7
	MOV	NUMBER_OF_BYTES,#003H		;3 byte instruction
	CALL	CHECK_AND_CHANGE_ASM_PC
	MOV	POINT0,#WORKING_SPACE
	MOV	@POINT0,INSTRUCTION		;Store instruction in WORKING_SPACE (0)
	INC	POINT0
	MOV	@POINT0,VALHGH			;Store VALHGH in WORKING_SPACE (1)
	INC	POINT0
	MOV	@POINT0,VALLOW			;Store VALLOW in WORKING_SPACE (2)
CHANGE_END:
	MOV	SELECT,#000H			;Select external ROM
	MOV	PNTLOW,OLD_ASM_PC_LOW
	MOV	PNTHGH,OLD_ASM_PC_HIGH		;Load pointer for store
	MOV	ASM_PC_LOW,OLD_ASM_PC_LOW
	MOV	ASM_PC_HIGH,OLD_ASM_PC_HIGH
	MOV	POINT0,#WORKING_SPACE
CHANGE_END_LOOP:
	MOV	A,@POINT0			;Parameter to be stored
	MOV	PARAM1,A
	CALL	STORE
	INC	POINT0
	INC	PNTLOW
	MOV	A,PNTLOW
	JNZ	CHANGE_END_A
	INC	PNTHGH
CHANGE_END_A:
	DJNZ	NUMBER_OF_BYTES,CHANGE_END_LOOP
						;Store until NUMBER_OF_BVTES=O
	MOV	ASM_PC_LOW,PNTLOW
	MOV	ASM_PC_HIGH,PNTHGH
	RET					;End of change routine
