TARGET = plm80.ov2
NAME = PL8040
STACK = 240

OBJ =	main2.obj plm2a.obj plm2b.obj plm2c.obj plm2d.obj plm2e.obj plm2f.obj plm2g.obj plm2h.obj\
	:f3:movmem.obj :f3:fill.obj :f3:fatal.obj :f3:creatf.obj\
	:f3:fflush.obj :f3:fwrite.obj :f3:fread.obj :f3:chain.obj\
	:f3:gitype.obj :f3:gilnkv.obj :f3:silnkv.obj :f3:tiflag.obj\
	:f3:gieid.obj :f3:gidim.obj :f3:giparo.obj :f3:sidim.obj\
	:f3:gibsev.obj :f3:sibsev.obj :f3:giparv.obj :f3:gidtyp.obj\
	:f3:gipcnt.obj :f3:gipid.obj :f3:adninf.obj :f3:exit.obj\
	:f3:load.obj :f3:prints.obj :f3:fatlIO.obj :f3:initf.obj\
	:f3:readf.obj :f3:writef.obj :f3:gilen.obj :f3:tstflg.obj\
	:f3:read.obj :f3:write.obj :f3:cpytil.obj :f3:itoa.obj

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
main2.obj: main2.plm $(F3)data.inc t2code.inc $(F3)chain.pex $(F3)creatf.pex \
           $(F3)fatal.pex $(F3)fflush.pex fill.aex $(F3)sibsev.pex $(F3)sidim.pex
plm2a.obj: plm2a.plm $(F3)data.inc t2code.inc $(F3)info.inc $(F3)flags.inc \
           fill.aex $(F3)fwrite.aex $(F3)fread.aex $(F3)movmem.aex \
           $(F3)gitype.pex $(F3)giparo.pex $(F3)sibsev.pex $(F3)tiflag.pex \
           $(F3)sidim.pex $(F3)gidim.pex $(F3)gieid.pex $(F3)gipid.pex \
           $(F3)gilnkv.pex
plm2b.obj: plm2b.pl3 $(F3)data.inc t2code.inc $(F3)info.inc $(F3)flags.inc \
           $(F3)gitype.pex $(F3)giparo.pex $(F3)tiflag.pex $(F3)gilnkv.pex
plm2c.obj: plm2c.plm $(F3)data.inc t2code.inc $(F3)info.inc $(F3)flags.inc \
           $(F3)gitype.pex $(F3)giparo.pex $(F3)sibsev.pex $(F3)tiflag.pex \
           $(F3)sidim.pex $(F3)gidim.pex $(F3)gieid.pex $(F3)gipid.pex \
           $(F3)gilnkv.pex
plm2d.obj: plm2d.plm $(F3)data.inc t2code.inc
plm2e.obj: plm2e.plm $(F3)data.inc t2code.inc $(F3)info.inc $(F3)flags.inc
plm2f.obj: plm2f.plm $(F3)data.inc t2code.inc $(F3)info.inc $(F3)flags.inc \
           $(F3)tiflag.pex $(F3)gibsev.pex $(F3)silnkv.pex $(F3)gipid.pex
plm2g.obj: plm2g.pl3 $(F3)data.inc t2code.inc $(F3)info.inc $(F3)flags.inc \
           $(F3)adninf.pex $(F3)gitype.pex $(F3)tiflag.pex $(F3)silnkv.pex \
           $(F3)gilnkv.pex $(F3)gipcnt.pex $(F3)giparv.pex
plm2h.obj: plm2h.plm $(F3)data.inc t2code.inc $(F3)info.inc $(F3)flags.inc \
           $(F3)gidtyp.pex $(F3)gipcnt.pex $(F3)adninf.pex $(F3)gitype.pex \
           $(F3)tiflag.pex $(F3)sibsev.pex $(F3)sidim.pex $(F3)gidim.pex
$(F3)adninf.obj: $(F3)adninf.plm $(F3)gilen.pex $(F3)gitype.pex
$(F3)chain.obj: $(F3)chain.plm $(F3)fatlio.pex $(F3)initf.pex $(F3)load.pex
$(F3)cpytil.obj: $(F3)cpytil.plm
$(F3)creatf.obj: $(F3)creatf.plm $(F3)readf.pex
$(F3)data.obj: $(F3)data.plm
$(F3)exit.obj: $(F3)exit.plm
$(F3)fatal.obj: $(F3)fatal.plm $(F3)exit.pex $(F3)prints.pex
$(F3)fatlio.obj: $(F3)fatlio.plm $(F3)prints.pex $(F3)itoa.pex $(F3)exit.pex
$(F3)fflush.obj: $(F3)fflush.plm $(F3)writef.pex
$(F3)fill.obj: $(F3)fill.asm
$(F3)fread.obj: $(F3)fread.asm
$(F3)fwrite.obj: $(F3)fwrite.asm
$(F3)gibsev.obj: $(F3)gibsev.plm
$(F3)gidim.obj: $(F3)gidim.plm
$(F3)gidtyp.obj: $(F3)gidtyp.plm $(F3)info.inc
$(F3)gieid.obj: $(F3)gieid.plm
$(F3)gilen.obj: $(F3)gilen.plm
$(F3)gilnkv.obj: $(F3)gilnkv.plm
$(F3)giparo.obj: $(F3)giparo.plm $(F3)info.inc
$(F3)giparv.obj: $(F3)giparv.plm
$(F3)gipcnt.obj: $(F3)gipcnt.plm $(F3)info.inc
$(F3)gipid.obj: $(F3)gipid.plm
$(F3)gitype.obj: $(F3)gitype.plm
$(F3)initf.obj: $(F3)initf.plm $(F3)fill.aex $(F3)cpytil.pex
$(F3)itoa.obj: $(F3)itoa.plm $(F3)movmem.aex $(F3)fill.aex
$(F3)load.obj: $(F3)load.plm
$(F3)movmem.obj: $(F3)movmem.asm
$(F3)prints.obj: $(F3)prints.plm $(F3)write.pex
$(F3)read.obj: $(F3)read.plm
$(F3)readf.obj: $(F3)readf.plm $(F3)read.pex $(F3)fatlio.pex
$(F3)sibsev.obj: $(F3)sibsev.plm
$(F3)sidim.obj: $(F3)sidim.plm
$(F3)silnkv.obj: $(F3)silnkv.plm
$(F3)tiflag.obj: $(F3)tiflag.plm $(F3)tstflg.pex
$(F3)tstflg.obj: $(F3)tstflg.plm
$(F3)write.obj: $(F3)write.plm
$(F3)writef.obj: $(F3)writef.plm $(F3)write.pex $(F3)fatlio.pex
