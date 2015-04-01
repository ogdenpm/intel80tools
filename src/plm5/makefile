TARGET = plm80.ov5
NAME = PL8050
STACK = 190

OBJ =	main5.obj :f3:movmem.obj :f3:exit.obj :f3:itoa.obj\
	:f3:prints.obj :f3:initf.obj :f3:openf.obj :f3:closef.obj\
	:f3:creatf.obj :f3:fflush.obj :f3:fwrite.obj :f3:fread.obj\
	:f3:deletf.obj :f3:gitype.obj :f3:gisym.obj :f3:giscop.obj\
	:f3:siscop.obj :f3:gilnkv.obj :f3:tiflag.obj :f3:gieid.obj\
	:f3:gidim.obj :f3:gibseo.obj :f3:giparo.obj :f3:gidim2.obj\
	:f3:gibsev.obj :f3:gidtyp.obj :f3:giintn.obj :f3:adninf.obj\
	plm5a.obj :f3:lstinf.obj :f3:endcom.obj lstsup.obj\
	:f3:fill.obj :f3:close.obj :f3:delete.obj :f3:open.obj\
	:f3:write.obj :f3:cpytil.obj :f3:fatlIO.obj :f3:readf.obj\
	:f3:writef.obj :f3:gilen.obj :f3:tstflg.obj :f3:wr2lst.obj\
	page1.obj :f3:putlst.obj :f3:read.obj :f3:wrclst.obj\
	:f3:wrslst.obj page2.obj

# common makefile info
ISIS=..\..\thames
V4=../../plm80v4/
V3=../../plm80v3/
F0=./
F3=../common/

.SUFFIXES: .plm .pl3
.plm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(F3)
	$(ISIS) :F1:PLM80 $(<:../common/=:F3:) code
	@if not exist $*.obj exit /b 1

.pl3.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F3=$(F3)
	$(ISIS) :F1:PLM80 $(<:../common/=:F3:) code
	@find $*.lst "     0 PROGRAM ERROR(S)" >nul || (del $*.obj & exit /b 1)

.asm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(F3)
	$(ISIS) :F1:ASM80 $(<:../common/=:F3:) 
	@find $*.lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $*.obj & exit /b 1)


$(TARGET): $(F3)data.abs $(OBJ::f3:=../common/)
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(F3)
!IF "$(TARGET)" == "plm80"
	$(ISIS) :F1:LINK :f3:data.abs, "&" < <<
!ELSE
	$(ISIS) :F1:LINK publics(:f3:data.abs), "&" < <<
!ENDIF
$(OBJ: =,&
),:f1:plm80.lib TO $*.rel
<<NOKEEP
	$(ISIS) :F1:LOCATE $*.rel to $@ print($*.map) code(3EA0H) name($(NAME)) stacksize($(STACK)) map symbols publics purge
	@del $*.rel
	@(find $*.map "UNSATISFIED EXTERNAL" >NUL && exit /b 1) || exit /b 0

$(F3)data.abs: $(F3)data.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(F3)
	$(ISIS) :F1:locate :f3:data.obj to :f3:data.abs SYMBOLS PUBLICS DATA(3800H) print(:f3:data.map)
	@if not exist $@ exit /b 1

clean:
	del *.obj *lst *.map *.*~ *~ $(TARGET)

verify: $(TARGET)
	fc /b $(TARGET) $(V4)$(TARGET)

# auto generated
lstsup.obj: lstsup.plm $(F3)data.inc $(F3)openf.pex $(F3)writef.pex $(F3)itoa.pex
main5.obj: main5.plm $(F3)data.inc $(F3)flags.inc $(F3)info.inc $(F3)fread.aex \
           $(F3)gitype.pex $(F3)adninf.pex $(F3)tiflag.pex $(F3)closef.pex \
           $(F3)gibsev.pex $(F3)gibseo.pex $(F3)giscop.pex $(F3)siscop.pex \
           $(F3)gisym.pex $(F3)openf.pex $(F3)creatf.pex $(F3)giparo.pex \
           $(F3)gidtyp.pex $(F3)giintn.pex $(F3)gieid.pex $(F3)gidim.pex \
           $(F3)gidim2.pex $(F3)gilnkv.pex $(F3)fflush.pex $(F3)deletf.pex \
           $(F3)endcom.pex $(F3)exit.pex $(F3)fwrite.aex
page1.obj: page1.plm $(F3)data.inc $(F3)itoa.pex
page2.obj: page2.plm $(F3)data.inc
plm5a.obj: plm5a.plm $(F3)data.inc $(F3)prints.pex $(F3)readf.pex
$(F3)adninf.obj: $(F3)adninf.plm $(F3)gilen.pex $(F3)gitype.pex
$(F3)closef.obj: $(F3)closef.plm $(F3)close.pex $(F3)fatlio.pex
$(F3)creatf.obj: $(F3)creatf.plm $(F3)readf.pex
$(F3)deletf.obj: $(F3)deletf.plm $(F3)delete.pex
$(F3)endcom.obj: $(F3)endcom.plm $(F3)itoa.pex $(F3)prints.pex
$(F3)fatlio.obj: $(F3)fatlio.plm $(F3)prints.pex $(F3)itoa.pex $(F3)exit.pex
$(F3)fflush.obj: $(F3)fflush.plm $(F3)writef.pex
$(F3)gidtyp.obj: $(F3)gidtyp.plm $(F3)info.inc
$(F3)giparo.obj: $(F3)giparo.plm $(F3)info.inc
$(F3)initf.obj: $(F3)initf.plm $(F3)fill.aex $(F3)cpytil.pex
$(F3)itoa.obj: $(F3)itoa.plm $(F3)movmem.aex $(F3)fill.aex
$(F3)lstinf.obj: $(F3)lstinf.plm $(F3)data.inc $(F3)gibsev.pex $(F3)closef.pex
$(F3)openf.obj: $(F3)openf.plm $(F3)open.pex $(F3)fatlio.pex
$(F3)prints.obj: $(F3)prints.plm $(F3)write.pex
$(F3)readf.obj: $(F3)readf.plm $(F3)read.pex $(F3)fatlio.pex
$(F3)tiflag.obj: $(F3)tiflag.plm $(F3)tstflg.pex
$(F3)writef.obj: $(F3)writef.plm $(F3)write.pex $(F3)fatlio.pex
