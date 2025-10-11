; FORMAT FOR THE SBX-218 ON AN SBC-80/24.
; COPYRIGHT JAMES R. GRIER
; MARCH 20, 1981
; STATUS DISPLAY ADDED JULY 29,
; REVISED FOR NEW BIOS 01-SEP-81
;
ORG     100H
NUMDSK  EQU     2
CR      EQU     13
LF      EQU     10
CONIN   EQU     0F209H
CONOUT  EQU     0F20CH
FRMAT   EQU     0FE80H          ; SPECIALIZED FORMATTING INSTRUCTION
;
START:
; FIRST, MOVE THE FORMAT ROUTINE INTO ON-BOARD RAM.
; THE FORMAT ROUTINE (FORMATX.ASM) MUST BE ASSEMBLED AND LOADED INTO
; MEMORY WITH AN OFFSET OF 800H TO BRING ITS START FROM FE80 TO 680.
        LXI     D,680H
        LXI     H,0FE80H
        MVI     B,80H
MVFMT:
        LDAX    D
        MOV     M,A
        INX     D
        INX     H
        DCR     B
        JNZ     MVFMT
;
        LXI     H,SIGNON        ; PRINT SIGNON MESSAGE
        CALL    PRINT
FORMAT:
        LXI     H,DSQUES        ; ASK WHICH DISK TO DO
        CALL    PRINT
        CALL    ECHO            ; GET INPUT
        CPI     CR              ; BOOT IF CR
        JZ      0
;
        STA     RDYDRV          ; SAVE FOR CONFIRMATION IESSAGE
        SUI     'A'
        JC      FORMAT          ; LOOP IF < 'A'
        CPI     NUMDSK
        JNC     FORMAT          ; CHECK FOR BEYOND SYSTEM CAPACITY
;
        STA     FDDIPB+1
        STA     FSDIPB+1        ; SETUP PROPER DRIVE FOR EITHER IOPB
        STA     DISKX
;
CHOICE:
        LXI     H, STQUES       ; ASK WHAT DENSITY
        CALL    PRINT
        CALL    ECHO
        CPI     'S'
        JZ      SINGLE          ; IF 'S', DO SINGLE DENSITY
        CPI     'D'
        JZ      DOUBLE          ; IF 'D', DOUBLE DENSITY
        CPI     CR
        JZ      FORMAT          ; IF CR, RESTART FORMAT
        JMP     CHOICE          ; ANYTHING ELSE, TRY AGAIN
;$EJECT
;
; IF SINGLE DENSITY, FORMAT ALL 77 TRACKS AS SINGLE DENSITY
SINGLE:
        LXI     H, SING
        CALL    CHKRDY
        LXI     H,NUMBERS
        CALL    PRINT
        MVI     C,0             ; TRACK NUMBER
SLOOP:
        CALL    FSD             ; PERFORM FORMAT
        MOV     A,C
        CPI     77
        JC      SLOOP           ; LOOP FOR ALL TRACKS
        JMP     FORMAT
;
; IF DOUBLE DENSITY, FORMAT TRACK 0 AS SINGLE, ALL OTHERS DOUBLE
DOUBLE:
        LXI     H,DOUB
        CALL    CHKRDY
        LXI     H,NUMBERS
        CALL    PRINT
        MVI     C,0 
        CALL    FSD             ; FORMAT TRACK 0 AS SINGLE
;
DLOOP:
        CALL    FDD
        MOV     A,C
        CPI     77
        JC      DLOOP           ; LOOP FOR ALL TRACKS
        JMP     FORMAT
;
; FORMAT IN SINGLE DENSITY.
FSD:
        LDA     DISKX
        MOV     B,A
        LXI     H,FSDIPB        ; POINT TO SINGLE DENSITY IOPB
        MVI     E,0             ; SET N VALUE
        JMP     GOFORM
;
; FORMAT IN DOUBLE DENSITY.
FDD:
        LDA     DISKX
        MOV     B,A
        LXI     H,FDDIPB        ; POINT TO DOUBLE DENSITY IOPB
        MVI     E,1             ; SET N VALUE
;
GOFORM:
        CALL    FRMAT           ; CALL SINGLE TRACK FORMATTER.
        INR     C               ; INCREMENT TRACK DUMBER.
        PUSH    B
        MVI     C,'F'
        CALL    CONOUT
        POP     B
        RET
;
; GET A CHARACTER, CONVERT LOWER TO UPPER, ECHO TO CONSOLE, AND RETURN DATA
ECHO:
        CALL    CONIN           ; GET CHARACTER
        ANI     5FH             ; CONVERT LOWER CASE TO UPPER
        PUSH    PSW             ; SAVE
        MOV     C,A
        CALL    CONOUT          ; ECHO
;$EJECT
        POP     PSW
        RET
;
; PRINT STRING AT HL UNTIL NULL FOUND.
PRINT:
        MOV     A,M
        ORA     A
        RZ
        MOV     C,A
        CALL    CONOUT
        INX     H
        JMP     PRINT
;
CHKRDY:
; MOVE IN APPROPRIATE DENSITY
        LXI     D,RDYFRM
        MVI     B,6
CKLP:
        MOV     A,M
        STAX    D
        INX     D
        INX     H
        DCR     B
        JNZ     CKLP
;
        LXI     H,READY
        CALL    PRINT           ; PRINT COMPLETED MESSAGE
        CALL    ECHO            ; TEST FOR RESPONSE
        CPI     'Y'
        RZ
        POP     H
        JMP     FORMAT          ; POP STACK AND RESTART IF NOT YES
;
FSDIPB: DB      0DH,0,0,1AH,1BH,0E5H
FDDIPB: DB      4DH,0,1,1AH,36H,0E5H
;
DISKX:  DB      0
;
SIGNON: DB 'HARVEY FORMAT V2.0  (REVISED 01-SEP-81)', 0
DSQUES: DB CR,LF,'ENTER DRIVE TO FORMAT (OR RETURN TO REBOOT): ',0
STQUES: DB CR,LF,'WHAT DENSITY - SINGLE, DOUBLE, OR RET TO START OVER: ',0
READY:  DB CR,LF,'READY TO FORMAT DRIVE '
RDYDRV: DB 0,' AS '
RDYFRM: DB 'XXXXXX DENSITY (Y/N)? ',0
SING:   DB 'SINGLE'
DOUB:   DB 'DOUBLE'
;
NUMBERS:
        DB CR,LF,CR,LF
        DB '                              CURRENT TRACK',CR,LF
        DB '0000000000111111111122222222223333333333'
        DB '4444444444S55555555566666666667777777',CR,LF
        DB '0123456789012345678901234567890123456789'
        DB '0123456789012345678901234567890123456',CR,LF
        DB 0
;       END
