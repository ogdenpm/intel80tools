# common makefile info
ROOT=..\..
ISIS=$(ROOT)\thames
PLMDIR=$(ROOT)\plm80v4
ASMDIR=$(ROOT)\plm80v4
OLDPLMDIR=$(ROOT)\plm80v3
TOOLS=$(ROOT)\tools

PLMFLAGS=code optimize 
ASMFLAGS=

.SUFFIXES: .plm .pl3
.plm.obj:
	@SET ISIS_F0=.
	@SET ISIS_F1=$(PLMDIR)
	$(ISIS) :F1:PLM80 $< $(PLMFLAGS)
	@if not exist $*.obj exit /b 1

.pl3.obj:
	@SET ISIS_F0=.
	@SET ISIS_F1=$(OLDPLMDIR)
	$(ISIS) :F1:PLM80 $< $(PLMFLAGS)
	@find $*.lst "     0 PROGRAM ERROR(S)" >nul || (del $*.obj & exit /b 1)

.asm.obj:
	@SET ISIS_F0=.
	@SET ISIS_F1=$(ASMDIR)
	$(ISIS) :F1:ASM80 $< $(ASMFLAGS)
	@find $*.lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $*.obj & exit /b 1)

all: tex20.com tex21.com


tex20.COM: tex20.obj cpm.obj
	@SET ISIS_F0=.
	@SET ISIS_F1=$(PLMDIR)
	$(ISIS) :F1:LINK TEX20.OBJ,CPM.OBJ,:F1:PLM80.LIB TO $*.REL 
	$(ISIS) :F1:LOCATE $*.REL TO $*.ABS CODE(100H) PRINT($*.map) symbols publics order(code stack data) stacksize(100)
	del $*.rel
	$(TOOLS)\obj2bin $*.abs $@
	del $*.abs
	$(ROOT)\relst.pl $*.map $*.lst

tex21.COM: tex21.obj cpmio.obj cpm.obj patch.txt
	@SET ISIS_F0=.
	@SET ISIS_F1=$(PLMDIR)
	$(ISIS) :F1:LINK TEX21.OBJ,CPMIO.OBJ,CPM.OBJ,:F1:PLM80.LIB TO $*.REL 
	$(ISIS) :F1:LOCATE $*.REL TO $*.ABS CODE(100H) PRINT($*.map) symbols publics order(code stack data) stacksize(100)
	del $*.rel
	$(TOOLS)\obj2bin $*.abs $@
	del $*.abs
	$(TOOLS)\patchbin patch.txt $@
	$(ROOT)\relst.pl $*.map $*.lst cpmio.lst
	-fc /b $@ $*.cpm

clean:
	del /q *.??~ *.lst.prn *.obj *.lst *.obj makefile~ *.map

vclean: clean
	del /q tex20.com tex21.com
