    name ISIS
    ASEG

; monitor interface
CI  equ 0f803h
RI  equ 0f806h
CO  equ 0f809h
PO  equ 0f80ch
LO  equ 0f80fh
IOCHK  equ 0f815h
IOSET  equ 0f818h


; IOPB offsets
IOCW    equ 0
IOINS   equ 1
NSEC    equ 2
TADR    equ 3
SADR    equ 4
IOBUF   equ 5

; enum E_FDCC
RDMODE  equ 1
WRMODE  equ 2
DCMD$RECAL   equ 3
OPCL    equ 4               ; disk completion status
DCMD$READ    equ 4
DCMD$WRITE   equ 6
FDCC$STATUS$0   equ 78h     ; disk status input port
LOW$ADDRESS$0   equ 79h     ; low(IOPB)
HIGH$ADDRESS$1  equ 7Ah     ; high(IOPB)
RESULT$BYTE$0   equ 7Bh     ; disk result status input port
TRACK0$LOAD$ADDRESS equ 3000h

; enum E_errors
OK  equ 0
NO$FREE$BUFFER  equ 1
BAD$AFT$NO  equ 2
AFT$FULL    equ 3
BAD$PATH    equ 4
BAD$DEVICE  equ 5
CANT$WRITE  equ 6
DISK$FULL   equ 7
DIRECTORY$FULL  equ 9
DIFFERENT$DISK  equ 0Ah
MULTIDEFINED    equ 0Bh
ALREADY$OPEN    equ 0Ch
NO$SUCH$FILE    equ 0Dh
WRITE$PROTECT   equ 0Eh
ISIS$OVERWRITE  equ 0Fh
BAD$LOAD$FORMAT equ 10h
NON$DISK$FILE   equ 11h
BAD$COMMAND equ 12h
CANT$RESCAN equ 15h
BAD$ACCESS  equ 16h
NULL$FILENAME   equ 17h
DISK$IO$ERROR   equ 18h
NULL$EXTENSION  equ 1Ch
DRIVE$NOT$READY equ 1Eh
CANT$DELETE equ 20h

; enum attrib_t, bitfield
INVISIBLE   equ 1
SYSTEM  equ 2
WRITEP  equ 4
FORMAT  equ 80h

; enum dev_t
F0DEV   equ 0
F1DEV   equ 1
TPDEV   equ 0Ch
P2DEV   equ 0Fh
CIDEV   equ 12h
CODEV   equ 13h
BBDEV   equ 14h

        org 8
loc_8:  jmp Reboot
; ---------------------------------------------------------------------------
copyright:  db 'COPYRIGHT (C) INTEL 1976'
            db '9EC', 0Dh, 0Ah, 7Fh         ; junk fill
            db ' :10107A006E260029118'
version:    db 1,1

; =============== S U B R O U T I N E =======================================


Isis:
        lxi     h, iscmd
        mov     m, c
        inx     h
        mov     m, e
        inx     h
        mov     m, d
        mov     a, c
        sui     44h             ;'D'
        jnz     loc_5C
        xchg
        inx     h               ; skip param0
        inx     h
        mov     c, m            ; param1 => drive
        inx     h
        mov     b, m
        inx     h
        mov     e, m            ; param2 => iopb
        inx     h
        mov     d, m
        call    Xeqiopb
        ret
; ---------------------------------------------------------------------------

loc_5C:
        lxi     h, 0
        dad     sp
        shld    usrsp
        lxi     h, rbyte        ; stack area
        sphl
        lxi     b, 0

loc_6A:                         ; copy the parameters over
        mvi     a, 4
        sub     c
        jc      loc_84
        lhld    ispb
        dad     b
        dad     b
        mov     e, m
        inx     h
        mov     d, m
        lxi     h, param0
        dad     b
        dad     b
        mov     m, e
        inx     h
        mov     m, d
        inr     c
        jnz     loc_6A

loc_84:                          ; work out the status return parameter and save it
        lxi     h, iscmd
        mov     c, m
        mvi     b, 0
        lxi     h, pcnts
        dad     b
        mov     c, m
        lxi     h, retStatus$p
        dad     b
        dad     b
        mov     e, m
        inx     h
        mov     d, m
        xchg
        shld    retStatus$p
        lxi     h, coldsf       ; cold start flag
        mov     a, m
        rrc
        jnc     loc_C8
        mvi     m, 0
        lda     6
        ani     1
        mvi     d, 0
        mov     e, a
        lxi     h, aTv          ; "TV"
        dad     d
        mov     a, m
        sta     cldin+1         ; " I: "
        sta     cldout+1        ; " O: "
        lxi     b, cldout       ; ": O: "
        mvi     e, 2
        call    Open
        lxi     b, cldin        ; ": I: "
        mvi     e, 1
        call    Open

loc_C8:                         ; command dispatch
        lda     iscmd
        cpi     0Dh
        jnc     CmdBad
        mov     l, a
        mvi     h, 0
        dad     h
        lxi     b, off_DD
        dad     b
        mov     e, m
        inx     h
        mov     d, m
        xchg
        pchl
; ---------------------------------------------------------------------------
off_DD: dw CmdOpen
        dw CmdClose
        dw CmdDelete
        dw CmdRead
        dw CmdWrite
        dw CmdBad
        dw CmdLoad
        dw CmdRename
        dw CmdBad
        dw CmdExit
        dw CmdAttrib
        dw CmdRescan
        dw CmdError
; ---------------------------------------------------------------------------

CmdOpen:
        lxi     h, param1
        mov     c, m            ; bc = path, e = access
        inx     h
        mov     b, m
        inx     h
        mov     e, m
        call    Open
        lhld    param0          ; save conn
        mov     m, a
        inx     h
        mvi     m, 0
        jmp     finish
; ---------------------------------------------------------------------------

CmdClose:
        call    chkaft
        lda     param0
        cpi     4
        mov     c, a            ; c = conn
        cnc     Close           ; only close conn >= 4
        jmp     finish
; ---------------------------------------------------------------------------

CmdDelete:
        lhld    param0
        mov     c, l
        mov     b, h
        call    Delete
        jmp     finish
; ---------------------------------------------------------------------------

