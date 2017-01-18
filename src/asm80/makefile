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
SMALLOBJS = globls.obj starts.obj asm1n.obj asm2n.obj\
	rdsrc.obj asm3s.obj asm4s.obj asm5n.obj\
	asm6n.obj pcktok.obj

OBJS4 = globlm.obj startm.obj pcktok.obj asm1m.obj\
	asm2m.obj asm4m.obj rdsrc.obj asm3m.obj\
	asm5m.obj asm6m.obj cntrlm.obj asm8m.obj\
	emitm.obj listm.obj initm.obj

OBJS5 = globlb.obj startb.obj pcktok.obj asm1n.obj\
	asm2n.obj asm4b.obj rdsrc.obj asm3b.obj\
	asm5n.obj asm6n.obj cntrln.obj emitn.obj\
	listn.obj initb.obj

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
asm1m.pl3 asm1n.pl3: asm1.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm1m.pl3 $(**F)
	$(PLMPP) -sSMALL -o asm1n.pl3 $(**F)

asm5m.pl3 asm5n.pl3: asm5.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm5m.pl3 $(**F)
	$(PLMPP) -sSMALL -o asm5n.pl3 $(**F)

asm6m.pl3 asm6n.plm: asm6.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm6m.pl3 $(**F)
	$(PLMPP) -sSMALL -o asm6n.plm $(**F)

asm2m.plm asm2n.plm: asm2.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm2m.plm $(**F)
	$(PLMPP) -sSMALL -o asm2n.plm $(**F)

asm4m.plm asm4b.plm asm4s.plm: asm4.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm4m.plm $(**F)
	$(PLMPP) -sOVL5 -o asm4b.plm $(**F)
	$(PLMPP) -sSMALL -o asm4s.plm $(**F)

cntrlm.pl3 cntrln.pl3: cntrl.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o cntrlm.pl3 $(**F)
	$(PLMPP) -sOVL0 -o cntrln.pl3 $(**F)

listm.pl3 listn.pl3: list.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o listm.pl3 $(**F)
	$(PLMPP) -sOVL1 -o listn.pl3 $(**F)

emitm.pl3 emitn.pl3: emit.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o emitm.pl3 $(**F)
	$(PLMPP) -sOVL2 -o emitn.pl3 $(**F)

asm3m.pl3 asm3b.pl3 asm3s.pl3: asm3.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o asm3m.pl3 $(**F)
	$(PLMPP) -sOVL5 -o asm3b.pl3 $(**F)
	$(PLMPP) -sSMALL -o asm3s.pl3 $(**F)

starts.plm startm.plm startb.plm: start.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o startm.plm $**
	$(PLMPP) -sOVL5 -o startb.plm $**
	$(PLMPP) -sSMALL -o starts.plm $**

globls.plm globlm.plm globlb.plm: globl.plx
	SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o globlm.plm $**
	$(PLMPP) -sOVL5 -o globlb.plm $**
	$(PLMPP) -sSMALL -o globls.plm $**

initm.plm initb.plm inits.plm: init.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sOVL4 -o initm.plm $(**F)
	$(PLMPP) -sOVL5 -o initb.plm $(**F)
	$(PLMPP) -sOVL3 -o inits.plm $(**F)

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

$(F0)\asm80.ov0 asm80.ov0: $(F0)\base.abs $(F0)\cprite.obj $(F0)\cntrln.obj 
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) publics(base.abs),cprite.obj,cntrln.obj to cntrln.rel map print(:f2:cntrln.lin)
	$(LOCATE) cntrln.rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) NAME(A80O0) PRINT(:f2:cntrln.map)
	del $(F0)\cntrln.rel
	@find $(F2)\cntrln.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\cntrln.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$(@F) PURGE


$(F0)\asm80.ov1 asm80.ov1: $(F0)\base.abs $(F0)\cprite.obj $(F0)\listn.obj 
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) publics(base.abs),cprite.obj,listn.obj,:F1:plm80.lib to listn.rel map print(:f2:listn.lin)
	$(LOCATE) listn.rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) NAME(A80O1) PRINT(:f2:listn.map)
	del $(F0)\listn.rel
	@find $(F2)\listn.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\listn.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$@ PURGE


