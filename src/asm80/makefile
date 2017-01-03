# path to root of build tree
ROOT=..\..
# paths of build tool directories
V4=$(ROOT)\plm80v4
V3=$(ROOT)\plm80v3
TOOLS=$(ROOT)\tools
# the core tools
ISIS=$(ROOT)\thames
PLMPP=$(TOOLS)\plmpp
NGENPEX=$(TOOLS)\ngenpex
ASM=$(ISIS) :f1:asm80
PLM=$(ISIS) :f1:plm80
LINK=$(ISIS) :f1:link
LOCATE=$(ISIS) :f1:locate
# the build directories
SRC=.^\
LST=.\list
OBJ=.\obj

# mapping to isis directories
F0=$(OBJ)
#F1 mapped to $(V3) or $(V4) as appropriate
F2=$(LST)
F3=$(SRC)

PEXFILE=asm80.pex
LOCATEOPT=MAP PUBLICS SYMBOLS LINES
PLMFLAGS=CODE PRINT(:f2:$(*B).lst) OBJECT(:f0:$(*B).obj) DEBUG

# base address of the overlays
OVERLAY=6D4Ch

# the following are now included from the plm v3.1 system.lib
#  close.obj delete.obj	error.obj exit.obj load.obj open.obj
#  read.obj rescan.obj seek.obj write.obj
# note v4 system.lib orders modules differently
# an option would be to specify the modules explicitly i.e.
# system.lib(close,delete,error,exit,load,open,read,rescan,seek,write)

# bulk obj names
BASEOBJS = glb.obj asm801.obj asm82.obj asm83.obj\
	asm46.obj asm85.obj asm851.obj asm86.obj\
	asm861.obj pcktkn.obj

OBJS4 = glb4.obj asm41.obj pcktkn.obj asm43.obj\
	asm44.obj asm45.obj asm46.obj asm47.obj\
	asm48.obj asm49.obj asm4A.obj asm4B.obj\
	asm4C.obj asm4D.obj asm4E.obj

OBJS5 = glb5.obj asm51.obj pcktkn.obj asm82.obj\
	asm83.obj asm55.obj asm46.obj asm57.obj\
	asm86.obj asm861.obj asmov0.obj asmov2.obj\
	asmov1.obj asm5D.obj

RELEASETARGETS = asm80 asm80.ov0 asm80.ov1 asm80.ov2 asm80.ov3 asm80.ov4 asm80.ov5 asxref

# compilation and assembly rules
.SUFFIXES: .plm .pl3 .asm
.plm{$(F0)}.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(PLM) :f3:$< $(PLMFLAGS)
	@if not exist $@ exit /b 1

.pl3{$(F0)}.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(PLM) :f3:$< $(PLMFLAGS)
	@find $(F2)\$(*B).lst "     0 PROGRAM ERROR(S)" >nul || (del $@ & exit /b 1)

.plm.ipx:
	@SET ISIS_F3=$(F3)
	$(NGENPEX) :f3:$(PEXFILE) :f3:$<

.pl3.ipx:
	@SET ISIS_F3=$(F3)
	$(NGENPEX) :f3:$(PEXFILE) :f3:$<
	
.asm{$(F0)}.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(ASM) :f3:$< PRINT(:f2:$(*B).lst) OBJECT(:f0:$(*B).obj)
	@find $(F2)\$(*B).lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $@ & exit /b 1)

# the default rule will force the makefile to be up to date
all: .extract

all:
	$(MAKE) $(RELEASETARGETS)

rebuild: uclean all

.extract: asm80_all.plm
	..\..\unpack.pl
	date /t >.extract



# checking targets first making sure files are up to date and cater for makefile change
verify: all
	$(MAKE) _verify

# this is the real verify
_verify:
	for %i in ($(RELEASETARGETS)) do fc /b %i $(V4)\%i


# source files generated from shared code
asm43.pl3 asm82.pl3: asm4382.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm43.pl3 $(**F)
	$(PLMPP) -sBASE -o asm82.pl3 $(**F)

asm48.pl3 asm86.pl3: asm4886.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm48.pl3 $(**F)
	$(PLMPP) -sBASE -o asm86.pl3 $(**F)

asm49.pl3 asm861.plm: asm49861.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm49.pl3 $(**F)
	$(PLMPP) -sBASE -o asm861.plm $(**F)

asm44.plm asm83.plm: as4483.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm44.plm $(**F)
	$(PLMPP) -sBASE -o asm83.plm $(**F)

asm45.plm asm55.plm asm851.plm: as4555851.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm45.plm $(**F)
	$(PLMPP) -sOVL5 -o asm55.plm $(**F)
	$(PLMPP) -sBASE -o asm851.plm $(**F)

asm4a.pl3 asmov0.pl3: asm4aov0.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm4a.pl3 $(**F)
	$(PLMPP) -sOVL0 -o asmov0.pl3 $(**F)

asm4d.pl3 asmov1.pl3: asm4dov1.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm4d.pl3 $(**F)
	$(PLMPP) -sOVL1 -o asmov1.pl3 $(**F)

