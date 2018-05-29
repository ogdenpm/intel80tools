# path to root of build tree
ROOT := ../..
PEXFILE:=isisUtil_4.3w.pex

include $(ROOT)/tools/isis.mk

# build options
LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:= DEBUG CODE
LINKFLAGS:=

TARGETS := idisk
REF:=$(call ipath,isis,4.3w)


IDISKOBJ =	idisk.obj alloc.obj direct.obj dblank.obj dlimit.obj ferror.obj fmttrk.obj\
	forupp.obj getdsk.obj mvasci.obj numout.obj serror.obj seq.obj setmap.obj\
	trange.obj wd.obj absio.obj clbuf.obj\
	diskio.obj rpterr.obj setblk.obj ucase.obj wdelim.obj wdisk.obj


all::	$(TARGETS)

idisk: idisk.rel
	$(call locate,$@,$^,stacksize(90h) name(IDISK) purge)

.INTERMEDIATE: idisk.rel

idisk.rel: $(IDISKOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

