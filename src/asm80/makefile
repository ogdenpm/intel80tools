ISIS=..\..\thames
V4=../../plm80v4/
V3=../../plm80v3/
TOOLS=../../tools/

LOCATEOPT=MAP PUBLICS SYMBOLS
PEXFILE=asm80.pex
PLMFLAGS=code print(:f2:$(*B).lst) object(:f3:$(*B).obj)
# support command line option to create debug variant
!IFDEF DEBUG
PLMFLAGS=$(PLMFLAGS) debug
LOCFINAL=$(LOCATEOPT)
ODIR=debug
!ELSE
LOCFINAL=$(LOCATEOPT) PURGE
ODIR=release
!ENDIF
F0=./
F2=.\list
F3=$(ODIR)
OVERLAY=6D4Ch

# bulk obj names
BASEOBJS = $(ODIR)\glb.obj $(ODIR)\asm801.obj $(ODIR)\asm82.obj $(ODIR)\asm83.obj\
	$(ODIR)\asm46.obj $(ODIR)\asm85.obj $(ODIR)\asm851.obj $(ODIR)\asm86.obj\
	$(ODIR)\asm861.obj $(ODIR)\pcktkn.obj $(ODIR)\close.obj $(ODIR)\delete.obj\
	$(ODIR)\error.obj $(ODIR)\exit.obj $(ODIR)\load.obj $(ODIR)\open.obj\
	$(ODIR)\read.obj $(ODIR)\rescan.obj $(ODIR)\seek.obj $(ODIR)\write.obj

OBJS4 =	$(ODIR)\glb4.obj $(ODIR)\asm41.obj $(ODIR)\pcktkn.obj $(ODIR)\asm43.obj\
	$(ODIR)\asm44.obj $(ODIR)\asm45.obj $(ODIR)\asm46.obj $(ODIR)\asm47.obj\
	$(ODIR)\asm48.obj $(ODIR)\asm49.obj $(ODIR)\asm4A.obj $(ODIR)\asm4B.obj\
	$(ODIR)\asm4C.obj $(ODIR)\asm4D.obj $(ODIR)\asm4E.obj $(ODIR)\close.obj\
	$(ODIR)\delete.obj $(ODIR)\error.obj $(ODIR)\exit.obj $(ODIR)\load.obj\
	$(ODIR)\open.obj $(ODIR)\read.obj $(ODIR)\rescan.obj $(ODIR)\seek.obj\
	$(ODIR)\write.obj

OBJS5 =	$(ODIR)\glb5.obj $(ODIR)\asm51.obj $(ODIR)\pcktkn.obj $(ODIR)\asm82.obj\
	$(ODIR)\asm83.obj $(ODIR)\asm55.obj $(ODIR)\asm46.obj $(ODIR)\asm57.obj\
	$(ODIR)\asm86.obj $(ODIR)\asm861.obj $(ODIR)\asmov0.obj $(ODIR)\asmov2.obj\
	$(ODIR)\asmov1.obj $(ODIR)\asm5D.obj $(ODIR)\close.obj $(ODIR)\delete.obj\
	$(ODIR)\error.obj $(ODIR)\exit.obj $(ODIR)\load.obj $(ODIR)\open.obj\
	$(ODIR)\read.obj $(ODIR)\rescan.obj $(ODIR)\seek.obj $(ODIR)\write.obj

!IFDEF DEBUG
BASEISIS = $(BASEOBJS:debug\=)
OBJS4ISIS = $(OBJS4:debug\=)
OBJS5ISIS = $(OBJS5:debug\=)
!ELSE
BASEISIS = $(BASEOBJS:release\=)
OBJS4ISIS = $(OBJS4:release\=)
OBJS5ISIS = $(OBJS5:release\=)
!ENDIF

P4SRCS = asm41.plm asm44.plm asm45.plm asm4e.plm asm51.plm asm55.plm asm5d.plm\
	asm801.plm asm83.plm asm851.plm asm861.plm asmov3.plm close.plm delete.plm\
	error.plm exit.plm glb.plm glb4.plm glb5.plm key4.plm keywrd.plm load.plm\
	open.plm read.plm rescan.plm seek.plm write.plm

P3SRCS = asm43.pl3 asm46.pl3 asm47.pl3 asm48.pl3 asm49.pl3 asm4a.pl3 asm4B.pl3 asm4c.pl3\
	asm4d.pl3 asm57.pl3 asm82.pl3 asm85.pl3 asm86.pl3 asmov0.pl3 asmov1.pl3 asmov2.pl3