asm4c.pl3 asmov2.pl3: asm4cov2.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm4c.pl3 $(**F)
	$(PLMPP) -sOVL2 -o asmov2.pl3 $(**F)

asm47.pl3 asm57.pl3 asm85.pl3: asm475785.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm47.pl3 $(**F)
	$(PLMPP) -sOVL5 -o asm57.pl3 $(**F)
	$(PLMPP) -sBASE -o asm85.pl3 $(**F)

asm41.plm asm51.plm asm801.plm: start.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm41.plm $**
	$(PLMPP) -sOVL5 -o asm51.plm $**
	$(PLMPP) -sBASE -o asm801.plm $**

glb.plm glb4.plm glb5.plm: global.plx
	SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o glb4.plm $**
	$(PLMPP) -sOVL5 -o glb5.plm $**
	$(PLMPP) -sBASE -o glb.plm $**

asm4e.plm asm5d.plm asmov3.plm: init.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm4e.plm $(**F)
	$(PLMPP) -sOVL5 -o asm5d.plm $(**F)
	$(PLMPP) -sOVL3 -o asmov3.plm $(**F)

# build rules to make the program and overlays
# creates the debug file versions in $(F0) and purged versions in the $(F3) directory
$(F0)\asm80 asm80: $(F0)\base.abs $(F0)\asm80.ov0 $(F0)\asm80.ov1 $(F0)\asm80.ov2 $(F0)\asm80.ov3 $(F0)\keywrd.abs
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) base.abs,publics(asm80.ov0,asm80.ov1,asm80.ov2),asm80.ov3,keywrd.abs to asm80.rel map print(:f2:asm80.lin)
	$(LOCATE) asm80.rel to $(@F) $(LOCATEOPT) STACKSIZE(0) NAME(ASM80) PRINT(:f2:asm80.map)
	del $(F0)\asm80.rel
	@find $(F2)\asm80.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asm80.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$@ PURGE

$(F0)\asm80.ov0 asm80.ov0: $(F0)\base.abs $(F0)\cprite.obj $(F0)\asmov0.obj 
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) publics(base.abs),cprite.obj,asmov0.obj to asmov0.rel map print(:f2:asmov0.lin)
	$(LOCATE) asmov0.rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) NAME(A80O0) PRINT(:f2:asmov0.map)
	del $(F0)\asmov0.rel
	@find $(F2)\asmov0.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov0.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$(@F) PURGE


$(F0)\asm80.ov1 asm80.ov1: $(F0)\base.abs $(F0)\cprite.obj $(F0)\asmov1.obj 
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) publics(base.abs),cprite.obj,asmov1.obj,:F1:plm80.lib to asmov1.rel map print(:f2:asmov1.lin)
	$(LOCATE) asmov1.rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) NAME(A80O1) PRINT(:f2:asmov1.map)
	del $(F0)\asmov1.rel
	@find $(F2)\asmov1.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov1.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$@ PURGE


$(F0)\asm80.ov2 asm80.ov2: $(F0)\base.abs $(F0)\cprite.obj $(F0)\asmov2.obj 
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) publics(base.abs),cprite.obj,asmov2.obj to asmov2.rel map print(:f2:asmov2.lin)
	$(LOCATE) asmov2.rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) NAME(A80O2) PRINT(:f2:asmov2.map)
	del $(F0)\asmov2.rel
	@find $(F2)\asmov2.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov2.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$@ PURGE

$(F0)\asm80.ov3 asm80.ov3: $(F0)\base.abs $(F0)\cprite.obj $(F0)\asmov3.obj $(F0)\keywrd.abs
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) publics(base.abs),cprite.obj,asmov3.obj,publics(keywrd.abs) to asmov3.rel map print(:f2:asmov3.lin)
	$(LOCATE) asmov3.rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) NAME(A80O3) PRINT(:f2:asmov3.map)
	del $(F0)\asmov3.rel
	@find $(F2)\asmov3.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov3.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$@ PURGE


$(F0)\asm80.ov4 asm80.ov4: $(F0)\$(OBJS4: = .\obj\) $(F0)\key4.obj
# initially set F0 to object directory to avoid losts of :f3: prefixes
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) "&" < <<
$(OBJS4: =, & 
),:f1:system.lib,:f1:plm80.lib,key4.obj to asmov4.rel map print(:f2:asmov4.lin)
<<NOKEEP
	$(LOCATE) asmov4.rel to $(@F) NAME(A80O4) "&" < <<
$(LOCATEOPT) code(3780h) STACKSIZE(144) print(:f2:asmov4.map)
exit
<<NOKEEP
	@find $(F2)\asmov4.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov4.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$@ PURGE


$(F0)\asm80.ov5 asm80.ov5: $(F0)\$(OBJS5: = .\obj\) $(F0)\keywrd.obj
# initially set F0 to object directory to avoid losts of :f3: prefixes
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) "&" < <<
$(OBJS5: =, &
),:f1:system.lib,:f1:plm80.lib,keywrd.obj to asmov5.rel map print(:f2:asmov5.lin)
<<NOKEEP
	$(LOCATE) asmov5.rel to $(@F) NAME(A80O5) "&" < <<
