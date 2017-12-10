    public PckTok
    public Isis
    extrn toksiz
    extrn tokst;

    aseg
Isis    equ    40h    ; definition of Isis entry, here to avoid another asm file

; pckTkn - packs the token pointed by tokst, with length toksiz into 4 bytes
; packed version replaces original and toksize set to 4 bytes

    CSEG
PckTok: lhld    tokst    ; pointer to the token to pack
    xchg
    lxi    h, toksiz
    mov    c, m    ; unpacked length
    mvi    m, 4    ; new packed length is 4 bytes
    call pack3    ; pack 3 chars into hl
    push    h
    call pack3
    pop    b
    xchg        ; put the packed data into the first 4 bytes
    dcx    h
    dcx    h
    dcx    h
    mov    m, d
    dcx    h
    mov    m, e
    dcx    h
    mov    m, b
    dcx    h
    mov    m, c
    ret
pack3:
    lxi    h, 0    ; get 3 chars packed into 2 bytes
    mvi    b, 3

L6291:
    push    d    ; pointer to next char
    mov    d, h    ; hl x 40
    mov    e, l
    dad    h
    dad    h
    dad    d
    dad    h
    dad    h
    dad    h
    pop    d    
    call pack1    ; add in next char
    add    l
    mov    l, a
    mvi    a, 0
    adc    h
    mov    h, a
    dcr    b    ; get the 3 chars
    jnz    L6291
    ret

pack1:    ldax    d        ; pick up next character
    inx    d        ; advance for next time
    dcr    c        ; check it is valid
    jm    L62B7
    sui    2Fh        ; /012345678  (0-9)
                ; 9?@ABCDEFG  (10-19)
                ; HIJKLMNOPQ  (20-29)
                ; RSTUVWXYZ[  (30-39)
    cpi    10h        ; digit so return
    rc
    sui    5        ; exclude : ; < = >
    ret

L62B7:
    sub    a        ; 0 no char
    ret

    end
