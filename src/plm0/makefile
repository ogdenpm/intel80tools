TARGET = plm80.ov0
NAME = PL8020
STACK = 314

# common makefile info
ISIS=..\..\thames
V4=../../plm80v4/
V3=../../plm80v3/
F0=./
F3=../common/
TOOLS=..\..\tools
PEXFILE=:f3:plm.pex

OBJ =	main0.obj plm0a.obj plm0b.obj plm0c.obj plm0d.obj plm0e.obj plm0f.obj\
	:f3:movmem.obj :f3:fill.obj :f3:strcmp.obj :f3:fatal.obj\
	:f3:initf.obj :f3:openf.obj :f3:readf.obj :f3:tellf.obj\
	:f3:seekf.obj :f3:rewind.obj :f3:backup.obj :f3:closef.obj\
	:f3:creatf.obj :f3:fflush.obj :f3:fwrite.obj :f3:chain.obj\
	:f3:gitype.obj :f3:sitype.obj :f3:gisym.obj :f3:giscop.obj\
	:f3:gilit.obj :f3:silit.obj :f3:setflg.obj :f3:tstflg.obj\
	:f3:zerflg.obj :f3:clrflg.obj :f3:cpyflg.obj :f3:siflag.obj\
	:f3:tiflag.obj :f3:sidim.obj :f3:sibseo.obj :f3:giparo.obj\
	:f3:siparo.obj :f3:gidtyp.obj :f3:sidtyp.obj :f3:siintn.obj\
	:f3:gipcnt.obj :f3:sipcnt.obj :f3:sipid.obj :f3:gicond.obj\
	:f3:sicond.obj :f3:creati.obj :f3:findmi.obj plm0g.obj\
	:f3:adninf.obj :f3:fiscop.obj :f3:fi.obj :f3:lookup.obj\
	:f3:alloc.obj plm0h.obj :f3:close.obj :f3:exit.obj\
	:f3:load.obj :f3:open.obj :f3:read.obj :f3:seek.obj\
	:f3:cpytil.obj :f3:prints.obj :f3:fatlio.obj :f3:writef.obj\
	:f3:gilen.obj :f3:silen.obj :f3:sisym.obj :f3:siscop.obj\
	:f3:gilnko.obj :f3:silnko.obj :f3:allocs.obj :f3:write.obj\
	:f3:itoa.obj


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

vclean: clean
	del *.ipx $(TARGET)

verify: $(TARGET)
	fc /b $(TARGET) $(V4)$(TARGET)