CmdRead:                        ; set actual = 0
        lhld    param3
        mvi     m, 0
        inx     h
        mvi     m, 0
        call    chkaft
        lda     param0          ; conn == 3 (note this is user'c conn == 1)
        sui     3
        jnz     loc_14D
        lhld    param1
        shld    edit_buffer
        lxi     h, param2       ; c = count
        mov     c, m
        lhld    param3          ; de = actual$p
        xchg
        call    Edit
        jmp     finish
; ---------------------------------------------------------------------------

loc_14D:
        lda     param0
        sta     read_conn
        lhld    param1
        shld    read_buffer
        lhld    param2
        mov     c, l            ; bc = count
        mov     b, h
        lhld    param3          ; de = actual$p
        xchg
        call    Read
        jmp     finish
; ---------------------------------------------------------------------------

CmdWrite:
        call    chkaft
        lda     param0
        sta     write_conn
        lhld    param1
        mov     c, l            ; bc = buf
        mov     b, h
        lhld    param2
        xchg                    ; de = count
        call    Write
        jmp     finish
; ---------------------------------------------------------------------------

CmdBad:
        mvi     c, BAD$COMMAND
        call    Err
        jmp     finish
; ---------------------------------------------------------------------------

CmdLoad:
        lhld    param0
        shld    load_path
        lhld    param1
        shld    load_bias
        lxi     h, param2
        mov     c, m            ; c = load Switch
        lhld    param3
        xchg                    ; de = load entry
        call    Load
        jmp     finish
; ---------------------------------------------------------------------------

CmdRename:
        lhld    param0
        mov     c, l            ; bc = oldname
        mov     b, h
        lhld    param1          ; de = newname
        xchg
        call    Rename
        jmp     finish
; ---------------------------------------------------------------------------

CmdExit:
        call    Exit
        jmp     finish
; ---------------------------------------------------------------------------

CmdAttrib:
        lhld    param0
        shld    attrib_name
        lxi     h, param1
        mov     c, m            ; c  = attrib
        lxi     h, param2
        mov     e, m            ; e = on/off
        call    Attrib
        jmp     finish
; ---------------------------------------------------------------------------

CmdRescan:
        lda     param0
        dcr     a
        mvi     c, CANT$RESCAN
        cnz     RetUsr          ; can only rescan the console
        call    Rescan
        jmp     finish
; ---------------------------------------------------------------------------

CmdError:                       ; this is user invoked so temporarily clear the abort flag
        lxi     h, abort
        mvi     m, 0
        push    h
        lxi     h, param0
        mov     c, m            ; c = error number
        call    Err
        pop     h
        mvi     m, 0FFh         ; set the abort flag

finish:
        mvi     c, OK
        call    RetUsr

chkaft:
        lxi     h, param0
        mov     a, m
        cpi     0FFh            ; 255, then ignore
        mvi     c, OK
        cz      RetUsr
        adi     2               ; bias conn by 2
        mov     m, a
        cpi     7
        mvi     c, BAD$AFT$NO
        cp      RetUsr          ; >= 7 then invalid
        cpi     2
        cm      RetUsr          ; also 0 or 1 invalid
        lhld    param0
        lxi     d, closed       ; return whether inuse
        dad     d
        mov     a, m
        rrc
        ret
; End of function Isis

; ---------------------------------------------------------------------------
cldin:  db ': I: '
cldout: db ': O: '
aTv:    db 'TV'
iscmd:  db 41h                  ; junk fill
ispb:           dw 3442h        ;     "
retStatus$p:    dw 3131h        ;     "
param0:         dw 3132h        ;     "
param1:         dw 4335h        ;     "
param2:         dw 3131h        ;     "
param3:         dw 3930h        ;     "
param4:         dw 4133h        ;     "
pcnts:  db 5, 2, 2, 5, 4
        db 5, 5, 3, 3, 1
        db 4, 2, 2, 3
coldsf:         db 0FFh
edit_lptr:      db 0
edit_inMode:    db 0FFh
usrsp:          dw 3A20h        ; junk fill
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
        db  0C7h, 0C7h, 0C7h, 0C7h
rbyte:  db  36h ; 6                 ; junk fill
rtype:  db  45h ; E
        db  42h ; B
        db  45h ; E
abort:  db 0FFh
debug:  db  44h ; D                 ; junk fill

binhex_val: dw 3133h                ; junk fill
binhex_buf: dw 4431h                ;     "
binhex_len: db  41h                 ;     "

hexdig: db '0123456789ABCDEF'

; =============== S U B R O U T I N E =======================================


Binhex:
        lxi     h, binhex_buf
        mov     m, c
        inx     h
        mov     m, b
        inx     h
        mov     m, e
        mvi     d, 0
        xchg
        dad     b
        shld    binhex_buf          ; update to point to end of buf

loc_2AB:                            ; while chars still to write
        lxi     h, binhex_len
        mov     a, m
        ora     a
        rz
        dcr     m
        lhld    binhex_buf          ; advance to next slot to fill
        dcx     h
        shld    binhex_buf
        lda     binhex_val          ; convert nibble to ascii
        ani     0Fh
        mov     c, a
        mvi     b, 0
        lxi     h, hexdig           ; "0123456789ABCDEF"
        dad     b
        mov     a, m
        lhld    binhex_buf          ; and save in buffer
        mov     m, a
        mvi     c, 4
        lhld    binhex_val          ; binhex_val >>= 4

loc_2CF:
        mov     a, h
        ora     a
        rar
        mov     h, a
        mov     a, l
        rar
        mov     l, a
        dcr     c
        jnz     loc_2CF
        shld    binhex_val
        jmp     loc_2AB
; End of function binhex


; =============== S U B R O U T I N E =======================================


ResetConsoleDev:
        call    IOCHK
        ani     0FCh
        mov     c, a
        lda     6
        ani     3
        ora     c
        mov     c, a
        call    IOSET
        ret
; End of function ResetConsoleDev

; ---------------------------------------------------------------------------
err_ErrorType:  db 39h          ; junk fill
err_MsgLen:     db 26
err_Msg:        db 0Dh, 0Ah
                db 'ERROR '
err_ErrorNum:   db ' :1 USER PC '   ; first  part is junk fill
err_UserSP:     db 'F8C3', 0Dh, 0Ah, 'FDCC='
err_FDCCError:  db '7CD0'        ; junk fill
                db 0Dh, 0Ah

; =============== S U B R O U T I N E =======================================


Err:
        lxi     h, err_ErrorType
        mov     m, c
        mvi     a, 26
        sta     err_MsgLen
        mov     a, c
        lxi     h, err_ErrorNum ; " :1 USER PC "
        mvi     c, 0            ; convert errNum to decimal ascii string

loc_327:                        ; c is count of 100's
        inr     c
        sui     100
        jnc     loc_327
        dcr     c               ; went too far
        adi     100
        push    psw             ; a is errnum % 100
        mov     a, c
        adi     '0'             ; convert to digit
        mov     m, a
        inx     h
        pop     psw
        mvi     c, 0            ; now c is count of 10's

loc_339:
        inr     c
        sui     10
        jp      loc_339
        dcr     c               ; went too far
        adi     10
        push    psw             ; a = errnum % 10
        mov     a, c
        adi     '0'             ; convert to ascii
        mov     m, a
        inx     h
        pop     psw
        adi     '0'             ; convert last digit
        mov     m, a
        lhld    usrsp           ; convert user's sp to ascii form
        mov     e, m
        inx     h
        mov     d, m
        xchg
        shld    binhex_val
        lxi     b, err_UserSP   ; "F8C3\r\nFDCC="
        mvi     e, 4
        call    Binhex
        call    ResetConsoleDev
        lda     err_ErrorType   ; if diskio error issue longer message
        sui     DISK$IO$ERROR
        jnz     loc_37C
        lxi     h, err_MsgLen
        mvi     m, 37
        lhld    rbyte
        shld    binhex_val
        lxi     b, err_FDCCError ; "7CD0\r\n"
        mvi     e, 4
        call    Binhex

loc_37C:
        lxi     h, err_Msg      ; "\r\nERROR "
        lda     err_MsgLen      ; write the error message
        mov     b, a

loc_383:
        mov     c, m
        call    CO
        inx     h
        dcr     b
        jnz     loc_383
        lxi     h, abort        ; check if aborting
        mov     a, m
        rrc
        rnc
        inx     h               ; check debug
        mov     a, m
        rrc
        jc      0               ; exit via monitor
        jmp     loc_8           ; else software bootstrap
; End of function Err


; =============== S U B R O U T I N E =======================================


RetUsr:
        lhld    retStatus$p     ; save the return status
        mov     m, c
        inx     h
        mvi     m, 0
        lhld    usrsp           ; restore the user stack and return
        sphl
        ret
; End of function RetUsr

; ---------------------------------------------------------------------------
buffers:                        ; 6 x 128 byte of buffers. Junk filled
        db 'A1136003A3DD8', 0Dh, 0Ah, 7Fh
        db ' :10122C00023D215A1196DA47124E060021DF1009B1', 0Dh, 0Ah, 7Fh
        db ' :10123C004ECD09F8215A1134C22A12C3A412213DF1', 0Dh, 0Ah, 7Fh
        db ' :10124C000236000E23CD09F80E0DCD09F80E0ACD8D', 0Dh, 0Ah, 7Fh
        db ' :10125C0009F8C3A412213D024E0C060021DF10092F', 0Dh, 0Ah, 7Fh
        db ' :10126C00360A4ECD09F8CD390DC3A4123A3D02B75A', 0Dh, 0Ah, 7Fh
        db ' :10127C00CAA4124F060021DF10093A5911774FCD3D', 0Dh, 0Ah, 7Fh
        db ' :10128C0009F8CD390DC3A4120E00CD09F80E24CDEA', 0Dh, 0Ah, 7Fh
        db ' :10129C0009F8CD390DC3A412213D027ED67ACC3982', 0Dh, 0Ah, 7Fh
        db ' :1012AC000DC36F112ADD1036002336002ADD107EA7', 0Dh, 0Ah, 7Fh
        db ' :1012BC00234621DC109678DE009F4F3A3E022FA188', 0Dh, 0Ah, 7Fh
        db ' :1012CC000FD03A3D02FE7AD216132ADD104E234679', 0Dh, 0Ah, 7Fh
        db ' :1012DC002ADA1009E54F060021DF10097EE1774F6D', 0Dh, 0Ah, 7Fh
        db ' :1012EC00D60AD6019F4779D61BD6019FB00FD205DF', 0Dh, 0Ah, 7Fh
        db ' :1012FC0013213E0236FF2B36FF2ADD104E23460308', 0Dh, 0Ah, 7Fh
        db ' :10130C00702B71213D0234C3B812213E0236FF2BE3', 0Dh, 0Ah, 7Fh
        db ' :10131C003600C3B8122AFF0636802A050736012A82', 0Dh, 0Ah, 7Fh
buffer$table:   db 0, 0, 0, 0, 0, 0

; =============== S U B R O U T I N E =======================================


ClrBuf:
        mvi     e, 128          ; clear 128 byte buffer
        xra     a

loc_6B0:
        stax    b
        inx     b
        dcr     e
        jnz     loc_6B0
        ret
; End of function ClrBuf


; =============== S U B R O U T I N E =======================================


FreBuf:
        lxi     h, -3A7h        ; (bc - buffers) >> 7
        dad     b
        mvi     e, 7

loc_6BD:
        mov     a, h
        ora     a
        rar
        mov     h, a
        mov     a, l
        rar
        mov     l, a
        dcr     e
        jnz     loc_6BD
        lxi     d, buffer$table ; mark this buffer as now being free
        dad     d
        mvi     m, 0
        ret
; End of function FreBuf


; =============== S U B R O U T I N E =======================================


GetBuf:
        mvi     c, 6            ; number of buffers
        lxi     h, buffer$table
        lxi     d, buffers

loc_6D7:                                ; check if in use
        mov     a, m
        ora     a
        jnz     loc_6E1
        mvi     m, 2            ; mark as in use
        mov     a, e            ; return the address
        mov     b, d
        ret
; ---------------------------------------------------------------------------

loc_6E1:                                ; next entry in buffer table
        inx     h
        push    h
        lxi     h, 128          ; next buffer location
        dad     d
        xchg
        pop     h
        dcr     c               ; try all buffers
        jnz     loc_6D7
        mvi     c, NO$FREE$BUFFER
        call    Err
; End of function GetBuf

; ---------------------------------------------------------------------------
aft$current:    db 0FFh
pointer$base:   dw 3733h        ; junk filled
datum$base:     dw 3230h        ;     "
close$p:        dw 3041h        ;     "
device$p:       dw 3033h        ;     "
access$p:       dw 3537h        ;     "
dbuf$p:         dw 3245h        ;     "
byteNo$p:       dw 4433h        ;     "
dnum$p:         dw 0D44h        ;     "
lbuf$p:         dw 7F0Ah        ;     "
dptr$p:         dw 3A20h        ;     "
lastByte$p:     dw 3530h        ;     "
blkCnt$p:       dw 3331h        ;     "
blkNo$p:        dw 4334h        ;     "
lAddr$p:        dw 3030h        ;     "
l1Addr$p:       dw 3635h        ;     "
dAddr$p:        dw 4443h        ;     "
closed:         db     0,    0, 0FFh, 0FFh, 0FFh, 0FFh, 0FFh
device:         db     0,    1,    0,    0,    0,    0,    0
access:         db     3,    3,    0,    0,    0,    0,    0
dbuf:           dw     0,    0,    0,    0,    0,    0,    0
byteNo:         db   80h,  80h,    0,    0,    0,    0,    0
dnum:           db     0,    0,    0,    0,    0,    0,    0
lbuf:           dw     0,    0,    0,    0,    0,    0,    0
dptr:           db     0,    0,    0,    0,    0,    0,    0
lastByte:       db   80h,  80h,    0,    0,    0,    0,    0
blkcnt:         dw    25,   25,    0,    0,    0,    0,    0
blkNo:          dw     0,    0,    0,    0,    0,    0,    0
lAddr:          dw     1,    1,    0,    0,    0,    0,    0
l1Addr:         dw  101h, 101h,    0,    0,    0,    0,    0
dAddr:          dw     2,    2,    0,    0,    0,    0,    0

; =============== S U B R O U T I N E =======================================
; Set up simple pointers to the various data elements for the given AFT


SetTab:
        lxi     h, aft$current
        mvi     b, 0
        mov     a, m
        sub     c
        jz      loc_80A         ; no change
        mov     m, c
        lxi     h, closed
        dad     b
        shld    close$p
        lxi     h, device
        dad     b
        shld    device$p
        lxi     h, access
        dad     b
        shld    access$p
        lxi     h, byteNo
        dad     b
        shld    byteNo$p
        lxi     h, dnum
        dad     b
        shld    dnum$p
        lxi     h, dptr
        dad     b
        shld    dptr$p
        lxi     h, lastByte
        dad     b
        shld    lastByte$p
        lxi     h, blkNo
        dad     b
        dad     b
        shld    blkNo$p
        lxi     h, blkcnt
        dad     b
        dad     b
        shld    blkCnt$p
        lxi     h, lAddr
        dad     b
        dad     b
        shld    lAddr$p
        lxi     h, l1Addr
        dad     b
        dad     b
        shld    l1Addr$p
        lxi     h, dAddr
        dad     b
        dad     b
        shld    dAddr$p

loc_80A:
        lxi     h, lbuf
        dad     b
        dad     b
        shld    lbuf$p
        mov     e, m
        inx     h
        mov     d, m
        xchg
        shld    pointer$base
        lxi     h, dbuf
        dad     b
        dad     b
        shld    dbuf$p
        mov     e, m
        inx     h
        mov     d, m
        xchg
        shld    datum$base
        ret
; End of function SetTab


; =============== S U B R O U T I N E =======================================


GetAft:
        lxi     h,  closed+2
        mvi     c, 2            ; check for free aft (2-6)

loc_82E:
        mvi     a, 6
        sub     c
        jc      loc_83D
        mov     a, m
        inx     h
        rrc
        mov     a, c            ; if free then return the aft which is in c
        rc
        inr     c               ; next aft
        jnz     loc_82E

loc_83D:
        mvi     c, AFT$FULL
        call    Err
; End of function GetAft

; ---------------------------------------------------------------------------
xeq_drive:      db  31h ; A     ; junk filled
xeq_dcb$p:      dw 3233h        ;     "
xeq_recal$pb:   db 80h, DCMD$RECAL, 0, 0, 0

; =============== S U B R O U T I N E =======================================


Xeqiopb:
        lxi     h, xeq_drive
        mov     m, c
        inx     h
        mov     m, e
        inx     h
        mov     m, d
        di
        lxi     h, xeq_recal$pb + IOINS
        mvi     m, DCMD$RECAL    ; recalibrate
        lxi     h, xeq_recal$pb + SADR
        mvi     m, 0
        mov     a, c            ; check if drive 0 or drive 1
        rrc
        jnc     loc_876
        mvi     m, 20h          ; select drive 1
        lxi     h, xeq_recal$pb + IOINS
        mvi     m, 33h
        xchg
        inx     h               ; update the user's iopb cmd to reflect drive 1
        mov     a, m
        ori     30h
        mov     m, a
        inx     h
        inx     h
        inx     h
        mov     a, m            ; and the sadr likewise
        ori     20h
        mov     m, a

loc_876:
        mvi     c, 10           ; retry count

loc_878:
        in      FDCC$STATUS$0   ; disk status input port
        ani     OPCL            ; disk completion status
        jz      loc_886
        in      LOW$ADDRESS$0   ; low(IOPB)
        in      RESULT$BYTE$0   ; disk result status input port
        jmp     loc_878
; ---------------------------------------------------------------------------

loc_886:
        in      FDCC$STATUS$0   ; disk status input port
        lxi     h, xeq_drive
        mov     e, m
        inr     e
        ana     e
        push    b               ; preserve retry count
        mvi     c, DRIVE$NOT$READY
        cz      Err
        pop     b
        lxi     h, xeq_dcb$p    ; issue the user's iopb command
        mov     a, m
        out     LOW$ADDRESS$0   ; low(IOPB)
        inx     h
        mov     a, m
        out     HIGH$ADDRESS$1  ; high(IOPB)

loc_89F:
        in      FDCC$STATUS$0   ; disk status input port
        ani     OPCL            ; disk completion status
        jz      loc_89F
        in      LOW$ADDRESS$0   ; low(IOPB)
        lxi     h, rtype        ; get the result type
        mov     m, a
        in      RESULT$BYTE$0   ; disk result status input port
        dcx     h
        mov     m, a            ; and result byte
        ora     a
        jnz     loc_8B6
        ei                      ; 0 then all done
        ret
; ---------------------------------------------------------------------------

loc_8B6:                                ; issue recalibrate
        lxi     d, xeq_recal$pb
        mov     a, e
        out     LOW$ADDRESS$0   ; low(IOPB)
        mov     a, d
        out     HIGH$ADDRESS$1  ; high(IOPB)

loc_8BF:                                ; disk status input port
        in      FDCC$STATUS$0
        ani     4
        jz      loc_8BF         ; wait till complete
        dcr     c               ; see if done all retries
        jnz     loc_878
        mvi     c, DISK$IO$ERROR
        call    Err
; ---------------------------------------------------------------------------
xio_Cmd:  db  33h               ; junk filled
xio_Drv:  db  41h               ; junk filled
xio_dcb:
        db 80h, 31h, 31h, 33h, 30h  ; all bar 80h junk filled
        dw 4446h                ; junk filled
        db '211'                ;     "
; ---------------------------------------------------------------------------

Xio:                                    ; de = buffer
        push    d
        push    b               ; bc = track/sec
        lxi     h, xio_dcb + IOINS
        lda     xio_Cmd         ; install the command
        mov     m, a
        inx     h
        mvi     m, 1            ; 1 sector
        inx     h
        pop     d
        mov     m, d            ; the track/sec
        inx     h
        mov     m, e
        inx     h
        pop     d
        mov     m, e            ; the buffer
        inx     h
        mov     m, d
        lxi     h, xio_Drv      ; pick up the drive
        mov     c, m
        lxi     d, xio_dcb
        call    Xeqiopb
        ret
; End of function Xeqiopb


; =============== S U B R O U T I N E =======================================


RdSec:
        lxi     h, xio_Cmd      ; bc = tracksec, de = buffer
        mvi     m, DCMD$READ
        lhld    device$p
        mov     a, m
        sta     xio_Drv
        call    Xio
        ret
; End of function RdSec


; =============== S U B R O U T I N E =======================================


WrSec:
        lxi     h, xio_Cmd
        mvi     m, CANT$WRITE
        lhld    device$p
        mov     a, m
        sta     xio_Drv
        call    Xio
        ret
; End of function WrSec

; ---------------------------------------------------------------------------
deviceNames:    db '0F1FITOTIVOV1I1ORTRH1R2RPTPH1P2PPL1LICOCBB'
pn:             db '210013097E' ; junk filled
pn2:            db '0D21521309' ;     "
canon_name:     dw 3639h        ;     "
canon_intfn$p:  dw 3243h        ;     "
canon_frst:     db 34h          ;     "
canon_second:   db 35h          ;     "
                db 31h          ;     "
canon_i:        db 34h          ;     "

; =============== S U B R O U T I N E =======================================


Canon:


        lxi     h, canon_name
        mov     m, c            ; bc = filename, de = intfn$p
        inx     h
        mov     m, b
        inx     h
        mov     m, e
        inx     h
        mov     m, d
        jmp     loc_9C2
; End of function Canon


; =============== S U B R O U T I N E =======================================


Alphanum:
        lhld    canon_name
        mov     a, m
        call    ToUpper
        sui     'A'
        sbb     a
        cma
        mov     c, a
        push    b
        mov     e, m
        mvi     a, 'Z'
        sub     e
        sbb     a
        cma
        pop     d
        ana     e
        mov     c, a
        push    b
        mov     a, m
        sui     '0'
        sbb     a
        cma
        mov     e, a
        push    d
        mov     c, m
        mvi     a, '9'
        sub     c
        sbb     a
        cma
        pop     b
        ana     c
        pop     b
        ora     c
        ret
; End of function Alphanum


; =============== S U B R O U T I N E =======================================


ToUpper:
        cpi     'a'
        rm
        cpi     'z' + 1
        rp
        ani     0DFh
        ret
; End of function ToUpper


; =============== S U B R O U T I N E =======================================


GetFnChar:
        call    Alphanum        ; only accept alpha numeric
        rrc
        jnc     locret_9C1
        lxi     h, canon_i      ; stuff the character
        mov     c, m
        mvi     b, 0
        lhld    canon_intfn$p   ; index into Fn
        dad     b
        xchg
        lhld    canon_name      ; get the name char
        mov     a, m
        stax    d               ; and save in Fn (note not uppercased !!)
        inx     h
        shld    canon_name
        lxi     h, canon_frst   ; clear flag to reflect we have a char
        mvi     m, 0

locret_9C1:
        ret
; End of function GetFnChar

; ---------------------------------------------------------------------------
; START OF FUNCTION CHUNK FOR Canon

loc_9C2:                                ; clear the Fn
        lhld    canon_intfn$p
        mvi     c, 10

loc_9C7:
        mvi     m, 0
        inx     h
        dcr     c
        jnz     loc_9C7

loc_9CE:                                ; skip leading spaces
        lhld    canon_name
        mov     a, m
        sui     ' '
        jnz     loc_9DE
        inx     h
        shld    canon_name
        jmp     loc_9CE
; ---------------------------------------------------------------------------

loc_9DE:                                ; check for defice
        lhld    canon_name
        mov     a, m
        sui     ':'
        jnz     loc_A38
        lhld    canon_intfn$p
        mvi     m, 0FFh         ; set device to 0xff, until determined otherwise
        lhld    canon_name
        inx     h
        mov     a, m
        sta     canon_second    ; interestingly not upper cased
        inx     h
        mov     a, m
        sta     canon_frst      ; bytes are stored swapped for the comparison
        inx     h
        mov     a, m
        sui     ':'             ; should be a closing :
        mvi     a, BAD$PATH
        rnz
        lxi     b, 0            ; hunt for a match, bc is the device

loc_A03:
        mvi     a, 20
        sub     c
        jc      loc_A25         ; all done
        lxi     h, deviceNames  ; "0F1FITOTIVOV1I1ORTRH1R2RPTPH1P2PPL1LICO"...
        dad     b
        dad     b
        mov     a, m
        inx     h
        mov     d, m
        lxi     h, canon_frst   ; first char compared is 2nd char of the device
        sub     m
        inx     h
        mov     e, a            ; save difference in second char
        mov     a, d            ; check the first char
        sbb     m
        ora     e
        jnz     loc_A21
        lhld    canon_intfn$p   ; save the device
        mov     m, c

loc_A21:                                ; keep looking (even if we found already !)
        inr     c
        jnz     loc_A03

loc_A25:                                ; check we got a device
        lhld    canon_intfn$p
        mov     a, m
        sui     0FFh
        mvi     a, BAD$DEVICE
        rz
        lxi     b, 4
        lhld    canon_name      ; advance past device
        dad     b
        shld    canon_name

loc_A38:                                ; use as a flag to indicate no chars
        lxi     h, canon_frst
        mvi     m, 0FFh
        lxi     h, canon_i
        mvi     m, 1            ; collect up to 6 chars of name

loc_A42:
        mvi     a, 6
        lxi     h, canon_i
        sub     m
        jc      loc_A55
        call    GetFnChar
        lxi     h, canon_i
        inr     m
        jnz     loc_A42

loc_A55:
        lxi     h, canon_frst
        mov     c, m
        inx     h               ; copy flag for whether no name
        mov     m, c
        lhld    canon_name
        mov     a, m
        sui     '.'
        sui     1
        sbb     a
        sta     canon_frst      ; second is true if we have a . i.e. we need an extent
        rrc
        jnc     loc_A8A
        lhld    canon_name      ; past the .
        inx     h
        shld    canon_name
        lxi     h, canon_i      ; collect the extent
        mvi     m, 7

loc_A77:
        mvi     a, 9
        lxi     h, canon_i
        sub     m
        jc      loc_A8A
        call    GetFnChar
        lxi     h, canon_i
        inr     m
        jnz     loc_A77

loc_A8A:
        lhld    canon_intfn$p
        mov     c, m
        mvi     a, 1
        sub     c
        jc      loc_AA2         ; jump if not F0 or F1
        lxi     h, canon_second ; error if we had no name
        mov     a, m
        rrc
        mvi     a, NULL$FILENAME
        rc
        dcx     h               ; error if we had no ext and . was seen
        mov     a, m
        rrc
        mvi     a, NULL$EXTENSION
        rc

loc_AA2:
        call    Alphanum
        mov     c, a            ; c reflects if alpha numeric
        lhld    canon_name
        mov     a, m
        sui     '.'
        sui     1
        sbb     a
        ora     c
        mov     e, a
        mov     a, m
        sui     ':'
        sui     1
        sbb     a
        ora     e
        rrc                     ; error if we have alphanumeric, . or : after name
        mvi     a, BAD$PATH
        rc
        xra     a
        ret
; END OF FUNCTION CHUNK FOR Canon

; =============== S U B R O U T I N E =======================================


GetFn:
        call    Canon
        ora     a
        rz
        mov     c, a
        call    RetUsr
; End of function GetFn

; ---------------------------------------------------------------------------
bitMapBuffer:               ; 512 byte bit map, junk filled
        db '9960E04DC9B7C', 0Dh, 0Ah, 7Fh
        db ' :1014AD00032146094ECDA607014609CD91130F0E16', 0Dh, 0Ah, 7Fh
        db ' :1014BD000DD49B03218B144E0600218D14097E2122', 0Dh, 0Ah, 7Fh
        db ' :1014CD005B13B6773A8C140FDAE914218B144E06A0', 0Dh, 0Ah, 7Fh
        db ' :1014DD0000218D14097E2F215B13A6773A46093220', 0Dh, 0Ah, 7Fh
        db ' :0F14ED003310015113111000CD4C10CD6413C9F1', 0Dh, 0Ah, 7Fh
        db ' :1014F'
mapChange:      db    0
mapDCB: db 80h, DCMD$WRITE, 2, 2, 2
        dw bitMapBuffer
        db '000'            ; junk filled
mapDrv: db 0FFh
baseTrkSec:   dw 3030h      ; junk filled
bitMapIndex:  dw 3031h
bitMask:      db   80h,  40h,  20h,  10h,    8,    4,    2,    1

; =============== S U B R O U T I N E =======================================


RwMap:
        lda     mapDCB + IOINS
        sui     CANT$WRITE
        sui     1
        sbb     a
        lxi     h, mapChange
        mov     c, a
        mov     a, m
        cma
        ana     c
        rrc
        rc                      ; op is write and no change so nothing to do
        mvi     m, 0            ; clear the mapChg
        lxi     h, 0
        shld    bitMapIndex
        inx     h
        shld    baseTrkSec      ; sets both sector and track
        lxi     h, mapDrv
        mov     c, m
        lxi     d, mapDCB
        call    Xeqiopb
        lxi     h, mapDCB + IOINS
        mvi     m, CANT$WRITE   ; mark next op to default to write
        ret
; End of function RwMap


; =============== S U B R O U T I N E =======================================


LoadMap:
        lda     mapDrv          ; same drive so no change
        sub     c
        rz
        push    b
        call    RwMap           ; write the old map if necessary
        pop     b
        lxi     h, mapDrv       ; update the new drive
        mov     m, c
        lxi     h, mapDCB + IOINS
        mvi     m, DCMD$READ    ; make a read op
        call    RwMap
        ret
; End of function LoadMap

; ---------------------------------------------------------------------------
alloc_Drv:      db 35h          ; all below are junk filled
alloc_bitNo:    db 31h
alloc_trkSec:   dw 3045h
alloc_bitMapByte$p:dw 3030h

; =============== S U B R O U T I N E =======================================


Allocate:
        lxi     h, alloc_Drv
        mov     m, c
        call    LoadMap         ; make sure we have a valid bit map
        lda     baseTrkSec+1
        sta     alloc_trkSec+1

loc_C36:
        mvi     a, 76
        lxi     h,  alloc_trkSec+1
        sub     m
        jc      loc_CB7
        lda     baseTrkSec
        sta     alloc_trkSec

loc_C45:
        mvi     a, 26
        lxi     h, alloc_trkSec
        sub     m
        jc      loc_CAB
        lda     bitMapIndex
        ani     7
        sta     alloc_bitNo
        mvi     e, 3            ; >> 3
        lhld    bitMapIndex

loc_C5B:
        mov     a, h
        ora     a
        rar
        mov     h, a
        mov     a, l
        rar
        mov     l, a
        dcr     e
        jnz     loc_C5B
        lxi     d, bitMapBuffer
        dad     d
        shld    alloc_bitMapByte$p
        lxi     h, alloc_bitNo
        mov     c, m
        mvi     b, 0
        lxi     h, bitMask
        dad     b
        mov     a, m
        lhld    alloc_bitMapByte$p
        ana     m
        jnz     loc_C9D
        lxi     h, mapChange    ; record bit map changed
        mvi     m, 0FFh
        lxi     h, alloc_bitNo
        mov     e, m
        mvi     d, 0
        lxi     h, bitMask
        dad     d
        mov     a, m
        lhld    alloc_bitMapByte$p
        ora     m
        mov     m, a
        lhld    alloc_trkSec
        shld    baseTrkSec
        mov     a, l
        mov     b, h
        ret
; ---------------------------------------------------------------------------

loc_C9D:
        lhld    bitMapIndex
        inx     h
        shld    bitMapIndex
        lxi     h, alloc_trkSec
        inr     m
        jnz     loc_C45

loc_CAB:
        lxi     h, baseTrkSec
        mvi     m, 1
        lxi     h,  alloc_trkSec+1
        inr     m
        jnz     loc_C36

loc_CB7:
        mvi     c, DISK$FULL
        call    Err
        ret
; End of function Allocate

; ---------------------------------------------------------------------------
dealloc_drive:  db  30h         ; all below are junk filled
dealloc_trksec: dw 3236h
dealloc_bitmapByte$p:dw 4641h
dealloc_bitno:  db 42h

; this routing implements the following PL/M function

;  DEALLOC: PROCEDURE(DRIVE, TRKSEC);
;  DECLARE DRIVE BYTE, TRKSEC ADDRESS;
;  DECLARE BITMAPBYTE$P ADDRESS, BITMAPBYTE BASED BITMAPBYTE$P BYTE;
;  DECLARE BITNO BYTE;
;
;  CALL RWMAP(DRIVE, DCMD$READ);
;  IF (BITMAPBYTE$P := HIGH(TRKSEC) * 26 + LOW(TRKSEC) - 1) < BITMAPINDEX THEN
;     DO;
;        BITMAPINDEX = BITMAPBYTE$P;
;        BASETRKSEC = TRKSEC;
;     END;
;        BITNO = LOW(BITMAPBYTE$P) AND 7;
;        BITMAPBYTE$P = .BITMAPBUFFER + SHR(BITMAPBYTE$P, 3);
;        BITMAPBYTE = BITMAPBYTE AND NOT BITMASK(BITNO);
;        MAP$CHANGE = TRUE;
;  END;

; =============== S U B R O U T I N E =======================================


Dealloc:
        lxi     h, dealloc_drive
        mov     m, c
        inx     h
        mov     m, e
        inx     h
        mov     m, d
        call    LoadMap
        lhld    dealloc_trksec+1 ; track
        mvi     h, 0
        push    h               ; stack = track
        dad     h
        xchg                    ; de = track * 2
        pop     h
        push    h
        dad     d
        dad     h
        dad     h               ; hl = track * 12
        xchg
        pop     h               ; hl = track * 13
        dad     d
        dad     h               ; hl = track * 26
        xchg
        lhld    dealloc_trksec
        mvi     h, 0
        dcx     h
        dad     d
        shld    dealloc_bitmapByte$p
        xchg
        mov     a, e
        lxi     h, bitMapIndex
        sub     m
        inx     h
        mov     a, d
        sbb     m
        jnc     loc_D02
        lhld    dealloc_bitmapByte$p
        shld    bitMapIndex
        lhld    dealloc_trksec
        shld    baseTrkSec

loc_D02:
        lda     dealloc_bitmapByte$p
        ani     7
        sta     dealloc_bitno
        lhld    dealloc_bitmapByte$p
        mvi     e, 3

loc_D0F:
        mov     a, h
        ora     a
        rar
        mov     h, a
        mov     a, l
        rar
        mov     l, a
        dcr     e
        jnz     loc_D0F
        lxi     d, bitMapBuffer
        dad     d
        shld    dealloc_bitmapByte$p
        mov     c, m
        lxi     h, dealloc_bitno
        mov     e, m
        mvi     d, 0
        lxi     h, bitMask
        dad     d
        mov     a, m
        cma
        ana     c
        lhld    dealloc_bitmapByte$p
        mov     m, a
        lxi     h, mapChange
        mvi     m, 0FFh
        ret
; End of function Dealloc


; =============== S U B R O U T I N E =======================================


Rescan:
        lxi     h, edit_lptr    ; reset edit inptr & in$mode
        mvi     m, 0
        inx     h
        mvi     m, 0
        ret
; End of function Rescan
; ---------------------------------------------------------------------------
ioMask: db  0FFh, 0FFh, 0FCh, 0FCh, 0FCh, 0FCh, 0FCh, 0FCh
        db  0F3h, 0F3h, 0F3h, 0F3h, 0CFh, 0CFh, 0CFh, 0CFh
        db   3Fh,  3Fh
ioFlag: db     0,    0,    0,    0,    1,    1,    3,    3
        db     0,    4,    8,  0Ch,    0,  10h,  20h,  30h
        db   80h, 0C0h
fio_conn:       db 7Fh          ; all below are junk filled
fio_buffer:     dw 3A20h
fio_count:      dw 3031h
fio_actual:     dw 3531h
fio_writeFlag:  db 38h
fio_needwrite:  db 45h
fio_work$buf:   dw 3030h
fio_newLinks$p: dw 3730h

; =============== S U B R O U T I N E =======================================


Fio:
        lxi     h, fio_actual
        mov     m, c
        inx     h
        mov     m, b
        inx     h
        mov     m, e
        mov     h, b
        mov     l, c
        mvi     m, 0            ; Actual = 0
        inx     h
        mvi     m, 0

loc_D82:                                ; while count > actual
        lhld    fio_actual
        mov     a, m
        inx     h
        mov     b, m
        lxi     h, fio_count
        sub     m
        inx     h
        mov     a, b
        sbb     m
        rnc
        lda     fio_writeFlag   ; fioFlg is true for write
        cma
        mov     c, a
        push    b
        lhld    blkCnt$p        ; return if reading and cur$blkcnt <= cur$blkno and cur$byteno = cur$lastbyte
        mov     e, m
        inx     h
        mov     d, m
        lhld    blkNo$p
        mov     a, m
        inx     h
        mov     b, m
        sub     e
        mov     a, b
        sbb     d
        sbb     a
        cma
        pop     d
        ana     e
        mov     c, a
        push    b
        lhld    byteNo$p
        mov     a, m
        lhld    lastByte$p
        sub     m
        sui     1
        sbb     a
        pop     d
        ana     e
        rrc
        rc
        lhld    byteNo$p        ; if cur$byteno = 128
        mov     a, m
        sui     128
        jnz     loc_F2B
        mvi     m, 0            ; cur$byteno = 0
        lhld    dptr$p          ; if (cur$dptr := cur$dptr + 1) = 64
        mov     a, m
        inr     a
        mov     m, a
        sui     64
        jnz     loc_E50
        lhld    pointer$base    ; if links(FLINK) = 0
        inx     h
        inx     h
        mov     a, m
        inx     h
        ora     m
        jnz     loc_E1B
        dcx     h
        push    h
        lhld    device$p        ; links(FLINK) = alloc(cur$device)
        mov     c, m
        call    Allocate
        pop     h
        mov     m, a
        inx     h
        mov     m, b
        lhld    dbuf$p          ; clrbuf(cur$dbuf)
        mov     c, m
        inx     h
        mov     b, m
        call    ClrBuf
        lhld    dbuf$p
        mov     e, m
        inx     h
        mov     d, m
        xchg
        shld    fio_newLinks$p  ; newlnk = cur$dbuf
        lhld    lAddr$p
        mov     e, m
        inx     h
        mov     d, m
        lhld    fio_newLinks$p  ; newlnk[0] = cur$laddr
        mov     m, e
        inx     h
        mov     m, d
        call    RwMap
        lhld    pointer$base
        inx     h
        inx     h
        mov     c, m
        inx     h
        mov     b, m
        lhld    dbuf$p
        mov     e, m
        inx     h
        mov     d, m
        call    WrSec           ; wrSec(links(FLINK), cur$dbuf)

loc_E1B:
        lda     fio_writeFlag
        rrc
        jnc     loc_E34         ; jmp if reading
        call    RwMap
        lhld    lAddr$p
        mov     c, m
        inx     h
        mov     b, m
        lhld    lbuf$p
        mov     e, m
        inx     h
        mov     d, m
        call    WrSec           ; WrSec(cur$lbuf, cur$Laddr)

loc_E34:                                ; cur$Laddr = links(FLINK)
        lhld    pointer$base
        inx     h
        inx     h
        mov     c, m
        inx     h
        mov     b, m
        lhld    lAddr$p
        mov     m, c
        inx     h
        mov     m, b
        lhld    lbuf$p
        mov     e, m
        inx     h
        mov     d, m
        call    RdSec           ; RdSec(cur$Laddr, cur$Lbuf)
        lhld    dptr$p          ; cur$dptr = 2
        mvi     m, 2

loc_E50:                                ; cur$Daddr = links(cur$dptr)
        lhld    dptr$p
        mov     c, m
        mvi     b, 0
        lhld    pointer$base
        dad     b
        dad     b
        mov     a, m
        inx     h
        mov     b, m
        lhld    dAddr$p
        mov     m, a
        inx     h
        mov     m, b
        lda     fio_writeFlag
        rrc
        jnc     loc_E86         ; jmp if reading
        lhld    device$p        ; links(cur$dptr), cur$daddr = alloc(cur$device)
        mov     c, m
        call    Allocate
        lhld    dptr$p
        mov     e, m
        mvi     d, 0
        lhld    pointer$base
        dad     d
        dad     d
        mov     m, a
        inx     h
        mov     m, b
        lhld    dAddr$p
        mov     m, a
        inx     h
        mov     m, b

loc_E86:                                ; cur$blkno = cur$blkno + 1
        lhld    blkNo$p
        mov     e, m
        inx     h
        mov     d, m
        inx     d
        mov     m, d
        dcx     h
        mov     m, e
        lhld    fio_actual      ;  if (needwrite := count - actual) >= 128 and
                                ;           (not reading or  cur$blkno < cur$blkcnt))
        mov     c, m
        inx     h
        mov     b, m
        lxi     h, fio_count
        mov     a, m
        inx     h
        mov     d, m
        sub     c
        mov     e, a
        mov     a, d
        sbb     b
        mov     d, a
        mov     a, e
        sui     80h
        mov     e, a
        mov     a, d
        sbi     0
        sbb     a
        cma
        lhld    blkNo$p
        mov     e, a
        mov     a, m
        inx     h
        mov     b, m
        push    d
        lhld    blkCnt$p
        mov     c, a
        mov     a, m
        inx     h
        mov     d, m
        mov     e, a
        mov     a, c
        sub     e
        mov     c, a
        mov     a, b
        sbb     d
        sbb     a
        lxi     h, fio_writeFlag
        ora     m
        pop     d
        ana     e
        inx     h
        mov     m, a
        rrc
        jnc     loc_EF6
        lhld    byteNo$p        ; cur$byteNo := 128
        mvi     m, 128
        lhld    fio_actual      ; actual += 128
        mov     a, m
        inx     h
        mov     h, m
        mov     l, a
        lxi     d, 128
        dad     d
        xchg
        lhld    fio_actual
        mov     m, e
        inx     h
        mov     m, d
        lhld    fio_buffer      ; workbuf = buffer
        shld    fio_work$buf
        lxi     b, 128
        lhld    fio_buffer      ; buffer += 128
        dad     b
        shld    fio_buffer
        jmp     loc_F00
