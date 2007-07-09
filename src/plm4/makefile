TARGET = plm80.ov4
NAME = PL8045
STACK = 266

OBJ =	pdata.obj main4.obj plm4a.obj plm4b.obj plm4c.obj\
	:f3:movmem.obj :f3:exit.obj :f3:itoa.obj :f3:fatal.obj\
	:f3:initf.obj :f3:openf.obj :f3:readf.obj :f3:tellf.obj\
	:f3:seekf.obj :f3:backup.obj :f3:closef.obj :f3:creatf.obj\
	:f3:fflush.obj :f3:fwrite.obj :f3:fread.obj :f3:chain.obj\
	:f3:deletf.obj :f3:gitype.obj :f3:gisym.obj :f3:gilnkv.obj\
	:f3:tiflag.obj :f3:gieid.obj wrec.obj :f3:lstinf.obj\
	:f3:endcom.obj :f3:putlst.obj lstsup.obj :f3:lstlin.obj\
	:f3:fill.obj :f3:close.obj :f3:delete.obj :f3:load.obj\
	:f3:open.obj :f3:read.obj :f3:seek.obj :f3:cpyTil.obj\
	:f3:prints.obj :f3:fatlIO.obj :f3:writef.obj :f3:tstflg.obj\
	:f3:gibsev.obj :f3:wrclst.obj :f3:wr2lst.obj page.obj\
	:f3:write.obj :f3:wrslst.obj 

# common makefile info
ISIS=..\..\isis
PLMDIR=..\..\plm80v4
ASMDIR=..\..\plm80v4
OLDPLMDIR=..\..\plm80v3

.SUFFIXES: .plm .pl3
.plm.obj:
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	@SET :F3:=..\common
	$(ISIS) :F1:PLM80 $(<:../common/=:F3:) code
	@if not exist $*.obj exit /b 1

.pl3.obj:
	@SET :F0:=.
	@SET :F1:=$(OLDPLMDIR)
	@SET :F3:=..\common
	$(ISIS) :F1:PLM80 $(<:../common/=:F3:) code
	@find $*.lst "     0 PROGRAM ERROR(S)" >nul || (del $*.obj & exit /b 1)

.asm.obj:
	@SET :F0:=.
	@SET :F1:=$(ASMDIR)
	@SET :F3:=..\common
	$(ISIS) :F1:ASM80 $(<:../common/=:F3:) 
	@find $*.lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $*.obj & exit /b 1)


$(TARGET): ../common/data.abs $(OBJ::f3:=../common/)
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	@SET :F3:=..\common
	$(ISIS) < <<
!IF "$(TARGET)" == "plm80"
:F1:LINK :f3:data.abs,&
!ELSE
:F1:LINK publics(:f3:data.abs),&
!ENDIF
$(OBJ: =,&
),:f1:plm80.lib TO $*.rel
:F1:LOCATE $*.rel to $@ print($*.map) code(3EA0H) name($(NAME))&
stacksize($(STACK)) map symbols publics purge
exit
<<NOKEEP
	@del $*.rel
	@(find $*.map "UNSATISFIED EXTERNAL" >NUL && exit /b 1) || exit /b 0

../common/data.abs: ../common/data.obj
	@SET :F0:=.
	@SET :F1:=$(PLMDIR)
	@SET :F3:=..\common
	$(ISIS) < <<
:F1:locate :f3:data.obj to :f3:data.abs SYMBOLS PUBLICS DATA(3800H) print(:f3:data.map)
exit
<<NOKEEP
	@if not exist $@ exit /b 1

clean:
	del *.obj *lst *.map $(TARGET)

verify: $(TARGET)
	fc /b $(TARGET) ..\..\plm80v4\$(TARGET)



