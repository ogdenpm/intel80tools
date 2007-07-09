ISIS=..\..\isis
PLMDIR=..\..\plm80v4
ASMDIR=..\..\plm80v4
OLDPLMDIR=..\..\plm80v3
OVERLAY=6D4Ch
#
# compilation and assembly rules
.SUFFIXES: .plm .pl3
.plm.obj:
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) :F1:PLM80 $< code
	@if not exist $*.obj exit /b 1

.pl3.obj:
	@SET :F0:=.
	@SET :F1:=$(OLDPLMDIR)
	$(ISIS) :F1:PLM80 $< code
	@find $*.lst "     0 PROGRAM ERROR(S)" >nul || (del $*.obj & exit /b 1)

.asm.obj:
	@SET :F0:=.
	@SET :F1:=$(ASMDIR)
	$(ISIS) :F1:ASM80 $< 
	@find $*.lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $*.obj & exit /b 1)

all: asm80 asm80.ov0 asm80.ov1 asm80.ov2 asm80.ov3 asm80.ov4 asm80.ov5

asm80: base.abs asmov0.abs asmov1.abs asmov2.abs asmov3.abs keywrd.abs 
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) < <<	
:F1:link base.abs,publics(asmov0.abs,asmov1.abs,asmov2.abs),&
asmov3.abs,keywrd.abs to $*.rel &
map print($*.lin)
:F1:locate $*.rel to $@ SYMBOLS MAP PUBLICS print($*.map)&
STACKSIZE(0) PURGE NAME(ASM80)
exit
<<NOKEEP
	del $*.rel
	@find $*.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $*.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)



asm80.ov0: asmov0.abs 
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) < <<	
:F1:locate $** to $@ SYMBOLS MAP PUBLICS print($*.mo0) &
STACKSIZE(0) PURGE NAME(A80O0)
exit
<<NOKEEP
	@(find $*.mo0 "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

asm80.ov1: asmov1.abs 
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) < <<	
:F1:locate $** to $@ SYMBOLS MAP PUBLICS print($*.mo1) &
STACKSIZE(0) PURGE NAME(A80O1)
exit
<<NOKEEP	
	@(find $*.mo1 "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)


asm80.ov2: asmov2.abs 
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) < <<	
:F1:locate $** to $@ SYMBOLS MAP PUBLICS print($*.mo2) &
STACKSIZE(0) PURGE NAME(A80O2)
exit
<<NOKEEP	
	@(find $*.mo2 "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

asm80.ov3: asmov3.abs
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) < <<	
:F1:locate $** to $@ SYMBOLS MAP PUBLICS print($*.mo3) &
STACKSIZE(0) PURGE NAME(A80O3)
exit
<<NOKEEP
	@(find $*.mo3 "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)




OBJS1 =	glb.obj asm801.obj asm82.obj asm83.obj asm84.obj asm85.obj\
	asm851.obj asm86.obj asm861.obj asm87.obj close.obj delete.obj error.obj\
       	exit.obj load.obj open.obj read.obj rescan.obj seek.obj write.obj

base.abs: $(OBJS1)
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) < <<
:F1:link $(OBJS1: =,&
),:f1:plm80.lib to $*.rel print($*.lin)
:F1:locate $*.rel to $@ SYMBOLS MAP PUBLICS STACKSIZE(120) print($*.map)
exit
<<NOKEEP	
	del $*.rel
	@find $*.lin "UNRESOLVED EXTERNAL" >nul || (del $@ & exit /b 1)


keywrd.abs: keywrd.obj memory.obj
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) < <<
:F1:link $(**: =,) to $*.rel map print($*.lin)
:F1:locate $*.rel to $@ SYMBOLS MAP PUBLICS CODE(075A2h) STACKSIZE(0) print($*.map)
exit
<<NOKEEP
	del $*.rel
	@find $*.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $*.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

asmov0.abs: base.abs cprite.obj asmov0.obj
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) < <<
:F1:link publics(base.abs),cprite.obj,asmov0.obj to $*.rel map print($*.la0)
:F1:locate $*.rel to $@ SYMBOLS MAP PUBLICS CODE($(OVERLAY)) STACKSIZE(0) print($*.ma0)
exit
<<NOKEEP
	del $*.rel
	@find $*.la0 "LINK MAP" >nul || (del $@ & exit /b 1)


