TARGET = lib
NAME = lib
STACK = 90

# common makefile info
ISIS=..\..\thames
V4=../../plm80v4/
V3=../../plm80v3/
F0=./
TOOLS=..\..\tools
PEXFILE=lib80.pex

OBJ =	lib.obj lib1.obj isis1.obj isisa.obj isis2.obj lib3.obj lib4.obj

.SUFFIXES: .plm .pl3
.plm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:PLM80 $< code
	@if not exist $*.obj exit /b 1

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

.plm.ipx:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(TOOLS)\ngenpex $(PEXFILE) $<

.pl3.ipx:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(TOOLS)\ngenpex $(PEXFILE) $<

all:	.depend
all:
	$(MAKE) $(TARGET)

$(TARGET): $(OBJ)
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:LINK "&" < <<
$(OBJ: =,&
),:f1:plm80.lib TO $*.rel MAP print($*.lmp)
<<NOKEEP
	$(ISIS) :F1:LOCATE $*.rel to $@ print($*.map) code(3680H) name($(NAME)) stacksize($(STACK)) map symbols publics purge
	@del $*.rel
	@(find $*.map "UNSATISFIED EXTERNAL" >NUL && exit /b 1) || exit /b 0

$(OBJ:.obj=.ipx) : $(PEXFILE)

.depend: lib80_all.plm
	..\..\unpack.pl
	..\..\mkdepend.pl
	date /t >.depend

clean:
	del *.obj *lst *.map *.abs *.rel *.*~ *~ *.lmp

vclean: clean
	del *.ipx $(TARGET) 

uclean: vclean
	del ??????.*  makefile .depend

gitprep: uclean
	..\..\unpack.pl

verify: all
	$(TOOLS:/=\)\diffbin $(TARGET) $(V4)$(TARGET)

# auto generated
isis1.obj: isis1.plm isis1.ipx
isis2.obj: isis2.plm isis2.ipx
lib.obj: lib.plm lib.ipx
lib1.obj: lib1.plm lib1.ipx
