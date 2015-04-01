OBJ=	adninf.obj alloc.obj allocs.obj backup.obj chain.obj close.obj \
	closef.obj clrflg.obj cpytil.obj creatf.obj creati.obj data.obj \
	delete.obj deletf.obj endcom.obj exit.obj fatal.obj fatlio.obj \
	fflush.obj fi.obj fill.obj findmi.obj fiscop.obj fread.obj \
	fwrite.obj gibin.obj gibseo.obj gibsev.obj gidim.obj gidim2.obj \
	gidtyp.obj gieid.obj giintn.obj gilen.obj gilnko.obj gilnkv.obj \
	giparo.obj giparv.obj gipcnt.obj gipid.obj giscop.obj gisym.obj \
	gitype.obj initf.obj itoa.obj load.obj lookup.obj lstinf.obj \
	lstlin.obj memchk.obj movmem.obj open.obj openf.obj prints.obj \
	putlst.obj read.obj readf.obj rewind.obj seek.obj seekf.obj \
	sekend.obj setflg.obj sibin.obj sibsev.obj sidim.obj sidtyp.obj \
	sieid.obj siflag.obj silen.obj silnko.obj silnkv.obj siparo.obj \
	sipcnt.obj siscop.obj sisym.obj sitype.obj strcmp.obj tellf.obj \
	tiflag.obj tstflg.obj wr2lst.obj wrclst.obj write.obj writef.obj \
	wrslst.obj 

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


$all: $(F3)data.abs $(OBJ::f3:=../common/)

$(F3)data.abs: $(F3)data.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	@SET ISIS_F3=$(F3)
	$(ISIS) :F1:locate :f3:data.obj to :f3:data.abs SYMBOLS PUBLICS DATA(3800H) print(:f3:data.map)
	@if not exist $@ exit /b 1

# individual makes are controlled from the plm(x) makefiles
clean:
	del *.obj *lst *.map *.abs *~ *.*~

# auto generated
adninf.obj: adninf.plm $(F3)gilen.pex $(F3)gitype.pex
alloc.obj: alloc.plm $(F3)err.inc $(F3)ftlerr.pex
allocs.obj: allocs.plm $(F3)err.inc $(F3)ftlerr.pex
backup.obj: backup.plm
chain.obj: chain.plm $(F3)fatlio.pex $(F3)initf.pex $(F3)load.pex
close.obj: close.plm
closef.obj: closef.plm $(F3)close.pex $(F3)fatlio.pex
clrflg.obj: clrflg.plm
cpytil.obj: cpytil.plm
creatf.obj: creatf.plm $(F3)readf.pex
creati.obj: creati.plm $(F3)err.inc $(F3)info.inc $(F3)fill.aex $(F3)alloc.pex \
            $(F3)ftlerr.pex $(F3)silnko.pex $(F3)sitype.pex $(F3)silen.pex \
            $(F3)siscop.pex $(F3)sisym.pex
data.obj: data.plm
delete.obj: delete.plm
deletf.obj: deletf.plm $(F3)delete.pex
endcom.obj: endcom.plm $(F3)itoa.pex $(F3)prints.pex
exit.obj: exit.plm
fatal.obj: fatal.plm $(F3)exit.pex $(F3)prints.pex
fatlio.obj: fatlio.plm $(F3)prints.pex $(F3)itoa.pex $(F3)exit.pex
fflush.obj: fflush.plm $(F3)writef.pex
fi.obj: fi.plm $(F3)fiscop.pex
fill.obj: fill.asm
findmi.obj: findmi.plm $(F3)flags.inc $(F3)gilnko.pex $(F3)giparo.pex \
            $(F3)tiflag.pex
fiscop.obj: fiscop.plm $(F3)tiflag.pex $(F3)giscop.pex $(F3)silnko.pex \
            $(F3)gilnko.pex $(F3)gitype.pex $(F3)flags.inc $(F3)info.inc
fread.obj: fread.asm
fwrite.obj: fwrite.asm
gibin.obj: gibin.plm
gibseo.obj: gibseo.plm
gibsev.obj: gibsev.plm
gidim.obj: gidim.plm
gidim2.obj: gidim2.plm
gidtyp.obj: gidtyp.plm $(F3)info.inc
gieid.obj: gieid.plm
giintn.obj: giintn.plm
gilen.obj: gilen.plm
gilnko.obj: gilnko.plm
gilnkv.obj: gilnkv.plm
giparo.obj: giparo.plm $(F3)info.inc
giparv.obj: giparv.plm
gipcnt.obj: gipcnt.plm $(F3)info.inc
gipid.obj: gipid.plm
giscop.obj: giscop.plm
gisym.obj: gisym.plm
gitype.obj: gitype.plm
initf.obj: initf.plm $(F3)fill.aex $(F3)cpytil.pex
itoa.obj: itoa.plm $(F3)movmem.aex $(F3)fill.aex
load.obj: load.plm
lookup.obj: lookup.plm $(F3)allocs.pex $(F3)movmem.aex $(F3)strcmp.pex \
            $(F3)alloc.pex
lstinf.obj: lstinf.plm $(F3)data.inc $(F3)gibsev.pex $(F3)closef.pex
lstlin.obj: lstlin.plm $(F3)itoa.pex
memchk.obj: memchk.asm
movmem.obj: movmem.asm
open.obj: open.plm
openf.obj: openf.plm $(F3)open.pex $(F3)fatlio.pex
prints.obj: prints.plm $(F3)write.pex
putlst.obj: putlst.asm
read.obj: read.plm
readf.obj: readf.plm $(F3)read.pex $(F3)fatlio.pex
rewind.obj: rewind.plm $(F3)seekf.pex
seek.obj: seek.plm
seekf.obj: seekf.plm $(F3)seek.pex $(F3)fatlio.pex
sekend.obj: sekend.plm $(F3)seek.pex $(F3)fatlio.pex
setflg.obj: setflg.plm
sibin.obj: sibin.plm
sibsev.obj: sibsev.plm
sidim.obj: sidim.plm
sidtyp.obj: sidtyp.plm $(F3)info.inc
sieid.obj: sieid.plm
siflag.obj: siflag.plm $(F3)setflg.pex
silen.obj: silen.plm
silnko.obj: silnko.plm
silnkv.obj: silnkv.plm
siparo.obj: siparo.plm $(F3)info.inc
sipcnt.obj: sipcnt.plm $(F3)info.inc
siscop.obj: siscop.plm
sisym.obj: sisym.plm
sitype.obj: sitype.plm
strcmp.obj: strcmp.plm
tellf.obj: tellf.plm $(F3)seek.pex $(F3)fatlio.pex
tiflag.obj: tiflag.plm $(F3)tstflg.pex
tstflg.obj: tstflg.plm
wr2lst.obj: wr2lst.plm
wrclst.obj: wrclst.asm
write.obj: write.plm
writef.obj: writef.plm $(F3)write.pex $(F3)fatlio.pex
wrslst.obj: wrslst.plm
$(F3)data.obj: $(F3)data.plm
