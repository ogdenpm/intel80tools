# path to root of build tree
ROOT := ../..
TARGETS := hexobj
PEXFILE:=hexobj.pex

include $(ROOT)/tools/isis.mk

REF = $(call ipath,isis,4.3)

# build options
LOCATEFLAGS:=SYMBOLS LINES
PLMFLAGS:=DEBUG CODE
LINKFLAGS:=

objs =	hexobj.obj dblank.obj dlimit.obj filerr.obj forupp.obj\
	path.obj scanin.obj seq.obj mvasci.obj reperr.obj upcase.obj wdlmt.obj

all:: $(TARGETS)

hexobj: hexobj.rel
	$(call locate,$@,$^,code(3680H) name(hexobj) stacksize(86) purge)

.INTERMEDIATE: hexobj.rel
hexobj.rel: $(objs)
	$(call link,$@,$^ $(system.lib) $(plm80.lib))