ASRCS = pcktkn.asm cprite.asm memory.asm

TARGETS = $(ODIR)\asm80 $(ODIR)\asm80.ov0 $(ODIR)\asm80.ov1 $(ODIR)\asm80.ov2 $(ODIR)\asm80.ov3 $(ODIR)\asm80.ov4 $(ODIR)\asm80.ov5

#
# compilation and assembly rules
.SUFFIXES:
.SUFFIXES: .plm .pl3 .asm
.plm{$(ODIR)}.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(ODIR)
	$(ISIS) :F1:PLM80 $< $(PLMFLAGS)
	@if not exist $@ exit /b 1

.pl3{$(ODIR)}.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(ODIR)
	$(ISIS) :F1:PLM80 $< $(PLMFLAGS)
	@find $(F2)\$(*B).lst "     0 PROGRAM ERROR(S)" >nul || (del $@ & exit /b 1)

.plm.ipx:
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)ngenpex $(PEXFILE) $<

.pl3.ipx:
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)ngenpex $(PEXFILE) $<
	
.asm{$(ODIR)}.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(ODIR)
	$(ISIS) :F1:ASM80 $< PRINT(:f2:$(*B).lst) OBJECT(:f3:$(*B).obj)
	@find $(F2)\$(*B).lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $@ & exit /b 1)

# the default rule will force the makefile to be up to date
all: source

all:
	$(MAKE) $(TARGETS)

rebuild: uclean all

.extract: asm80_all.plm
	..\..\unpack.pl
	date /t >.extract


source: .extract $(ASRCS) $(P3SRCS) $(P4SRCS) $(P3SRCS:.pl3=.ipx) $(P4SRCS:.plm=.ipx)

# checking targets
# note if you change these two rules then you should do a nmake all or namke .extract first
# otherwise whilst the build will be ok the old verify/compare rules will be run

verify: all
	for %i in ($(TARGETS)) do $(TOOLS:/=\)diffbin %i $(V4)\%i

# compare doesn't work for debug builds
!IFNDEF DEBUG
compare: all
	for %i in ($(TARGETS)) do fc /b %i $(V4)%i
!ENDIF

# source files generated from shared code
asm43.pl3 asm82.pl3: asm4382.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm43.pl3 $**
	$(TOOLS:/=\)plmpp -sBASE -o asm82.pl3 $**

asm48.pl3 asm86.pl3: asm4886.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm48.pl3 $**
	$(TOOLS:/=\)plmpp -sBASE -o asm86.pl3 $**

asm49.pl3 asm861.plm: asm49861.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm49.pl3 $**
	$(TOOLS:/=\)plmpp -sBASE -o asm861.plm $**

asm44.plm asm83.plm: as4483.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm44.plm $**
	$(TOOLS:/=\)plmpp -sBASE -o asm83.plm $**

asm45.plm asm55.plm asm851.plm: as4555851.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm45.plm $**
	$(TOOLS:/=\)plmpp -sOVL5 -o asm55.plm $**
	$(TOOLS:/=\)plmpp -sBASE -o asm851.plm $**

asm4a.pl3 asmov0.pl3: asm4aov0.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm4a.pl3 $**
	$(TOOLS:/=\)plmpp -sOVL0 -o asmov0.pl3 $**

asm4d.pl3 asmov1.pl3: asm4dov1.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm4d.pl3 $**
	$(TOOLS:/=\)plmpp -sOVL1 -o asmov1.pl3 $**

asm4c.pl3 asmov2.pl3: asm4cov2.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm4c.pl3 $**
	$(TOOLS:/=\)plmpp -sOVL2 -o asmov2.pl3 $**

asm47.pl3 asm57.pl3 asm85.pl3: asm475785.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm47.pl3 $**
	$(TOOLS:/=\)plmpp -sOVL5 -o asm57.pl3 $**
	$(TOOLS:/=\)plmpp -sBASE -o asm85.pl3 $**

asm41.plm asm51.plm asm801.plm: start.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm41.plm $**
	$(TOOLS:/=\)plmpp -sOVL5 -o asm51.plm $**
	$(TOOLS:/=\)plmpp -sBASE -o asm801.plm $**

