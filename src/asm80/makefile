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
ROOTOBJS = globls.obj starts.obj asm1n.obj asm2n.obj\
	rdsrc.obj asm3s.obj asm4s.obj asm5n.obj\
	asm6n.obj pcktok.obj

MACROOBJS = globlm.obj startm.obj pcktok.obj asm1m.obj\
	asm2m.obj asm4m.obj rdsrc.obj asm3m.obj\
	asm5m.obj asm6m.obj cntrlm.obj asm8m.obj\
	emitm.obj listm.obj initm.obj

BIGOBJS = globlb.obj startb.obj pcktok.obj asm1n.obj\
	asm2n.obj asm4b.obj rdsrc.obj asm3b.obj\
	asm5n.obj asm6n.obj cntrln.obj emitn.obj\
	listn.obj initb.obj

RELEASETARGETS = asm80 asm80.ov0 asm80.ov1 asm80.ov2 asm80.ov3 asm80.ov4 asm80.ov5 asxref

# compilation and assembly rules
.SUFFIXES: .plm .asm

.plm{$(F0)}.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(NGENPEX) :f3:$(PEXFILE) :f3:$<
	$(PLM) :f3:$< $(PLMFLAGS)
	@find $(F2)\$(*B).lst "     0 PROGRAM ERROR(S)" >nul || (del $@ & exit /b 1)

.asm{$(F0)}.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
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
asm1m.plm asm1n.plm: asm1.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sMACRO -o asm1m.plm $(**F)
	$(PLMPP) -o asm1n.plm $(**F)

asm5m.plm asm5n.plm: asm5.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sMACRO -o asm5m.plm $(**F)
	$(PLMPP) -o asm5n.plm $(**F)

asm6m.plm asm6n.plm: asm6.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sMACRO -o asm6m.plm $(**F)
	$(PLMPP) -o asm6n.plm $(**F)

asm2m.plm asm2n.plm: asm2.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sMACRO -o asm2m.plm $(**F)
	$(PLMPP) -o asm2n.plm $(**F)

asm4m.plm asm4b.plm asm4s.plm: asm4.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sMACRO -o asm4m.plm $(**F)
	$(PLMPP) -sBIG -o asm4b.plm $(**F)
	$(PLMPP) -sSMALL -o asm4s.plm $(**F)

cntrlm.plm cntrln.plm: cntrl.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sMACRO -o cntrlm.plm $(**F)
	$(PLMPP) -o cntrln.plm $(**F)

listm.plm listn.plm: list.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sMACRO -o listm.plm $(**F)
	$(PLMPP) -o listn.plm $(**F)

emitm.plm emitn.plm: emit.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sMACRO -o emitm.plm $(**F)
	$(PLMPP) -o emitn.plm $(**F)

asm3m.plm asm3b.plm asm3s.plm: asm3.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sMACRO -o asm3m.plm $(**F)
	$(PLMPP) -sBIG -o asm3b.plm $(**F)
	$(PLMPP) -sSMALL -o asm3s.plm $(**F)

starts.plm startm.plm startb.plm: start.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sMACRO -o startm.plm $**
	$(PLMPP) -sBIG -o startb.plm $**
	$(PLMPP) -sSMALL -o starts.plm $**

globls.plm globlm.plm globlb.plm: globl.plx
	SET ISIS_F0=$(F3)
	$(PLMPP) -sMACRO -o globlm.plm $**
	$(PLMPP) -sBIG -o globlb.plm $**
	$(PLMPP) -sSMALL -o globls.plm $**

initm.plm initb.plm inits.plm: init.plx
	@SET ISIS_F0=$(F3)
	$(PLMPP) -sMACRO -o initm.plm $(**F)
	$(PLMPP) -sBIG -o initb.plm $(**F)
	$(PLMPP) -sSMALL -o inits.plm $(**F)

# build rules to make the program and overlays
# creates the debug file versions in $(F0) and purged versions in the $(F3) directory
$(F0)\asm80 asm80: $(F0)\root.abs $(F0)\asm80.ov0 $(F0)\asm80.ov1 $(F0)\asm80.ov2 $(F0)\asm80.ov3 $(F0)\keyn.abs 
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) root.abs,asm80.ov3,keyn.abs,publics(asm80.ov0,asm80.ov1,asm80.ov2) to asm80.rel map print(:f2:asm80.lin)
	$(LOCATE) asm80.rel to $(@F) $(LOCATEOPT) STACKSIZE(0) NAME(ASM80) PRINT(:f2:asm80.map)
	del $(F0)\asm80.rel
	@find $(F2)\asm80.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asm80.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$@ PURGE

