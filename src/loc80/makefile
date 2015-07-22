TARGET=locate
# common makefile info
ISIS=..\..\thames
V4=../../plm80v4/
V3=../../plm80v3/
F0=./
TOOLS=..\..\tools
PEXFILE=loc80.pex
#
# compilation and assembly rules
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
	@SET ISIS_F3=$(F3)
	$(TOOLS)\ngenpex $(PEXFILE) $<

.pl3.ipx:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(F3)
	$(TOOLS)\ngenpex $(PEXFILE) $<

LOWOBJ=loc1.obj loc2a.obj memmov.obj loc2b.obj loc3.obj loc4.obj loc4a.obj loc5a.obj loc5b.obj isis.obj
HIGHOBJ=loc6.obj loc7.obj loc8.obj loc8a.obj loc9.obj

# locate is built in two parts the value of the macro is the base address of the higher part
# it should be the same as the MEMORY address at the end of the loctmp.map file
HICODE=6656H

# locate build rules
# default target
all:	.depend
all:
	$(MAKE) $(TARGET)

$(TARGET): loctmp.abs $(HIGHOBJ)
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link "&" < <<
$(**: =,&
),:f1:system.lib,:f1:plm80.lib to $*.rel
<<NOKEEP
	$(ISIS) :F1:locate $*.rel to locate MAP PRINT($*.map) SYMBOLS PUBLICS STACKSIZE(0) CODE($(HICODE)) PURGE
	@del $*.rel $*.abs
	@(find $*.map "UNSATISFIED EXTERNAL" >NUL && exit /b 1) || exit /b 0

# intermediate target - note will have unsatisfied externals see also HICODE above
loctmp.abs: $(LOWOBJ)
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	echo $(**: =,)
	$(ISIS) :F1:link "&" < <<
$(**: =,&
),:f1:plm80.lib to $*.rel 
<<NOKEEP
	$(ISIS) :F1:locate $*.rel to $@ STACKSIZE(6CH) map print(loctmp.map)
	@del $*.rel
	@if not exist $@ exit /b 1

$(LOWOBJ:.obj=.ipx) : $(PEXFILE)
$(HIGHOBJ:.obj=.ipx) : $(PEXFILE)

.depend: loc80_all.plm
	..\..\unpack.pl
	..\..\mkdepend.pl
	date /t >.depend

clean:
	del *.obj *lst *.map *.abs *.rel *.*~ *~

vclean: clean
	del *.ipx $(TARGET) 

uclean: vclean
	del $(LOWOBJ:.obj=.plm) $(HIGHOBJ:.obj=.plm) *.asm *.pex makefile .depend

gitprep: uclean
	..\..\unpack.pl

# make this target to check the build matches the original binary
verify: all
	fc /b $(TARGET) $(V4)$(TARGET)



