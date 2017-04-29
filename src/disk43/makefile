# path to root of build tree
ROOT := ../..
TARGETS := fixmap format idisk
REF:=ref

# build options
LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:= DEBUG
LINKFLAGS:=
include $(ROOT)/tools/isis.mk

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

all::	$(TARGETS)

fixmap: fixmap.rel
	$(call locate,$@,$^,stacksize(96) name(FIXMAP) purge)

format: format.rel
	$(call locate,$@,$^,stacksize(124) name(FORMAT) purge)

idisk: idisk.rel
	$(call locate,$@,$^,stacksize(138) name(IDISK) purge)

.INTERMEDIATE: fixmap.rel format.rel idisk.rel

fixmap.rel: $(FIXMAPOBJ)
	$(call link,$@,$^ $(call v40dir,system.lib plm80.lib))

format.rel: $(FORMATOBJ)
	$(call link,$@,$^ $(call v40dir,system.lib plm80.lib))


idisk.rel: $(IDISKOBJ)
	$(call link,$@,$^ $(call v40dir,system.lib plm80.lib))

