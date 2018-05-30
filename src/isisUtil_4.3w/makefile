# path to root of build tree
ROOT := ../..
PEXFILE:=isisUtil_4.3w.pex

include $(ROOT)/tools/isis.mk

# build options
LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:= DEBUG CODE
LINKFLAGS:=

TARGETS := format idisk
REF:=$(call ipath,isis,4.3w)

FORMATOBJ =	format.obj alloc.obj direct.obj dblank.obj dlimit.obj ferror.obj fmttrk.obj\
	forupp.obj mvasci.obj numout.obj rpterr.obj seq.obj setmap.obj\
	trange.obj wd.obj ypath.obj absio.obj clbuf.obj\
	diskio.obj setblk.obj ucase.obj wdelim.obj wdisk.obj

IDISKOBJ =	idisk.obj alloc.obj direct.obj dblank.obj dlimit.obj ferror.obj fmttrk.obj\
	forupp.obj getdsk.obj mvasci.obj numout.obj serror.obj seq.obj setmap.obj\
	trange.obj wd.obj absio.obj clbuf.obj\
	diskio.obj rpterr.obj setblk.obj ucase.obj wdelim.obj wdisk.obj


all::	$(TARGETS)

format: format.rel
	$(call locate,$@,$^,stacksize(82h) name(FORMAT) purge)

idisk: idisk.rel
	$(call locate,$@,$^,stacksize(90h) name(IDISK) purge)

.INTERMEDIATE: format.rel idisk.rel

format.rel: $(FORMATOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

idisk.rel: $(IDISKOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

