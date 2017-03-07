# path to root of build tree
ROOT := ../..
TARGETS := idisk

# build options
LOCATEOPT:=SYMBOLS PUBLICS
PLMOPT:= DEBUG
LINKOPT:=
KEEP=1
include $(ROOT)/tools/isistools.mk

# set default tools
#COMPARE := $(ROOT)/tools/diffbin
REFDIR:=ref

PEXFILE:=idisk.pex

OBJ =	idisk.obj alloc.obj dblank.obj dlimit.obj filerr.obj fmttrk.obj\
	forupp.obj getdsk.obj mvasci.obj sfierr.obj seq.obj chkbad.obj\
	gettar.obj wrdir.obj absio.obj clrbuf.obj\
	diskio.obj numout.obj rpterr.obj setblk.obj upcase.obj wdlimt.obj

all: $(TARGETS)

idisk: $(OBJ)
	$(call link,idisk.rel,$^ $(call tools-v40,system.lib plm80.lib))
	$(call locate,$@,idisk.rel,stacksize(138) name(IDISK) purge)
	@del idisk.rel

print: idisk
	perl ../../relst.pl idisk.map $(OBJ:.obj=.lst)
