TARGET = plm80
NAME = PL8010
STACK = 136

OBJ =	main.obj plma.obj plmb.obj :f3:memchk.obj\
	:f3:movmem.obj :f3:fill.obj plmc.obj :f3:exit.obj\
	:f3:prints.obj :f3:fatal.obj :f3:fatlio.obj :f3:initf.obj\
	:f3:openf.obj :f3:readf.obj :f3:chain.obj :f3:siflag.obj\
	:f3:sidtyp.obj :f3:sipcnt.obj :f3:sibin.obj :f3:creati.obj\
	:f3:lookup.obj plmd.obj :f3:load.obj :f3:open.obj\
	:f3:read.obj :f3:write.obj :f3:strcmp.obj :f3:cpytil.obj\
	:f3:itoa.obj :f3:silen.obj :f3:sitype.obj :f3:sisym.obj\
	:f3:siscop.obj :f3:silnko.obj :f3:setflg.obj :f3:allocs.obj\
	:f3:alloc.obj plme.obj

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
main.obj: main.plm $(F3)fatal.pex $(F3)exit.pex $(F3)chain.pex
plma.obj: plma.plm $(F3)movmem.aex fill.aex $(F3)initf.pex $(F3)openf.pex \
          $(F3)memchk.aex $(F3)fatal.pex $(F3)prints.pex $(F3)fatlio.pex \
          $(F3)readf.pex $(F3)data.inc
plmb.obj: plmb.plm $(F3)data.inc $(F3)flags.inc $(F3)info.inc $(F3)lookup.pex \
          $(F3)creati.pex $(F3)sibin.pex $(F3)sipcnt.pex $(F3)siflag.pex \
          $(F3)sidtyp.pex
plmc.obj: plmc.plm
plmd.obj: plmd.plm $(F3)data.inc $(F3)movmem.aex $(F3)fill.aex
plme.obj: plme.plm
$(F3)alloc.obj: $(F3)alloc.plm $(F3)err.inc $(F3)ftlerr.pex
$(F3)allocs.obj: $(F3)allocs.plm $(F3)err.inc $(F3)ftlerr.pex
$(F3)chain.obj: $(F3)chain.plm $(F3)fatlio.pex $(F3)initf.pex $(F3)load.pex
$(F3)cpytil.obj: $(F3)cpytil.plm
$(F3)creati.obj: $(F3)creati.plm $(F3)err.inc $(F3)info.inc $(F3)fill.aex \
                 $(F3)alloc.pex $(F3)ftlerr.pex $(F3)silnko.pex $(F3)sitype.pex \
                 $(F3)silen.pex $(F3)siscop.pex $(F3)sisym.pex
$(F3)data.obj: $(F3)data.plm
$(F3)exit.obj: $(F3)exit.plm
$(F3)fatal.obj: $(F3)fatal.plm $(F3)exit.pex $(F3)prints.pex
$(F3)fatlio.obj: $(F3)fatlio.plm $(F3)prints.pex $(F3)itoa.pex $(F3)exit.pex
$(F3)fill.obj: $(F3)fill.asm
$(F3)initf.obj: $(F3)initf.plm $(F3)fill.aex $(F3)cpytil.pex
$(F3)itoa.obj: $(F3)itoa.plm $(F3)movmem.aex $(F3)fill.aex
$(F3)load.obj: $(F3)load.plm
$(F3)lookup.obj: $(F3)lookup.plm $(F3)allocs.pex $(F3)movmem.aex $(F3)strcmp.pex \
                 $(F3)alloc.pex
$(F3)memchk.obj: $(F3)memchk.asm
$(F3)movmem.obj: $(F3)movmem.asm
$(F3)open.obj: $(F3)open.plm
$(F3)openf.obj: $(F3)openf.plm $(F3)open.pex $(F3)fatlio.pex
$(F3)prints.obj: $(F3)prints.plm $(F3)write.pex
$(F3)read.obj: $(F3)read.plm
$(F3)readf.obj: $(F3)readf.plm $(F3)read.pex $(F3)fatlio.pex
$(F3)setflg.obj: $(F3)setflg.plm
$(F3)sibin.obj: $(F3)sibin.plm
$(F3)sidtyp.obj: $(F3)sidtyp.plm $(F3)info.inc
$(F3)siflag.obj: $(F3)siflag.plm $(F3)setflg.pex
$(F3)silen.obj: $(F3)silen.plm
$(F3)silnko.obj: $(F3)silnko.plm
$(F3)sipcnt.obj: $(F3)sipcnt.plm $(F3)info.inc
$(F3)siscop.obj: $(F3)siscop.plm
$(F3)sisym.obj: $(F3)sisym.plm
$(F3)sitype.obj: $(F3)sitype.plm
$(F3)strcmp.obj: $(F3)strcmp.plm
$(F3)write.obj: $(F3)write.plm
