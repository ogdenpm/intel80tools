TARGET = link
NAME = LINK
OVERLAY = link.ovl
OVERLAYNAME = LINKOVL
#
# common makefile info
ISIS=..\..\thames
V4=../../plm80v4/
V3=../../plm80v3/
F0=./
TOOLS=..\..\tools
PEXFILE=link80.pex

OBJ0 =	link.obj link1a.obj wrerr.obj memmov.obj errrpt.obj binasc.obj \
	strequ.obj skpspc.obj pstafn.obj pstfn.obj isis.obj isisen.obj
PUB1 =	link0.abs
OBJ1 =	link3.obj
PUB2 = 	link0.abs link1.abs
OBJ2 =	link3a.obj mkfnam.obj lineuc.obj
OBJOVL =linkov.obj

# compilation and assembly rules
.SUFFIXES: .plm .pl3
.plm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:PLM80 $< code
	@if not exist $*.obj exit /b 1

.plm.ipx:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(F3)
	$(TOOLS)\ngenpex $(PEXFILE) $<

.pl3.ipx:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(F3)
	$(TOOLS)\ngenpex $(PEXFILE) $<

.pl3.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	$(ISIS) :F1:PLM80 $< code
	@find $*.lst "     0 PROGRAM ERROR(S)" >nul || (del $*.obj & exit /b 1)

.asm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:ASM80 $< 
	@find $*.lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $*.obj & exit /b 1)


all: $(TARGET) $(OVERLAY)


# link build rule
$(TARGET): link0.abs link1.abs link2.abs linkov.abs
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link link0.abs,link1.abs,link2.abs,PUBLICS(linkov.abs) to $*.rel
	$(ISIS) :F1:locate "&" < <<
$*.rel TO $@ MAP PRINT($*.map) SYMBOLS PURGE PUBLICS &
STACKSIZE(0) COLUMNS(3) NAME(LINK) PURGE
<<NOKEEP
	@del $*.rel
	@(find $*.map "UNSATISFIED EXTERNAL" >NUL && exit /b 1) || exit /b 0

$(OVERLAY): link0.abs linkov.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link publics(link0.abs),linkov.obj,:F1:system.lib,:f1:plm80.lib To linkov.rel
	$(ISIS) :F1:locate "&" < <<
linkov.rel to $@ MAP PRINT(linkov.map) SYMBOLS PUBLICS&
code(48DEH) stacksize(0) purge name(LINKOVL)
<<NOKEEP
	@del linkov.rel
	@(find $*.map "UNSATISFIED EXTERNAL" >NUL && exit /b 1) || exit /b 0


link0.abs: $(OBJ0)
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link "&" < <<
$(**: =,&
),:f1:plm80.lib to $*.rel
<<NOKEEP
	$(ISIS)	:F1:locate $*.rel to $@ SYMBOLS PUBLICS STACKSIZE(84) COLUMNS(3)
	@del $*.rel
	@if not exist $@ exit /b 1

link1.abs: $(PUB1) $(OBJ1)
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link publics($(PUB1: =,)),$(OBJ1: =,),:F1:plm80.lib to $*.rel MAP
	$(ISIS) :F1:locate $*.rel to $@ SYMBOLS PUBLICS CODE(48DEH) STACKSIZE(0)
#	@del $*.rel
	@if not exist $@ exit /b 1

link2.abs: $(OBJ2)
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link "&" < <<
publics($(PUB2: =,)),$(OBJ2: =,),&
:F1:system.lib(delete),:f1:system.lib to $*.rel MAP
<<NOKEEP
	$(ISIS) :f1:locate $*.rel to $@ SYMBOLS PUBLICS CODE(629DH) STACKSIZE(0)
	@del $*.rel
	@if not exist $@ exit /b 1

linkov.abs: $(OBJOVL)
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:locate $*.obj TO $@ SYMBOLS PUBLICS CODE(48DEH) STACKSIZE(0)
	@if not exist $@ exit /b 1

$(OBJ0:.obj=.ipx) : $(PEXFILE)
$(OBJ1:.obj=.ipx) : $(PEXFILE)
$(OBJ2:.obj=.ipx) : $(PEXFILE)
$(OBJOVL:.obj=.ipx) : $(PEXFILE)


clean:
	del *.obj *lst *.map *.*~ *~ *.abs *.rel

vclean: clean
	del *.ipx $(TARGET) $(OVERLAY)

verify: all
	$(TOOLS)\diffbin $(TARGET) $(V4)$(TARGET)
	$(TOOLS)\diffbin $(OVERLAY) $(V4)$(OVERLAY)



# auto generated
errrpt.obj: errrpt.plm errrpt.ipx
isis.obj: isis.plm isis.ipx
lineuc.obj: lineuc.plm lineuc.ipx
link.obj: link.plm link.ipx
link1a.obj: link1a.plm link1a.ipx
link3.obj: link3.plm link3.ipx
link3a.obj: link3a.plm link3a.ipx
linkov.obj: linkov.plm linkov.ipx
mkfnam.obj: mkfnam.plm mkfnam.ipx
pstafn.obj: pstafn.plm pstafn.ipx
wrerr.obj: wrerr.plm wrerr.ipx
