TARGET=isis.cli
NAME=CLI
STACK=68
# common makefile info
ISIS=..\..\thames
V4=../../plm80v4/
V3=../../plm80v3/
F0=./
TOOLS=..\..\tools
PEXFILE=cli.pex

PLMFLAGS=code optimize

OBJS=	cli.obj dblank.obj dlimit.obj filerr.obj forupp.obj monitr.obj seq.obj\
	reperr.obj upcase.obj wdlmt.obj 

.SUFFIXES: .asm .pl3
.pl3.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	$(ISIS) :F1:PLM80 $< $(PLMFLAGS)
	@find $*.lst "     0 PROGRAM ERROR(S)" >nul || (del $*.obj & exit /b 1)

.asm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:ASM80 $< 
	@find $*.lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $*.obj & exit /b 1)

.pl3.ipx:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	@SET ISIS_F3=$(F3)
	$(TOOLS)\ngenpex $(PEXFILE) $<
all:	.depend
all:
	$(MAKE) $(TARGET)

$(TARGET): $(OBJS)
	@SET ISIS_F0=.
	@SET ISIS_F1=$(V3)
	$(ISIS) :F1:LINK "&" < <<
$(OBJS: =,&
),:f1:system.lib,:f1:plm80.lib TO $*.rel MAP print($*.lmp)
<<NOKEEP
	$(ISIS) :F1:LOCATE $*.rel to $@ print($*.map) code(3680H) name($(NAME)) stacksize($(STACK)) map symbols publics purge
	@del $*.rel
	@(find $*.map "UNSATISFIED EXTERNAL" >NUL && exit /b 1) || exit /b 0

$(OBJS): makefile

filerr.ipx forupp.ipx reperr.ipx cli.ipx : $(PEXFILE)

.depend: cli_all.plm
	..\..\unpack.pl
	..\..\mkdepend.pl
	date /t >.depend

verify: all
	fc /b $(TARGET) $(TARGET).ref

clean:
       	del *.*~ *.obj *.lst *~ *.map *.imp

vclean: clean
	del *.ipx .depend *.pex

gitprep: vclean
	del *.asm *.pl3 $(TARGET) makefile
	..\..\unpack.pl

# auto generated
cli.obj: cli.pl3 cli.ipx
filerr.obj: filerr.pl3 filerr.ipx
forupp.obj: forupp.pl3 forupp.ipx
reperr.obj: reperr.pl3 reperr.ipx
