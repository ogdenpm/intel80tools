# path to root of build tree
ROOT := ../..
TARGETS := fixmap format idisk

# build options
LOCATEOPT:=SYMBOLS PUBLICS
PLMOPT:= DEBUG
LINKOPT:=
KEEP=1
include $(ROOT)/tools/isistools.mk

# set default tools
#COMPARE := $(ROOT)/tools/diffbin
REFDIR:=ref

PEXFILE:=disk.pex

FIXMAPOBJ = fixmap.obj alloc.obj dblank.obj dlimit.obj ferror.obj fupper.obj numout.obj\
	popul.obj scanin.obj seq.obj setblk.obj rpterr.obj ucase.obj wdelim.obj

FORMATOBJ =	format.obj alloc.obj direct.obj dblank.obj dlimit.obj ferror.obj fmttrk.obj\
	fupper.obj mascii.obj rpterr.obj seq.obj chkbad.obj\
	devdsk.obj wd.obj unpath.obj absio.obj clbuf.obj\
	diskio.obj numout.obj setblk.obj ucase.obj wdelim.obj

IDISKOBJ =	idisk.obj alloc.obj direct.obj dblank.obj dlimit.obj ferror.obj fmttrk.obj\
	fupper.obj getdsk.obj mascii.obj serror.obj seq.obj chkbad.obj\
	devdsk.obj wd.obj absio.obj clbuf.obj\
	diskio.obj numout.obj rpterr.obj setblk.obj ucase.obj wdelim.obj

all: $(TARGETS)

fixmap: $(FIXMAPOBJ)
	$(call link,$@.rel,$^ $(call tools-v40,system.lib plm80.lib))
	$(call locate,$@,$@.rel,stacksize(96) name(FIXMAP) purge)
	@del $@.rel

format: $(FORMATOBJ)
	$(call link,$@.rel,$^ $(call tools-v40,system.lib plm80.lib))
	$(call locate,$@,$@.rel,stacksize(124) name(FORMAT) purge)
	@del $@.rel

idisk: $(IDISKOBJ)
	$(call link,$@.rel,$^ $(call tools-v40,system.lib plm80.lib))
	$(call locate,$@,$@.rel,stacksize(138) name(IDISK) purge)
	@del $@.rel
