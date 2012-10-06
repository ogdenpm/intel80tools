TARGET = lib
NAME = lib
STACK = 90

OBJ =	lib.obj lib1.obj isis1.obj isisa.obj isis2.obj lib3.obj lib4.obj

# common makefile info
ISIS=..\..\thames
PLMDIR=..\..\plm80v4
ASMDIR=..\..\plm80v4
OLDPLMDIR=..\..\plm80v3

.SUFFIXES: .plm .pl3
.plm.obj:
	@SET ISIS_F0=.
	@SET ISIS_F1=$(PLMDIR)
	@SET ISIS_F3=..\common
	$(ISIS) :F1:PLM80 $(<:../common/=:F3:) code
	@if not exist $*.obj exit /b 1

.pl3.obj:
	@SET ISIS_F0=.
	@SET ISIS_F1=$(OLDPLMDIR)
	@SET ISIS_F3=..\common
	$(ISIS) :F1:PLM80 $(<:../common/=:F3:) code
	@find $*.lst "     0 PROGRAM ERROR(S)" >nul || (del $*.obj & exit /b 1)

.asm.obj:
	@SET ISIS_F0=.
	@SET ISIS_F1=$(ASMDIR)
	@SET ISIS_F3=..\common
	$(ISIS) :F1:ASM80 $(<:../common/=:F3:) 
	@find $*.lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $*.obj & exit /b 1)


$(TARGET): $(OBJ::f3:=../common/)
	@SET ISIS_F0=.
	@SET ISIS_F1=$(PLMDIR)
	@SET ISIS_F3=..\common
	$(ISIS) :F1:LINK "&" < <<
$(OBJ: =,&
),:f1:plm80.lib TO $*.rel MAP print($*.lmp)
<<NOKEEP
	$(ISIS) :F1:LOCATE $*.rel to $@ print($*.map) code(3680H) name($(NAME)) stacksize($(STACK)) map symbols publics purge
	@del $*.rel
	@(find $*.map "UNSATISFIED EXTERNAL" >NUL && exit /b 1) || exit /b 0

clean:
	del *.obj *lst *.map $(TARGET)

verify: $(TARGET)
	..\..\diffbin $(TARGET) ..\..\plm80v4\$(TARGET)

