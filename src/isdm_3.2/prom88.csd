;
;  *-*-*  PROM88.CSD  *-*-*
;
;  Format iSDM monitor code and burn into EPROMs to run on 88/188-based boards.
;
;  Invocation: submit prom88(source file)
;
;
delete %0.prm
;
ipps


initialize 86

;
; Format monitor code.  
;
format %0(0F8000H) p
3
1
1
0 to %0.prm

exit
;
; 
; Program the monitor into EPROM by inserting a blank EPROM into the 
; programmer and typing the following command from the IPPS command line:
;
;        initialize 86
;        type 27128
;        1
;        copy %0.prm to prom
;
; The IPPS software will prompt when the first EPROM is finished and the
; second EPROM should be put in the programmer for programming.



