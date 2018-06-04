# path to root of build tree
ROOT := ../..
PEXFILE:=isisUtil_4.3w.pex

include $(ROOT)/tools/isis.mk

# build options
LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:= DEBUG CODE
LINKFLAGS:=

TARGETS := format idisk altmap
REF:=$(call ipath,isis,4.3w)

ALTMAPOBJ = altmap.obj dblank.obj dlimit.obj ferror.obj\
	forupp.obj numout.obj wdskio.obj popul.obj scanin.obj\
	seq.obj setblk.obj alloc.obj wdisk.obj rpter2.obj ucase.obj wdelim.obj

FORMATOBJ =	format.obj alloc.obj direct.obj dblank.obj dlimit.obj ferror.obj fmttrk.obj\
	forupp.obj mvasci.obj numout.obj rpterr.obj seq.obj setmap.obj\
	trange.obj wd.obj ypath.obj absio.obj clbuf.obj\
	diskio.obj wdskio.obj setblk.obj ucase.obj wdelim.obj wdisk.obj

IDISKOBJ =	idisk.obj alloc.obj direct.obj dblank.obj dlimit.obj ferror.obj fmttrk.obj\
	forupp.obj getdsk.obj mvasci.obj numout.obj serror.obj seq.obj setmap.obj\
	trange.obj wd.obj absio.obj clbuf.obj\
	diskio.obj wdskio.obj rpterr.obj setblk.obj ucase.obj wdelim.obj wdisk.obj


all::	$(TARGETS)

altmap: altmap.rel
	$(call locate,$@,$^,stacksize(54h) name(ALTMAP) purge)

format: format.rel
	$(call locate,$@,$^,stacksize(82h) name(FORMAT) purge)

idisk: idisk.rel
	$(call locate,$@,$^,stacksize(90h) name(IDISK) purge)

.INTERMEDIATE: altmap.rel format.rel idisk.rel

altmap.rel: $(ALTMAPOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

format.rel: $(FORMATOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

idisk.rel: $(IDISKOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

