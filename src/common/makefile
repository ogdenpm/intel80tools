# common makefile info
ISIS=..\..\thames
V4=../../plm80v4/
V3=../../plm80v3/
F0=./
F3=../common/
TOOLS=..\..\tools
PEXFILE=:f3:plm.pex

OBJ=	:f3:adninf.obj :f3:alloc.obj :f3:allocs.obj :f3:backup.obj :f3:chain.obj :f3:close.obj \
	:f3:closef.obj :f3:clrflg.obj :f3:cpytil.obj :f3:creatf.obj :f3:creati.obj :f3:data.obj \
	:f3:delete.obj :f3:deletf.obj :f3:endcom.obj :f3:exit.obj :f3:fatal.obj \
	:f3:fatlio.obj :f3:fflush.obj :f3:fi.obj :f3:fill.obj :f3:findmi.obj :f3:fiscop.obj \
	:f3:fread.obj :f3:fwrite.obj :f3:gibin.obj :f3:gibseo.obj :f3:gibsev.obj :f3:gidim.obj \
	:f3:gidim2.obj :f3:gidtyp.obj :f3:gieid.obj :f3:giintn.obj :f3:gilen.obj :f3:gilnko.obj \
	:f3:gilnkv.obj :f3:giparo.obj :f3:giparv.obj :f3:gipcnt.obj :f3:gipid.obj :f3:giscop.obj \
	:f3:gisym.obj :f3:gitype.obj :f3:initf.obj :f3:itoa.obj :f3:load.obj :f3:lookup.obj \
	:f3:lstinf.obj :f3:lstlin.obj :f3:memchk.obj :f3:movmem.obj :f3:open.obj :f3:openf.obj \
	:f3:prints.obj :f3:putlst.obj :f3:read.obj :f3:readf.obj :f3:rewind.obj :f3:seek.obj \
	:f3:seekf.obj :f3:sekend.obj :f3:setflg.obj :f3:sibin.obj :f3:sibsev.obj :f3:sidim.obj \
	:f3:sidtyp.obj :f3:sieid.obj :f3:siflag.obj :f3:silen.obj :f3:silnko.obj :f3:silnkv.obj \
	:f3:siparo.obj :f3:sipcnt.obj :f3:siscop.obj :f3:sisym.obj :f3:sitype.obj :f3:strcmp.obj \
	:f3:tellf.obj :f3:tiflag.obj :f3:tstflg.obj :f3:wr2lst.obj :f3:wrclst.obj :f3:write.obj \
	:f3:writef.obj :f3:wrslst.obj \
	:f3:cpyflg.obj :f3:gicond.obj :f3:gilit.obj :f3:sibseo.obj :f3:sicond.obj :f3:siintn.obj \
	:f3:silit.obj :f3:sipid.obj

.SUFFIXES: .plm .pl3
.plm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(F3)
	$(ISIS) :F1:PLM80 $(<:../common/=:F3:) code
	@if not exist $*.obj exit /b 1

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


all: $(F3)data.abs $(OBJ::f3:=../common/)

$(F3)lstsp5.obj: $(F3)lstsup.plm
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(F3)
	$(ISIS) :F1:PLM80 $(<:../common/=:F3:) code SET(OVL=5)
	@if not exist $*.obj exit /b 1


$(F3)data.abs: $(F3)data.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(F3)
	$(ISIS) :F1:locate :f3:data.obj to :f3:data.abs SYMBOLS PUBLICS DATA(3800H) print(:f3:data.map)
	@if not exist $@ exit /b 1

# individual makes are controlled from the plm(x) makefiles
IPX = 	$(OBJ:.obj=.ipx)
$(IPX::F3:=../common/): $(PEXFILE::f3:=../common/)

clean:
	del *.obj *lst *.map *.*~ *~ *.abs

vclean:	clean
	del *.ipx $(TARGET)

$(IPX::F3:=../common/): ../common/plm.pex

