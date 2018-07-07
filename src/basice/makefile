# common makefile info
ISIS=..\..\thames
PLMDIR=..\..\plm80v30
ASMDIR=..\..\plm80v4
OLDPLMDIR=..\..\plm80v30

PLMFLAGS=code 
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
	@SET ISIS_TRACE=4
	$(ISIS) :F1:ASM80 $< $(ASMFLAGS)
	@find $*.lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $*.obj & exit /b 1)

