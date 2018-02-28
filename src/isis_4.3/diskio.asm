
	name diskio
	CSEG
	extrn	@P0103	; library routine


; ****************** some notes about this code ********************
; most of the function diskio seems to have been originally been in PLM based on
; the code sequences generated. However two of the ADDRESS variables have been
; initialised to 0 in the final isis.bin file which indicates that this they
; have been set as dw 0, statements in assembler as neither the parameter iopb
; nor the realTK variable could legally be initialisd in PLM
; There also appear to be a few of minor hand crafted code changes
; those to ignoring r$byte and r$type could have been overcome by defining
; different name entry points in the disk2.asm code
; others like noting that the do loop variable is in the hl register at both
; the initialisation and the increment so doesnot need to be loaded for the
; loop test are not detected in plm80 4.0

; In this file I have left in the PLM code and the assembly generated from it
; the PLM code is commented out and the original code is left in upper case
; code that has been optimised out or changed has been commented out with a
; leading ;-, and replacement code has been entered in lower case

; Note the include statements and unrequired declarations have been deleted
; to keep the file reasonably clean

; DECLARE DCB$T LITERALLY 'STRUCTURE (iocw BYTE, ioins BYTE, nsec BYTE, tadr BYTE, sadr BYTE, buf ADDRESS)',
;         TRUE        LITERALLY   '0ffh',
;         FALSE       LITERALLY   '0',
;         DISK$IO$ERROR   LITERALLY   '24',   /* Disk error */       
;         DRIVE$NOT$READY LITERALLY   '30',   /* Drive not ready. */
;         c$dk$sat ADDRESS EXTERNAL,
	extrn	cdksat
;	  errdrv BYTE EXTERNAL;
	extrn	errdrv

; DECLARE vec$0E ADDRESS EXTERNAL
	extrn	vec0E
; abortx: PROCEDURE(errcode) EXTERNAL; DECLARE errcode BYTE; END;
	extrn	abortx
; trap0B: PROCEDURE EXTERNAL; END;
	extrn	trap0B

; dk$stat: PROCEDURE BYTE EXTERNAL; END;
	extrn	dkstat
; r$type: PROCEDURE BYTE EXTERNAL; END;
	extrn	rtype
; r$byte: PROCEDURE BYTE EXTERNAL; END;
	extrn	rbyte
; strt$io: PROCEDURE(iopb) EXTERNAL; DECLARE iopb ADDRESS; END;
	extrn	strtio

; DECLARE  a$dev$p ADDRESS EXTERNAL,
	extrn	adevp
;          a$device BASED a$dev$p BYTE;

; DECLARE CMD$RECAL   LITERALLY   '3';
; DECLARE CMD$READ    LITERALLY   '4';
; DECLARE CMD$WRITE   LITERALLY   '6';
; DECLARE DD$TYPE     LITERALLY   '1',
;         SD$TYPE     LITERALLY	  '2',
;	  ISD$TYPE    LITERALLY   '3',
;	  HD$TYPE     LITERALLY	  '4';


; DECLARE rcal$pb STRUCTURE(iocw BYTE, ioins BYTE, nsec BYTE, tadr BYTE, sadr BYTE)
;		            INITIAL(80h, CMD$RECAL, 1, 0, 0);
rcalpb: db	80h, 3, 1, 0, 0

; DECLARE absdcb DCB$T INITIAL(80h, 0, 1, 0, 0, 0);
absdcb: db	80h, 0, 1, 0, 0
	dw	0
; DECLARE dk$cf$tb(10) BYTE PUBLIC,
	public	dkcftb
dkcftb: ds	10
;         rdy$msk(10) BYTE PUBLIC,
	public rdymsk
rdymsk: ds	10
;         unit$mp(10) BYTE PUBLIC,
	public unitmp
unitmp: ds	10
;         dk$ct$tb(10) BYTE PUBLIC,  /* disk controller table */
	public dkcttb
dkcttb:	ds	10
;         dkcont BYTE PUBLIC,
	public	dkcont