# auto generated
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
$(F3)delete.obj: $(F3)delete.plm $(F3)delete.ipx
$(F3)deletf.obj: $(F3)deletf.plm $(F3)deletf.ipx
$(F3)endcom.obj: $(F3)endcom.plm $(F3)endcom.ipx
$(F3)exit.obj: $(F3)exit.plm $(F3)exit.ipx
$(F3)fatal.obj: $(F3)fatal.plm $(F3)fatal.ipx
$(F3)fatlio.obj: $(F3)fatlio.plm $(F3)fatlio.ipx
$(F3)fflush.obj: $(F3)fflush.plm $(F3)fflush.ipx
$(F3)fi.obj: $(F3)fi.plm $(F3)fi.ipx
$(F3)findmi.obj: $(F3)findmi.plm $(F3)findmi.ipx
$(F3)fiscop.obj: $(F3)fiscop.plm $(F3)fiscop.ipx
$(F3)gibin.obj: $(F3)gibin.plm $(F3)gibin.ipx
$(F3)gibseo.obj: $(F3)gibseo.plm $(F3)gibseo.ipx
$(F3)gibsev.obj: $(F3)gibsev.plm $(F3)gibsev.ipx
$(F3)gicond.obj: $(F3)gicond.plm $(F3)gicond.ipx
$(F3)gidim.obj: $(F3)gidim.plm $(F3)gidim.ipx
$(F3)gidim2.obj: $(F3)gidim2.plm $(F3)gidim2.ipx
$(F3)gidtyp.obj: $(F3)gidtyp.plm $(F3)gidtyp.ipx
$(F3)gieid.obj: $(F3)gieid.plm $(F3)gieid.ipx
$(F3)giintn.obj: $(F3)giintn.plm $(F3)giintn.ipx
$(F3)gilen.obj: $(F3)gilen.plm $(F3)gilen.ipx
$(F3)gilit.obj: $(F3)gilit.plm $(F3)gilit.ipx
$(F3)gilnko.obj: $(F3)gilnko.plm $(F3)gilnko.ipx
$(F3)gilnkv.obj: $(F3)gilnkv.plm $(F3)gilnkv.ipx
$(F3)giparo.obj: $(F3)giparo.plm $(F3)giparo.ipx
$(F3)giparv.obj: $(F3)giparv.plm $(F3)giparv.ipx
$(F3)gipcnt.obj: $(F3)gipcnt.plm $(F3)gipcnt.ipx
$(F3)gipid.obj: $(F3)gipid.plm $(F3)gipid.ipx
$(F3)giscop.obj: $(F3)giscop.plm $(F3)giscop.ipx
$(F3)gisym.obj: $(F3)gisym.plm $(F3)gisym.ipx
$(F3)gitype.obj: $(F3)gitype.plm $(F3)gitype.ipx
$(F3)initf.obj: $(F3)initf.plm $(F3)initf.ipx
$(F3)itoa.obj: $(F3)itoa.plm $(F3)itoa.ipx
$(F3)load.obj: $(F3)load.plm $(F3)load.ipx
$(F3)lookup.obj: $(F3)lookup.plm $(F3)lookup.ipx
$(F3)lstinf.obj: $(F3)lstinf.plm $(F3)lstinf.ipx
$(F3)lstlin.obj: $(F3)lstlin.plm $(F3)lstlin.ipx
$(F3)open.obj: $(F3)open.plm $(F3)open.ipx
$(F3)openf.obj: $(F3)openf.plm $(F3)openf.ipx
$(F3)prints.obj: $(F3)prints.plm $(F3)prints.ipx
$(F3)read.obj: $(F3)read.plm $(F3)read.ipx
$(F3)readf.obj: $(F3)readf.plm $(F3)readf.ipx
$(F3)rewind.obj: $(F3)rewind.plm $(F3)rewind.ipx
$(F3)seek.obj: $(F3)seek.plm $(F3)seek.ipx
$(F3)seekf.obj: $(F3)seekf.plm $(F3)seekf.ipx
$(F3)sekend.obj: $(F3)sekend.plm $(F3)sekend.ipx
$(F3)setflg.obj: $(F3)setflg.plm $(F3)setflg.ipx
$(F3)sibin.obj: $(F3)sibin.plm $(F3)sibin.ipx
$(F3)sibseo.obj: $(F3)sibseo.plm $(F3)sibseo.ipx
$(F3)sibsev.obj: $(F3)sibsev.plm $(F3)sibsev.ipx
$(F3)sicond.obj: $(F3)sicond.plm $(F3)sicond.ipx
$(F3)sidim.obj: $(F3)sidim.plm $(F3)sidim.ipx
$(F3)sidtyp.obj: $(F3)sidtyp.plm $(F3)sidtyp.ipx
$(F3)sieid.obj: $(F3)sieid.plm $(F3)sieid.ipx
$(F3)siflag.obj: $(F3)siflag.plm $(F3)siflag.ipx
$(F3)siintn.obj: $(F3)siintn.plm $(F3)siintn.ipx
$(F3)silen.obj: $(F3)silen.plm $(F3)silen.ipx
$(F3)silit.obj: $(F3)silit.plm $(F3)silit.ipx
$(F3)silnko.obj: $(F3)silnko.plm $(F3)silnko.ipx
$(F3)silnkv.obj: $(F3)silnkv.plm $(F3)silnkv.ipx
$(F3)siparo.obj: $(F3)siparo.plm $(F3)siparo.ipx
$(F3)sipcnt.obj: $(F3)sipcnt.plm $(F3)sipcnt.ipx
$(F3)sipid.obj: $(F3)sipid.plm $(F3)sipid.ipx
$(F3)siscop.obj: $(F3)siscop.plm $(F3)siscop.ipx
$(F3)sisym.obj: $(F3)sisym.plm $(F3)sisym.ipx
$(F3)sitype.obj: $(F3)sitype.plm $(F3)sitype.ipx
$(F3)tellf.obj: $(F3)tellf.plm $(F3)tellf.ipx
$(F3)tiflag.obj: $(F3)tiflag.plm $(F3)tiflag.ipx
$(F3)tstflg.obj: $(F3)tstflg.plm $(F3)tstflg.ipx
$(F3)wr2lst.obj: $(F3)wr2lst.plm $(F3)wr2lst.ipx
$(F3)write.obj: $(F3)write.plm $(F3)write.ipx
$(F3)writef.obj: $(F3)writef.plm $(F3)writef.ipx
$(F3)wrslst.obj: $(F3)wrslst.plm $(F3)wrslst.ipx