# auto generated
main0.obj: main0.plm main0.ipx
plm0a.obj: plm0a.plm plm0a.ipx
plm0b.obj: plm0b.plm plm0b.ipx
plm0d.obj: plm0d.plm plm0d.ipx
plm0e.obj: plm0e.plm plm0e.ipx
plm0f.obj: plm0f.plm plm0f.ipx $(F3)info.inc
plm0g.obj: plm0g.plm plm0g.ipx
plm0h.obj: plm0h.plm plm0h.ipx
$(F3)adninf.obj: $(F3)adninf.plm $(F3)adninf.ipx
$(F3)alloc.obj: $(F3)alloc.plm $(F3)alloc.ipx
$(F3)allocs.obj: $(F3)allocs.plm $(F3)allocs.ipx
$(F3)backup.obj: $(F3)backup.plm $(F3)backup.ipx
$(F3)chain.obj: $(F3)chain.plm $(F3)chain.ipx
$(F3)close.obj: $(F3)close.plm $(F3)close.ipx
$(F3)closef.obj: $(F3)closef.plm $(F3)closef.ipx
$(F3)clrflg.obj: $(F3)clrflg.plm $(F3)clrflg.ipx
$(F3)cpyflg.obj: $(F3)cpyflg.plm $(F3)cpyflg.ipx
$(F3)creatf.obj: $(F3)creatf.plm $(F3)creatf.ipx
$(F3)creati.obj: $(F3)creati.plm $(F3)creati.ipx
$(F3)data.obj: $(F3)data.plm $(F3)data.ipx
$(F3)exit.obj: $(F3)exit.plm $(F3)exit.ipx
$(F3)fatal.obj: $(F3)fatal.plm $(F3)fatal.ipx
$(F3)fatlio.obj: $(F3)fatlio.plm $(F3)fatlio.ipx
$(F3)fflush.obj: $(F3)fflush.plm $(F3)fflush.ipx
$(F3)fi.obj: $(F3)fi.plm $(F3)fi.ipx
$(F3)findmi.obj: $(F3)findmi.plm $(F3)findmi.ipx
$(F3)fiscop.obj: $(F3)fiscop.plm $(F3)fiscop.ipx
$(F3)gicond.obj: $(F3)gicond.plm $(F3)gicond.ipx
$(F3)gidtyp.obj: $(F3)gidtyp.plm $(F3)gidtyp.ipx
$(F3)gilen.obj: $(F3)gilen.plm $(F3)gilen.ipx
$(F3)gilit.obj: $(F3)gilit.plm $(F3)gilit.ipx
$(F3)gilnko.obj: $(F3)gilnko.plm $(F3)gilnko.ipx
$(F3)giparo.obj: $(F3)giparo.plm $(F3)giparo.ipx
$(F3)gipcnt.obj: $(F3)gipcnt.plm $(F3)gipcnt.ipx
$(F3)giscop.obj: $(F3)giscop.plm $(F3)giscop.ipx
$(F3)gisym.obj: $(F3)gisym.plm $(F3)gisym.ipx
$(F3)gitype.obj: $(F3)gitype.plm $(F3)gitype.ipx
$(F3)initf.obj: $(F3)initf.plm $(F3)initf.ipx
$(F3)itoa.obj: $(F3)itoa.plm $(F3)itoa.ipx
$(F3)load.obj: $(F3)load.plm $(F3)load.ipx
$(F3)lookup.obj: $(F3)lookup.plm $(F3)lookup.ipx
$(F3)open.obj: $(F3)open.plm $(F3)open.ipx
$(F3)openf.obj: $(F3)openf.plm $(F3)openf.ipx
$(F3)prints.obj: $(F3)prints.plm $(F3)prints.ipx
$(F3)read.obj: $(F3)read.plm $(F3)read.ipx
$(F3)readf.obj: $(F3)readf.plm $(F3)readf.ipx
$(F3)rewind.obj: $(F3)rewind.plm $(F3)rewind.ipx
$(F3)seek.obj: $(F3)seek.plm $(F3)seek.ipx
$(F3)seekf.obj: $(F3)seekf.plm $(F3)seekf.ipx
$(F3)setflg.obj: $(F3)setflg.plm $(F3)setflg.ipx
$(F3)sibseo.obj: $(F3)sibseo.plm $(F3)sibseo.ipx
$(F3)sicond.obj: $(F3)sicond.plm $(F3)sicond.ipx
$(F3)sidim.obj: $(F3)sidim.plm $(F3)sidim.ipx
$(F3)sidtyp.obj: $(F3)sidtyp.plm $(F3)sidtyp.ipx
$(F3)siflag.obj: $(F3)siflag.plm $(F3)siflag.ipx
$(F3)siintn.obj: $(F3)siintn.plm $(F3)siintn.ipx
$(F3)silen.obj: $(F3)silen.plm $(F3)silen.ipx
$(F3)silit.obj: $(F3)silit.plm $(F3)silit.ipx
$(F3)silnko.obj: $(F3)silnko.plm $(F3)silnko.ipx
$(F3)siparo.obj: $(F3)siparo.plm $(F3)siparo.ipx
$(F3)sipcnt.obj: $(F3)sipcnt.plm $(F3)sipcnt.ipx
$(F3)sipid.obj: $(F3)sipid.plm $(F3)sipid.ipx
$(F3)siscop.obj: $(F3)siscop.plm $(F3)siscop.ipx
$(F3)sisym.obj: $(F3)sisym.plm $(F3)sisym.ipx
$(F3)sitype.obj: $(F3)sitype.plm $(F3)sitype.ipx
$(F3)tellf.obj: $(F3)tellf.plm $(F3)tellf.ipx
$(F3)tiflag.obj: $(F3)tiflag.plm $(F3)tiflag.ipx
$(F3)tstflg.obj: $(F3)tstflg.plm $(F3)tstflg.ipx
$(F3)write.obj: $(F3)write.plm $(F3)write.ipx
$(F3)writef.obj: $(F3)writef.plm $(F3)writef.ipx