; ---------------------------------------------------------------------------

loc_EF6:                                ; workBuf = cur$dbuf
        lhld    dbuf$p
        mov     e, m
        inx     h
        mov     d, m
        xchg
        shld    fio_work$buf

loc_F00:
        lxi     h, fio_writeFlag
        mov     a, m
        rrc
        jnc     loc_F1E         ; jmp if reading
        inx     h
        mov     a, m
        rrc
        jnc     loc_F2B
        lhld    dAddr$p
        mov     c, m
        inx     h
        mov     b, m
        lhld    fio_work$buf
        xchg
        call    WrSec           ; WrSec(cur$daddr, workBuf)
        jmp     loc_F2B
; ---------------------------------------------------------------------------

loc_F1E:                                ; RdSec(cur$daddr, workBuf)
        lhld    dAddr$p
        mov     c, m
        inx     h
        mov     b, m
        lhld    fio_work$buf
        xchg
        call    RdSec

loc_F2B:                                ; if cur$byteno != 128
        lhld    byteNo$p
        mov     a, m
        sui     128
        jz      loc_D82
        lda     fio_writeFlag
        rrc
        jnc     loc_F4E         ; jmp if reading
        lhld    byteNo$p        ; datum[cur$byteNo) = *buffer
        mov     c, m
        mvi     b, 0
        lhld    datum$base
        dad     b
        xchg
        lhld    fio_buffer
        mov     a, m
        stax    d
        jmp     loc_F5D
