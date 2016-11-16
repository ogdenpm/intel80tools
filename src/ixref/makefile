
# common makefile info
ISIS=..\..\thames
PLMDIR=..\..\plm80v4
ASMDIR=..\..\plm80v4
OLDPLMDIR=..\..\plm80v3
TOOLS=..\..\tools
#PLMFLAGS=optimize symbols code debug
PLMFLAGS=optimize code
OBJS = ixref.obj ixref2.obj ixref3.obj ixref4.obj ixref5.obj ixref6.obj ixref7.obj  

.SUFFIXES: .plm
.plm.obj:
	@SET ISIS_F0=.
	@SET ISIS_F1=$(PLMDIR)
	$(ISIS) :F1:PLM80 $< $(PLMFLAGS)
	@if not exist $*.obj exit /b 1

.plm.ipx:
	@SET ISIS_F0=.
	$(TOOLS)\ngenpex ixref.pex $<

	
all: .extract

all:
	$(MAKE) ixref

ixref: $(OBJS)
	@SET ISIS_F0=.
	@SET ISIS_F1=$(PLMDIR)
	@SET ISIS_F2=$(OLDPLMDIR)
	$(ISIS) :F1:LINK $(OBJS: =,),:F2:system.lib,:F1:PLM80.LIB TO $*.REL 
	$(ISIS) :F1:LOCATE $*.REL TO $* CODE(3880H) PRINT($*.map)  SYMBOLS PUBLICS PURGE order(code stack data) stacksize(136)
	..\..\tools\diffbin ixref ixref.ref >diffs
	..\..\relst.pl $*.map $(OBJS:.obj=.lst)

*.ipx: ixref.pex
*.obj: makefile

.extract: ixref_all.plm
	..\..\unpack.pl
	date /t >.extract

clean:
	del /q *.obj *.tmp *.lst *.prn *.rel *.map

vclean: clean
	del /q ixref *.ipx *.pex .extract

gitprep: vclean
	..\..\unpack.pl
	del /q ??????.plm *.pex

# auto generated
ixref.obj: ixref.plm ixref.ipx
ixref2.obj: ixref2.plm ixref2.ipx
ixref3.obj: ixref3.plm ixref3.ipx
ixref4.obj: ixref4.plm ixref4.ipx
ixref6.obj: ixref6.plm ixref6.ipx
ixref7.obj: ixref7.plm ixref7.ipx
