TARGET = plm80.ov5
NAME = PL8050
STACK = 190

# common makefile info
ISIS=..\..\thames
V4=../../plm80v4/
V3=../../plm80v3/
F0=./
F3=../common/
TOOLS=..\..\tools
PEXFILE=:f3:plm.pex

OBJ =	main5.obj :f3:movmem.obj :f3:exit.obj :f3:itoa.obj\
	:f3:prints.obj :f3:initf.obj :f3:openf.obj :f3:closef.obj\
	:f3:creatf.obj :f3:fflush.obj :f3:fwrite.obj :f3:fread.obj\
	:f3:deletf.obj :f3:gitype.obj :f3:gisym.obj :f3:giscop.obj\
	:f3:siscop.obj :f3:gilnkv.obj :f3:tiflag.obj :f3:gieid.obj\
	:f3:gidim.obj :f3:gibseo.obj :f3:giparo.obj :f3:gidim2.obj\
	:f3:gibsev.obj :f3:gidtyp.obj :f3:giintn.obj :f3:adninf.obj\
	plm5a.obj :f3:lstinf.obj :f3:endcom.obj lstsp5.obj\
	:f3:fill.obj :f3:close.obj :f3:delete.obj :f3:open.obj\
	:f3:write.obj :f3:cpytil.obj :f3:fatlIO.obj :f3:readf.obj\
	:f3:writef.obj :f3:gilen.obj :f3:tstflg.obj :f3:wr2lst.obj\
	page1.obj :f3:putlst.obj :f3:read.obj :f3:wrclst.obj\
	:f3:wrslst.obj page2.obj


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

.plm.ipx:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(F3)
	$(TOOLS)\ngenpex $(PEXFILE) $(<:../common/=:F3:)

.pl3.ipx:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(F3)
	$(TOOLS)\ngenpex $(PEXFILE) $(<:../common/=:F3:)

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

IPX = 	$(OBJ:.obj=.ipx)
$(IPX::F3:=../common/): $(PEXFILE::f3:=../common/)

clean:
	del *.obj *lst *.map *.*~ *~

vclean:	clean
	del *.ipx $(TARGET)

verify: $(TARGET)
	fc /b $(TARGET) $(V4)$(TARGET)

# auto generated
lstsp5.obj: lstsp5.plm lstsp5.ipx $(F3)lstsup.cmn
main5.obj: main5.plm main5.ipx
page1.obj: page1.plm page1.ipx
page2.obj: page2.plm page2.ipx
plm5a.obj: plm5a.plm plm5a.ipx
$(F3)adninf.obj: $(F3)adninf.plm $(F3)adninf.ipx
$(F3)close.obj: $(F3)close.plm $(F3)close.ipx
$(F3)closef.obj: $(F3)closef.plm $(F3)closef.ipx
$(F3)creatf.obj: $(F3)creatf.plm $(F3)creatf.ipx
$(F3)data.obj: $(F3)data.plm $(F3)data.ipx
$(F3)delete.obj: $(F3)delete.plm $(F3)delete.ipx
$(F3)deletf.obj: $(F3)deletf.plm $(F3)deletf.ipx
$(F3)endcom.obj: $(F3)endcom.plm $(F3)endcom.ipx
$(F3)exit.obj: $(F3)exit.plm $(F3)exit.ipx
$(F3)fatlio.obj: $(F3)fatlio.plm $(F3)fatlio.ipx
$(F3)fflush.obj: $(F3)fflush.plm $(F3)fflush.ipx
$(F3)gibseo.obj: $(F3)gibseo.plm $(F3)gibseo.ipx
$(F3)gibsev.obj: $(F3)gibsev.plm $(F3)gibsev.ipx
$(F3)gidim.obj: $(F3)gidim.plm $(F3)gidim.ipx
$(F3)gidim2.obj: $(F3)gidim2.plm $(F3)gidim2.ipx
$(F3)gidtyp.obj: $(F3)gidtyp.plm $(F3)gidtyp.ipx
$(F3)gieid.obj: $(F3)gieid.plm $(F3)gieid.ipx
$(F3)giintn.obj: $(F3)giintn.plm $(F3)giintn.ipx
$(F3)gilen.obj: $(F3)gilen.plm $(F3)gilen.ipx
$(F3)gilnkv.obj: $(F3)gilnkv.plm $(F3)gilnkv.ipx
$(F3)giparo.obj: $(F3)giparo.plm $(F3)giparo.ipx
$(F3)giscop.obj: $(F3)giscop.plm $(F3)giscop.ipx
$(F3)gisym.obj: $(F3)gisym.plm $(F3)gisym.ipx
$(F3)gitype.obj: $(F3)gitype.plm $(F3)gitype.ipx
$(F3)initf.obj: $(F3)initf.plm $(F3)initf.ipx
$(F3)itoa.obj: $(F3)itoa.plm $(F3)itoa.ipx
$(F3)lstinf.obj: $(F3)lstinf.plm $(F3)lstinf.ipx
$(F3)open.obj: $(F3)open.plm $(F3)open.ipx
$(F3)openf.obj: $(F3)openf.plm $(F3)openf.ipx
$(F3)prints.obj: $(F3)prints.plm $(F3)prints.ipx
$(F3)read.obj: $(F3)read.plm $(F3)read.ipx
$(F3)readf.obj: $(F3)readf.plm $(F3)readf.ipx
$(F3)siscop.obj: $(F3)siscop.plm $(F3)siscop.ipx
$(F3)tiflag.obj: $(F3)tiflag.plm $(F3)tiflag.ipx
$(F3)tstflg.obj: $(F3)tstflg.plm $(F3)tstflg.ipx
$(F3)wr2lst.obj: $(F3)wr2lst.plm $(F3)wr2lst.ipx
$(F3)write.obj: $(F3)write.plm $(F3)write.ipx
$(F3)writef.obj: $(F3)writef.plm $(F3)writef.ipx
$(F3)wrslst.obj: $(F3)wrslst.plm $(F3)wrslst.ipx