; ---------------------------------------------------------------------------

loc_F4E:                                ; *buffer = dataum[cur$byteno]
        lhld    byteNo$p
        mov     c, m
        mvi     b, 0
        lhld    datum$base
        dad     b
        mov     a, m
        lhld    fio_buffer
        mov     m, a

loc_F5D:                                ; if ++cur$byteno = 128 and writing
        lhld    byteNo$p
        mov     a, m
        inr     a
        mov     m, a
        sui     128
        sui     1
        sbb     a
        lxi     h, fio_writeFlag
        ana     m
        rrc
        jnc     loc_F7F
        lhld    dAddr$p         ; WrSec(cur$daddr, cur$dbuf)
        mov     c, m
        inx     h
        mov     b, m
        lhld    dbuf$p
        mov     e, m
        inx     h
        mov     d, m
        call    WrSec

loc_F7F:                                ; ++buffer
        lhld    fio_buffer
        inx     h
        shld    fio_buffer
        lhld    fio_actual      ; actual--
        mov     e, m
        inx     h
        mov     d, m
        inx     d
        mov     m, d
        dcx     h
        mov     m, e
        jmp     loc_D82
; End of function Fio


; =============== S U B R O U T I N E =======================================


SetDev:
        lhld    device$p
        mov     e, m
        mvi     d, 0
        lxi     h, ioMask
        dad     d
        call    IOCHK
        ana     m
        mov     c, a
        lxi     h, ioFlag
        dad     d
        mov     a, m
        ora     c
        mov     c, a
        call    IOSET
        ret
