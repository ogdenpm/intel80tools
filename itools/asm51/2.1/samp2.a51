   NAME CONSOLE_IO
;
IO_ROUTINES segment CODE
   RSEG  IO_ROUTINES
; This is the console IO routine cluster.
   PUBLIC put_crlf, put_string, put_data_str, get_num
   USING  0
;
; This routine outputs a Carriage Return and
; a Line Feed
PUT_CRLF:
CR equ  0DH                ; carriage return
LF equ  0AH                ; line feed 
;
   mov  A,#cr
   call put_char
   mov  A,#lf
   call put_char
   ret
;
; Routine outputs a null-terminated string located
; in CODE memory, whose address is given in DPTR.
PUT_STRING:
   clr  A
   movc A,@A+DPTR
   jz   exit
   call put_char
   inc  DPTR
   jmp  put_string
EXIT:
   ret
;
; Routine outputs a string located in DATA memory,
; whose address is in R1 and its length in R2.
PUT_DATA_STR:
   mov  A,@R1
   call put_char
   inc  R1
   djnz R2,put_data_str
   ret
;
; This routine outputs a single character to console.
; The character is given in A.
PUT_CHAR:
   jnb  TI,$
   clr  TI
   mov  SBUF,A
   ret
;
; This routine gets a 4 character string from console
; and stores it in memory at the address given in R0.
; If a ^X is received, routine starts over again.
GET_NUM:
   mov  R2,#4    ; set up string length as 4
   mov  R1,AR0   ; R0 value may be needed for restart
GET_LOOP:
   call get_char
; Next 4 instructions handle ^X- the routine starts
; over if received
      clr  ACC.7           ; clear the parity bit
      cjne A,#18H,GO_ON    ; if not ^X- go on
      call put_crlf
      jmp  get_num
GO_ON:
   mov  @R1,A
   inc  R1
   djnz R2,get_loop
   ret
;
; This routine gets a single character from console.
; The character is returned in A.
GET_CHAR:
   jnb  RI,$
   clr  RI
   mov  A,SBUF
   ret
;
END
