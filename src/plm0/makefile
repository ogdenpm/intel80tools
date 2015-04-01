TARGET = plm80.ov0
NAME = PL8020
STACK = 314

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
main0.obj: main0.plm $(F3)lex.inc $(F3)data.inc $(F3)initf.pex $(F3)openf.pex \
           $(F3)seekf.pex $(F3)tellf.pex $(F3)backup.pex $(F3)closef.pex \
           $(F3)chain.pex
plm0a.obj: plm0a.plm cclass.inc $(F3)lex.inc toks.inc $(F3)data.inc \
           $(F3)movmem.aex $(F3)openf.pex $(F3)fflush.pex $(F3)rewind.pex \
           $(F3)fwrite.aex $(F3)fatal.pex $(F3)creatf.pex
plm0b.obj: plm0b.plm cclass.inc $(F3)err.inc $(F3)lex.inc $(F3)info.inc \
           $(F3)data.inc $(F3)fatal.pex $(F3)movmem.aex $(F3)fill.aex \
           $(F3)initf.pex $(F3)openf.pex $(F3)tellf.pex $(F3)backup.pex \
           $(F3)closef.pex $(F3)lookup.pex $(F3)fiscop.pex $(F3)ftlerr.pex \
           $(F3)creati.pex $(F3)sicond.pex $(F3)gicond.pex $(F3)strcmp.pex
plm0d.obj: plm0d.plm $(F3)err.inc $(F3)lex.inc $(F3)data.inc $(F3)sitype.pex \
           $(F3)initf.pex $(F3)openf.pex $(F3)closef.pex $(F3)seekf.pex
plm0e.obj: plm0e.plm toks.inc $(F3)err.inc $(F3)lex.inc cclass.inc $(F3)flags.inc \
           $(F3)info.inc $(F3)gilit.pex $(F3)lookup.pex $(F3)fi.pex \
           $(F3)sitype.pex $(F3)gitype.pex $(F3)giscop.pex $(F3)tiflag.pex \
           $(F3)siflag.pex $(F3)sidim.pex $(F3)siparo.pex $(F3)cpyflg.pex \
           $(F3)zerflg.pex $(F3)silit.pex $(F3)sibseo.pex $(F3)findmi.pex \
           $(F3)giparo.pex $(F3)gidtyp.pex $(F3)sidtyp.pex $(F3)sipcnt.pex \
           $(F3)siintn.pex $(F3)sipid.pex $(F3)ftlerr.pex $(F3)tstflg.pex \
           $(F3)clrflg.pex $(F3)setflg.pex $(F3)fiscop.pex $(F3)creati.pex
plm0f.obj: plm0f.plm $(F3)lex.inc $(F3)err.inc toks.inc $(F3)data.inc plm0.inc \
           $(F3)flags.inc $(F3)info.inc $(F3)ftlerr.pex $(F3)adninf.pex \
           $(F3)sipid.pex $(F3)fiscop.pex $(F3)tiflag.pex $(F3)siflag.pex \
           $(F3)giscop.pex $(F3)gisym.pex $(F3)lookup.pex $(F3)alloc.pex \
           $(F3)creati.pex $(F3)gipcnt.pex
plm0g.obj: plm0g.plm $(F3)movmem.aex $(F3)allocs.pex
plm0h.obj: plm0h.plm $(F3)data.inc $(F3)fill.aex $(F3)movmem.aex
$(F3)adninf.obj: $(F3)adninf.plm $(F3)gilen.pex $(F3)gitype.pex
$(F3)alloc.obj: $(F3)alloc.plm $(F3)err.inc $(F3)ftlerr.pex
$(F3)allocs.obj: $(F3)allocs.plm $(F3)err.inc $(F3)ftlerr.pex
$(F3)chain.obj: $(F3)chain.plm $(F3)fatlio.pex $(F3)initf.pex $(F3)load.pex
$(F3)closef.obj: $(F3)closef.plm $(F3)close.pex $(F3)fatlio.pex
$(F3)creatf.obj: $(F3)creatf.plm $(F3)readf.pex
$(F3)creati.obj: $(F3)creati.plm $(F3)err.inc $(F3)info.inc $(F3)fill.aex \
                 $(F3)alloc.pex $(F3)ftlerr.pex $(F3)silnko.pex $(F3)sitype.pex \
                 $(F3)silen.pex $(F3)siscop.pex $(F3)sisym.pex
$(F3)fatal.obj: $(F3)fatal.plm $(F3)exit.pex $(F3)prints.pex
$(F3)fatlio.obj: $(F3)fatlio.plm $(F3)prints.pex $(F3)itoa.pex $(F3)exit.pex
$(F3)fflush.obj: $(F3)fflush.plm $(F3)writef.pex
$(F3)fi.obj: $(F3)fi.plm $(F3)fiscop.pex
$(F3)findmi.obj: $(F3)findmi.plm $(F3)flags.inc $(F3)gilnko.pex $(F3)giparo.pex \
                 $(F3)tiflag.pex
$(F3)fiscop.obj: $(F3)fiscop.plm $(F3)tiflag.pex $(F3)giscop.pex $(F3)silnko.pex \
                 $(F3)gilnko.pex $(F3)gitype.pex $(F3)flags.inc $(F3)info.inc
$(F3)gidtyp.obj: $(F3)gidtyp.plm $(F3)info.inc
$(F3)giparo.obj: $(F3)giparo.plm $(F3)info.inc
$(F3)gipcnt.obj: $(F3)gipcnt.plm $(F3)info.inc
$(F3)initf.obj: $(F3)initf.plm $(F3)fill.aex $(F3)cpytil.pex
$(F3)itoa.obj: $(F3)itoa.plm $(F3)movmem.aex $(F3)fill.aex
$(F3)lookup.obj: $(F3)lookup.plm $(F3)allocs.pex $(F3)movmem.aex $(F3)strcmp.pex \
                 $(F3)alloc.pex
$(F3)openf.obj: $(F3)openf.plm $(F3)open.pex $(F3)fatlio.pex
$(F3)prints.obj: $(F3)prints.plm $(F3)write.pex
$(F3)readf.obj: $(F3)readf.plm $(F3)read.pex $(F3)fatlio.pex
$(F3)rewind.obj: $(F3)rewind.plm $(F3)seekf.pex
$(F3)seekf.obj: $(F3)seekf.plm $(F3)seek.pex $(F3)fatlio.pex
$(F3)sidtyp.obj: $(F3)sidtyp.plm $(F3)info.inc
$(F3)siflag.obj: $(F3)siflag.plm $(F3)setflg.pex
$(F3)siparo.obj: $(F3)siparo.plm $(F3)info.inc
$(F3)sipcnt.obj: $(F3)sipcnt.plm $(F3)info.inc
$(F3)tellf.obj: $(F3)tellf.plm $(F3)seek.pex $(F3)fatlio.pex
$(F3)tiflag.obj: $(F3)tiflag.plm $(F3)tstflg.pex
$(F3)writef.obj: $(F3)writef.plm $(F3)write.pex $(F3)fatlio.pex