; End of function SetDev

; ---------------------------------------------------------------------------
read_conn:      db 46h          ; all below are junk filled
read_buffer:    dw 4346h
read_count:     dw 3739h
read_actual:    dw 0D35h

; =============== S U B R O U T I N E =======================================


Read:
        lxi     h, read_count
        mov     m, c
        inx     h
        mov     m, b
        inx     h
        mov     m, e
        inx     h
        mov     m, d
        xchg
        mvi     m, 0            ; actual = 0
        inx     h
        mvi     m, 0
        lxi     h, read_conn
        mov     c, m
        call    SetTab
        call    SetDev
        lhld    device$p
        mov     c, m
        mvi     a, 1
        sub     c
        jc      loc_FF5         ; jmp if not file
        lda     read_conn
        sta     fio_conn
        lhld    read_buffer
        shld    fio_buffer
        lhld    read_count
        shld    fio_count
        lhld    read_actual
        mov     c, l            ; bc = actual$p
        mov     b, h
        mvi     e, 0
        call    Fio
        ret
; ---------------------------------------------------------------------------

loc_FF5:                                ; while actual != cnt
        lhld    read_actual
        mov     a, m
        inx     h
        mov     b, m
        lhld    read_count
        sub     l
        mov     c, a
        mov     a, b
        sbb     h
        ora     c
        rz
        lhld    device$p
        mov     a, m
        cpi     8
        jnc     loc_1017        ; jmp device >= 8
        call    CI              ; get from CI
        lhld    read_buffer
        mov     m, a
        jmp     loc_101F
; ---------------------------------------------------------------------------

loc_1017:                               ; get from RI
        call    RI
        lhld    read_buffer
        mov     m, a
        rc                      ; return if carry set

loc_101F:                               ; advance buffer & increase actual
        lhld    read_buffer
        inx     h
        shld    read_buffer
        lhld    read_actual
        mov     e, m
        inx     h
        mov     d, m
        inx     d
        mov     m, d
        dcx     h
        mov     m, e
        jmp     loc_FF5
; End of function Read

; ---------------------------------------------------------------------------
write_conn:     db 36h          ; write_conn to write_actual are junk filled
write_buffer:   dw 3930h
write_count:    dw 4443h
write_actual:   dw 3139h
writeCase:      db     0,    0, 0FFh,    1, 0FFh,    1, 0FFh,    1
                db  0FFh, 0FFh, 0FFh, 0FFh,    2,    2,    2,    2
                db     3,    3

; =============== S U B R O U T I N E =======================================


Write:
        lxi     h, write_buffer
        mov     m, c
        inx     h
        mov     m, b
        inx     h
        mov     m, e
        inx     h
        mov     m, d
        lxi     h, write_conn
        mov     c, m
        call    SetTab
        lhld    close$p
        mov     a, m
        rrc
        mvi     c, CANT$WRITE
        cc      RetUsr          ; error if read mode
        call    SetDev

loc_106A:                               ; while bytes to write
        lhld    write_count
        mov     a, l
        ora     h
        rz
        lhld    device$p
        mov     e, m
        mvi     d, 0
        lxi     h, writeCase    ; note read only skipped above
        dad     d
        mov     l, m
        mvi     h, 0
        dad     h
        lxi     d, off_1087
        dad     d
        mov     e, m
        inx     h
        mov     d, m
        xchg
        pchl
; ---------------------------------------------------------------------------
off_1087:
        dw wrFile
        dw wrConsole
        dw wrPunch
        dw wrList
; ---------------------------------------------------------------------------

wrFile:                                 ; file I/O
        lda     write_conn
        sta     fio_conn
        lhld    write_buffer
        shld    fio_buffer
        lhld    write_count
        shld    fio_count
        lxi     b, write_actual
        mvi     e, 0FFh
        call    Fio
        ret
; ---------------------------------------------------------------------------

wrConsole:                              ; console out
        lhld    write_buffer
        mov     c, m
        call    CO
        jmp     loc_10C8
; ---------------------------------------------------------------------------

wrPunch:                                ; punch out
        lhld    write_buffer
        mov     c, m
        call    PO
        jmp     loc_10C8
; ---------------------------------------------------------------------------

wrList:                                 ; list out
        lhld    write_buffer
        mov     c, m
        call    LO
        jmp     loc_10C8
; ---------------------------------------------------------------------------

loc_10C8:                               ; one less to process
        lhld    write_count
        dcx     h
        shld    write_count
        lhld    write_buffer    ; advance to next byte
        inx     h
        shld    write_buffer
        jmp     loc_106A
; ---------------------------------------------------------------------------
        ret
; End of function Write

; ---------------------------------------------------------------------------
edit_buffer:    dw 3330h        ; all below except edit_specialChars are junk filled
edit_count:     db  32h
edit_actual:    dw 3431h
edit_bytes:
        db 0Ah, '09097E1221541634C2A7162AF9B8', 0Dh, 0Ah, 7Fh
        db ' :1016C500067E323310015113111000CD4C10CD643C', 0Dh, 0Ah, 7Fh
        db ' :0216D50013C937', 0Dh, 0Ah, 7Fh
        db ' :1016DA0021D71671215113'
edit_char:      db 33h
edit_i:         db 36h
edit_selector:  db 46h
edit_specialChars:
        db    0                ; null
        db  7Fh                ; del key
        db  1Ah                ; control Z
        db  12h                ; control R
        db  18h                ; control X
        db  0Dh                ; CR
        db  0Ah                ; LF
        db  1Bh                ; ESC

; =============== S U B R O U T I N E =======================================


Edit:
        lxi     h, edit_count
        mov     m, c
        inx     h
        mov     m, e
        inx     h
        mov     m, d
        call    ResetConsoleDev

loc_116F:                               ; while inMode
        lda     edit_inMode
        rrc
        jnc     loc_12B0
        call    CI
        ani     7Fh
        sta     edit_char
        cpi     0Ah             ; skip putting \n in buffer
        jz      loc_1193
        lxi     h, edit_lptr    ; save & echo the character
        mov     c, m
        mvi     b, 0
        lxi     h, edit_bytes
        dad     b
        xchg
        mov     c, a
        stax    d
        call    CO

loc_1193:                               ; check for special characters
        lxi     h, edit_selector
        mvi     m, 0
        lxi     b, 0

loc_119B:
        mvi     a, 7
        sub     c
        jc      loc_11B4
        lxi     h, edit_specialChars
        dad     b
        lda     edit_char
        cmp     m
        jnz     loc_11B0
        mov     a, c
        sta     edit_selector

loc_11B0:
        inr     c
        jnz     loc_119B

loc_11B4:
        lhld    edit_selector
        mvi     h, 0
        dad     h
        lxi     b, off_11C3
        dad     b
        mov     e, m
        inx     h
        mov     d, m
        xchg
        pchl
; ---------------------------------------------------------------------------
off_11C3:
        dw edit_normal
        dw edit_rubout
        dw edit_controlZ
        dw edit_controlR
        dw edit_controlX
        dw edit_CR
        dw edit_LF
        dw edit_escape
; ---------------------------------------------------------------------------

edit_normal:                            ; normal just bump the character count
        lxi     h, edit_lptr
        inr     m
        jmp     loc_12A4
; ---------------------------------------------------------------------------

edit_rubout:                            ; ring bell if already at begining of line
        lxi     h, edit_lptr
        mov     a, m
        ora     a
        jz      loc_11F1
        dcr     m               ; echo the previous char
        mov     c, m
        mvi     b, 0
        lxi     h, edit_bytes
        dad     b
        mov     c, m
        call    CO
        jmp     loc_12A4
; ---------------------------------------------------------------------------

loc_11F1:                               ; ring the bell
        mvi     c, 7
        call    CO
        jmp     loc_12A4
; ---------------------------------------------------------------------------

edit_controlZ:                          ; actual = 0
        lhld    edit_actual
        mvi     m, 0
        inx     h
        mvi     m, 0
        lxi     h, edit_lptr    ; reset lptr & prime in$mode for next time
        mvi     m, 0
        inx     h
        mvi     m, 0FFh
        mvi     c, 0Dh          ; echo cr/lf
        call    CO
        mvi     c, 0Ah
        call    CO
        ret
; ---------------------------------------------------------------------------

edit_controlR:                          ; crlf
        mvi     c, 0Dh
        call    CO
        mvi     c, 0Ah
        call    CO
        lda     edit_lptr       ; if we have characters re echo the line
        ora     a
        jz      loc_12A4
        lxi     h, edit_i       ; retype the whole line
        mvi     m, 0

