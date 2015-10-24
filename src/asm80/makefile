ISIS=..\..\thames
V4=..\..\plm80v4^\
V3=..\..\plm80v3^\
TOOLS=..\..\tools^\
PLMPP=$(TOOLS)plmpp
NGENPEX=$(TOOLS)ngenpex
SRC=.^\
LST=.\list
OBJ=.\obj

LOCATEOPT=MAP PUBLICS SYMBOLS LINES
PEXFILE=asm80.pex
PLMFLAGS=CODE PRINT(:f2:$(*B).lst) OBJECT(:f3:$(*B).obj) DEBUG

OVERLAY=6D4Ch

# bulk obj names
BASEOBJS = glb.obj asm801.obj asm82.obj asm83.obj\
	asm46.obj asm85.obj asm851.obj asm86.obj\
	asm861.obj pcktkn.obj close.obj delete.obj\
	error.obj exit.obj load.obj open.obj\
	read.obj rescan.obj seek.obj write.obj


OBJS4 =	glb4.obj asm41.obj pcktkn.obj asm43.obj\
	asm44.obj asm45.obj asm46.obj asm47.obj\
	asm48.obj asm49.obj asm4A.obj asm4B.obj\
	asm4C.obj asm4D.obj asm4E.obj close.obj\
	delete.obj error.obj exit.obj load.obj\
	open.obj read.obj rescan.obj seek.obj\
	write.obj


OBJS5 =	glb5.obj asm51.obj pcktkn.obj asm82.obj\
	asm83.obj asm55.obj asm46.obj asm57.obj\
	asm86.obj asm861.obj asmov0.obj asmov2.obj\
	asmov1.obj asm5D.obj close.obj delete.obj\
	error.obj exit.obj load.obj open.obj\
	read.obj rescan.obj seek.obj write.obj


P4SRCS = asm41.plm asm44.plm asm45.plm asm4e.plm asm51.plm asm55.plm asm5d.plm\
	asm801.plm asm83.plm asm851.plm asm861.plm asmov3.plm close.plm delete.plm\
	error.plm exit.plm glb.plm glb4.plm glb5.plm key4.plm keywrd.plm load.plm\
	open.plm read.plm rescan.plm seek.plm write.plm

P3SRCS = asm43.pl3 asm46.pl3 asm47.pl3 asm48.pl3 asm49.pl3 asm4a.pl3 asm4B.pl3 asm4c.pl3\
	asm4d.pl3 asm57.pl3 asm82.pl3 asm85.pl3 asm86.pl3 asmov0.pl3 asmov1.pl3 asmov2.pl3

ASRCS = pcktkn.asm cprite.asm memory.asm

DEBUGTARGETS = $(OBJ)\asm80 $(OBJ)\asm80.ov0 $(OBJ)\asm80.ov1 $(OBJ)\asm80.ov2 $(OBJ)\asm80.ov3\
	$(OBJ)\asm80.ov4 $(OBJ)\asm80.ov5\

RELEASETARGETS = asm80 asm80.ov0 asm80.ov1 asm80.ov2 asm80.ov3 asm80.ov4 asm80.ov5

#
# compilation and assembly rules
.SUFFIXES: .plm .pl3 .asm
.plm{$(OBJ)}.obj:
	@SET ISIS_F0=$(SRC)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	@SET ISIS_F3=$(OBJ)
	$(ISIS) :F1:PLM80 $< $(PLMFLAGS)
	@if not exist $@ exit /b 1

.pl3{$(OBJ)}.obj:
	@SET ISIS_F0=$(SRC)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(LST)
	@SET ISIS_F3=$(OBJ)
	$(ISIS) :F1:PLM80 $< $(PLMFLAGS)
	@find $(LST)\$(*B).lst "     0 PROGRAM ERROR(S)" >nul || (del $@ & exit /b 1)

.plm.ipx:
	@SET ISIS_F0=$(SRC)
	$(NGENPEX) $(PEXFILE) $<

.pl3.ipx:
	@SET ISIS_F0=$(SRC)
	$(NGENPEX) $(PEXFILE) $<
	
.asm{$(OBJ)}.obj:
	@SET ISIS_F0=$(SRC)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	@SET ISIS_F3=$(OBJ)
	$(ISIS) :F1:ASM80 $< PRINT(:f2:$(*B).lst) OBJECT(:f3:$(*B).obj)
	@find $(LST)\$(*B).lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $@ & exit /b 1)

# the default rule will force the makefile to be up to date
all: source

all:
	$(MAKE) $(DEBUGTARGETS) $(RELEASETARGETS)

rebuild: uclean all

