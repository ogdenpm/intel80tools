TARGET=locate
ISIS=..\..\thames
V4=../../plm80v4/
V3=../../plm80v3/
F0=./
F3=../common/
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

clean:
	del *.obj *lst *.map *.abs *.rel $(TARGET) *.*~ *~

# make this target to check the build matches the original binary
verify: locate
	fc /b locate $(V4)$(TARGET)



# auto generated
loc1.obj: loc1.plm isis.ext loc.lit loc2a.ext loc3.ext loc4.ext loc5.ext loc6.ext \
          loc7.ext loc.bas
loc2a.obj: loc2a.plm isis.ext
loc2b.obj: loc2b.plm isis.ext loc.lit loc1.ext loc3.ext loc4.ext loc5.ext \
           loc6.ext loc.bas
loc3.obj: loc3.plm isis.ext loc.lit loc1.ext loc2a.ext loc2b.ext loc4.ext \
          loc5.ext loc6.ext loc7.ext loc8.ext loc9.ext loc.bas
loc4.obj: loc4.plm isis.ext loc.lit loc1.ext loc2a.ext loc3.ext loc5.ext loc6.ext \
          loc.bas
loc6.obj: loc6.plm isis.ext loc.lit loc1.ext loc2a.ext loc2b.ext loc3.ext \
          loc4.ext loc5.ext loc7.ext loc8.ext loc9.ext loc.bas
loc7.obj: loc7.plm isis.ext loc.lit loc1.ext loc2a.ext loc2b.ext loc3.ext \
          loc4.ext loc5.ext loc6.ext loc8.ext loc9.ext loc.bas
loc8.obj: loc8.plm loc.lit loc9.ext
loc8a.obj: loc8a.plm loc.lit loc9.ext