$(F0)\asm80.ov0 asm80.ov0: $(F0)\cprite.obj $(F0)\cntrln.obj $(F0)\root.abs
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) cprite.obj,cntrln.obj,publics(root.abs) to asmov0.rel map print(:f2:asmov0.lin)
	$(LOCATE) asmov0.rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) NAME(A80O0) PRINT(:f2:asmov0.map)
	del $(F0)\asmov0.rel
	@find $(F2)\asmov0.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov0.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$(@F) PURGE


$(F0)\asm80.ov1 asm80.ov1: $(F0)\cprite.obj $(F0)\listn.obj $(F0)\root.abs
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) cprite.obj,listn.obj,publics(root.abs),:F1:plm80.lib to asmov1.rel map print(:f2:asmov1.lin)
	$(LOCATE) asmov1.rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) NAME(A80O1) PRINT(:f2:asmov1.map)
	del $(F0)\asmov1.rel
	@find $(F2)\asmov1.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov1.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$@ PURGE


$(F0)\asm80.ov2 asm80.ov2: $(F0)\cprite.obj $(F0)\emitn.obj $(F0)\root.abs 
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) cprite.obj,emitn.obj,publics(root.abs) to asmov2.rel map print(:f2:asmov2.lin)
	$(LOCATE) asmov2.rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) NAME(A80O2) PRINT(:f2:asmov2.map)
	del $(F0)\asmov2.rel
	@find $(F2)\asmov2.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov2.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$@ PURGE

$(F0)\asm80.ov3 asm80.ov3: $(F0)\cprite.obj $(F0)\inits.obj $(F0)\root.abs $(F0)\keyn.abs
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) cprite.obj,inits.obj,publics(root.abs,keyn.abs) to asmov3.rel map print(:f2:asmov3.lin)
	$(LOCATE) asmov3.rel to $(@F) $(LOCATEOPT) CODE($(OVERLAY)) STACKSIZE(0) NAME(A80O3) PRINT(:f2:asmov3.map)
	del $(F0)\asmov3.rel
	@find $(F2)\asmov3.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov3.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$@ PURGE


$(F0)\asm80.ov4 asm80.ov4: $(F0)\$(MACROOBJS: = .\obj\) $(F0)\keym.obj
# initially set F0 to object directory to avoid losts of :f3: prefixes
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) "&" < <<
$(MACROOBJS: =, & 
),:f1:system.lib,:f1:plm80.lib,keym.obj to asmov4.rel map print(:f2:asmov4.lin)
<<NOKEEP
	$(LOCATE) asmov4.rel to $(@F) NAME(A80O4) "&" < <<
$(LOCATEOPT) code(3780h) STACKSIZE(144) print(:f2:asmov4.map)
exit
<<NOKEEP
	@find $(F2)\asmov4.lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\asmov4.map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)
	$(LOCATE) $@ to :f3:$@ PURGE


$(F0)\asm80.ov5 asm80.ov5: $(F0)\$(BIGOBJS: = .\obj\) $(F0)\keyn.obj
# initially set F0 to object directory to avoid losts of :f3: prefixes
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LINK) "&" < <<
$(BIGOBJS: =, &
),:f1:system.lib,:f1:plm80.lib,keyn.obj to asmov5.rel map print(:f2:asmov5.lin)
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
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	@SET ISIS_F3=$(F3)
	$(LOCATE) asxref.obj to $(@F) $(LOCATEOPT) code(3680h) order(code,stack,data) stacksize(32) print(:f2:asxref.map)
	$(LOCATE) $@ to :f3:$@ PURGE

# rules for intermediate files

$(F0)\root.abs: $(F0)\$(ROOTOBJS: = .\obj\)
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	$(LINK) "&" < <<
$(ROOTOBJS: =, & 
),:f1:system.lib,:f1:plm80.lib to $(*B).rel print(:f2:$(*B).lin)
<<NOKEEP
	$(LOCATE) $(*B).rel to $(@F) $(LOCATEOPT) STACKSIZE(120) print(:f2:$(*B).map)
	del $*.rel
	@find $(F2)\$(*B).lin "UNRESOLVED EXTERNAL" >nul || (del $@ & exit /b 1)


$(F0)\keyn.abs: $(F0)\keyn.obj $(F0)\edata.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F2=$(F2)
	$(LINK) keyn.obj,edata.obj to $(*B).rel map print(:f2:$(*B).lin)
	$(LOCATE) $(*B).rel to $(@F) $(LOCATEOPT) CODE(075A2h) STACKSIZE(0) print(:f2:$(*B).map)
	del $*.rel
	@find $(F2)\$(*B).lin "LINK MAP" >nul || (del $@ & exit /b 1)
	@(find $(F2)\$(*B).map "UNSATISFIED EXTERNAL" >nul && (del $@ & exit /b 1)) || exit /b 0)

# pex file and makefile dependencies
$(F0)\*.obj: makefile $(PEXFILE)

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

