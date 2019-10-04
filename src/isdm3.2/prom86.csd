;
;  *-*-*  PROM86.CSD  *-*-*
;
;  Format iSDM monitor code and burn into two 27128 EPROMs.
;
;  Invocation: submit prom86(source file,omf type,EPROM start address)
;
;
delete %0.evn, %0.odd
;
ipps


initialize %1

;
; Format monitor code into even and odd addressed bytes.
;
format %0(%2) p
3
2
1
0 to %0.evn
1 to %0.odd

exit
; Program the first EPROM by inserting a blank EPROM into the programmer 
; and typing the following command from the IPPS command line:
;
;        initialize %1
;        type 27128
;        1
;        copy %0.evn to prom
;
; Program the second EPROM by inserting a blank EPROM into the programmer
; and typing the following command from the IPPS command line:
;
;        copy %0.odd to prom