loc_122A:
        lda     edit_lptr
        dcr     a
        lxi     h, edit_i
        sub     m
        jc      loc_1247
        mov     c, m
        mvi     b, 0
        lxi     h, edit_bytes
        dad     b
        mov     c, m
        call    CO
        lxi     h, edit_i
        inr     m
        jnz     loc_122A

loc_1247:
        jmp     loc_12A4
; ---------------------------------------------------------------------------

edit_controlX:                          ; reset inptr, will remain in in$mode
        lxi     h, edit_lptr
        mvi     m, 0
        mvi     c, '#'          ; echo # CR, LF
        call    CO
        mvi     c, 0Dh
        call    CO
        mvi     c, 0Ah
        call    CO
        jmp     loc_12A4
; ---------------------------------------------------------------------------

edit_CR:                                ; append the LF
        lxi     h, edit_lptr
        mov     c, m
        inr     c
        mvi     b, 0
        lxi     h, edit_bytes
        dad     b
        mvi     m, 0Ah
        mov     c, m
        call    CO
        call    Rescan          ; set lptr to 0 and clear in$mode
        jmp     loc_12A4
; ---------------------------------------------------------------------------

edit_LF:                                ; ignore if at start of line
        lda     edit_lptr
        ora     a
        jz      loc_12A4
        mov     c, a
        mvi     b, 0
        lxi     h, edit_bytes
        dad     b
        lda     edit_char       ; append and write the LF
        mov     m, a
        mov     c, a
        call    CO
        call    Rescan          ; set lptr = 0 & clear in$mode
        jmp     loc_12A4
; ---------------------------------------------------------------------------

edit_escape:                            ; echo NULL, $ and rescan
        mvi     c, 0
        call    CO
        mvi     c, '$'
        call    CO
        call    Rescan          ; set lptr = 0 and clear in$mode
        jmp     loc_12A4
; ---------------------------------------------------------------------------

loc_12A4:                               ; rescan if we have 122 chars
        lxi     h, edit_lptr
        mov     a, m
        sui     122
        cz      Rescan          ; reset lptr = 0 and clear in$mode
        jmp     loc_116F
; ---------------------------------------------------------------------------
; Here when buffer has information and need to pass back to caller

loc_12B0:                               ; actual = 0
        lhld    edit_actual
        mvi     m, 0
        inx     h
        mvi     m, 0

loc_12B8:                               ; while actual < count & not in$mode
        lhld    edit_actual
        mov     a, m
        inx     h
        mov     b, m
        lxi     h, edit_count
        sub     m
        mov     a, b
        sbi     0
        sbb     a
        mov     c, a
        lda     edit_inMode
        cma
        ana     c
        rrc
        rnc
        lda     edit_lptr
        cpi     122             ; if lptr < 122 // max line length
        jnc     loc_1316
        lhld    edit_actual
        mov     c, m
        inx     h
        mov     b, m
        lhld    edit_buffer
        dad     b
        push    h
        mov     c, a
        mvi     b, 0            ; buffer[actual] = bytes[lptr]
        lxi     h, edit_bytes
        dad     b
        mov     a, m
        pop     h
        mov     m, a
        mov     c, a
        sui     0Ah             ; if bytes[lptr] == LF or bytes[lptr] == ESC
        sui     1
        sbb     a
        mov     b, a
        mov     a, c
        sui     1Bh
        sui     1
        sbb     a
        ora     b
        rrc
        jnc     loc_1305
        lxi     h, edit_inMode  ; set in$mode and lptr = 255
        mvi     m, 0FFh         ; reset for in$mode and set lptr = 255
        dcx     h
        mvi     m, 0FFh

loc_1305:                               ; actual++
        lhld    edit_actual
        mov     c, m
        inx     h
        mov     b, m
        inx     b
        mov     m, b
        dcx     h
        mov     m, c
        lxi     h, edit_lptr    ; lptr++
        inr     m
        jmp     loc_12B8
; ---------------------------------------------------------------------------

loc_1316:                               ; set inMode and clear lptr
        lxi     h, edit_inMode
        mvi     m, 0FFh
        dcx     h
        mvi     m, 0
        jmp     loc_12B8
; End of function Edit

; =============== S U B R O U T I N E =======================================


Rewind:
        lhld    byteNo$p
        mvi     m, 80h
        lhld    dptr$p
        mvi     m, 1
        lhld    blkNo$p
        mvi     m, 0
        inx     h
        mvi     m, 0
        lhld    dAddr$p
        mvi     m, 0
        inx     h
        mvi     m, 0
        lhld    l1Addr$p
        mov     c, m
        inx     h
        mov     b, m
        lhld    lAddr$p
        mov     m, c
        inx     h
        mov     m, b
        lhld    lbuf$p
        mov     e, m
        inx     h
        mov     d, m
        call    RdSec
        ret
; End of function Rewind

; ---------------------------------------------------------------------------
direct$empty:   db 45h          ; all below are junk filled
direct$file:    db '23462AFD0'
direct$attrib:  db 36h
direct$EOF$count:db 35h
direct$blk:     dw 3245h
direct$hdr$blk: dw 3533h
rcount:         dw 4336h
direct$ino:     db 44h

; =============== S U B R O U T I N E =======================================


Sync:
        lhld    dAddr$p         ; write the directory sector
        mov     c, m
        inx     h
        mov     b, m
        lhld    dbuf$p
        mov     e, m
        inx     h
        mov     d, m
        call    WrSec
        ret
; End of function Sync


; =============== S U B R O U T I N E =======================================


RdDir:
        lhld    device$p
        mov     a, m
        sta     read_conn
        lxi     h, direct$empty
        shld    read_buffer
        lxi     b, 16
        lxi     d, rcount
        call    Read
        ret
; End of function RdDir

; ---------------------------------------------------------------------------
search_fn:      dw 4533h        ; all below are junk filled
serach_i:       db 30h
search_aft:     db 31h
search_dnum:    db 32h
search_retval:  db 31h

; =============== S U B R O U T I N E =======================================


Search:
        lxi     h, search_fn
        mov     m, c
        inx     h
        mov     m, b
        lhld    device$p        ; dbuf[tmpAft = cur$device] = GetBuf();
        mov     a, m
        sta     search_aft
        mov     c, a
        mvi     b, 0
        lxi     h, dbuf
        dad     b
        dad     b
        push    h
        call    GetBuf
        pop     h
        mov     m, a
        inx     h
        mov     m, b
        lxi     h, search_aft   ; lbuf[tmpAft] = GetBuf();
        mov     c, m
        mvi     b, 0
        lxi     h, lbuf
        dad     b
        dad     b
        push    h
        call    GetBuf
        pop     h
        mov     m, a
        inx     h
        mov     m, b
        lxi     h, search_aft
        mov     c, m
        call    SetTab          ; setTab(tmpAft);
        call    Rewind
        lxi     h, direct$ino
        mvi     m, 0FFh
        lxi     h, search_dnum
        mvi     m, 0
        lxi     h, 1
        shld    rcount
        lxi     h, direct$empty
        mvi     m, 0
        lxi     h, search_retval
        mvi     m, 0

loc_13E5:                               ; while rcount <> 0 and direct$empty <> 7fh
        lxi     h, rcount
        mov     a, m
        inx     h
        mov     b, m
        ora     b
        jz      loc_144C
        lda     direct$empty
        sui     7Fh
        jz      loc_144C
        call    RdDir
        lda     direct$empty
        rrc
        jnc     loc_1411        ; jmp if in use
        lxi     h, direct$ino
        mov     a, m
        cpi     0FFh
        jnz     loc_1445        ; set direct$I$No if not already set
        lda     search_dnum     ; save this slot
        mov     m, a
        jmp     loc_1445
; ---------------------------------------------------------------------------

loc_1411:                               ; check the name & extent for a match
        lxi     h, serach_i
        mvi     m, 1

loc_1416:
        mvi     a, 9
        cmp     m
        jc      loc_1434
        mov     c, m
        mvi     b, 0
        lhld    search_fn
        dad     b
        mov     a, m
        dcr     c
        lxi     h, direct$file
        dad     b
        sub     m
        jnz     loc_1445        ; jmp if different
        lxi     h, serach_i
        inr     m
        jnz     loc_1416

loc_1434:                               ; match found
        lda     search_dnum
        sta     direct$ino      ; save the direct$I$No
        lxi     h, search_retval ; set retVal true
        mvi     m, 0FFh
        lxi     h, 0
        shld    rcount          ; clear rcound so we will exit

loc_1445:                               ; try next slot
        lxi     h, search_dnum
        inr     m
        jmp     loc_13E5
; ---------------------------------------------------------------------------

loc_144C:
        lda     direct$ino
        cpi     0FFh
        jz      loc_1473        ; jmp if not found
        sta     serach_i        ; save the direct$I$NO
        call    Rewind          ; rewind the directory file

loc_145A:                               ; step through to position at the correct entry
        lxi     h, serach_i
        mov     a, m
        sui     0FFh
        jz      loc_146C
        push    h
        call    RdDir
        pop     h
        dcr     m
        jmp     loc_145A
; ---------------------------------------------------------------------------

loc_146C:                               ; back of byteNo to start of the directory entry
        lhld    byteNo$p
        mov     a, m
        sui     10h
        mov     m, a

loc_1473:
        lhld    dbuf$p
        mov     c, m
        inx     h
        mov     b, m
        call    FreBuf          ; free the working buffer
        lhld    lbuf$p          ; and associated links buffer
        mov     c, m
        inx     h
        mov     b, m
        call    FreBuf
        lda     search_retval   ; return the outcome
        ret
; End of function Search

; ---------------------------------------------------------------------------
attrib_name:    dw 3639h        ; all below except attrib_mask are junk filled
attrib_swid:    db  43h ; C
attrib_value:   db 0Dh
attrib_mask:    db INVISIBLE
        db SYSTEM
        db WRITEP
        db FORMAT

; =============== S U B R O U T I N E =======================================


Attrib:
        lxi     h, attrib_swid
        mov     m, c
        inx     h
        mov     m, e
        lxi     h, attrib_name
        mov     c, m
        inx     h
        mov     b, m
        lxi     d, pn
        call    GetFn           ; GetNn(filename, .pn)
        mvi     a, 1
        lxi     h, pn           ; error if not file
        sub     m
        mvi     c, BAD$PATH
        cc      RetUsr
        lxi     h, pn           ; set up the file pointers
        mov     c, m
        call    SetTab
        lxi     b, pn           ; find the file
        call    Search
        rrc
        mvi     c, NO$SUCH$FILE ; not found
        cnc     RetUsr
        lxi     h, attrib_swid  ; set the relevant attribute
        mov     c, m
        mvi     b, 0
        lxi     h, attrib_mask
        dad     b
        mov     a, m
        lxi     h, direct$attrib
        ora     m
        mov     m, a
        lda     attrib_value
        rrc
        jc      loc_14E9        ; jmp if was setting
        lxi     h, attrib_swid  ; clear the relevant attibute
        mov     c, m
        mvi     b, 0
        lxi     h, attrib_mask
        dad     b
        mov     a, m
        cma
        lxi     h, direct$attrib
        ana     m
        mov     m, a

loc_14E9:                        ; write back the directory entry
        lda     pn
        sta     write_conn
        lxi     b, direct$empty
        lxi     d, 10h
        call    Write
        call    Sync            ; and sync
        ret
; End of function Attrib

; ---------------------------------------------------------------------------
close_conn:     db 33h          ; junk filled
dirCnt:         db 43h          ;     "
close_crlf:     db 0Dh, 0Ah
zero:           db 0
closeCase:      db     0,    0,    1,    2,    1,    2,    1,    2
                db     1,    1,    1,    1,    3,    3,    3,    3
                db     4,    4

; =============== S U B R O U T I N E =======================================


Close:
        lxi     h, close_conn
        mov     m, c
        call    SetTab
        lhld    close$p         ; check if already closed
        mov     a, m
        rrc
        rc
        lhld    device$p
        mov     c, m
        mvi     b, 0
        lxi     h, closeCase
        dad     b
        mov     l, m
        mvi     h, 0
        dad     h
        lxi     b, off_1537
        dad     b
        mov     e, m
        inx     h
        mov     d, m
        xchg
        pchl