dkcont:	ds	1
;         track BYTE PUBLIC,
	public	track
track:	ds	1
;         sector BYTE PUBLIC,
	public	sector
sector:	ds	1
;	  temp ADDRESS,
;         (temp2, temp1) BYTE;
temp:
temp2:	ds	1
temp1:	ds	1

; DECLARE MAX$RETRIES LITERALLY   '10';
drive:	ds	1
iopb:	dw	0
i:	ds	1
diskHW:	ds	1
realTk:	dw	0	; tell tail sign of assembly code
ival:	ds	1
sddrv:	ds	1
cmd:	ds	1
disk:	ds	1
block:	dw	0
buffer:	dw	0

; diskio: PROCEDURE(drive, iopb) PUBLIC;
	public	diskio
DISKIO:
         LXI     H,IOPB+1H
         MOV     M,D
         DCX     H
         MOV     M,E
         DCX     H
         MOV     M,C
;     DECLARE drive BYTE, iopb ADDRESS;
;     DECLARE dcb BASED iopb DCB$T;
;     DECLARE i BYTE,
;             diskHW BYTE,
;             realTk ADDRESS,
;             ival BYTE,
;             sd$drv BYTE;
;
;     track = dcb.tadr;
         LXI     B,3H
         LHLD    IOPB
         DAD     B
         MOV     A,M
         STA     TRACK
;     sector = dcb.sadr;
;-         LHLD    IOPB		; hand optimisation
;-         INX     B
;-         DAD     B
	inx	h
         MOV     A,M
         STA     SECTOR
;     ival = input(0fch);
         IN      0FCH
         STA     IVAL
;     output(0fch) = ival or 2;
         ORI     2H
         OUT     0FCH
;     dkcont = dk$ct$tb(drive);
         LHLD    DRIVE
         MVI     H,0
         LXI     B,DKCTTB
         DAD     B
         MOV     A,M
         STA     DKCONT
;     sd$drv = FALSE;
         LXI     H,SDDRV
         MVI     M,0H
;     diskHW = dk$cf$tb(drive);
         LHLD    DRIVE
         MVI     H,0
         LXI     B,DKCFTB
         DAD     B
         MOV     A,M
         STA     DISKHW
;     if diskHW = SD$TYPE AND (1 = 1 AND drive) then    /* SD disk (Not ISD) & odd numbered drive */
         SUI     2H
         SUI     1
         SBB     A
         PUSH    PSW 
         MVI     A,1H	; this code seems odd and presumably
         SUI     1H	; was masked by LITERALLY defined names
         SUI     1
         SBB     A
         LXI     H,DRIVE
         ANA     M
         POP     B
         MOV     C,B
         ANA     C
         RAR
         JNC     @1
;     do;
;         sd$drv = TRUE;
         LXI     H,SDDRV
         MVI     M,0FFH
;         dcb.sadr = dcb.sadr OR 20h;     /* set selector flag */
         LXI     B,4H
         LHLD    IOPB
         DAD     B
         MVI     A,20H
         ORA     M
         MOV     M,A
;     end;
@1:
;     if diskHW = 4 then  /* hard disk */
         LDA     DISKHW
         CPI     4H
         JNZ     @2
;     do;
;         realTk = DOUBLE(dcb.tadr) * 2;
         LXI     B,3H
         LHLD    IOPB
         DAD     B
         MOV     C,M
         MVI     B,0
         MOV     H,B
         MOV     L,C
         DAD     H
         SHLD    REALTK
;         if dcb.sadr > 72 then   /* then map to odd track number */
	lxi	b,4	; PLM v4.0 optimised to an increment of BC
         LHLD    IOPB
;-         INX     B
         DAD     B
         MVI     A,48H
         CMP     M
         JNC     @3
;         do;
;             realTk = realTk + 1;
         LHLD    REALTK
         INX     H
         SHLD    REALTK
;             dcb.sadr = dcb.sadr - 72;
         LXI     B,4H
         LHLD    IOPB
         DAD     B
         MOV     A,M
         SUI     48H
         MOV     M,A