$(LOCATEOPT) code(3780h) STACKSIZE(144) print(:f2:asmov5.map)
exit
<<NOKEEP
	@find $(F2)\asmov5.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov5.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$@ PURGE

$(F0)\asxref asxref: $(F0)\asxref.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LOCATE) asxref.obj to $(@F) $(LOCATEOPT) code(3680h) order(code,stack,data) stacksize(32) print(:f2:asxref.map)
	$(LOCATE) $@ to :f3:$@ PURGE

# rules for intermediate files

$(F0)\base.abs: $(F0)\$(BASEOBJS: = .\obj\)
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	$(LINK) "&" < <<
$(BASEOBJS: =, & 
),:f1:system.lib,:f1:plm80.lib to $(*B).rel print(:f2:$(*B).lin)
<<NOKEEP
	$(LOCATE) $(*B).rel to $(@F) $(LOCATEOPT) STACKSIZE(120) print(:f2:$(*B).map)
	del $*.rel
	@find $(F2)\$(*B).lin "UNRESOLVED EXTERNAL" >nul || (del $@ & exit /b 1)


$(F0)\keywrd.abs: $(F0)\keywrd.obj $(F0)\memory.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	$(LINK) keywrd.obj,memory.obj to $(*B).rel map print(:f2:$(*B).lin)
	$(LOCATE) $(*B).rel to $(@F) $(LOCATEOPT) CODE(075A2h) STACKSIZE(0) print(:f2:$(*B).map)
	del $*.rel
	@find $(F2)\$(*B).lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\$(*B).map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

# pex file and makefile dependencies
*.ipx: $(PEXFILE) 
$(F0)\*.obj: makefile

# housekeeping rules
clean:
	del /q *.*~ *~ *.ipx 
	del /q $(F0)\*.obj $(F0)\*.abs $(F0)\*.rel $(F0)\*.*~ $(F0)\*~ 
	del /q $(F2)\*.*~ $(F2)\*.???

vclean: clean
	del /q $(RELEASETARGETS) $(F0)\*.???

uclean: vclean
	del /q ??????.* makefile *.plx .extract

gitprep: uclean
	..\..\unpack.pl



# modified from auto generated to include $(F0) prefix
$(F0)\asm41.obj: asm41.plm asm41.ipx
$(F0)\asm43.obj: asm43.pl3 asm43.ipx
$(F0)\asm44.obj: asm44.plm asm44.ipx
$(F0)\asm45.obj: asm45.plm asm45.ipx
$(F0)\asm46.obj: asm46.pl3 asm46.ipx
$(F0)\asm47.obj: asm47.pl3 asm47.ipx
$(F0)\asm48.obj: asm48.pl3 asm48.ipx
$(F0)\asm49.obj: asm49.pl3 asm49.ipx
$(F0)\asm4a.obj: asm4a.pl3 asm4a.ipx
$(F0)\asm4b.obj: asm4b.pl3 asm4b.ipx
$(F0)\asm4c.obj: asm4c.pl3 asm4c.ipx
$(F0)\asm4d.obj: asm4d.pl3 asm4d.ipx
$(F0)\asm4e.obj: asm4e.plm asm4e.ipx
$(F0)\asm51.obj: asm51.plm asm51.ipx
$(F0)\asm55.obj: asm55.plm asm55.ipx
$(F0)\asm57.obj: asm57.pl3 asm57.ipx
$(F0)\asm5d.obj: asm5d.plm asm5d.ipx
$(F0)\asm801.obj: asm801.plm asm801.ipx
$(F0)\asm82.obj: asm82.pl3 asm82.ipx
$(F0)\asm83.obj: asm83.plm asm83.ipx
$(F0)\asm85.obj: asm85.pl3 asm85.ipx
$(F0)\asm851.obj: asm851.plm asm851.ipx
$(F0)\asm86.obj: asm86.pl3 asm86.ipx
$(F0)\asm861.obj: asm861.plm asm861.ipx
$(F0)\asmov0.obj: asmov0.pl3 asmov0.ipx
$(F0)\asmov1.obj: asmov1.pl3 asmov1.ipx
$(F0)\asmov2.obj: asmov2.pl3 asmov2.ipx
$(F0)\asmov3.obj: asmov3.plm asmov3.ipx
$(F0)\close.obj: close.plm close.ipx
$(F0)\delete.obj: delete.plm delete.ipx
$(F0)\error.obj: error.plm error.ipx
$(F0)\exit.obj: exit.plm exit.ipx
$(F0)\glb.obj: glb.plm glb.ipx
$(F0)\glb4.obj: glb4.plm glb4.ipx
$(F0)\glb5.obj: glb5.plm glb5.ipx
$(F0)\key4.obj: key4.plm key4.ipx
$(F0)\keywrd.obj: keywrd.plm keywrd.ipx
$(F0)\load.obj: load.plm load.ipx
$(F0)\open.obj: open.plm open.ipx
$(F0)\read.obj: read.plm read.ipx
$(F0)\rescan.obj: rescan.plm rescan.ipx
$(F0)\seek.obj: seek.plm seek.ipx
$(F0)\write.obj: write.plm write.ipx
