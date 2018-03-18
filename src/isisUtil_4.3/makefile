# path to root of build tree
ROOT := ../..
PEXFILE:=isisUtil_4.3.pex

include $(ROOT)/tools/isis.mk

# build options
LOCATEFLAGS:=SYMBOLS PUBLICS
PLMFLAGS:= DEBUG CODE
LINKFLAGS:=

TARGETS := fixmap format idisk submit hexobj objhex vers
REF:=$(call ipath,isis,4.3)

FIXMAPOBJ = fixmap.obj alloc.obj dblank.obj dlimit.obj ferror.obj fupper.obj numout.obj\
	popul.obj scanin.obj seq.obj setblk.obj rpterr.obj ucase.obj wdelim.obj

FORMATOBJ =	format.obj alloc.obj direct.obj dblank.obj dlimit.obj ferror.obj fmttrk.obj\
	fupper.obj mvasci.obj rpterr.obj seq.obj setmap.obj\
	trange.obj wd.obj ypath.obj absio.obj clbuf.obj\
	diskio.obj numout.obj setblk.obj ucase.obj wdelim.obj

IDISKOBJ =	idisk.obj alloc.obj direct.obj dblank.obj dlimit.obj ferror.obj fmttrk.obj\
	fupper.obj getdsk.obj mvasci.obj serror.obj seq.obj setmap.obj\
	trange.obj wd.obj absio.obj clbuf.obj\
	diskio.obj numout.obj rpterr.obj setblk.obj ucase.obj wdelim.obj

SUBMITOBJ = submit.obj dblank.obj dlimit.obj ferror.obj numout.obj\
	    path.obj scanin.obj seq.obj trange.obj ucase.obj\
	    unpath.obj mvasci.obj rpterr.obj wdelim.obj

HEXOBJOBJ =	hexobj.obj dblank.obj dlimit.obj filerr.obj forupp.obj\
	path.obj scanin.obj seq.obj mvasci.obj rpterr.obj upcase.obj wdlmt.obj 

OBJHEXOBJ =	objhex.obj dblank.obj dlimit.obj filerr.obj forupp.obj numout.obj\
	path.obj seq.obj mvasci.obj rpterr.obj upcase.obj wdlmt.obj

VERSOBJ = vers.obj

all::	$(TARGETS)

fixmap: fixmap.rel
	$(call locate,$@,$^,stacksize(96) name(FIXMAP) purge)

format: format.rel
	$(call locate,$@,$^,stacksize(124) name(FORMAT) purge)

idisk: idisk.rel
	$(call locate,$@,$^,stacksize(138) name(IDISK) purge)

submit: submit.rel
	$(call locate,$@,$^,stacksize(102) name(SUBMIT) purge)

hexobj: hexobj.rel
	$(call locate,$@,$^,code(3680H) name(hexobj) stacksize(86) purge)

objhex: objhex.rel
	$(call locate,$@,$^,code(3680H) name(objhex) stacksize(84) purge)

vers: vers.rel
	$(call locate,$@,$^,code(3680H) name(vers) stacksize(44) purge)

.INTERMEDIATE: fixmap.rel format.rel idisk.rel submit.rel hexobj.rel objhex.rel

fixmap.rel: $(FIXMAPOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

format.rel: $(FORMATOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))


idisk.rel: $(IDISKOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

submit.rel: $(SUBMITOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

hexobj.rel: $(HEXOBJOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

objhex.rel: $(OBJHEXOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

vers.rel: $(VERSOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))