.extract: asm80_all.plm
	..\..\unpack.pl
	date /t >.extract


source: .extract $(ASRCS) $(P3SRCS) $(P4SRCS) $(P3SRCS:.pl3=.ipx) $(P4SRCS:.plm=.ipx)

# checking targets
# note if you change these two rules then you should do a nmake all or nmake .extract first
# otherwise whilst the build will be ok the old verify/compare rules will be run

verify: all
	for %i in ($(RELEASETARGETS)) do $(TOOLS)diffbin %i $(V4)\%i

compare: all
	for %i in ($(RELEASETARGETS)) do fc /b %i $(V4)%i

# source files generated from shared code
asm43.pl3 asm82.pl3: asm4382.plx
	@SET ISIS_F0=$(SRC)
	$(PLMPP) -sOVL4 -o asm43.pl3 $**
	$(PLMPP) -sBASE -o asm82.pl3 $**

asm48.pl3 asm86.pl3: asm4886.plx
	@SET ISIS_F0=$(SRC)
	$(PLMPP) -sOVL4 -o asm48.pl3 $**
	$(PLMPP) -sBASE -o asm86.pl3 $**

asm49.pl3 asm861.plm: asm49861.plx
	@SET ISIS_F0=$(SRC)
	$(PLMPP) -sOVL4 -o asm49.pl3 $**
	$(PLMPP) -sBASE -o asm861.plm $**

asm44.plm asm83.plm: as4483.plx
	@SET ISIS_F0=$(SRC)
	$(PLMPP) -sOVL4 -o asm44.plm $**
	$(PLMPP) -sBASE -o asm83.plm $**

asm45.plm asm55.plm asm851.plm: as4555851.plx
	@SET ISIS_F0=$(SRC)
	$(PLMPP) -sOVL4 -o asm45.plm $**
	$(PLMPP) -sOVL5 -o asm55.plm $**
	$(PLMPP) -sBASE -o asm851.plm $**

asm4a.pl3 asmov0.pl3: asm4aov0.plx
	@SET ISIS_F0=$(SRC)
	$(PLMPP) -sOVL4 -o asm4a.pl3 $**
	$(PLMPP) -sOVL0 -o asmov0.pl3 $**

asm4d.pl3 asmov1.pl3: asm4dov1.plx
	@SET ISIS_F0=$(SRC)
	$(PLMPP) -sOVL4 -o asm4d.pl3 $**
	$(PLMPP) -sOVL1 -o asmov1.pl3 $**

asm4c.pl3 asmov2.pl3: asm4cov2.plx
	@SET ISIS_F0=$(SRC)
	$(PLMPP) -sOVL4 -o asm4c.pl3 $**
	$(PLMPP) -sOVL2 -o asmov2.pl3 $**

asm47.pl3 asm57.pl3 asm85.pl3: asm475785.plx
	@SET ISIS_F0=$(SRC)
	$(PLMPP) -sOVL4 -o asm47.pl3 $**
	$(PLMPP) -sOVL5 -o asm57.pl3 $**
	$(PLMPP) -sBASE -o asm85.pl3 $**

asm41.plm asm51.plm asm801.plm: start.plx
	@SET ISIS_F0=$(SRC)
	$(PLMPP) -sOVL4 -o asm41.plm $**
	$(PLMPP) -sOVL5 -o asm51.plm $**
	$(PLMPP) -sBASE -o asm801.plm $**

glb.plm glb4.plm glb5.plm: global.plx
	@SET ISIS_F0=$(SRC)
	$(PLMPP) -sOVL4 -o glb4.plm $**
	$(PLMPP) -sOVL5 -o glb5.plm $**
	$(PLMPP) -sBASE -o glb.plm $**

asm4e.plm asm5d.plm asmov3.plm: init.plx
	@SET ISIS_F0=$(SRC)
	$(PLMPP) -sOVL4 -o asm4e.plm $**
	$(PLMPP) -sOVL5 -o asm5d.plm $**
	$(PLMPP) -sOVL3 -o asmov3.plm $**

# build rules to make the program and overlays
# The release targets - built by purging symbols fnd debug info from the debug variants
asm80: $(OBJ)\asm80
	@SET ISIS_F0=$(SRC)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(OBJ)
	$(ISIS) :F1:locate :F3:$@ to $@ PURGE

asm80.ov0: $(OBJ)\asm80.ov0
	@SET ISIS_F0=$(SRC)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(OBJ)
	$(ISIS) :F1:locate :F3:$@ to $@ PURGE

asm80.ov1: $(OBJ)\asm80.ov1
	@SET ISIS_F0=$(SRC)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(OBJ)
	$(ISIS) :F1:locate :F3:$@ to $@ PURGE

