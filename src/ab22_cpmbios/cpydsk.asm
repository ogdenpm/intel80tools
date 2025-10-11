; COPYDISK FOR THE SBX-218 ON AN SBC-80/24.
; COPYRIGHT JAMES R. GRIER
; MARCH 19, 1981
; MAY 6, 1981 -  EXTENDED AND COMMENTED
; MAY 24, 1981 - COPYDISK
; JUNE 23, 1981 - COPY WITH VERIFY
; JULY 21, 1981 - SCREEN DISPLAY AND AUTO-FORMAT
; SEPT 1, 1981 - REVISED FOR NEW BIOS V2.0
; SEPT 2, 1981 - USES SNIFF ROUTINE TO DETERMINE SOURCE DENSITY
;
ORG     100H
NUMDSK  EQU     2
CR      EQU     13
LF      EQU     10
BIOS    EQU     0F200H
CONIN   EQU     BIOS + 09H
CONOUT  EQU     BIOS + 0CH
DRDISK  EQU     0FF00H
SNIFF   EQU     0FF06H
BIOFRM  EQU     0FE80H
;
START:
;
        LXI     H,SIGNON
        CALL    PRINT           ; PRINT SIGNON IESSAGE
;
        LXI     H,FMTQUE        ; CHECK IF FORMATTING DESIRED
        CALL    PRINT
        CALL    ECHO            ; GET ANSWER
        PUSH    PSW             ; SAVE
        MVI     C,0             ; SNIFF DRIVE 0 FOR DENSITY
        CALL    SNIFF
        MVI     A,'D'           ; SETUP FOR DOUBLE DENSITY
        JZ      ISDBL           ; ZERO FLAG IS SET IF DISK DOUBLE DENSITY
        MVI     A,'S'           ; IF NOT SET, ASSUME SINGLE
ISDBL:
        STA     DNSITY          ; SAVE VALUE
;
        POP     PSW             ; RECOVER ANSWER
        CPI     'Y'
        JNZ     NOFMT           ; IF NOT Y, SKIP FORMATTING
;
REFORM:
        LXI     H,NUMBERS
        CALL    PRINT           ; PRINT THE HEADER
        CALL    CRLF
; NOW MOVE THE FORMAT ROUTINE  INTO THE BUFFER SPACE IN BIOS
        LXI     D,680H
        LXI     H,BIOFRM
        MVI     B,80H
MVFMT:
        LDAX    D
        MOV     M,A
        INX     D
        INX     H
        DCR     B
        JNZ     MVFMT
;
        LDA     DNSITY
        CPI     'S'
        JNZ     DOUBLE
;
; IF SINGLE DENSITY, FORMAT ALL 77 TRACKS AS SINGLE DENSITY
SINGLE:
        MVI     C,0             ; TRACK NUMBER
SLOOP:
        CALL    FSD             ; PERFORM FORMAT
        MOV     A,C
        CPI     77
        JC      SLOOP           ; LOOP FOR ALL TRACKS
        JMP     NOWCOPY
;
; IF DOUBLE DENSITY, FORMAT TRACK 0 AS SINGLE, ALL OTHERS DOUBLE
DOUBLE:
        MVI     C,0
        CALL    FSD             ; FORMAT TRACK 0 AS SINGLE
DLOOP:
        CALL    FDD
        MOV     A, C
        CPI     77
        JC      DLOOP           ; LOOP FOR ALL TRACKS
        JMP     NOWCOPY
;
; FORMAT IN SINGLE DENSITY.
FSD:
        LXI     H,FSDIPB        ; POINT TO SINGLE DENSITY IOPB
        MVI     E,0             ; SET N VALLE
        JMP         GOFORM
;
; FORMAT IN DOUBLE DENSITY.
FDD:
        LXI     H, FDDIPB       ; POINT TO DOUBLE DENSITY IOPB
        MVI     E,1             ; SET N VALLE
;
GOFORM:
        MVI     B,1             ; DISK 1
        CALL    BIOFRM          ; CALL SINGLE TRACK FORMATTER.
        INR     C               ; INCREMENT TRACK NUMBER.
        PUSH    B
        MVI     C,'F'
        CALL    CONOUT          ;
        POP     B
        RET
;
FSDIPB: DB      0DH,1,0,1AH,1BH,0E5H
FDDIPB: DB      4DH,1,1,1AH,36H,0E5H
;
NOFMT:
;
        MVI     C,1
        CALL    SNIFF           ; CHECK DESTINATION
        MVI     C,'D'
        JZ      ISDDBL
        MVI     C,'S'
ISDDBL:
        LDA     DNSITY
        CMP     C
        JZ      DNSMATCH
;
        LXI     H,NOMATCH
        CALL    PRINT
        CALL    ECHO
        CPI     'Y'
        JZ      REFORM
        JMP     0
;
DNSMATCH:
        LXI     H,NUMBERS
        CALL    PRINT
;
NOWCOPY:
        CALL    CRLF
        MVI     A,0             ; TRACK 1
COPYLOOP:
        STA     TRAK
        STA     LSIOPB+2
        STA     LDIOPB+2
        STA     SSIOPB+2
        STA     SDIOPB+2
        STA     CKSIPB+2
        STA     CKDIPB+2
;
        XRA     A
        STA     ERRCNT
;
        LDA     DNSITY          ; TEST DENSITY
        CPI     'S'
        JZ      LDLSGL          ; JUMP IF SINGLE DENSITY DRIVE
;
        LDA     TRAK
        CPI     0
        JZ      LDLSGL          ; ALWAYS SINGLE IF TRACK 0