; ---------------------------------------------------------------------------
off_1537:       dw closeDisk
        dw closeTrivial        ; console type input
        dw closeCo
        dw closePo
        dw closeTrivial        ; line printer
; ---------------------------------------------------------------------------

closeDisk:                              ; disk close
        lhld    lbuf$p
        mov     c, m
        inx     h
        mov     b, m
        call    FreBuf
        lhld    dbuf$p
        mov     c, m
        inx     h
        mov     b, m
        call    FreBuf
        lhld    access$p
        mov     a, m
        sui     2
        jnz     closeTrivial    ; jmp if file was in read mode
        lhld    lAddr$p
        mov     c, m
        inx     h
        mov     b, m
        lhld    lbuf$p
        mov     e, m
        inx     h
        mov     d, m
        call    WrSec
        lhld    byteNo$p
        mov     a, m
        sui     80h
        jz      loc_1583
        lhld    dAddr$p
        mov     c, m
        inx     h
        mov     b, m
        lhld    dbuf$p
        mov     e, m
        inx     h
        mov     d, m
        call    WrSec

loc_1583:                               ; flush the bit map if necessary
        call    RwMap
        lhld    device$p
        mov     c, m
        mvi     b, 0
        lxi     h, dbuf         ; update dbuf
        dad     b
        dad     b
        xchg
        lhld    dbuf$p
        mov     a, m
        inx     h
        stax    d
        inx     d
        mov     a, m
        stax    d
        lhld    device$p
        mov     c, m
        mvi     b, 0
        lxi     h, lbuf         ; update lbuf
        dad     b
        dad     b
        xchg
        lhld    lbuf$p
        mov     a, m
        inx     h
        stax    d
        inx     d
        mov     a, m
        stax    d
        lhld    dnum$p
        mov     a, m
        sta     dirCnt
        lhld    device$p
        mov     c, m
        call    SetTab
        call    Rewind

loc_15C1:                               ; seek to the directory location
        lxi     h, dirCnt
        mov     a, m
        sui     0FFh
        jz      loc_15D3
        push    h
        call    RdDir
        pop     h
        dcr     m
        jmp     loc_15C1
; ---------------------------------------------------------------------------

loc_15D3:                               ; back up to start of dir entry
        lhld    byteNo$p
        mov     a, m
        sui     10h
        mov     m, a
        lxi     h, close_conn   ; set the directory entry's eof for this conn
        mov     c, m
        mvi     b, 0
        lxi     h, byteNo
        dad     b
        mov     a, m
        sta     direct$EOF$count
        lxi     h, close_conn   ; and blk entry
        mov     c, m
        mvi     b, 0
        lxi     h, blkNo
        dad     b
        dad     b
        mov     a, m
        inx     h
        mov     b, m
        lxi     h, direct$blk
        mov     m, a
        inx     h
        mov     m, b
        lhld    device$p        ; and write back the dir entry
        mov     a, m
        sta     write_conn
        lxi     b, direct$empty
        lxi     d, 10h
        call    Write
        call    Sync
        jmp     closeTrivial
; ---------------------------------------------------------------------------

closeCo:                                ; console out devices
        lda     close_conn
        sta     write_conn      ; write cr lf
        lxi     b, close_crlf     ; "\r\n"
        lxi     d, 2
        call    Write
        jmp     closeTrivial
; ---------------------------------------------------------------------------

closePo:                                ; write 120 null trailer
        lxi     h, dirCnt
        mvi     m, 0

loc_1629:
        mvi     a, 119
        sub     m
        jc      closeTrivial
        dcx     h
        mov     a, m
        sta     write_conn
        lxi     b, zero
        lxi     d, 1
        call    Write
        lxi     h, dirCnt
        inr     m
        jnz     loc_1629
        jmp     closeTrivial
; ---------------------------------------------------------------------------

closeTrivial:                           ; mark the device as closed
        lxi     h, close_conn
        mov     c, m
        mvi     b, 0
        lxi     h, closed
        dad     b
        mvi     m, 0FFh
        ret
; End of function Close
; ---------------------------------------------------------------------------
rename_i: db  46h               ; junk filled

; =============== S U B R O U T I N E =======================================


Rename:
        push    d               ; bc = oldname, de = newname
        lxi     d, pn2
        call    GetFn           ; getfn(oldname, .pn2)
        pop     b
        lxi     d, pn
        call    GetFn           ; getfn(newname, .pn)
        mvi     a, 1            ; check if disk file
        lxi     h, pn
        sub     m
        mvi     c, NON$DISK$FILE
        cc      RetUsr
        lda     pn2             ; check if same disk
        sub     m
        mvi     c, DIFFERENT$DISK
        cnz     RetUsr
        lxi     h, pn
        mov     c, m
        call    SetTab
        lxi     b, pn
        call    Search          ; search for newname
        push    psw             ; psw (already$exists)
        lxi     b, pn2
        call    Search          ; search for oldname
        rrc
        mvi     c, NO$SUCH$FILE
        cnc     RetUsr
        lda     direct$attrib   ; check not write protected
        ani     WRITEP or FORMAT
        mvi     c, WRITE$PROTECT
        cnz     RetUsr
        pop     psw             ; recover status of looking for newname
        rrc
        mvi     c, MULTIDEFINED
        cc      RetUsr
        lxi     h, rename_i
        mvi     m, 0

loc_16A7:
        mvi     a, 8
        sub     m
        jc      loc_16C3
        mov     c, m
        mvi     b, 0
        lxi     h, direct$file  ; copy new name to the old name directory entry found with 2nd search
        dad     b
        xchg
        inx     b
        lxi     h, pn
        dad     b
        mov     a, m
        stax    d
        lxi     h, rename_i
        inr     m
        jnz     loc_16A7

loc_16C3:                               ; update the directory and flush to disk
        lhld    device$p
        mov     a, m
        sta     write_conn
        lxi     b, direct$empty
        lxi     d, 16
        call    Write
        call    Sync
        ret
; End of function Rename

; ---------------------------------------------------------------------------
scratch_drive:  db 30h          ; junk filled
scratch_linkbuf$p:dw 3739h
; =============== S U B R O U T I N E =======================================


Scratch:
        lxi     h, scratch_drive
        mov     m, c
        lxi     h, direct$empty ; mark directory entry as free
        mvi     m, 0FFh
        call    SetTab          ; c already holds drive so set up file pointers for this
        lda     scratch_drive
        sta     write_conn
        lxi     b, direct$empty
        lxi     d, 10h
        call    Write           ; write the directory entry
        lxi     h, xio_Cmd
        mvi     m, DCMD$WRITE
        lda     scratch_drive
        sta     xio_Drv
        lhld    dAddr$p
        mov     c, m
        inx     h
        mov     b, m
        lhld    dbuf$p
        mov     e, m
        inx     h
        mov     d, m
        call    Xio             ; xio(DCMD$WRITE, scratch_drive, cur$daddr, cur$dbuf)
        lhld    byteNo$p        ; backup to start of directory entry
        mov     a, m
        sui     10h
        mov     m, a
        lhld    dbuf$p
        mov     e, m
        inx     h
        mov     d, m
        xchg
        shld    scratch_linkbuf$p ; linkbuf$p = cur$dbuf

loc_1720:                               ; while direct$hdr$blk <> 0
        lxi     h, direct$hdr$blk
        mov     a, m
        mov     e, a
        inx     h
        ora     m
        mov     d, m
        jz      loc_177C
        lxi     h, scratch_drive
        mov     c, m
        call    Dealloc         ; Dealloc(drive, direct$hdr$blk)
        lxi     h, xio_Cmd
        mvi     m, 4
        lda     scratch_drive
        sta     xio_Drv
        lxi     h, direct$hdr$blk
        mov     c, m
        inx     h
        mov     b, m
        lhld    scratch_linkbuf$p
        xchg
        call    Xio
        lhld    scratch_linkbuf$p
        inx     h
        inx     h
        mov     e, m
        inx     h
        mov     d, m
        xchg
        shld    direct$hdr$blk
        lxi     b, 2

loc_1759:
        mvi     a, 3Fh ; '?'
        sub     c
        jc      loc_1720
        lhld    scratch_linkbuf$p
        dad     b
        dad     b
        mov     a, m
        mov     e, m
        inx     h
        ora     m
        mov     d, m
        jz      loc_1775
        push    b
        lxi     h, scratch_drive
        mov     c, m
        call    Dealloc
        pop     b

loc_1775:
        inr     c
        jnz     loc_1759
        jmp     loc_1720
; ---------------------------------------------------------------------------

loc_177C:
        lxi     h, xio_Cmd
        mvi     m, 4
        lda     scratch_drive
        sta     xio_Drv
        lhld    dAddr$p
        mov     c, m
        inx     h
        mov     b, m
        lhld    dbuf$p
        mov     e, m
        inx     h
        mov     d, m
        call    Xio
        call    RwMap
        ret
; End of function Scratch


; =============== S U B R O U T I N E =======================================


Delete:
        lxi     d, pn           ; check the name
        call    GetFn
        mvi     a, 1            ; error if not disk file
        lxi     h, pn
        sub     m
        mvi     c, NON$DISK$FILE
        cc      RetUsr
        mov     c, m
        call    SetTab          ; set up the pointer to the file info
        lxi     b, pn           ; find on disk
        call    Search
        cma
        rrc
        mvi     c, NO$SUCH$FILE ; error if not foundj
        cc      RetUsr
        lda     direct$attrib   ; or write protected
        ani     WRITEP or FORMAT
        mvi     c, WRITE$PROTECT
        cnz     RetUsr
        lxi     b, 2            ; check if file is currently open

loc_17C9:
        mvi     a, 6
        sub     c
        jc      loc_17FB
        lxi     h, closed
        dad     b
        mov     a, m
        cma
        lxi     h, device       ; device match
        dad     b
        mov     e, a
        lda     pn
        sub     m
        sui     1
        sbb     a
        ana     e
        mov     e, a
        lxi     h, dnum         ; and direc$ino matches
        dad     b
        lda     direct$ino
        sub     m
        sui     1
        sbb     a
        ana     e
        rrc
        push    b
        mvi     c, CANT$DELETE
        cc      RetUsr
        pop     b
        inr     c
        jnz     loc_17C9

loc_17FB:                               ; clean out the file info
        lxi     h, pn
        mov     c, m
        call    Scratch
        ret
; End of function Delete
; ---------------------------------------------------------------------------
access$support: db     3,    3,    1,    2,    1,    2,    1,    2
                db     1,    1,    1,    1,    2,    2,    2,    2
                db     2,    2,    1,    2
open_access:    db 41h          ; junk filled
open_aft:       db 30h          ;     "
open_found:     db 37h          ;     "
open_zero:      db 0

; =============== S U B R O U T I N E =======================================


Open:

; FUNCTION CHUNK AT 183B SIZE 00000202 BYTES

        push    b               ; open path
        lxi     h, open_access  ; save the open access
        mov     m, e
        jmp     loc_183B
; End of function Open


; =============== S U B R O U T I N E =======================================


Free3:
        push    b               ; bc = errNum
        lhld    dbuf$p
        mov     c, m
        inx     h
        mov     b, m
        call    FreBuf          ; free$buf(cur$dbuf)
        lhld    lbuf$p
        mov     c, m
        inx     h
        mov     b, m
        call    FreBuf          ; free$buf(cur$lbuf)
        pop     b
        call    RetUsr          ; retusr(errnum)
        ret
; End of function Free3

; ---------------------------------------------------------------------------
; START OF FUNCTION CHUNK FOR Open