glb.plm glb4.plm glb5.plm: global.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o glb4.plm $**
	$(TOOLS:/=\)plmpp -sOVL5 -o glb5.plm $**
	$(TOOLS:/=\)plmpp -sBASE -o glb.plm $**

asm4e.plm asm5d.plm asmov3.plm: init.plx
	@SET ISIS_F0=$(F0)
	$(TOOLS:/=\)plmpp -sOVL4 -o asm4e.plm $**
	$(TOOLS:/=\)plmpp -sOVL5 -o asm5d.plm $**
	$(TOOLS:/=\)plmpp -sOVL3 -o asmov3.plm $**

# build rules to make the program and overlays

$(ODIR)\asm80: $(ODIR)\base.abs $(ODIR)\asmov0.abs $(ODIR)\asmov1.abs $(ODIR)\asmov2.abs $(ODIR)\asmov3.abs $(ODIR)\keywrd.abs
	@SET ISIS_F0=$(ODIR)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:link base.abs,publics(asmov0.abs,asmov1.abs,asmov2.abs), "&" < <<
asmov3.abs,keywrd.abs to $(*B).rel &
map print(:f2:$(*B).lin) 
<<NOKEEP
	$(ISIS) :F1:locate $(*B).rel to $(@F) $(LOCFINAL) print(:f2:$(*B).map) "&" < <<
STACKSIZE(0) SYMBOLS  NAME(ASM80)
exit
<<NOKEEP
	del $*.rel
	@find $(F2)\$(*B).lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\$(*B).map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

$(ODIR)\asm80.ov0: $(ODIR)\asmov0.abs 
	@SET ISIS_F0=$(ODIR)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:locate asmov0.abs to $(@F) $(LOCFINAL) print(:f2:asmov0.map) "&" < <<
STACKSIZE(0) symbols NAME(A80O0)
<<NOKEEP
	@(find $(F2)\asmov0.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

$(ODIR)\asm80.ov1: $(ODIR)\asmov1.abs 
	@SET ISIS_F0=$(ODIR)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:locate asmov1.abs to $(@F) $(LOCFINAL) print(:f2:asmov1.map) "&" < <<
STACKSIZE(0) symbols NAME(A80O1)
<<NOKEEP	
	@(find $(F2)\asmov1.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)


$(ODIR)\asm80.ov2: $(ODIR)\asmov2.abs 
	@SET ISIS_F0=$(ODIR)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:locate asmov2.abs to $(@F) $(LOCFINAL) print(:f2:asmov2.map) "&" < <<
STACKSIZE(0) SYMBOLS  NAME(A80O2)
<<NOKEEP	
	@(find $(F2)\asmov2.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

$(ODIR)\asm80.ov3: $(ODIR)\asmov3.abs
	@SET ISIS_F0=$(ODIR)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:locate asmov3.abs to $(@F) $(LOCFINAL) print(:f2:asmov3.map) "&" < <<
STACKSIZE(0) SYMBOLS  NAME(A80O3)
<<NOKEEP
	@(find $(F2)\asmov3.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)


$(ODIR)\asm80.ov4: $(OBJS4) $(ODIR)\key4.obj
	@SET ISIS_F0=$(ODIR)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:link "&" < <<
$(OBJS4ISIS: =, & 
),:f1:plm80.lib,key4.obj to asmov4.rel map print(:f2:asmov4.lin)
<<NOKEEP
	$(ISIS) :F1:locate asmov4.rel to $(@F) NAME(A80O4) "&" < <<
$(LOCFINAL) code(3780h) STACKSIZE(144) print(:f2:asmov4.map)
exit
<<NOKEEP
	@find $(F2)\asmov4.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov4.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)


$(ODIR)\asm80.ov5: $(OBJS5) $(ODIR)\keywrd.obj
	@SET ISIS_F0=$(ODIR)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:link "&" < <<
$(OBJS5ISIS: =, &
),:f1:plm80.lib,keywrd.obj to asmov5.rel map print(:f2:asmov5.lin)
<<NOKEEP
	$(ISIS) :F1:locate asmov5.rel to $(@F) NAME(A80O5) "&" < <<
$(LOCFINAL) code(3780h) STACKSIZE(144) print(:f2:asmov5.map)
exit
<<NOKEEP
	@find $(F2)\asmov5.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov5.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)



# rules for intermediate files

