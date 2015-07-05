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
TOOLS=..\..\tools
PEXFILE=:f3:plm.pex

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
main.obj: main.plm main.ipx
plma.obj: plma.plm plma.ipx
plmb.obj: plmb.plm plmb.ipx
plmd.obj: plmd.plm plmd.ipx
$(F3)alloc.obj: $(F3)alloc.plm $(F3)alloc.ipx
$(F3)allocs.obj: $(F3)allocs.plm $(F3)allocs.ipx
$(F3)chain.obj: $(F3)chain.plm $(F3)chain.ipx
$(F3)creati.obj: $(F3)creati.plm $(F3)creati.ipx
$(F3)data.obj: $(F3)data.plm $(F3)data.ipx
$(F3)exit.obj: $(F3)exit.plm $(F3)exit.ipx
$(F3)fatal.obj: $(F3)fatal.plm $(F3)fatal.ipx
$(F3)fatlio.obj: $(F3)fatlio.plm $(F3)fatlio.ipx
$(F3)initf.obj: $(F3)initf.plm $(F3)initf.ipx
$(F3)itoa.obj: $(F3)itoa.plm $(F3)itoa.ipx
$(F3)load.obj: $(F3)load.plm $(F3)load.ipx
$(F3)lookup.obj: $(F3)lookup.plm $(F3)lookup.ipx
$(F3)open.obj: $(F3)open.plm $(F3)open.ipx
$(F3)openf.obj: $(F3)openf.plm $(F3)openf.ipx
$(F3)prints.obj: $(F3)prints.plm $(F3)prints.ipx
$(F3)read.obj: $(F3)read.plm $(F3)read.ipx
$(F3)readf.obj: $(F3)readf.plm $(F3)readf.ipx
$(F3)setflg.obj: $(F3)setflg.plm $(F3)setflg.ipx
$(F3)sibin.obj: $(F3)sibin.plm $(F3)sibin.ipx
$(F3)sidtyp.obj: $(F3)sidtyp.plm $(F3)sidtyp.ipx
$(F3)siflag.obj: $(F3)siflag.plm $(F3)siflag.ipx
$(F3)silen.obj: $(F3)silen.plm $(F3)silen.ipx
$(F3)silnko.obj: $(F3)silnko.plm $(F3)silnko.ipx
$(F3)sipcnt.obj: $(F3)sipcnt.plm $(F3)sipcnt.ipx
$(F3)siscop.obj: $(F3)siscop.plm $(F3)siscop.ipx
$(F3)sisym.obj: $(F3)sisym.plm $(F3)sisym.ipx
$(F3)sitype.obj: $(F3)sitype.plm $(F3)sitype.ipx
$(F3)write.obj: $(F3)write.plm $(F3)write.ipx