loc_183B:                               ; access > WRMODE
        mvi     a, WRMODE
        sub     m
        sbb     a
        mov     c, a
        mov     a, m            ; or access == 0
        ora     a
        sui     1
        sbb     a
        ora     c
        rrc
        mvi     c, BAD$ACCESS
        cc      RetUsr          ; error if not RDMODE or WRMODE (doen't support RWMODE as no seek)
        pop     b               ; bc = path
        lxi     d, pn
        call    GetFn
        lxi     h, pn
        mov     a, m
        sui     BBDEV           ; check for BB
        mvi     a, 0FFh         ; conn is 0xff
        rz
        mov     a, m
        sui     0FFh            ; check for bad device
        mvi     c, BAD$DEVICE
        cz      RetUsr
        mov     c, m
        mvi     b, 0
        lxi     h, access$support ; check if device support ths r/w mode
        dad     b
        lda     open_access
        ana     m
        mvi     c, BAD$ACCESS
        cz      RetUsr
        lxi     h, pn
        mov     a, m
        sui     CIDEV           ; check for CI
        mvi     a, 1            ; conn = 1
        rz
        mov     a, m
        sui     CODEV           ; check for CO, conn = 0
        rz
        call    GetAft          ; allocate an aft
        sta     open_aft
        mov     c, a
        call    SetTab          ; set pointers for it
        lda     open_access
        lhld    access$p        ; set the access mode
        mov     m, a
        lxi     h, pn
        mov     c, m            ; save the device
        lhld    device$p
        mov     m, c
        mvi     a, 1
        sub     c
        jc      loc_19DB        ; jmp if not file device
        lxi     b, pn           ; lookup the file
        call    Search
        lxi     h, open_found
        mov     m, a
        dcx     h
        mov     c, m            ; c = aft
        call    SetTab          ; reset the pointers
        lda     open_found
        rrc
        jnc     loc_18ED        ; jmp if new file
        lxi     b, 2            ; check file isn't already open

loc_18B9:
        mvi     a, 6
        sub     c
        jc      loc_18ED
        lxi     h, closed
        dad     b
        mov     a, m
        cma
        rrc
        jnc     loc_18E9
        lxi     h, dnum
        dad     b
        lda     direct$ino      ; ino & device match then already open
        sub     m
        sui     1
        sbb     a
        mov     e, a
        lxi     h, device
        dad     b
        lda     pn
        sub     m
        sui     1
        sbb     a
        ana     e
        rrc
        push    b
        mvi     c, ALREADY$OPEN
        cc      RetUsr
        pop     b

loc_18E9:
        inr     c
        jnz     loc_18B9

loc_18ED:                               ; set the aft values from the directory lookup
        lda     direct$ino
        lhld    dnum$p          ; allocate data and link buffers
        mov     m, a
        call    GetBuf
        lhld    dbuf$p
        mov     m, a
        inx     h
        mov     m, b
        call    GetBuf
        lhld    lbuf$p
        mov     m, a
        inx     h
        mov     m, b
        lda     open_access
        dcr     a
        jnz     loc_1937        ; jmp if write access
        lda     open_found      ; if read and not found then error
        rrc
        mvi     c, NO$SUCH$FILE
        cnc     Free3
        lhld    direct$hdr$blk
        xchg
        lhld    l1Addr$p        ; set first link address
        mov     m, e
        inx     h
        mov     m, d
        lhld    direct$blk      ; and block count
        xchg
        lhld    blkCnt$p
        mov     m, e
        inx     h
        mov     m, d
        lda     direct$EOF$count ; and last sector size
        lhld    lastByte$p
        mov     m, a
        call    Rewind          ; set up rest of data to point at start of the file
        jmp     loc_1A32
; ---------------------------------------------------------------------------

loc_1937:                               ; if writing, check not write protected
        lda     open_found
        rrc
        jnc     loc_1956
        lda     direct$attrib
        ani     84h
        mvi     c, WRITE$PROTECT
        cnz     Free3
        lhld    device$p        ; delete the existing contents
        mov     c, m
        call    Scratch
        lxi     h, open_aft     ; reset the pointer info
        mov     c, m
        call    SetTab

loc_1956:                               ; check we didn't run out of directory space
        lda     direct$ino
        sui     0FFh
        mvi     c, DIRECTORY$FULL
        cz      Free3
        lxi     h, direct$attrib ; setup for initial zero size file
        mvi     m, 0            ; direct$attrib = 0
        inx     h
        mvi     m, 128          ; direct$eof$count = 128
        inx     h
        mvi     m, 0            ; direct$blk = 0
        inx     h
        mvi     m, 0
        lhld    device$p        ; allocate a sector for the header block
        mov     c, m
        call    Allocate
        lxi     h, direct$hdr$blk
        mov     m, a
        inx     h
        mov     m, b
        lhld    l1Addr$p        ; and set l1Addr$p to also point to it
        mov     m, a
        inx     h
        mov     m, b
        lxi     h, direct$empty ; update directory entry to say now used
        mvi     m, 0
        lxi     b, 0            ; copy the filename over

loc_1989:
        mvi     a, 8
        sub     c
        jc      loc_19A0
        lxi     h, direct$file
        dad     b
        xchg
        inx     b
        lxi     h, pn
        dad     b
        dcx     b
        mov     a, m
        stax    d
        inr     c
        jnz     loc_1989

loc_19A0:
        lhld    device$p
        mov     a, m
        sta     write_conn
        lxi     b, direct$empty ; write the directory entry
        lxi     d, 16
        call    Write           ; write the directory info
        call    Sync
        lxi     h, open_aft     ; get up aft pointers and rewind file
        mov     c, m
        call    SetTab
        call    Rewind
        lhld    lbuf$p          ; zero the link buffer
        mov     c, m
        inx     h
        mov     b, m
        call    ClrBuf
        call    RwMap           ; make sure bit map is loaded
        lhld    l1Addr$p        ; write the link buffer
        mov     c, m
        inx     h
        mov     b, m
        lhld    lbuf$p
        mov     e, m
        inx     h
        mov     d, m
        call    WrSec
        jmp     loc_1A32
; ---------------------------------------------------------------------------

loc_19DB:                               ; check for non file device already open
        lxi     b, 2

loc_19DE:
        mvi     a, 6
        sub     c
        jc      loc_1A02
        lxi     h, closed
        dad     b
        mov     a, m
        cma
        rrc
        jnc     loc_19FE
        lhld    device$p
        mov     a, m
        lxi     h, device
        dad     b
        sub     m
        push    b
        mvi     c, ALREADY$OPEN
        cz      RetUsr
        pop     b

loc_19FE:
        inr     c
        jnz     loc_19DE

loc_1A02:                               ; see if punch device
        lhld    device$p
        mov     a, m
        sui     TPDEV
        sbb     a
        cma
        mov     c, a
        mov     e, m
        mvi     a, P2DEV
        sub     e
        sbb     a
        cma
        ana     c
        rrc
        jnc     loc_1A32
        mvi     c, 120          ; write 120 null leader
        lhld    close$p
        mvi     m, 0

loc_1A1D:
        lda     open_aft
        sta     write_conn
        push    b
        lxi     b, open_zero
        lxi     d, 1
        call    Write
        pop     b
        dcr     c
        jnz     loc_1A1D

loc_1A32:
        lhld    close$p
        mvi     m, 0
        lda     open_aft        ; remove aft bias
        sui     2
        ret
; END OF FUNCTION CHUNK FOR Open
; ---------------------------------------------------------------------------
load_path:      dw 3130h        ; all below are junk filled
load_bias:      dw 4539h
load_retsw:     db 41h
load_entry$p:   dw 3030h
load_len:       dw 4630h
load_addr:      dw 3244h
load_actual:    dw 4546h
load_conn:      db 31h

; =============== S U B R O U T I N E =======================================


Load:
        lxi     h, load_retsw
        mov     m, c
        inx     h
        mov     m, e
        inx     h
        mov     m, d
        lhld    load_path
        mov     b, h
        mov     c, l
        mvi     e, RDMODE
        call    Open
        adi     2               ; convert to external conn
        sta     load_conn
        lxi     h, 1
        shld    load_len        ; force read of first record

loc_1A68:                               ; all done
        lhld    load_len
        mov     a, h
        ora     l
        jz      loc_1ACA
        lda     load_conn       ; read in the header
        sta     read_conn
        lxi     h, load_len
        shld    read_buffer
        lxi     b, 4            ; read length and address
        lxi     d, load_actual
        call    Read
        lhld    load_addr       ; adjust for user specified offset
        xchg
        lhld    load_bias
        dad     d
        shld    load_addr
        lxi     b, EndIS        ; check we are above ISIS itself
        lhld    load_addr
        mov     a, l
        sub     c
        mov     e, a
        mov     a, h
        sbb     b
        mvi     c, ISIS$OVERWRITE
        cc      Err
        lda     load_conn
        sta     read_conn
        lhld    load_addr
        shld    read_buffer
        lhld    load_len
        mov     c, l
        mov     b, h
        lxi     d, load_actual
        call    Read            ; read the actual data into memory
        lhld    load_actual     ; check we read all of the data
        mov     a, l
        mov     b, h
        lhld    load_len
        sub     l
        mov     a, b
        sbb     h
        mvi     c, BAD$LOAD$FORMAT
        cc      Err
        jmp     loc_1A68
; ---------------------------------------------------------------------------

loc_1ACA:                               ; close the file
        lxi     h, load_conn
        mov     c, m
        call    Close
        lda     load_retsw      ; check if just loading
        ora     a
        jnz     loc_1AE3
        lhld    load_addr
        xchg
        lhld    load_entry$p    ; save the found entry point
        mov     m, e
        inx     h
        mov     m, d
        ret
; ---------------------------------------------------------------------------

loc_1AE3:                               ; switch to user stack space
        lhld    usrsp
        sphl
        lda     load_retsw      ; switch == 1 ?
        dcr     a
        jnz     loc_1AF9
        lxi     h, debug        ; clear the debug flag
        mvi     m, 0
        inx     sp
        inx     sp
        lhld    load_addr       ; go execute
        pchl
; ---------------------------------------------------------------------------

loc_1AF9:                               ; code looks a little convoluted
        lxi     h, 0            ; pop h ; lhld entry; push h would be shorter
        dad     sp
        shld    load_entry$p    ; save the current stack
        lhld    load_addr       ; write the entry point at the top of the stack (i.e. return address)
        xchg
        lhld    load_entry$p
        mov     m, e
        inx     h
        mov     m, d
        lxi     h, debug        ; set debug flag
        mvi     m, 0FFh
        jmp     0               ; goto monitor
; End of function Load

; ---------------------------------------------------------------------------
isisCli:        db 'ISIS.CLI '

; =============== S U B R O U T I N E =======================================


Exit:
        mvi     c, 4            ; close files 4, 5, 6

loc_1B1D:
        mvi     a, 6
        sub     c
        jc      loc_1B2C
        push    b
        call    Close
        pop     b
        inr     c
        jnz     loc_1B1D

loc_1B2C:
        lxi     h, mapChange
        mvi     m, 0
        lxi     h, mapDCB + IOINS
        mvi     m, 6
        lxi     h, mapDrv
        mvi     m, 0FFh
        lxi     h, isisCli      ; "ISIS.CLI "
        shld    load_path
        lxi     h, 0
        shld    load_bias
        mvi     c, 1
        lxi     d, 0
        call    Load            ; continue with ISIS.CLI
        ret
; End of function Exit

; ---------------------------------------------------------------------------
bootload: db 80h, DCMD$READ, 26, 0, 1
        dw 3000h               ; boot memory location
; ---------------------------------------------------------------------------
; START OF FUNCTION CHUNK FOR Err

Reboot:                                 ; reset stack
        lxi     h, rbyte
        sphl
        mvi     c, 2            ; close files 2 - 6

loc_1B5D:
        mvi     a, 6
        sub     c
        jc      loc_1B6C
        push    b
        call    Close
        pop     b
        inr     c
        jnz     loc_1B5D

loc_1B6C:
        mvi     c, 0
        lxi     d, bootload
        call    Xeqiopb
        jmp     3000h
; END OF FUNCTION CHUNK FOR Err
; ---------------------------------------------------------------------------
EndIS:
        db 'C602324A1A21010022441A2A441AC0', 0Dh, 0Ah, 7Fh
        db ' :101A6B007CB5CACA1A3A4A1A32AD0F21441A22AEB1', 0Dh, 0Ah, 7Fh
        db ' :101A7B000F01040011481ACDB40F2A461AEB2A3F66', 0Dh, 0Ah, 7Fh
        db ' :101A8B00'
        end 0

