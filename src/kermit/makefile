TARGET = kermit
NAME = kermit

# common makefile info
ISIS=..\..\thames
V4=../../plm80v4/
F0=./

OBJ =	md2ker.obj md2con.obj md2sen.obj md2rec.obj

.SUFFIXES: .plm .pl3
.plm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:PLM80 $< code
	@if not exist $*.obj exit /b 1

.asm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:ASM80 $< 
	@find $*.lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $*.obj & exit /b 1)

$(TARGET): $(OBJ)
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:LINK "&" < <<
$(OBJ: =,&
),:f1:system.lib,:f1:plm80.lib TO $*.rel MAP print($*.lmp)
<<NOKEEP
	$(ISIS) :F1:LOCATE $*.rel to $@ print($*.map) name($(NAME)) map
	@del $*.rel
	@(find $*.map "UNSATISFIED EXTERNAL" >NUL && exit /b 1) || exit /b 0

clean:
	del /q *.obj *lst *.map *.abs *.rel *.*~ *~ *.lmp

vclean: clean
	del /q $(TARGET)
