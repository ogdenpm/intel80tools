   NAME NUM_CONVERSION
;
NUM_ROUTINES segment CODE
   RSEG  NUM_ROUTINES
; This module handles conversion from ASCII to binary
; and back. The binary numbers are signed one-byte
; integers, i.e. their range is -128 to +127. Their
; ASCII represantation is always 4 characters long-
; i.e. a sign followed by 3 digits.
   PUBLIC ascbin, binasc
   USING  0
ZERO   EQU  '0'
PLUS   EQU  '+'
MINUS  EQU  '-'
;
; This routine converts ASCII to binary.
; INPUT-  a 4 character string pointed at by R1. The
;    number range must be -128 to +127, and the
;    string must have 3 digits preceded by a sign.
; OUTPUT- a signed one-byte integer, located where
;    the input string started (pointed at by R1).
ASCBIN:
   mov  R0,AR1    ; R1 original value is needed later
; Compute first digit value, and store it in TEMP
TEMP equ R3
   inc  R0
   mov  A,@R0
   clr  C
   subb A,#zero
   mov  B,#100
   mul  AB
   mov  TEMP,A
; Compute the second digit value
   inc  R0
   mov  A,@R0
   subb A,#zero
   mov  B,#10
   mul  AB
; Add the value of the second digit to num.
   add  A,TEMP
   mov  TEMP,A
; get third digit and its value to total
   inc  R0
   mov  A,@R0
   clr  C
   subb A,#zero
   add  A,TEMP
   mov  TEMP,A
; test the sign, and complement the number if the
; sign is a minus
   mov  A,@R1
   cjne A,#minus,pos    ;skip the next 4 instructions
                        ;if the number is positive
   mov  A,TEMP
   cpl  A
   inc  A
   mov  TEMP,A
;
; epilogue- store the result and exit
pos:
   mov  A,TEMP
   mov  @R1,A
   ret
;
; This routine converts binary to ASCII.
; INPUT-  a signed one-byte integer, pointed at by R1
; OUTPUT- a 4 character string, located where the
;    input number was (pointed at by R1).
BINASC:
SIGN bit ACC.7
; Get the number, find its sign and store its sign
   mov  A,@R1
   mov  @R1,#plus        ;store a plus sign (over-
                         ;written by minus if needed)
   jnb  sign,go_on2      ;test the sign bit
; Next 3 instructions handle negative numbers
   mov  @R1,#minus       ;store a minus sign
   dec  A
   cpl  A
; Factor out the first digit
GO_ON2:
   inc  R1
   mov  B,#100
   div  AB
   add  A,#zero
   mov  @R1,A            ;store the first digit
; Factor out the second digit
   inc  R1
   mov  A,B
   mov  B,#10
   div  AB
   add  A,#zero
   mov  @R1,A            ;store the second digit
; Store the third digit
   inc  R1
   mov  A,B
   add  A,#zero
   mov  @R1,A            ;store the third digit
; note that we return without restoring R1
   ret
;
END