$(ODIR)\base.abs: $(BASEOBJS)
	@SET ISIS_F0=$(ODIR)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:link "&" < <<
$(BASEISIS: =, & 
),:f1:plm80.lib to $(*B).rel print(:f2:$(*B).lin)
<<NOKEEP
	$(ISIS) :F1:locate $(*B).rel to $(@F) $(LOCATEOPT) STACKSIZE(120) print(:f2:$(*B).map)
	del $*.rel
	@find $(F2)\$(*B).lin "UNRESOLVED EXTERNAL" >nul || (del $@ & exit /b 1)


$(ODIR)\keywrd.abs: $(ODIR)\keywrd.obj $(ODIR)\memory.obj
	@SET ISIS_F0=$(ODIR)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:link keywrd.obj,memory.obj to $(*B).rel map print(:f2:$(*B).lin)
	$(ISIS) :F1:locate $(*B).rel to $(@F) $(LOCATEOPT) CODE(075A2h) STACKSIZE(0) print(:f2:$(*B).map)
	del $*.rel
	@find $(F2)\$(*B).lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\$(*B).map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

$(ODIR)\asmov0.abs: $(ODIR)\base.abs $(ODIR)\cprite.obj $(ODIR)\asmov0.obj
	@SET ISIS_F0=$(ODIR)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:link publics(base.abs),cprite.obj,asmov0.obj to $(*B).rel map print(:f2:$(*B).la0)
	$(ISIS) :F1:locate $(*B).rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) print(:f2:$(*B).ma0)
	del $*.rel
	@find $(F2)\$(*B).la0 "LINK MAP" >nul || (del $@ & exit /b 1)


$(ODIR)\asmov1.abs: $(ODIR)\base.abs $(ODIR)\cprite.obj $(ODIR)\asmov1.obj
	@SET ISIS_F0=$(ODIR)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:link publics(base.abs),cprite.obj,asmov1.obj,:F1:plm80.lib "&" < <<
to $(*B).rel map print(:f2:$(*B).la1)
<<NOKEEP
	$(ISIS) :F1:locate $(*B).rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) print(:f2:$(*B).ma1)
	del $*.rel
	@find $(F2)\$(*B).la1 "LINK MAP" >nul || (del $@ & exit /b 1)


$(ODIR)\asmov2.abs: $(ODIR)\base.abs $(ODIR)\cprite.obj $(ODIR)\asmov2.obj
	@SET ISIS_F0=$(ODIR)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:link publics(base.abs),cprite.obj,asmov2.obj to $(*B).rel map print(:f2:$(*B).la2)
	$(ISIS) :F1:locate $(*B).rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) print(:f2:$(*B).ma2)
	del $*.rel
	@find $(F2)\$(*B).la2 "LINK MAP" >nul || (del $@ & exit /b 1)


$(ODIR)\asmov3.abs: $(ODIR)\base.abs $(ODIR)\cprite.obj $(ODIR)\asmov3.obj $(ODIR)\keywrd.abs
	@SET ISIS_F0=$(ODIR)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(ISIS) :F1:link publics(base.abs),cprite.obj,asmov3.obj,publics(keywrd.abs) "&" < <<
to $(*B).rel map print(:f2:$(*B).la3)
<<NOKEEP
	$(ISIS) :F1:locate $(*B).rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) print(:f2:$(*B).ma3)
	del $*.rel
	@find $(F2)\$(*B).la3 "LINK MAP" >nul || (del $@ & exit /b 1)

# pex file and makefile dependencies
$(P3SRCS:.pl3=.ipx) $(P3SRCS:.pl3=.obj): $(PEXFILE) makefile
$(P4SRCS:.plm=.ipx) $(P4SRCS:.plm=.obj): $(PEXFILE) makefile
$(ASRCS:.asm=.obj): makefile 

# housekeeping rules
clean:
	del /q *.*~ *~ *.ipx *.*~ 
	del /q release\*.obj release\*.abs release\*.rel release\*.*~ release\*~ 
	del /q debug\*.obj debug\*.abs debug\*.rel debug\*.*~ debug\*~ 
	del /q $(F2)\*.*~ $(F2)\*.???

vclean: clean
	del /q release\asm80 release\asm80.ov?
	del /q debug\asm80 debug\asm80.ov?

uclean: vclean
	del /q ??????.* makefile *.plx .extract

gitprep: uclean
	..\..\unpack.pl



# auto generated

