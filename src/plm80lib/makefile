# common makefile info
ROOT=../..
ISIS=$(ROOT:/=\)\thames
TOOLS=$(ROOT:/=\)\tools
F0=./
# alternatives for :F1:
V4=$(ROOT)/plm80v4
V3=$(ROOT)/plm80v3

TARGET=plm80.lib

PLMFLAGS=code optimize
ASMFLAGS=

OBJS=	P0000.obj P0002.obj P0004.obj P0007.obj P0011.obj P0014.obj \
	P0018.obj P0020.obj P0022.obj P0025.obj P0029.obj P0031.obj \
	P0034.obj P0036.obj P0039.obj P0042.obj P0045.obj P0048.obj \
	P0050.obj P0052.obj P0055.obj P0059.obj P0061.obj P0064.obj \
	P0066.obj P0069.obj P0071.obj P0073.obj P0075.obj P0077.obj \
	P0080.obj P0082.obj P0084.obj P0086.obj P0089.obj P0091.obj \
	P0094.obj P0096.obj P0098.obj P0101.obj P0103.obj P0105.obj \
	P0106.obj P0108.obj P0110.obj P0113.obj PRMSK.obj PSMSK.obj

.SUFFIXES: .asm
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

ref:	$(OBJS:P=ref/@P)

$(OBJS:P=ref/@P):
	$(TOOLS)\delib.pl $(V4)/$(TARGET) ref

verify: ref $(OBJS)
	for %i in ($(OBJS)) do $(TOOLS)\diffbin %i ref/@%i

clean:
       	del *.*~ *.obj *.lst *~

vclean: clean
	del $(TARGET) 

uclean:	vclean
	del /Q makefile *.asm ref\*.obj

gitprep: uclean
	..\..\unpack.pl

# auto generated