$(F0)\asm80.ov2 asm80.ov2: $(F0)\base.abs $(F0)\cprite.obj $(F0)\emitn.obj 
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) publics(base.abs),cprite.obj,emitn.obj to emitn.rel map print(:f2:emitn.lin)
	$(LOCATE) emitn.rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) NAME(A80O2) PRINT(:f2:emitn.map)
	del $(F0)\emitn.rel
	@find $(F2)\emitn.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\emitn.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$@ PURGE

$(F0)\asm80.ov3 asm80.ov3: $(F0)\base.abs $(F0)\cprite.obj $(F0)\inits.obj $(F0)\keywrd.abs
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) publics(base.abs),cprite.obj,inits.obj,publics(keywrd.abs) to inits.rel map print(:f2:inits.lin)
	$(LOCATE) inits.rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) NAME(A80O3) PRINT(:f2:inits.map)
	del $(F0)\inits.rel
	@find $(F2)\inits.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\inits.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
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

$(F0)\base.abs: $(F0)\$(SMALLOBJS: = .\obj\)
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	$(LINK) "&" < <<
$(SMALLOBJS: =, & 
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
$(F0)\startm.obj: startm.plm startm.ipx
$(F0)\asm1m.obj: asm1m.pl3 asm1m.ipx
$(F0)\asm2m.obj: asm2m.plm asm2m.ipx
$(F0)\asm4m.obj: asm4m.plm asm4m.ipx
$(F0)\rdsrc.obj: rdsrc.pl3 rdsrc.ipx
$(F0)\asm3m.obj: asm3m.pl3 asm3m.ipx
$(F0)\asm5m.obj: asm5m.pl3 asm5m.ipx
$(F0)\asm6m.obj: asm6m.pl3 asm6m.ipx
$(F0)\cntrlm.obj: cntrlm.pl3 cntrlm.ipx
$(F0)\asm8m.obj: asm8m.pl3 asm8m.ipx
$(F0)\emitm.obj: emitm.pl3 emitm.ipx
$(F0)\listm.obj: listm.pl3 listm.ipx
$(F0)\initm.obj: initm.plm initm.ipx
$(F0)\startb.obj: startb.plm startb.ipx
$(F0)\asm4b.obj: asm4b.plm asm4b.ipx
$(F0)\asm3b.obj: asm3b.pl3 asm3b.ipx
$(F0)\initb.obj: initb.plm initb.ipx
$(F0)\starts.obj: starts.plm starts.ipx
$(F0)\asm1n.obj: asm1n.pl3 asm1n.ipx
$(F0)\asm2n.obj: asm2n.plm asm2n.ipx
$(F0)\asm3s.obj: asm3s.pl3 asm3s.ipx
$(F0)\asm4s.obj: asm4s.plm asm4s.ipx
$(F0)\asm5n.obj: asm5n.pl3 asm5n.ipx
$(F0)\asm6n.obj: asm6n.plm asm6n.ipx
$(F0)\cntrln.obj: cntrln.pl3 cntrln.ipx
$(F0)\listn.obj: listn.pl3 listn.ipx
$(F0)\emitn.obj: emitn.pl3 emitn.ipx
$(F0)\inits.obj: inits.plm inits.ipx
$(F0)\close.obj: close.plm close.ipx
$(F0)\delete.obj: delete.plm delete.ipx
$(F0)\error.obj: error.plm error.ipx
$(F0)\exit.obj: exit.plm exit.ipx
$(F0)\globls.obj: globls.plm globls.ipx
$(F0)\globlm.obj: globlm.plm globlm.ipx
$(F0)\globlb.obj: globlb.plm globlb.ipx
$(F0)\key4.obj: key4.plm key4.ipx
$(F0)\keywrd.obj: keywrd.plm keywrd.ipx
$(F0)\load.obj: load.plm load.ipx
$(F0)\open.obj: open.plm open.ipx
$(F0)\read.obj: read.plm read.ipx
$(F0)\rescan.obj: rescan.plm rescan.ipx
$(F0)\seek.obj: seek.plm seek.ipx
$(F0)\write.obj: write.plm write.ipx
