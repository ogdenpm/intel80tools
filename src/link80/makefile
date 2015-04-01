ISIS=..\..\thames
V4=../../plm80v4/
V3=../../plm80v3/
TOOLS=../../tools/
F0=./
F3=../common/
#
# compilation and assembly rules
.SUFFIXES: .plm .pl3
.plm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:PLM80 $< code
	@if not exist $*.obj exit /b 1

.pl3.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	$(ISIS) :F1:PLM80 $< code
	@find $*.lst "     0 PROGRAM ERROR(S)" >nul || (del $*.obj & exit /b 1)

.asm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:ASM80 $< 
	@find $*.lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $*.obj & exit /b 1)


all: link link.ovl


# link build rule
link: link0.abs link1.abs link2.abs linkov.abs
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link link0.abs,link1.abs,link2.abs,PUBLICS(linkov.abs) to $*.rel
	$(ISIS) :F1:locate "&" < <<
$*.rel TO $@ MAP PRINT($*.map) SYMBOLS PURGE PUBLICS &
STACKSIZE(0) COLUMNS(3) NAME(LINK) PURGE
<<NOKEEP
	@del $*.rel
	@(find $*.map "UNSATISFIED EXTERNAL" >NUL && exit /b 1) || exit /b 0

link.ovl: link0.abs linkov.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link publics(link0.abs),linkov.obj,:F1:system.lib,:f1:plm80.lib To linkov.rel
	$(ISIS) :F1:locate "&" < <<
linkov.rel to $@ MAP PRINT(linkov.map) SYMBOLS PUBLICS&
code(48DEH) stacksize(0) purge name(LINKOVL)
<<NOKEEP
	@del linkov.rel
	@(find $*.map "UNSATISFIED EXTERNAL" >NUL && exit /b 1) || exit /b 0


link0.abs: link.obj link1a.obj wrerr.obj memmov.obj errrpt.obj binasc.obj \
 strequ.obj skpspc.obj pstafn.obj pstfn.obj isis.obj isisen.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link "&" < <<
$(**: =,&
),:f1:plm80.lib to $*.rel
<<NOKEEP
	$(ISIS)	:F1:locate $*.rel to $@ SYMBOLS PUBLICS STACKSIZE(84) COLUMNS(3)
	@del $*.rel
	@if not exist $@ exit /b 1

link1.abs: link0.abs link3.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link publics(link0.abs),link3.obj,:F1:plm80.lib to $*.rel MAP
	$(ISIS) :F1:locate $*.rel to $@ SYMBOLS PUBLICS CODE(48DEH) STACKSIZE(0)
#	@del $*.rel
	@if not exist $@ exit /b 1

link2.abs: link0.abs link1.abs link3a.obj mkfnam.obj lineuc.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:link "&" < <<
publics(link0.abs,link1.abs),link3a.obj,mkfnam.obj,lineuc.obj,&
:F1:system.lib(delete),:f1:system.lib to $*.rel MAP
<<NOKEEP
	$(ISIS) :f1:locate $*.rel to $@ SYMBOLS PUBLICS CODE(629DH) STACKSIZE(0)
	@del $*.rel
	@if not exist $@ exit /b 1

linkov.abs: linkov.obj
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:locate $*.obj TO $@ SYMBOLS PUBLICS CODE(48DEH) STACKSIZE(0)
	@if not exist $@ exit /b 1

clean:
	del *.obj *lst *.map *.abs *.rel *.*~ *~ link.ovl link

verify: link link.ovl
	$(TOOLS:/=\)diffbin link $(V4)link
	$(TOOLS:/=\)diffbin link.ovl $(V4)link.ovl



# auto generated
link.obj: link.plm link.ext
link1a.obj: link1a.plm link.ext
link3.obj: link3.plm link.ext
link3a.obj: link3a.plm link.ext
linkov.obj: linkov.plm link.ext
