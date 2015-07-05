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

clean:
	del *.obj *lst *.map *.abs *.rel *.*~ *~

vclean: clean
	del *.ipx $(TARGET) 

uclean: vclean
	del $(LOWOBJ:.obj=.plm) $(HIGHOBJ:.obj=.plm) *.asm *.pex makefile

# make this target to check the build matches the original binary
verify: locate
	fc /b locate $(V4)$(TARGET)



# auto generated
isis.obj: isis.plm isis.ipx
loc1.obj: loc1.plm loc1.ipx
loc2a.obj: loc2a.plm loc2a.ipx
loc2b.obj: loc2b.plm loc2b.ipx
loc3.obj: loc3.plm loc3.ipx
loc4.obj: loc4.plm loc4.ipx
loc4a.obj: loc4a.plm loc4a.ipx
loc5a.obj: loc5a.plm loc5a.ipx
loc6.obj: loc6.plm loc6.ipx
loc7.obj: loc7.plm loc7.ipx
loc8.obj: loc8.plm loc8.ipx
loc8a.obj: loc8a.plm loc8a.ipx