;         end;
@3:
;         if dcb.sadr < 37 then
;-         LXI     B,4H		; hand optimised as hl points to dcb.sadr on all paths
;-         LHLD    IOPB
;=         DAD     B
         MOV     A,M
         CPI     25H
         JNC      @4
;             dcb.ioins = dcb.ioins OR 8; /* select head */
         LHLD    IOPB
         INX     H
         MVI     A,8H
         ORA     M
         MOV     M,A
         JMP     @5
@4:
;         else
;             dcb.sadr = dcb.sadr - 36;
         LXI     B,4H
         LHLD    IOPB
         DAD     B
         MOV     A,M
         SUI     24H
         MOV     M,A
@5:
;         if realTk > 255 then
         MVI     A,0FFH
         LXI     H,REALTK
         CALL    @P0103 
         JNC     @6
;             dcb.sadr = dcb.sadr + 80h;  /* track extension bit */
         LXI     B,4H
         LHLD    IOPB
         DAD     B
         MVI     A,80H
         ADD     M
         MOV     M,A
@6:
;         dcb.tadr = realTk;  
         LXI     B,3H
         LHLD    IOPB
         DAD     B
         PUSH    H
         LHLD    REALTK
         XCHG
         POP     H
         MOV     M,E
;     end;
@2:
;     dcb.ioins = dcb.ioins OR unit$mp(drive);
         LHLD    IOPB
         INX     H
         PUSH    H
         LHLD    DRIVE
         MVI     H,0
         LXI     B,UNITMP
         DAD     B
         MOV     A,M
         POP     H
         ORA     M
         MOV     M,A
;     do i = 0 to MAX$RETRIES;    /* 0-10 */
         LXI     H,I
         MVI     M,0H
@10:
         MVI     A,0AH
;-         LXI     H,I	; hand optimised
         CMP     M
         JC      @11
;         do while (dk$stat AND 4) <> 0;
@12:
         CALL    DKSTAT
         ANI     4H
         CPI     0H
         JZ      @13
;             temp1 = r$type; /* assignments removed in asm */
         CALL    RTYPE
;-         STA     TEMP1	; optimised (but non return version possible)
;             temp1 = r$byte;
         CALL    RBYTE
;-         STA     TEMP1	; optimised (but non return version possible)
;         end;
         JMP     @12
@13:
;         if (dk$stat AND rdy$msk(drive)) = 0 then
         CALL    DKSTAT
         LHLD    DRIVE
         MVI     H,0
         LXI     B,RDYMSK
         DAD     B
         ANA     M
         CPI     0H
         JNZ     @7
;             call abortx(DRIVE$NOT$READY);
         MVI     C,1EH
         CALL    ABORTX
@7:
;         call strt$io(iopb);
         LHLD    IOPB
         MOV     B,H
         MOV     C,L
         CALL    STRTIO
;         temp1 = r$type;
         CALL    RTYPE
         STA     TEMP1
;         if (temp2 := r$byte) = 0 then
         CALL    RBYTE
         STA     TEMP2
         CPI     0H
         JNZ     @8
;         do;
;             output(0fch) = ival;
         LDA     IVAL
         OUT     0FCH
;             return;
         RET
;         end;
@8:
;         if sd$drv then
         LDA     SDDRV
         RAR
         JNC     @9
;             rcal$pb.sadr = rcal$pb.sadr OR 20h;
         LDA     RCALPB+4H
         ORI     20H
         STA     RCALPB+4H
@9:
;         rcal$pb.ioins = rcal$pb.ioins OR unit$mp(drive);
         LHLD    DRIVE
         MVI     H,0
         LXI     B,UNITMP
         DAD     B
         LDA     RCALPB+1H
         ORA     M
         STA     RCALPB+1H
;         call strt$io(.rcal$pb);
         LXI     B,RCALPB
         CALL    STRTIO
;     end;
@14:
         LXI     H,I
         INR     M
         JNZ     @10
