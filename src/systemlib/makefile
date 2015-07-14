# common makefile info
ROOT=d:/usr/sandbox/intel80
ISIS=$(ROOT:/=\)\thames
TOOLS=$(ROOT:/=\)\tools
F0=./
# alternatives for :F1:
V4=$(ROOT)/plm80v4
V3=$(ROOT)/plm80v3

TARGET=system.lib

PLMFLAGS=code optimize
ASMFLAGS=

OBJS= 	attrib.obj ci.obj close.obj co.obj consol.obj csts.obj delete.obj exit.obj\
	iochk.obj iodef.obj ioset.obj isis.obj lo.obj load.obj memck.obj open.obj\
	po.obj read.obj rename.obj rescan.obj ri.obj seek.obj WHOCON.obj write.obj\
	ui.obj uo.obj upps.obj

.SUFFIXES: .asm .plm .pl3
.plm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:PLM80 $< $(PLMFLAGS) code
	@if not exist $*.obj exit /b 1

.pl3.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V3)
	$(ISIS) :F1:PLM80 $< $(PLMFLAGS) code
	@find $*.lst "     0 PROGRAM ERROR(S)" >nul || (del $*.obj & exit /b 1)

.asm.obj:
	@SET ISIS_F0=$(F0)
	@SET ISIS_F1=$(V4)
	$(ISIS) :F1:ASM80 $< $(ASMFLAGS)
	@find $*.lst "ASSEMBLY COMPLETE,   NO ERRORS" >nul || (del $*.obj & exit /b 1)


$(TARGET): $(OBJS)
	@SET ISIS_F0=.
	@SET ISIS_F1=$(V4)
	del $@
	$(ISIS) :F1:lib < <<
create $@
add $(**: =,&
) TO $@
exit
<<NOKEEP

verify: $(OBJS)
	$(TOOLS)\delib.pl $(V4)/$(TARGET) ref
	for %i in ($**) do $(TOOLS)\diffbin %i ref/%i

clean:
       	del *.*~ *.obj *.lst *~

vclean: clean
	del $(TARGET) 

uclean:	vclean
	del /q makefile *.asm *.pl3 ref\*.obj

gitprep: uclean
	..\..\unpack.pl

# auto generated