asmov1.abs: base.abs cprite.obj asmov1.obj
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) < <<
:F1:link publics(base.abs),cprite.obj,asmov1.obj,:F1:plm80.lib &
to $*.rel map print($*.la1)
:F1:locate $*.rel to $@ SYMBOLS MAP PUBLICS CODE($(OVERLAY)) STACKSIZE(0) print($*.ma1)
exit
<<NOKEEP
	del $*.rel
	@find $*.la1 "LINK MAP" >nul || (del $@ & exit /b 1)



asmov2.abs: base.abs cprite.obj asmov2.obj
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) < <<
:F1:link publics(base.abs),cprite.obj,asmov2.obj to $*.rel map print($*.la2)
:F1:locate $*.rel to $@ SYMBOLS MAP PUBLICS CODE($(OVERLAY)) STACKSIZE(0) print($*.ma2)
exit
<<NOKEEP
	del $*.rel
	@find $*.la2 "LINK MAP" >nul || (del $@ & exit /b 1)


asmov3.abs: base.abs cprite.obj asmov3.obj keywrd.abs
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) < <<
:F1:link publics(base.abs),cprite.obj,asmov3.obj,publics(keywrd.abs) &
to $*.rel map print($*.la3)
:F1:locate $*.rel to $@ SYMBOLS MAP PUBLICS CODE($(OVERLAY)) STACKSIZE(0) print($*.ma3)
exit
<<NOKEEP
	del $*.rel
	@find $*.la3 "LINK MAP" >nul || (del $@ & exit /b 1)



clean:
	del *.obj *lst *.map *.abs *.rel *.lin *.*~ *.la? *.ma? *.lo? *.mo? makefile~

vclean: clean
	del asm80 asm80.ov?

verify: asm80 asm80.ov0 asm80.ov1 asm80.ov2 asm80.ov3 asm80.ov4 asm80.ov5
	for %i in ($**) do ..\..\diffbin %i $(PLMDIR)\%i

compare: asm80 asm80.ov0 asm80.ov1 asm80.ov2 asm80.ov3 asm80.ov4 asm80.ov5
	for %i in ($**) do fc /b %i $(PLMDIR)\%i


OBJS4 =	glb4.obj asm41.obj asm42.obj asm87.obj asm43.obj asm44.obj asm45.obj asm46.obj\
	asm47.obj asm48.obj asm49.obj asm4A.obj asm4B.obj asm4C.obj asm4D.obj asm4E.obj\
	close.obj delete.obj error.obj exit.obj\
	load.obj open.obj read.obj rescan.obj\
	seek.obj write.obj

asm80.ov4: $(OBJS4) key4.obj
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) < <<	
:F1:link $(OBJS4: =,&
),:f1:plm80.lib,key4.obj to asmov4.rel map print(asmov4.lin)
:F1:locate asmov4.rel to $@ SYMBOLS NAME(A80O4) PURGE &
MAP PUBLICS code(3780h) STACKSIZE(144) print(asmov4.map)
exit
<<NOKEEP
	@find asmov4.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find asmov4.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)



OBJS5 =	glb5.obj asm51.obj asm52.obj asm87.obj asm82.obj asm83.obj asm55.obj asm84.obj\
	asm57.obj asm86.obj asm861.obj asmov0.obj asmov2.obj asmov1.obj asm5D.obj\
	close.obj delete.obj error.obj exit.obj\
	load.obj open.obj read.obj rescan.obj\
	seek.obj write.obj

asm80.ov5: $(OBJS5) keywrd.obj
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) < <<	
:F1:link $(OBJS5: =,&
),:f1:plm80.lib,keywrd.obj to asmov5.rel map print(asmov5.lin)
:F1:locate asmov5.rel to $@ SYMBOLS NAME(A80O5) PURGE &
MAP PUBLICS code(3780h) STACKSIZE(144) print(asmov5.map)
exit
<<NOKEEP
	@find asmov5.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find asmov5.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)


glb5.obj: glb5.plm global.cmn
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) :F1:PLM80 glb5.plm code
	@if not exist $*.obj exit /b 1

glb.obj: glb.plm global.cmn
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) :F1:PLM80 glb.plm code
	@if not exist $*.obj exit /b 1

glb4.obj: glb4.plm global.cmn
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	$(ISIS) :F1:PLM80 glb4.plm code
	@if not exist $*.obj exit /b 1
