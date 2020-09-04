NAME SAMPLE
;
   EXTRN code (put_crlf, put_string, put_data_str,   get_num)
   EXTRN code (binasc, ascbin)
;
CSEG
; This is the initializing section. Execution always
; starts at address 0 on power-up.
ORG  0
mov  TMOD,#00100000B  ; set timer mode to auto-reload
mov  TH1,#(-253)      ; set timer for 110 BAUD
mov  SCON,#11011010B  ; prepare the Serial Port 
setb TR1              ; start clock
;
; This is the main program. It's an infinite loop,
; where each iteration prompts the console for 2
; input numbers and types out their sum.
START:
; type message explaining how to correct a typo
mov  DPTR,#typo_msg
call put_string
call put_crlf
; get first number from console
mov  DPTR,#num1_msg
call put_string
call put_crlf
mov  R0,#num1
call get_num
call put_crlf
; get second number from console
mov  DPTR,#num2_msg
call put_string
call put_crlf
mov  R0,#num2
call get_num
call put_crlf
; convert the ASCII numbers to binary
mov  R1,#num1
call ascbin
mov  R1,#num2
call ascbin
; add the 2 numbers, and store the results in SUM
mov  a,num1
add  a,num2
mov  sum,a
; convert SUM from binary to ASCII
mov  R1,#sum
call binasc
; output sum to console
mov  DPTR,#sum_msg
call put_string
mov  R1,#sum
mov  R2,#4
call put_data_str
jmp  start
;
   DSEG  at 8
STACK:  ds  8   ; at power-up the stack pointer is
                ;initialized to point here
;
DATA_AREA     segment DATA
CONSTANT_AREA segment CODE
;
RSEG  data_area
NUM1: ds  4
NUM2: ds  4
SUM:  ds  4
;
RSEG  constant_area
TYPO_MSG: db 'TYPE ^X TO RETYPE A NUMBER',00H
NUM1_MSG: db 'TYPE IN FIRST NUMBER: ',00H
NUM2_MSG: db 'TYPE IN SECOND NUMBER: ',00H
SUM_MSG:  db 'THE SUM IS ',00H
;
END