@11:
;     errDrv = drive;
         LDA     DRIVE
         STA     ERRDRV
;     vec$0E, c$dk$sat, vec$0E = temp2;
         LHLD    TEMP
         SHLD    VEC0E
         SHLD    CDKSAT
;     output(0fch) = ival;
         LDA     IVAL
         OUT     0FCH
;     call trap0B;
         CALL    TRAP0B
;     output(0fch) = ival OR 2;
         LDA     IVAL
         ORI     2H
         OUT     0FCH
;     call abortx(DISK$IO$ERROR);
         MVI     C,18H
         CALL    ABORTX
; end;
         RET

; absio: PROCEDURE(cmd, disk, block$p, buffer)  PUBLIC;
	public	absio
ABSIO:
         LXI     H,BUFFER+1H
         MOV     M,D
         DCX     H
         MOV     M,E
         DCX     H
         MOV     M,B
         DCX     H
         MOV     M,C
         DCX     H
         POP     D       
         POP     B       
         MOV     M,C
         DCX     H
         POP     B       
         MOV     M,C
         PUSH    D       
;     DECLARE (cmd, disk) BYTE, (block$p, buffer) ADDRESS;
;     DECLARE tsadr ADDRESS AT (.absdcb.tadr); /* to allow tadr & sadr write together */
tsadr	equ	absdcb+3
;     DECLARE block BASED block$p;
;     absdcb.ioins = cmd;
abscom:				; major optimisation to share code
         LDA     CMD
         STA     ABSDCB+1H
;     tsadr = block;
         LHLD    BLOCK
;         SHLD    TSADR
	xchg
	lxi	h,tsadr
	mov	m,d
	inx	h
	mov	m,e
;     absdcb.buf = buffer;
         LHLD    BUFFER
         SHLD    ABSDCB+5H
;     call diskio(disk, .absdcb);
         LHLD    DISK
         MOV     C,L
         LXI     D,ABSDCB
         CALL    DISKIO
; end;
         RET
; abs$rd: PROCEDURE(block, buffer) PUBLIC;
	public	absrd
ABSRD:
	mvi	a,4	; CMD$READ
absrw:
	sta	cmd	; store in cmd location for absio
         LXI     H,BUFFER+1H	; stores in parameters for absio
         MOV     M,D
         DCX     H
         MOV     M,E
         DCX     H
         MOV     M,B
         DCX     H
         MOV     M,C
;     DECLARE (block, buffer) ADDRESS;
;     call absio(CMD$READ, a$device, block, buffer);
;-         MVI     C,4H	; put in A reg as part of hand optimisation
;-         PUSH    B	; see ABSRD, ABDWR and storing at absrw
         LHLD    ADEVP
	mov	a,m	; a$device
	sta	disk	; store in the disk parameter for absio
	jmp	abscom	; jump to shared code
;-         MOV     C,M
;-         PUSH    B       ; 2
;-         LHLD    BLOCK
;-         MOV     B,H
;-         MOV     C,L
;-         LHLD    BUFFER
;-         XCHG
;-         CALL    ABSIO
; end;
;-         RET
; abs$wr: PROCEDURE(block, buffer) PUBLIC;
	public	abswr
ABSWR:
	mvi	a,6	; CMD$WRITE
	jmp	absrw	; share code with absrd
;-         LXI     H,BUFFER+1H
;-         MOV     M,D
;-         DCX     H
;-         MOV     M,E
;-         DCX     H
;-         MOV     M,B
;-         DCX     H
;-         MOV     M,C
;     DECLARE (block, buffer) ADDRESS;
;     call absio(CMD$WRITE, a$device, block, buffer);
;-         MVI     C,6H
;-         PUSH    B
;-         LHLD    ADEVP
;-         MOV     C,M
;-         PUSH    B       ; 2
;-         LHLD    BLOCK
;-         MOV     B,H
;-         MOV     C,L
;-         LHLD    BUFFER
;-         XCHG
;-         CALL    ABSIO
; end;
;-         RET
; end;
; eof
	end

