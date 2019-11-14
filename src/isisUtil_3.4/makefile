# path to root of build tree
ROOT := ../..
TARGETS := binobj
PEXFILE:=isisUtil_3.4.pex

include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,3.4)

# build options
LOCATEFLAGS:=SYMBOLS LINES
PLMFLAGS:=DEBUG CODE
LINKFLAGS:=
PLM80 = 3.1

BINOBJOBJ =	binobj.obj dblank.obj dlimit.obj filerr.obj forupp.obj\
	path.obj reperr.obj seq.obj upcase.obj wdlmt.obj mvasci.obj\
	numout.obj

all::
	$(MAKE) $(TARGETS)

binobj: binobj.rel
	$(call locate,$@,$^,code(3680H) name(binobj) stacksize(84) purge)

.INTERMEDIATE: binobj.rel
binobj.rel: $(BINOBJOBJ)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))