asm80.ov2: $(OBJ)\asm80.ov2
	@SET ISIS_F0=$(SRC)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(OBJ)
	$(ISIS) :F1:locate :F3:$@ to $@ PURGE

asm80.ov3: $(OBJ)\asm80.ov3
	@SET ISIS_F0=$(SRC)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(OBJ)
	$(ISIS) :F1:locate :F3:$@ to $@ PURGE

asm80.ov4: $(OBJ)\asm80.ov4
	@SET ISIS_F0=$(SRC)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(OBJ)
	$(ISIS) :F1:locate :F3:$@ to $@ PURGE

asm80.ov5: $(OBJ)\asm80.ov4
	@SET ISIS_F0=$(SRC)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(OBJ)
	$(ISIS) :F1:locate :F3:$@ to $@ PURGE

# The debug targets - keeps local symbols and debug info
$(OBJ)\asm80: $(OBJ)\base.abs $(OBJ)\asmov0.abs $(OBJ)\asmov1.abs $(OBJ)\asmov2.abs $(OBJ)\asmov3.abs $(OBJ)\keywrd.abs
	@SET ISIS_F0=$(OBJ)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	$(ISIS) :F1:link base.abs,publics(asmov0.abs,asmov1.abs,asmov2.abs), "&" < <<
asmov3.abs,keywrd.abs to $(*B).rel &
map print(:f2:$(*B).lin) 
<<NOKEEP
	$(ISIS) :F1:locate $(*B).rel to $(@F) $(LOCATEOPT) print(:f2:$(*B).map) "&" < <<
STACKSIZE(0) SYMBOLS  NAME(ASM80)
exit
<<NOKEEP
	del $*.rel
	@find $(LST)\$(*B).lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(LST)\$(*B).map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

$(OBJ)\asm80.ov0: $(OBJ)\asmov0.abs 
	@SET ISIS_F0=$(OBJ)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	$(ISIS) :F1:locate asmov0.abs to $(@F) $(LOCATEOPT) print(:f2:asmov0.map) "&" < <<
STACKSIZE(0) symbols NAME(A80O0)
<<NOKEEP
	@(find $(LST)\asmov0.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

$(OBJ)\asm80.ov1: $(OBJ)\asmov1.abs 
	@SET ISIS_F0=$(OBJ)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	$(ISIS) :F1:locate asmov1.abs to $(@F) $(LOCATEOPT) print(:f2:asmov1.map) "&" < <<
STACKSIZE(0) symbols NAME(A80O1)
<<NOKEEP	
	@(find $(LST)\asmov1.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)


$(OBJ)\asm80.ov2: $(OBJ)\asmov2.abs 
	@SET ISIS_F0=$(OBJ)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	$(ISIS) :F1:locate asmov2.abs to $(@F) $(LOCATEOPT) print(:f2:asmov2.map) "&" < <<
STACKSIZE(0) SYMBOLS  NAME(A80O2)
<<NOKEEP	
	@(find $(LST)\asmov2.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

$(OBJ)\asm80.ov3: $(OBJ)\asmov3.abs
	@SET ISIS_F0=$(OBJ)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	$(ISIS) :F1:locate asmov3.abs to $(@F) $(LOCATEOPT) print(:f2:asmov3.map) "&" < <<
STACKSIZE(0) SYMBOLS  NAME(A80O3)
<<NOKEEP
	@(find $(LST)\asmov3.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)


$(OBJ)\asm80.ov4: $(OBJ)\$(OBJS4: = .\obj\) $(OBJ)\key4.obj
	@SET ISIS_F0=$(OBJ)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	$(ISIS) :F1:link "&" < <<
$(OBJS4: =, & 
),:f1:plm80.lib,key4.obj to asmov4.rel map print(:f2:asmov4.lin)
<<NOKEEP
	$(ISIS) :F1:locate asmov4.rel to $(@F) NAME(A80O4) "&" < <<
$(LOCATEOPT) code(3780h) STACKSIZE(144) print(:f2:asmov4.map)
exit
<<NOKEEP
	@find $(LST)\asmov4.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(LST)\asmov4.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)


$(OBJ)\asm80.ov5: $(OBJ)\$(OBJS5: = .\obj\) $(OBJ)\keywrd.obj
	@SET ISIS_F0=$(OBJ)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	$(ISIS) :F1:link "&" < <<
$(OBJS5: =, &
),:f1:plm80.lib,keywrd.obj to asmov5.rel map print(:f2:asmov5.lin)
<<NOKEEP
	$(ISIS) :F1:locate asmov5.rel to $(@F) NAME(A80O5) "&" < <<
$(LOCATEOPT) code(3780h) STACKSIZE(144) print(:f2:asmov5.map)
exit
<<NOKEEP
	@find $(LST)\asmov5.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(LST)\asmov5.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)



