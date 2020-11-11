# path to root of build tree
ITOOLS ?= ../..
TARGETS := binobj format
PEXFILE:=isisUtil_3.4.pex

include $(ITOOLS)/tools/isis.mk

REF = $(call ipath,isis,3.4)

# build options
LOCATEFLAGS:=SYMBOLS LINES
PLMFLAGS:=DEBUG CODE
LINKFLAGS:=
PLM80 = 3.1

BINOBJOBJ =	binobj.obj dblank.obj dlimit.obj filerr.obj forupp.obj\
	path.obj reperr.obj seq.obj upcase.obj wdlmt.obj mvasci.obj\
	numout.obj

FORMATOBJ = format.obj mask.obj absio.obj clrbuf.obj dblank.obj\
		   	diskio.obj dlimit.obj filerr.obj fmttrk.obj\
			forupp.obj mvasci.obj reperr.obj seq.obj setblk.obj upcase.obj\
			unpath.obj wrdir.obj wdlmt.obj numout.obj

all::
	$(MAKE) $(TARGETS)

binobj: binobj.rel
	$(call locate,$@,$^,code(3680H) name(binobj) stacksize(84) purge)

format: format.rel
	$(call locate,$@,$^,code(3680H) name(format) stacksize(102) purge )

.INTERMEDIATE: binobj.rel
binobj.rel: $(BINOBJOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

format.rel: $(FORMATOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))