;
; DOUBLE DENSITY LOAD
DDLOAD:
        LXI     D,LDIOPB
        LXI     B,1A00H          ; LOAD DISK I/O PARAMETERS
        LXI     H,900H
        CALL    DRDISK
;
        MVI     C,0             ; NUMBER OF SECTORS
        LXI     H,900H          ; START OF MEMORY
DDD:
        PUSH    B
        MOV     A,C
        LXI     H,TRNTBL
        MVI     B, 0
        DAD     B
        MOV     A,M             ; READ TRANSLATE TABLE
        STA     SDIOPB+4
        MOV     D,A
        MVI     E,0
        LXI     H,900H - 100H
        DAD     D
        LXI     D,SDIOPB
        LXI     B,0100H
        CALL    DRDISK
        POP     B
        LXI     D,100H
        DAD     D
        INR     C
        MOV     A,C
        CPI     26
        JNZ     DDD
;
        LXI     D,CKDIPB
        LXI     B,1A00H
        LXI     H,2900H         ; VERIFY BUFFER
        CALL    DRDISK          ; REREAD WRITTEN DATA
;
        LXI     D,900H
        LXI     H,2900H
        LXI     B,1A00H
CHLP2:
        LDAX    D
        CMP     M
        JZ      LPOK2
        CALL    ERRCHK
        JMP     DDLOAD
LPOK2:
        INX     D
        INX     H
        DCX     B
        MOV     A,C
        ORA     B
        JNZ     CHLP2
        JMP     LDNXT
;
; LOAD SINGLE DENSITY DISK
LDLSGL:
        LXI     D,LSIOPB
        LXI     B,1A80H         ; DISK I/O PARAMETERS
        LXI     H,900H
        CALL    DRDISK          ; READ TRACK
;
        LXI     D,SSIOPB
        LXI     B,1A80H
        LXI     H,900H
        CALL    DRDISK
;
        LXI     D,CKSIPB
        LXI     B,1A80H
        LXI     H,2900H         ; VERIFY BUFFER
        CALL    DRDISK          ; REREAD WRITTEN DATA
;
        LXI     D,900H
        LXI     H,2900H
        LXI     B,0D00H
CHLP1:
        LDAX    D
        CMP     M
        JZ      LPOK1
        CALL    ERRCHK
        JMP     LDLSGL
LPOK1:
        INX     D
        INX     H
        DCX     B
        MOV     A,C
        ORA     B
        JNZ     CHLP1
;
LDNXT:
        MVI     C,'#'
        CALL    CONOUT
        LDA     TRAK
        INR     A
        CPI     77
        JNZ     COPYLOOP
;
        LXI     H,DONEMSG
        CALL    PRINT
        JMP     0
;
PRINT:
; PRINT STRING AT HL UNTIL NULL FOUND.
        MOV     A,M
        ORA     A
        RZ
        MOV     C,A
        CALL    CONOUT
        INX     H
        JMP     PRINT
;
CRLF:
        LXI     H,CRLFST
        JMP     PRINT
;
CRLFST: DB      CR,LF,0
;
ECHO:
        CALL    CONIN
        ANI     5FH
        PUSH    PSW
        MOV     C, A
        CALL    CONOUT
        POP     PSW
        CPI     3
        JZ      0
        RET
;
ERRCHK:
        LXI     H,ERRCNT
        INR     M
        MOV     A,M
        CPI     3
        RNZ
        LXI     H,BOMB
        CALL    PRINT
        JMP     0
;
LSIOPB: DB      6,0,0,0,1,0,1AH,7,80H           ; IOPB FOR SD READ
LDIOPB: DB      46H,0,0,0,1,1,1AH,14,0FFH       ; IOPB FOR DD READ
;
SSIOPB: DB      5,1,0,0,1,0,1AH,7,80H           ; IOPB FOR SD WRITE
SDIOPB: DB      45H,1,0,0,1,1,1AH,14,0FFH       ; IOPB FOR DD WRITE
;
CKSIPB: DB      6,1,0,0,1,0,1AH,7,80H           ; IOPB TO VERIFY SD
CKDIPB: DB      46H,1,0,0,1,1,1AH,14,0FFH       ; IOPB TO VERIFY DD
;
SIGNON: DB CR,LF,'HARVEY COPYDISK V2.1  (REVISED 02-SEP-81)'
LDPROC: DB CR,LF,'LOAD SOURCE DISK IN DRIVE A, DESTINATION DISK IN DRIVE B',0
FMTQUE: DB CR,LF,'DO YOU WISH TO FORMAT DESTINATION DISKETTE (Y/N)? ',0
NOMATCH:DB CR,LF,'DESTINATION NOT SAME DENSITY AS SOURCE  FORMAT (Y/N)? ',0
;
NUMBERS:
        DB CR,LF,CR,LF
        DB '                              CURRENT TRACK',CR,LF
        DB '0000000000111111111122222222223333333333'
        DB '4444444444S55555555566666666667777777',CR,LF
        DB '0123456789012345678901234567890123456789'
        DB '0123456789012345678901234567890123456',0
;
BOMB:   DB CR,LF,'CAN NOT VERIFY WRITTEN DATA ON DESTINATIW4 REBOOTING.. .',0
;
DONEMSG:        DB      CR,LF,'DISK COPY DONE...',0
;
TRNTBL:
        DB 1,3,5,7,9,11,13,15,17,19,21,23,25
        DB 2,4,6,8,10,12,14,16,18,20,22,24,26
;
TRAK:   DS      1
DNSITY: DS      1
ERRCNT: DS      1
;
END