# rules for intermediate files

$(OBJ)\base.abs: $(OBJ)\$(BASEOBJS: = .\obj\)
	@SET ISIS_F0=$(OBJ)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	$(ISIS) :F1:link "&" < <<
$(BASEOBJS: =, & 
),:f1:plm80.lib to $(*B).rel print(:f2:$(*B).lin)
<<NOKEEP
	$(ISIS) :F1:locate $(*B).rel to $(@F) $(LOCATEOPT) STACKSIZE(120) print(:f2:$(*B).map)
	del $*.rel
	@find $(LST)\$(*B).lin "UNRESOLVED EXTERNAL" >nul || (del $@ & exit /b 1)


$(OBJ)\keywrd.abs: $(OBJ)\keywrd.obj $(OBJ)\memory.obj
	@SET ISIS_F0=$(OBJ)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	$(ISIS) :F1:link keywrd.obj,memory.obj to $(*B).rel map print(:f2:$(*B).lin)
	$(ISIS) :F1:locate $(*B).rel to $(@F) $(LOCATEOPT) CODE(075A2h) STACKSIZE(0) print(:f2:$(*B).map)
	del $*.rel
	@find $(LST)\$(*B).lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(LST)\$(*B).map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

$(OBJ)\asmov0.abs: $(OBJ)\base.abs $(OBJ)\cprite.obj $(OBJ)\asmov0.obj
	@SET ISIS_F0=$(OBJ)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	$(ISIS) :F1:link publics(base.abs),cprite.obj,asmov0.obj to $(*B).rel map print(:f2:$(*B).la0)
	$(ISIS) :F1:locate $(*B).rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) print(:f2:$(*B).ma0)
	del $*.rel
	@find $(LST)\$(*B).la0 "LINK MAP" >nul || (del $@ & exit /b 1)


$(OBJ)\asmov1.abs: $(OBJ)\base.abs $(OBJ)\cprite.obj $(OBJ)\asmov1.obj
	@SET ISIS_F0=$(OBJ)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	$(ISIS) :F1:link publics(base.abs),cprite.obj,asmov1.obj,:F1:plm80.lib "&" < <<
to $(*B).rel map print(:f2:$(*B).la1)
<<NOKEEP
	$(ISIS) :F1:locate $(*B).rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) print(:f2:$(*B).ma1)
	del $*.rel
	@find $(LST)\$(*B).la1 "LINK MAP" >nul || (del $@ & exit /b 1)


$(OBJ)\asmov2.abs: $(OBJ)\base.abs $(OBJ)\cprite.obj $(OBJ)\asmov2.obj
	@SET ISIS_F0=$(OBJ)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	$(ISIS) :F1:link publics(base.abs),cprite.obj,asmov2.obj to $(*B).rel map print(:f2:$(*B).la2)
	$(ISIS) :F1:locate $(*B).rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) print(:f2:$(*B).ma2)
	del $*.rel
	@find $(LST)\$(*B).la2 "LINK MAP" >nul || (del $@ & exit /b 1)


$(OBJ)\asmov3.abs: $(OBJ)\base.abs $(OBJ)\cprite.obj $(OBJ)\asmov3.obj $(OBJ)\keywrd.abs
	@SET ISIS_F0=$(OBJ)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(LST)
	$(ISIS) :F1:link publics(base.abs),cprite.obj,asmov3.obj,publics(keywrd.abs) "&" < <<
to $(*B).rel map print(:f2:$(*B).la3)
<<NOKEEP
	$(ISIS) :F1:locate $(*B).rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) print(:f2:$(*B).ma3)
	del $*.rel
	@find $(LST)\$(*B).la3 "LINK MAP" >nul || (del $@ & exit /b 1)

# pex file and makefile dependencies
*.ipx: $(PEXFILE) 
$(OBJ)\*.obj: makefile

# housekeeping rules
clean:
	del /q *.*~ *~ *.ipx 
	del /q $(OBJ)\*.obj $(OBJ)\*.abs $(OBJ)\*.rel $(OBJ)\*.*~ $(OBJ)\*~ 
	del /q $(LST)\*.*~ $(LST)\*.???

vclean: clean
	del /q $(DEBUGTARGETS) $(RELEASETARGETS)

uclean: vclean
	del /q ??????.* makefile *.plx .extract

gitprep: uclean
	..\..\unpack.pl



# auto generated

