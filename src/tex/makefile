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
	@find $*.lst "     0 PROGRAM ERROR(S)" >nul || (del /q $*.obj & exit /b 1)

.asm.obj:
	@SET ISIS_F0=.
	@SET ISIS_F1=$(ASMDIR)
	$(ISIS) :F1:ASM80 $< $(ASMFLAGS)
	@find $*.lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del /q $*.obj & exit /b 1)

all: tex10.com tex12.com tex21.com tex21a.com

tex10.com: tex10.obj x0100.obj
	@SET ISIS_F0=.
	@SET ISIS_F1=$(PLMDIR)
	$(ISIS) :F1:LINK TEX10.OBJ,X0100.OBJ,:F1:PLM80.LIB TO $*.REL 
	$(ISIS) :F1:LOCATE $*.REL TO $*.ABS CODE(100H) PRINT($*.map) symbols publics order(code stack data) stacksize(60)
	del /q $*.rel
	$(TOOLS)\obj2bin $*.abs $@
	del /q $*.abs
	$(TOOLS)\patchbin patch10.txt $@
	$(ROOT)\relst.pl $*.map $*.lst
	@echo **** $*p.cpm longer is expected ****
	-@fc /b $@ $*p.cpm

tex12.COM: tex12.obj cpm.obj
	@SET ISIS_F0=.
	@SET ISIS_F1=$(PLMDIR)
	$(ISIS) :F1:LINK TEX12.OBJ,CPM.OBJ,:F1:PLM80.LIB TO $*.REL 
	$(ISIS) :F1:LOCATE $*.REL TO $*.ABS CODE(100H) PRINT($*.map) symbols publics order(code stack data) stacksize(100)
	del /q $*.rel
	$(TOOLS)\obj2bin $*.abs $@
	del /q $*.abs
	$(ROOT)\relst.pl $*.map $*.lst
	@echo **** $*.cpm longer is expected ****
	-@fc /b $@ $*.cpm

tex21.COM: tex21.obj cpmio.obj cpm.obj patch21.txt
	@SET ISIS_F0=.
	@SET ISIS_F1=$(PLMDIR)
	$(ISIS) :F1:LINK TEX21.OBJ,CPMIO.OBJ,CPM.OBJ,:F1:PLM80.LIB TO $*.REL 
	$(ISIS) :F1:LOCATE $*.REL TO $*.ABS CODE(100H) PRINT($*.map) symbols publics order(code stack data) stacksize(100)
	del /q $*.rel
	$(TOOLS)\obj2bin $*.abs $@
	del /q $*.abs
	$(TOOLS)\patchbin patch21.txt $@
	$(ROOT)\relst.pl $*.map $*.lst cpmio.lst
	@echo **** $*.cpm longer is expected ****
	-@fc /b $@ $*.cpm

tex21a.COM: tex21a.obj cpmio.obj cpm.obj patch21.txt
	@SET ISIS_F0=.
	@SET ISIS_F1=$(PLMDIR)
	$(ISIS) :F1:LINK TEX21a.OBJ,CPMIO.OBJ,CPM.OBJ,:F1:PLM80.LIB TO $*.REL 
	$(ISIS) :F1:LOCATE $*.REL TO $*.ABS CODE(100H) PRINT($*.map) symbols publics order(code stack data) stacksize(100)
	del /q $*.rel
	$(TOOLS)\obj2bin $*.abs $@
	del /q $*.abs
	$(ROOT)\relst.pl $*.map $*.lst cpmio.lst

clean:
	del /q *.??~ *.lst.prn *.obj *.lst *.obj makefile~ *.map

vclean: clean
	del /q tex10.com tex12.com tex21.com
